#!/usr/bin/env python3
import struct
import argparse

from elftools.elf.elffile import ELFFile
from elftools import elf


def leb128_encode_unsigned(i: int) -> bytearray:
    r = []
    while True:
        byte = i & 0x7F
        i = i >> 7
        if i == 0:
            r.append(byte)
            return bytearray(r)
        r.append(0x80 | byte)


def leb128_decode_unsigned(b: bytearray) -> int:
    r = 0
    for i, e in enumerate(b):
        r = r + ((e & 0x7F) << (i * 7))
    return r


def leb128_encode_signed(i: int) -> bytearray:
    r = []
    while True:
        byte = i & 0x7F
        i = i >> 7
        if (i == 0 and byte & 0x40 == 0) or (i == -1 and byte & 0x40 != 0):
            r.append(byte)
            return bytearray(r)
        r.append(0x80 | byte)


def leb128_decode_signed(b: bytearray) -> int:
    r = 0
    for i, e in enumerate(b):
        r = r + ((e & 0x7F) << (i * 7))
    if e & 0x40 != 0:
        r |= -(1 << (i * 7) + 7)
    return r


alignment = 8  # 64k (uint16_max) * 8 = 512k max section size

SECTION_TEXT = 0x10000000
SECTION_DATA = 0x20000000
SECTION_NONRESIDENT = 0x30000000

RELOC_TYPE_ABS32 = 0x01000000
RELOC_TYPE_REL32 = 0x02000000

reloc_enum_names = {}
for n in elf.enums.ENUM_RELOC_TYPE_ARM.keys():
    reloc_enum_names[elf.enums.ENUM_RELOC_TYPE_ARM[n]] = n

section_reloc_mapping = {
    ".rel.text": SECTION_TEXT,
    ".rel.data": SECTION_DATA,
    ".rel.nonresident": SECTION_NONRESIDENT,
}

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
        libinfo_nonresident_size,
        libinfo_entrypoint,
    ) = struct.unpack_from("<IIIIIIIIIIIIIIII", libinfo_data, 0)

    encoded_info = bytearray()
    encoded_info += leb128_encode_unsigned(libinfo_text_start)
    encoded_info += leb128_encode_unsigned(libinfo_text_size)
    encoded_info += leb128_encode_unsigned(libinfo_code_start)
    encoded_info += leb128_encode_unsigned(libinfo_code_size)
    encoded_info += leb128_encode_unsigned(libinfo_preinit_array_start)
    encoded_info += leb128_encode_unsigned(libinfo_preinit_array_count)
    encoded_info += leb128_encode_unsigned(libinfo_init_array_start)
    encoded_info += leb128_encode_unsigned(libinfo_init_array_count)
    encoded_info += leb128_encode_unsigned(libinfo_fini_array_start)
    encoded_info += leb128_encode_unsigned(libinfo_fini_array_count)
    encoded_info += leb128_encode_unsigned(libinfo_data_start)
    encoded_info += leb128_encode_unsigned(libinfo_data_size)
    encoded_info += leb128_encode_unsigned(libinfo_bss_start)
    encoded_info += leb128_encode_unsigned(libinfo_bss_size)
    encoded_info += leb128_encode_unsigned(libinfo_nonresident_size)
    encoded_info += leb128_encode_unsigned(libinfo_entrypoint)

    encoded_info_size = len(encoded_info)

    text_section = elffile.get_section_by_name(".text")
    nonresident_section = elffile.get_section_by_name(".nonresident")
    bss_section = elffile.get_section_by_name(".bss")

    text_address = text_section["sh_addr"]
    text_length = text_section["sh_size"]
    new_text_length = ((text_length + alignment - 1) // alignment) * alignment
    print(f"       .text length: {text_length:4d} -> {new_text_length:4d}")

    text_binary = text_section.data()
    while len(text_binary) < new_text_length:
        text_binary += b"\x00"

    nonresident_address = 0
    new_nonresident_length = 0
    if nonresident_section is not None:
        nonresident_address = nonresident_section["sh_addr"]
        nonresident_length = nonresident_section["sh_size"]
        new_nonresident_length = ((nonresident_length + alignment - 1) // alignment) * alignment
        print(f".nonresident length: {nonresident_length:4d} -> {new_nonresident_length:4d}")

        nonresident_binary = nonresident_section.data()
        while len(nonresident_binary) < new_nonresident_length:
            nonresident_binary += b"\x00"

    bss_length = bss_section["sh_size"]
    new_bss_length = ((bss_length + alignment - 1) // alignment) * alignment
    print(f"        .bss length: {bss_length:4d} -> {new_bss_length:4d}")

    relocs = []

    def parse_relocs(section_name):
        print("Parsing relocs for " + section_name)
        rel_text = elffile.get_section_by_name(section_name)
        if not isinstance(rel_text, elf.relocation.RelocationSection):
            print("ELF file has no " + section_name + " section")
            return

        for reloc in rel_text.iter_relocations():
            offset = reloc["r_offset"]
            if reloc["r_info_type"] == elf.enums.ENUM_RELOC_TYPE_ARM["R_ARM_ABS32"]:
                reloc_type = RELOC_TYPE_ABS32
            elif reloc["r_info_type"] == elf.enums.ENUM_RELOC_TYPE_ARM["R_ARM_REL32"]:
                reloc_type = RELOC_TYPE_REL32
            else:
                print("Invalid relocation " + str(reloc["r_info_type"]) + ".")
                continue

            if offset >= RELOC_TYPE_ABS32:
                print("Reloc " + str(reloc["r_info_type"]) + " has an invalid offset.")
                continue
            relocs.append(section_reloc_mapping[section_name] | reloc_type | reloc["r_offset"])
            print(f"Reloc {reloc_enum_names[reloc['r_info_type']]} offset: {int(str(reloc['r_offset'])):08x}")

    for section_name in section_reloc_mapping.keys():
        parse_relocs(section_name)

    executable = bytearray("QKE1", encoding="utf8")
    executable += struct.pack("<B", 0)  # TBD
    executable += struct.pack("<B", alignment)
    executable += struct.pack("<H", new_text_length // alignment)
    executable += struct.pack("<H", new_nonresident_length // alignment)
    executable += struct.pack("<H", new_bss_length // alignment)
    executable += struct.pack("<H", len(relocs))

    executable += text_binary
    if new_nonresident_length > 0:
        executable += nonresident_binary

    for reloc in relocs:
        print(f"Writing reloc: {reloc:08x} -- Section flag: {reloc & 0xF0000000:08x}, type: {reloc & 0x0F000000:08x}, offset: {reloc & 0x00FFFFFF:08x}")
        executable += struct.pack("<I", reloc)

    with open(args.elf + ".qke", "wb") as out:
        out.write(executable)
