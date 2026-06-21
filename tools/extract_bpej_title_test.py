#!/usr/bin/env python3
import sys
import zlib
from pathlib import Path

ROM_CRC32 = 0x4881F3F8

ASSETS = {
    "pokemon_logo_gfx": 0x517C18,
    "pokemon_logo_tilemap": 0x517AA0,
    "emerald_version_gfx": 0x51938C,
}


def align4(data: bytes) -> bytes:
    return data + (b"\x00" * ((4 - (len(data) % 4)) % 4))


def lz77_decompress_with_used(data: bytes, offset: int):
    if data[offset] != 0x10:
        raise ValueError(f"0x{offset:X}: not GBA LZ77 data")

    size = data[offset + 1] | (data[offset + 2] << 8) | (data[offset + 3] << 16)
    src = offset + 4
    out = bytearray()

    while len(out) < size:
        flags = data[src]
        src += 1

        for bit in range(7, -1, -1):
            if len(out) >= size:
                break

            if flags & (1 << bit):
                b1 = data[src]
                b2 = data[src + 1]
                src += 2

                length = (b1 >> 4) + 3
                disp = ((b1 & 0x0F) << 8) | b2
                disp += 1

                for _ in range(length):
                    out.append(out[-disp])
            else:
                out.append(data[src])
                src += 1

    used = src - offset
    return bytes(out), used


def main():
    if len(sys.argv) < 2:
        print("usage: python3 tools/extract_bpej_title_test.py JP/baserom_jp.gba JP/title_screen")
        raise SystemExit(1)

    rom_path = Path(sys.argv[1])
    out_dir = Path(sys.argv[2]) if len(sys.argv) >= 3 else Path("JP/title_screen")
    out_dir.mkdir(parents=True, exist_ok=True)

    if not rom_path.exists():
        raise FileNotFoundError(rom_path)

    rom = rom_path.read_bytes()
    crc = zlib.crc32(rom) & 0xFFFFFFFF

    if crc != ROM_CRC32:
        raise SystemExit(
            f"Unsupported ROM CRC32: 0x{crc:08X}, expected 0x{ROM_CRC32:08X}"
        )

    for name, offset in ASSETS.items():
        raw, used = lz77_decompress_with_used(rom, offset)
        compressed = rom[offset:offset + used]

        (out_dir / f"{name}.lzout").write_bytes(raw)
        (out_dir / f"{name}.lz").write_bytes(align4(compressed))

        print(
            f"{name}: offset=0x{offset:X}, "
            f"raw=0x{len(raw):X}, compressed=0x{used:X}"
        )

    (out_dir / "pokemon_logo.gbapal").write_bytes(
        align4(rom[0x517B58:0x517B58 + 0x120])
    )
    (out_dir / "press_start.gbapal").write_bytes(
        align4(rom[0x5176D4:0x5176D4 + 0x20])
    )

    print(f"wrote assets to {out_dir}")


if __name__ == "__main__":
    main()
