# Turns a .c file exported from piskel into a pattern text file for use with FP-GAme, given a src
#   color palette text file (FP-GAme format, can be found exported from gpl_to_palette.py).
# The piskel .c file must be of dimension 8x8.
# The colors in the piskel .c file must match the colors of the src palette text file. The easiest
#   way to ensure this is to export both palette (.gpl) and pattern (.c) from the same piskel file.
#   And then, use gpl_to_palette.py to convert the palette, and use this script to convert the
#   pattern using that converted palette.

# Piskel can be found here:
# https://github.com/piskelapp/piskel

import sys

def main(src_path, input_path, output_path):
    palette_fr = open(src_path, 'r')
    palette_lines = palette_fr.readlines()
    palette_color_list = [x.strip() for x in palette_lines]

    c_fr = open(input_path, 'r')
    c_lines = c_fr.readlines()

    fw = open(output_path, 'w')

    # We only care about lines 10-17 (inclusive) of the .c file, since these contain the color data
    #   array which we can then link to palette entries.
    # Firstly, generate the list of hex-pixel colors.
    hex_color_list = []
    for color_line in c_lines[10:18]:
        hexstr_colors = [x.strip() for x in (color_line.split(","))[0:8]]
        # Correctly reorder the bytes for each color component
        for hexstr_color in hexstr_colors:
            r = hexstr_color[8:10].upper()
            g = hexstr_color[6:8].upper()
            b = hexstr_color[4:6].upper()
            hex_color_list.append("%s%s%s" % (r, g, b))

    # Now, modify the hex_color_list by replacing the colors with an index matching that of the
    #   color's palette entry.
    linked_hex_color_list = []
    pixel = 0
    for color in hex_color_list:
        # The following index() will abort the program if the color doesn't exist in palette!
        try:
            link = palette_color_list.index(color)
        except ValueError:
            row = pixel // 8
            col = pixel % 8
            print ("Couldn't find color for pixel (%d, %d): %s" % (col, row, color))
            quit()

        linked_hex_color_list.append(link)
        pixel += 1

    for row in range(0, 8):
        for col in range(0, 8):
            append_newline = "\n" if (col == 7 and row != 7) else ""
            print("%X\n" % linked_hex_color_list[row*8 + col])
            fw.write("%X%s" % (linked_hex_color_list[row*8 + col], append_newline))

if __name__ == "__main__":
    if len(sys.argv) == 4:
        main(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print("Expecting 3 arguments: <src palette .txt file>, <src .c file>, and <dest .txt file>")