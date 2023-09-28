#!/usr/bin/env python3
import struct
import argparse

from elftools.elf.elffile import ELFFile

parser = argparse.ArgumentParser()
parser.add_argument("elf", help="ELF file to convert")
args = parser.parse_args()

with open(args.elf, "rb") as f:
    elffile = ELFFile(f)

    text_section = elffile.get_section_by_name(".text")
    data_section = elffile.get_section_by_name(".data")
    got_section = elffile.get_section_by_name(".got")
    bss_section = elffile.get_section_by_name(".bss")

    text_length = text_section["sh_size"]
    new_text_length = ((text_length + 3) // 4) * 4
    print(f'.text length: {text_length:4d} -> {new_text_length:4d}')

    text_binary = text_section.data()
    while len(text_binary) < new_text_length:
        text_binary += b"\x00"

    data_length = data_section["sh_size"]
    new_data_length = ((data_length + 3) // 4) * 4
    print(f'.data length: {data_length:4d} -> {new_data_length:4d}')

    data_binary = data_section.data()
    while len(data_binary) < new_data_length:
        data_binary += b"\x00"

    got_length = got_section["sh_size"]
    new_got_length = ((got_length + 3) // 4) * 4
    print(f' .got length: {got_length:4d} -> {new_got_length:4d}')

    got_binary = got_section.data()
    while len(got_binary) < new_got_length:
        got_binary += b"\x00"

    bss_length = bss_section["sh_size"]
    new_bss_length = ((bss_length + 3) // 4) * 4
    print(f' .bss length: {bss_length:4d} -> {new_bss_length:4d}')

    executable = bytearray("QE", encoding="utf8")
    executable += struct.pack("<H", new_text_length // 4)
    executable += struct.pack("<H", new_data_length // 4)
    executable += struct.pack("<H", new_got_length // 4)
    executable += struct.pack("<H", new_bss_length // 4)

    executable += text_binary
    executable += data_binary
    executable += got_binary

    with open(args.elf + ".qke", "wb") as out:
        out.write(executable)
