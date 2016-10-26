lut_size = 1024
sin_width = 22
values_per_line = 8

from math import sin, pi

for i in range(0, lut_size, values_per_line):
    s = ''
    for j in range(values_per_line):
        phi = (1 + (i + j) / lut_size) * pi / 2
        sin_phi = round(sin(phi) * (2**sin_width - 1))
        s += '0x{:06X}, '.format(sin_phi)
    print(s)
