keys_number = 256
value_width = 16
values_per_line = 8

from math import sin, pi

stempl = '0x{:0%uX}, ' % round(value_width / 4 + .4)
for i in range(0, keys_number, values_per_line):
    s = ''
    for j in range(values_per_line):
        phi = (i + j) / keys_number * pi / 2
        sin_phi = round(sin(phi) * (2 ** value_width))
        if sin_phi >= 2 ** value_width:
            sin_phi = 0     # stays for fixed point 1.0 represented with 0.0 modulo 1.0
        s += stempl.format(sin_phi)
    print(s)
