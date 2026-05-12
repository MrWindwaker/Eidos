#!/usr/bin/env python3
# tools/mkeidar.py - Eidos Archive Builder
# usage mkeidar.py output.img file1.elf file2.elf ...

import sys
import struct
import os

MAGIC = 0x45494400
NAME_LEN = 32

def build(output, files):
    num_files = len(files)

    header_size = 8 + num_files * 40
    data_start = (header_size + 4095) & ~4095

    offsets = []
    pos = data_start

    for f in files:
        offsets.append(pos)
        pos += os.path.getsize(f)
        pos = (pos + 3) & ~3

    with open(output, 'wb') as out:
        out.write(struct.pack('<II', MAGIC, num_files))

        for i, f in enumerate(files):
            name = os.path.basename(f).encode('utf-8')[:NAME_LEN-1]
            name = name.ljust(NAME_LEN, b'\x00')
            size = os.path.getsize(f)
            out.write(name)
            out.write(struct.pack('<II', size, offsets[i]))
        
        pos = 8 + num_files * 40
        out.write(b'\x00' * (data_start-pos))

        for i, f in enumerate(files):
            out.seek(offsets[i])
            with open(f, 'rb') as fin:
                out.write(fin.read())
            
            pos = out.tell()
            pad = (4 - pos %4) %4
            out.write(b'\x00' * pad)

    print(f"mkeidar: wrote {num_files} file(s) to {output}")
    for i,f in enumerate(files):
        print(f"  [{i}] {os.path.basename(f)} @ offset {offsets[i]:#x} ({os.path.getsize(f)} bytes)")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f"usage: {sys.argv[0]} output.img file.elf ...")
        sys.exit(1)
    build(sys.argv[1], sys.argv[2:])