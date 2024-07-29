#!/usr/bin/env python3
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
import argparse
import binascii
import struct
import sys
from pathlib import Path

from elftools import elf
from elftools.elf.elffile import ELFFile
from fnvhash import fnv1a_32

sys.path.append(str(Path(__file__).resolve().parents[1]))
try:
    from leb128 import leb128_encode_unsigned
except ImportError:
    print("Error: leb128.py not found. Please ensure leb128.py is in the same directory as this script.")
    sys.exit(1)

section_info = {
    ".text": {"id": 1, "offset": 0x04000000, "length": 0x02000000},
}

reloc_mapping = {
    elf.enums.ENUM_RELOC_TYPE_ARM["R_ARM_ABS32"]: 0,
    elf.enums.ENUM_RELOC_TYPE_ARM["R_ARM_TARGET1"]: 0,
    elf.enums.ENUM_RELOC_TYPE_ARM["R_ARM_RELATIVE"]: 1,
    elf.enums.ENUM_RELOC_TYPE_ARM["R_ARM_REL32"]: 2,
}

reloc_short_names = {0: "ABS", 1: "REL", 2: "R32"}

reloc_enum_names = {}
for n in elf.enums.ENUM_RELOC_TYPE_ARM.keys():
    reloc_enum_names[elf.enums.ENUM_RELOC_TYPE_ARM[n]] = f"{n} ({elf.enums.ENUM_RELOC_TYPE_ARM[n]})"

reloc_section_names = [".rel.text", ".rel.data", ".rel.nonresident", ".rel.dyn", ".relocs"]


class SectionAddr:
    def __init__(self, addr: int | None = None):
        self.addr = addr
        self.section_name = None
        self.section_id = 0  # i.e. null
        self.section_base = None
        self.section_offset = 0
        self.abs_offset = None

        if addr is not None:
            for s_name, section in section_info.items():
                id = section["id"]
                offset = section["offset"]
                length = section["length"]

                if addr >= offset and addr < (offset + length):
                    self.section_name = s_name
                    self.section_id = id
                    self.section_base = offset
                    self.section_offset = addr - offset
                    self.abs_offset = addr
                    return

    def __repr__(self) -> str:
        if self.is_null:
            return f"SectionAddr<None = [{binascii.hexlify(self.bytes)}]>"
        return f"SectionAddr<{self.section_name}+0x{self.section_offset:X} = [{binascii.hexlify(self.bytes)}]>"

    @property
    def is_null(self) -> bool:
        return self.section_id is None or self.section_id == 0

    @property
    def bytes(self) -> bytearray:
        r = bytearray()
        if self.addr is not None:
            r += leb128_encode_unsigned(self.section_id)
            if self.section_id is not None and self.section_id > 0:
                r += leb128_encode_unsigned(self.section_offset)
        return r


class PackedReloc:
    def __init__(self, type: int, offset: SectionAddr, target: SectionAddr | None = None):
        self.type = type
        self.offset = offset
        self.target = target

    def __repr__(self) -> str:
        if self.target is not None:
            return f"PackedReloc<type={self.type}({reloc_short_names[self.type]}), reloc:{self.offset}...{self.target} = [{binascii.hexlify(self.bytes)}]>"
        return f"PackedReloc<type={self.type}({reloc_short_names[self.type]}), reloc:{self.offset} = [{binascii.hexlify(self.bytes)}]>"

    @property
    def bytes(self) -> bytearray:
        r = bytearray()
        r += leb128_encode_unsigned(self.type)
        r += self.offset.bytes
        if self.target is not None:
            r += self.target.bytes
        return r


parser = argparse.ArgumentParser()
parser.add_argument("elf", help="ELF file to convert")
args = parser.parse_args()

