# user_tools
This directory contains programs or other tools to assist game developers in making or translating
their game assets to the FP-GAme platform.

## Art Tools
FP-GAme can work with any art tool given that the right file converters are used. Since FP-GAme's
file formats are atypical, though, you may have to write your own converter.

We provide converters for the tools Piskel and Tiled. See the fpgame_developers_manual.pdf in the
docs subdirectory for specific instructions on how to export formats from Piskel or Tiled for use
with our provided scripts.

### Piskel
Piskel is a small pixel editor which runs online or on Windows, MacOS, or Linux. It supports the
sprite/pattern sizes that FP-GAme supports and can also preview animations.

https://www.piskelapp.com/

Notably, it can export .gpl files containing palette colors and .c files containing pixel patterns.

### gpl_to_palette.py
This script converts a Piskel-exported 16-color .gpl file to an FP-GAme .palette file.

The resulting .palette file can be loaded into your game through the use of the PPU user library
function ppu_load_pattern.

### c_to_pattern.py
This file converts a Piskel-exported .c file to an FP-GAme .pattern file. The resulting .pattern
file can be imported into your game via the PPU user library function ppu_load_pattern.

NOTE: This script requires a matching palette exported from gpl_to_palette.py. You must ensure that
all colors in the Piskel-exported .c file can be matched with colors in the .palette you choose to
use.

### Tiled
Tiled is a level editor / planner which can export tile layouts (tilemaps) to .csv format.'
Importantly, this editor supports horizontally and vertically mirrored tiles. It also runs on
Windows, MacOS, or Linux.

https://www.mapeditor.org/

Loading .tilemaps isn't strictly necessary in FP-GAme. But if your game is organized into discrete
levels or "rooms", this is a perfect way to plan out and load a predetermined set of tiles.

NOTE: Tiled also supported rotated tiles, but FP-GAme does not. The tiled_csv_to_tilemap.py script
discussed below will warn you if it finds rotated tiles.

### tiled_csv_to_tilemap.py
This script converts a Tiled-exported .csv file to an .tilemap.

The resulting .tilemap file can be loaded into your game through the use of the PPU user library
function ppu_load_tilemap.

## Audio Tools
FP-GAme uses raw 8-bit signed PCM files (.raw) which are automatically included in the game binary
built with the provided Makefile. An example of this can be found in examples/techdemo.

### Audacity
We recommend the use of Audacity to generate these raw files from your sound files. Audacity runs on
Windows MacOS, and Linux.

https://www.audacityteam.org/

See the fpgame_developers_manual.pdf in the docs subdirectory for specific instructions on how to
export the .raw format for use with FP-GAme.

Once you have exported a .raw file, you can simply rename the extension to .bin to get the provided
Makefile to recognize it.