#!/usr/bin/env python3
from pathlib import Path
import sys
from PIL import Image


def read_gba_palette(path: Path, max_colors: int = 256):
    data = path.read_bytes()
    colors = []

    for i in range(0, min(len(data), max_colors * 2), 2):
        value = data[i] | (data[i + 1] << 8)

        r = value & 0x1F
        g = (value >> 5) & 0x1F
        b = (value >> 10) & 0x1F

        r = (r << 3) | (r >> 2)
        g = (g << 3) | (g >> 2)
        b = (b << 3) | (b >> 2)

        colors.append((r, g, b, 255))

    while len(colors) < max_colors:
        colors.append((0, 0, 0, 255))

    return colors


def decode_8bpp_tiles(gfx: bytes):
    if len(gfx) % 64 != 0:
        print(f"warning: gfx size 0x{len(gfx):X} is not a multiple of 64 bytes")

    tiles = []
    tile_count = len(gfx) // 64

    for tile_index in range(tile_count):
        base = tile_index * 64
        tile = []

        for y in range(8):
            row = []
            for x in range(8):
                row.append(gfx[base + y * 8 + x])
            tile.append(row)

        tiles.append(tile)

    return tiles


def draw_tile(img, px, py, tile, palette):
    for y in range(8):
        for x in range(8):
            color_index = tile[y][x]
            img.putpixel((px + x, py + y), palette[color_index])


def render_affine_logo(gfx_path: Path, tilemap_path: Path, pal_path: Path,
                       width_tiles: int, height_tiles: int, out_path: Path):
    gfx = gfx_path.read_bytes()
    tilemap = tilemap_path.read_bytes()
    palette = read_gba_palette(pal_path, 256)
    tiles = decode_8bpp_tiles(gfx)

    needed = width_tiles * height_tiles
    if needed > len(tilemap):
        raise ValueError(f"tilemap too small: need {needed}, got {len(tilemap)}")

    img = Image.new("RGBA", (width_tiles * 8, height_tiles * 8), (0, 0, 0, 0))

    for i in range(needed):
        tile_index = tilemap[i]

        if tile_index >= len(tiles):
            print(
                f"warning: tile index {tile_index} out of range "
                f"(tile count {len(tiles)}), drawing blank"
            )
            continue

        x = (i % width_tiles) * 8
        y = (i // width_tiles) * 8
        draw_tile(img, x, y, tiles[tile_index], palette)

    img.save(out_path)
    print(f"wrote {out_path} ({width_tiles}x{height_tiles} tiles)")


def render_linear_8bpp_tiles(gfx_path: Path, pal_path: Path,
                             width_tiles: int, height_tiles: int, out_path: Path):
    gfx = gfx_path.read_bytes()
    palette = read_gba_palette(pal_path, 256)
    tiles = decode_8bpp_tiles(gfx)

    needed = width_tiles * height_tiles
    if needed > len(tiles):
        raise ValueError(f"not enough tiles: need {needed}, got {len(tiles)}")

    img = Image.new("RGBA", (width_tiles * 8, height_tiles * 8), (0, 0, 0, 0))

    for i in range(needed):
        x = (i % width_tiles) * 8
        y = (i // width_tiles) * 8
        draw_tile(img, x, y, tiles[i], palette)

    img.save(out_path)
    print(f"wrote {out_path} ({width_tiles}x{height_tiles} tiles)")


def render_emerald_version_obj(gfx_path: Path, pal_path: Path, out_path: Path):
    gfx = gfx_path.read_bytes()
    palette = read_gba_palette(pal_path, 256)
    tiles = decode_8bpp_tiles(gfx)

    # JP Emerald version banner:
    # Left sprite:  64x32 = 8x4 tiles = 32 tiles, starts at tile 0
    # Right sprite: 32x32 = 4x4 tiles = 16 tiles, starts at tile 32
    img = Image.new("RGBA", ((8 + 4) * 8, 4 * 8), (0, 0, 0, 0))

    def draw_block(start_tile, dst_tile_x, block_w, block_h):
        for i in range(block_w * block_h):
            tile_index = start_tile + i
            if tile_index >= len(tiles):
                continue

            x = (i % block_w) * 8 + dst_tile_x * 8
            y = (i // block_w) * 8
            draw_tile(img, x, y, tiles[tile_index], palette)

    draw_block(0, 0, 8, 4)    # left 64x32
    draw_block(32, 8, 4, 4)   # right 32x32

    img.save(out_path)
    print(f"wrote {out_path}")


def main():
    if len(sys.argv) < 2:
        print("usage: python3 tools/render_bpej_title_logo.py JP/title_screen")
        raise SystemExit(1)

    asset_dir = Path(sys.argv[1])

    gfx_path = asset_dir / "pokemon_logo_gfx.lzout"
    tilemap_path = asset_dir / "pokemon_logo_tilemap.lzout"
    pal_path = asset_dir / "pokemon_logo.gbapal"

    for path in [gfx_path, tilemap_path, pal_path]:
        if not path.exists():
            raise FileNotFoundError(path)

    # Pokemon logo:
    # BG2 affine / 256-color BG.
    # Tilemap is 1 byte per tile index.
    # 0x120 bytes = 288 entries.
    layouts = [
        (32, 9),
        (24, 12),
        (18, 16),
        (16, 18),
    ]

    for width, height in layouts:
        out_path = asset_dir / f"pokemon_logo_affine_preview_{width}x{height}.png"
        render_affine_logo(gfx_path, tilemap_path, pal_path, width, height, out_path)

    emerald_path = asset_dir / "emerald_version_gfx.lzout"
    if emerald_path.exists():
        # Debug layouts.
        render_linear_8bpp_tiles(
            emerald_path,
            pal_path,
            16,
            4,
            asset_dir / "emerald_version_preview_16x4.png",
        )
        render_linear_8bpp_tiles(
            emerald_path,
            pal_path,
            8,
            8,
            asset_dir / "emerald_version_preview_8x8.png",
        )

        # Likely actual OBJ composition.
        render_emerald_version_obj(
            emerald_path,
            pal_path,
            asset_dir / "emerald_version_preview_obj.png",
        )


if __name__ == "__main__":
    main()