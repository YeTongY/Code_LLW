#!/usr/bin/env python3
"""Utility for slicing 32x32 sprite sheets into 32x64 character crops."""

from __future__ import annotations

import argparse
import logging
from pathlib import Path
from typing import Iterable

from PIL import Image


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Slice 32x32 based sprite sheets into cropped 32x64 (1x2 tile) sprites. "
            "All images inside the input directory (recursively) will be processed."
        )
    )
    parser.add_argument(
        "-i",
        "--input",
        type=Path,
        default=Path("res/graphics/enemies"),
        help="Root directory that contains sprite sheets (default: res/graphics/enemies)",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("res/graphics/enemies_cut"),
        help="Directory where cropped sprites will be written (default: res/graphics/enemies_cut)",
    )
    parser.add_argument(
        "--tile-size",
        type=int,
        default=32,
        help="Base square tile size in pixels (default: 32)",
    )
    parser.add_argument(
        "--vertical-tiles",
        type=int,
        default=2,
        help="How many stacked tiles form a single output sprite (default: 2 => 32x64)",
    )
    parser.add_argument(
        "--extensions",
        type=str,
        nargs="*",
        default=[".png", ".bmp", ".tga"],
        help="Image extensions that should be processed (default: .png .bmp .tga)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Only report the crops that would be produced without writing files",
    )
    return parser.parse_args()


def iter_sprite_sheets(root: Path, extensions: Iterable[str]) -> Iterable[Path]:
    normalized_ext = {ext.lower() for ext in extensions}
    for path in root.rglob("*"):
        if path.is_file() and path.suffix.lower() in normalized_ext:
            yield path


def slice_sheet(
    image_path: Path,
    input_root: Path,
    output_root: Path,
    tile_size: int,
    vertical_tiles: int,
    dry_run: bool,
) -> int:
    img = Image.open(image_path).convert("RGBA")
    if img.width % tile_size or img.height % tile_size:
        logging.warning(
            "Skipping %s (not aligned to %dx%d tiles)",
            image_path,
            tile_size,
            tile_size,
        )
        return 0

    cols = img.width // tile_size
    rows = img.height // tile_size
    if rows < vertical_tiles:
        logging.warning(
            "Skipping %s (needs at least %d tile rows, got %d)",
            image_path,
            vertical_tiles,
            rows,
        )
        return 0

    produced = 0
    relative_folder = image_path.parent.relative_to(input_root)
    target_dir = output_root / relative_folder
    if not dry_run:
        target_dir.mkdir(parents=True, exist_ok=True)

    for col in range(cols):
        for row in range(0, rows - vertical_tiles + 1, vertical_tiles):
            top = row * tile_size
            left = col * tile_size
            bottom = top + tile_size * vertical_tiles
            crop = img.crop((left, top, left + tile_size, bottom))

            out_name = f"{image_path.stem}_c{col:02d}_r{row // vertical_tiles:02d}.png"
            out_path = target_dir / out_name

            logging.info("%s -> %s", image_path, out_path)
            if not dry_run:
                crop.save(out_path)
            produced += 1

    return produced


def main() -> None:
    args = parse_args()

    logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")

    if not args.input.exists():
        raise SystemExit(f"Input directory {args.input} does not exist")

    sheets = list(iter_sprite_sheets(args.input, args.extensions))
    if not sheets:
        logging.warning("No sprite sheets found under %s", args.input)
        return

    total = 0
    for sheet in sheets:
        total += slice_sheet(
            sheet,
            args.input,
            args.output,
            args.tile_size,
            args.vertical_tiles,
            args.dry_run,
        )

    logging.info("Finished. Produced %d cropped sprites from %d sheets", total, len(sheets))


if __name__ == "__main__":
    main()
