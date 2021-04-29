# Turns a .gpl file exported from piskel or GIMP into a text file for use with FP-GAme
# The .gpl file MUST have 16 colors, and the first color is always reserved for transparency

# Piskel can be found here:
# https://github.com/piskelapp/piskel

import sys

def main(input_path, output_path):
    fr = open(input_path, 'r')
    lines = fr.readlines()

    fw = open(output_path+".palette", 'w')

    # We only care about lines 5-20, since these contain color data.
    idx = 0
    for color_line in lines[4:20]:
        color_data = color_line.split()
        r = int(color_data[0])
        g = int(color_data[1])
        b = int(color_data[2])

        append_newline = "" if (idx == 15) else "\n"
        fw.write("%02X%02X%02X%s" % (r, g, b, append_newline))
        idx += 1

if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(sys.argv[1], sys.argv[2])
    else:
        print("Expecting 2 arguments: <src .gpl file> and <dest filename no extension>")