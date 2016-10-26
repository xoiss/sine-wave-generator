keys_num = 256
value_width = 16
values_per_line = 8

from math import sin, pi

templ = '0x{:0%uX}, ' % (value_width / 4)
for i in range(0, keys_num, values_per_line):
    s = ''
    for j in range(values_per_line):
        phi = (1 + (i + j) / keys_num) * pi / 2
        sin_phi = round(sin(phi) * (2**value_width - 1))
        s += templ.format(sin_phi)
    print(s)
