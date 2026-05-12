#!/usr/bin/env python3
import sys

img_path  = sys.argv[1]
bytes_out = sys.argv[2]
size_out  = sys.argv[3]

data = open(img_path, 'rb').read()

with open(bytes_out, 'w') as f:
    f.write(', '.join(str(b) for b in data))

with open(size_out, 'w') as f:
    f.write('#pragma once\n')
    f.write('extern const unsigned char _ramdisk_start[];\n')
    f.write('static const unsigned int _ramdisk_size = ' + str(len(data)) + ';\n')

print(f'mkramdisk_h: {len(data)} bytes -> {bytes_out}, {size_out}')