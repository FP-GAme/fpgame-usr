# Turns a .c file exported from piskel into a pattern text file for use with FP-GAme, given a src
#   color palette text file (FP-GAme format, can be found exported from gpl_to_palette.py).
# The piskel .c file must be of dimension 8x8, 16x16, 24x24, 32x32 or any mix of those widths and
#   heights.
# Any number of frames can be used, however, they will be exported as separate files.
# The colors in the piskel .c file must match the colors of the src palette text file. The easiest
#   way to ensure this is to export both palette (.gpl) and pattern (.c) from the same piskel file.
#   And then, use gpl_to_palette.py to convert the palette, and use this script to convert the
#   pattern using that converted palette.
# If more than one color in the palette is identical, the final pixel color will be the lowest index
#   in the palette. This also means that your "transparent" color must be different than all other
#   colors in your palette. Otherwise, colors you intend to be visible may not be.

# Piskel can be found here:
# https://github.com/piskelapp/piskel

import sys

def main(src_path, input_path, output_path):
    palette_fr = open(src_path, 'r')
    palette_lines = palette_fr.readlines()
    palette_color_list = [x.strip() for x in palette_lines]

    c_fr = open(input_path, 'r')
    c_lines = c_fr.readlines()

    # read lines 2-4 (0-indexed) to determine width, height, and frame count
    frame_count = int(c_lines[2].split(" ")[2])
    frame_width = int(c_lines[3].split(" ")[2])
    frame_height = int(c_lines[4].split(" ")[2])

    if frame_width not in {8, 16, 24, 32}:
        print("Error: Width not supported! Only widths of 8, 16, 24, or 32 are supported.")
        return

    if frame_height not in {8, 16, 24, 32}:
        print("Error: Height not supported! Only heights of 8, 16, 24, or 32 are supported.")
        return

    # The first line to have color data
    color_start_line = 10

    # We only care about lines 10-17 (inclusive) of the .c file, since these contain the color data
    #   array which we can then link to palette entries.
    # Firstly, generate the list of hex-pixel colors.
    # This array contains frame_count sub arrays each containing hex colors from the .c file for
    #   that specific frame.
    hex_color_list = []
    for i in range(frame_count):
        hex_color_list_i = [] # sub array to append to hex_color_list
        for color_line in c_lines[color_start_line:color_start_line+frame_height]:
            hexstr_colors = [x.strip() for x in (color_line.split(","))[0:frame_width]]
            # Correctly reorder the bytes for each color component
            for hexstr_color in hexstr_colors:
                r = hexstr_color[8:10].upper()
                g = hexstr_color[6:8].upper()
                b = hexstr_color[4:6].upper()
                hex_color_list_i.append("%s%s%s" % (r, g, b))
        assert(len(hex_color_list_i) == frame_width*frame_height)
        hex_color_list.append(hex_color_list_i)
        color_start_line += frame_height + 2
    
    # For each frame, link the colors to the palette indices and write the result to a text file.
    for i in range(frame_count):
        pathappendstr = ("-%d.pattern" % i) if frame_count != 1 else ".pattern"
        fw = open(output_path + pathappendstr, 'w')

        # Now, modify the hex_color_list by replacing the colors with an index matching that of the
        #   color's palette entry.
        linked_hex_color_list = []
        pixel = 0
        for color in hex_color_list[i]:
            # The following index() will abort the program if the color doesn't exist in palette!
            try:
                link = palette_color_list.index(color)
            except ValueError:
                row = pixel // frame_width
                col = pixel % frame_width
                print ("Couldn't find color for pixel (%d, %d): %s" % (col, row, color))
                quit()

            linked_hex_color_list.append(link)
            pixel += 1

        for row in range(frame_height):
            for col in range(frame_width):
                append_newline = "\n" if (col == frame_width-1 and row != frame_height-1) else ""
                fw.write("%X%s" % (linked_hex_color_list[row*frame_width + col], append_newline))

if __name__ == "__main__":
    if len(sys.argv) == 4:
        main(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print("Expecting 3 arguments: <src .palette file>, <src .c file>, and <dest filename no"
        " extension>")