with open(args.elf, "rb") as f:
    elffile = ELFFile(f)

    libinfo_section = elffile.get_section_by_name(".lib_info")
    libinfo_size = libinfo_section["sh_size"]
    libinfo_data = libinfo_section.data()
    (
        libinfo_text_start,
        libinfo_text_size,
        libinfo_code_start,
        libinfo_code_size,
        libinfo_preinit_array_start,
        libinfo_preinit_array_count,
        libinfo_init_array_start,
        libinfo_init_array_count,
        libinfo_fini_array_start,
        libinfo_fini_array_count,
        libinfo_data_start,
        libinfo_data_size,
        libinfo_bss_start,
        libinfo_bss_size,
        libinfo_got_start,
        libinfo_got_size,
        libinfo_plt_start,
        libinfo_plt_size,
    ) = struct.unpack_from("<IIIIIIIIIIIIIIIIII", libinfo_data, 0)

    print("==========================================")
    print("Library information:")
    print("==========================================")
    print("      Field                Length")
    print("==========================================")
    print(f"         text: 0x{libinfo_text_start:08x}, {libinfo_text_size:4d} (0x{libinfo_text_size:04x})")
    print(f"         code: 0x{libinfo_code_start:08x}, {libinfo_code_size:4d} (0x{libinfo_code_size:04x})")
    print(f"preinit_array: 0x{libinfo_preinit_array_start:08x}, {libinfo_preinit_array_count:4d}")
    print(f"   init_array: 0x{libinfo_init_array_start:08x}, {libinfo_init_array_count:4d}")
    print(f"   fini_array: 0x{libinfo_fini_array_start:08x}, {libinfo_fini_array_count:4d}")
    print(f"         data: 0x{libinfo_data_start:08x}, {libinfo_data_size:4d} (0x{libinfo_data_size:04x})")
    print(f"          bss: 0x{libinfo_bss_start:08x}, {libinfo_bss_size:4d} (0x{libinfo_bss_size:04x})")
    print(f"          got: 0x{libinfo_got_start:08x}, {libinfo_got_size:4d} (0x{libinfo_got_size:04x})")
    print(f"          plt: 0x{libinfo_plt_start:08x}, {libinfo_plt_size:4d} (0x{libinfo_plt_size:04x})")

    text_section = elffile.get_section_by_name(".text")
    bss_section = elffile.get_section_by_name(".bss")

    text_address = text_section["sh_addr"]
    text_length = text_section["sh_size"]
    print(f"       .text length: {text_length:4d}")

    text_binary = text_section.data()

    bss_length = bss_section["sh_size"]
    print(f"        .bss length: {bss_length:4d}")

    relocs = []

    def parse_relocs(section_name):
        print("Parsing relocs for " + section_name)
        section = elffile.get_section_by_name(section_name)

        if not isinstance(section, elf.relocation.RelocationSection):
            print("ELF file has no " + section_name + " section")
            return

        symtable = elffile.get_section(section["sh_link"])

        for reloc in section.iter_relocations():
            reloc_type_name = reloc_enum_names[reloc["r_info_type"]]
            if reloc["r_info_type"] in reloc_mapping:
                reloc_type = reloc_mapping[reloc["r_info_type"]]
            else:
                print(f"Invalid relocation {reloc_type_name}.")
                continue

            reloc_offset = reloc["r_offset"]
            reloc_value = None
            for _, section in section_info.items():
                if reloc["r_info_sym"] != 0:
                    symbol = symtable.get_symbol(reloc["r_info_sym"])
                    reloc_value = symbol["st_value"]

            reloc = PackedReloc(
                reloc_type,
                SectionAddr(reloc_offset),
                SectionAddr(reloc_value),
            )
            relocs.append(reloc)
            print(reloc)

    for section_name in reloc_section_names:
        parse_relocs(section_name)

    if libinfo_preinit_array_count > 0:
        raise ValueError("preinit array unsupported")

    executable = bytearray("QKE1", encoding="utf8")
    executable += leb128_encode_unsigned(text_length)
    executable += leb128_encode_unsigned(bss_length)
    executable += leb128_encode_unsigned(libinfo_init_array_start)
    executable += leb128_encode_unsigned(libinfo_init_array_count)
    executable += leb128_encode_unsigned(libinfo_fini_array_start)
    executable += leb128_encode_unsigned(libinfo_fini_array_count)

    executable += leb128_encode_unsigned(len(relocs))

    executable += text_binary

    for reloc in relocs:
        executable += reloc.bytes

    fnv1a_32_hash = fnv1a_32(bytes(executable))
    executable += struct.pack("<I", fnv1a_32_hash)

    with open(args.elf + ".qke", "wb") as out:
        out.write(executable)
