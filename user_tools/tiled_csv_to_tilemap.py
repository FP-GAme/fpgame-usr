# Turns a .csv file exported from Tiled into a .tilemap text file for use with our Tile-RAM.
# This program is very strict about the format of the input file.
# The input file must be:
# * A .csv file.
# * Each comma separated entry has a single 32-bit integer.
# * The first 3 MSBs of the 32-bit integers are reserved, and the top of those two indicate the
# *   horizontal mirror and vertical mirror. The 3rd MSB should be 0, as rotations are incompatible
# *   with FP-GAme.

# Tiled can be found here:
# https://www.mapeditor.org/

import sys
import csv

def main(input_path, output_path, palette_ID):
    fr = open(input_path, 'r')
    csv_reader = csv.reader(fr, delimiter=',')

    fw = open(output_path + ".tilemap", 'w')

    # read file into list
    # [ [tile-row 0 tiles], [tile-row 1 tiles], ..., [tile-row 63] ]
    data_2d = list(csv_reader)

    if (len(data_2d) == 0 or len(data_2d[0]) == 0):
        print("Malformed .csv file!")
        quit()

    # Iterate through each tile entry and find three things:
    # 1. Tile ID Reference
    # 2. Horizontal mirror bit
    # 3. Vertical mirror bit
    # (and while we are at it, notify the user of any incompatible rotated tiles)
    for row in range(len(data_2d)):
        for col in range(len(data_2d[0])):
            entry = int(data_2d[row][col])
            pattern_addr = entry & 0x1FFFFFFF
            h = (entry & 0x80000000) > 0
            v = (entry & 0x40000000) > 0
            r = (entry & 0x20000000) > 0
            mirror = (v << 1) | h
            if r:
                print("ERROR: Rotated Tile at row %d column %d!" % (row, entry))
                print("Fix this by ensuring there are no rotated tiles!")

            # Write entry:
            prepend_space = " " if (col != 0) else ""
            append_newline = "\n" if (col == len(data_2d[0]) - 1 and row != len(data_2d) - 1) else ""
            fw.write("%s(%03X,%X,%X)%s" % (prepend_space, pattern_addr, int(palette_ID), mirror,
                     append_newline))

if __name__ == "__main__":
    if len(sys.argv) == 4:
        main(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print("Expecting 3 arguments: <src .csv file>, <dest filename no extension>, and "
            "<palette_ID>")
