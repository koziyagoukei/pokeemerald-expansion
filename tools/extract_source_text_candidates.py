#!/usr/bin/env python3
"""
Extract source-verified text candidates for the JP text restoration workflow.

This tool deliberately does NOT trust an existing translations.csv row-to-symbol
mapping. It reads strings directly from source files, then optionally matches
those source strings against an en_msg/ja-Hrkt_msg dump pair.

Output CSV columns:
  file,line,symbol,kind,source_text,jp_candidate,action,reason,notes

Typical usage from the repository root:
  python3 tools/extract_source_text_candidates.py \
    --en en_msg.txt \
    --ja ja-Hrkt_msg.txt \
    --out source_text_candidates.csv

The output is for review or for a later apply step. It does not modify sources.
"""

from __future__ import annotations

import argparse
import csv
import re
from collections import Counter, defaultdict
from pathlib import Path
from typing import Iterable

DEFAULT_TARGET_FILES = {
    "src/battle_message.c",
    "src/strings.c",
    "src/menu.c",
    "src/menu_helpers.c",
    "src/list_menu.c",
    "src/window.c",
    "src/text_window.c",
    "src/item.c",
    "src/item_use.c",
    "src/bag_menu.c",
    "src/party_menu.c",
    "src/pokemon_summary_screen.c",
    "src/pokemon_storage_system.c",
    "src/pokedex.c",
    "src/pokedex_area_screen.c",
    "src/start_menu.c",
    "src/option_menu.c",
    "src/save.c",
    "src/shop.c",
    "src/field_message_box.c",
    "src/move_relearner.c",
}

# item_menu has several split files in expansion forks.
def is_default_target_file(path: Path) -> bool:
    s = path.as_posix()
    return s in DEFAULT_TARGET_FILES or (s.startswith("src/item_menu") and s.endswith(".c"))

UNSAFE_SYMBOL_RE = re.compile(
    r"(^\.?(name|trainerName|monName)$)|"
    r"(Healthbox|Gender|Color|Dashes|GameTime|Nickname|TrainerCardName|TrainerClass|FacilityClass|Class|Name)$|"
    r"(gTrainerClasses|sJPText_Flavors)"
)

EXPLICIT_SYMBOL_BLACKLIST = {
    "gText_TwoDashes",
    "gText_GameTime",
    "gText_PlayerMon1Name",
    "gText_OpponentMon1Name",
}

# Symbols manually inspected and allowed even though they look fixed-length in C.
# gText_123Dot also requires a C declaration change to pointer array if translated.
MANUAL_SYMBOL_ALLOW = {
    "gText_123Dot",
}

JAPANESE_RE = re.compile(r"[\u3040-\u30ff\u3400-\u9fff]")
KANJI_RE = re.compile(r"[\u3400-\u4DBF\u4E00-\u9FFF\uF900-\uFAFF]")
BRACKET_TAG_RE = re.compile(r"\[[A-Z0-9_]+(?: [A-Z0-9_ ]+)?\]")
RAW_NULL_RE = re.compile(r"(^|[^A-Z0-9_])(NULL|NONE)([^A-Z0-9_]|$)")
ONLY_TAGS_RE = re.compile(r"^(?:\s|\{[A-Z0-9_]+(?: [A-Z0-9_ ]+)?\})+$")
UPPER_LABEL_RE = re.compile(r"^[A-Z0-9_ .:!?\-+/{}'’“”\"#$%&()*<>=@\[\]\\^`|~]+$")

# Curly tag names known from source are safe. Unknown square-bracket tags are not.
CURVY_TAG_RE = re.compile(r"\{([A-Z0-9_]+)(?: [A-Z0-9_ ]+)?\}")

SEEDED_ALLOWED_TAGS = {
    "JPN",
    "ENG",
    "AUTO",
    "PLAYER",
    "KUN",
    "RIVAL",
    "VERSION",
    "STR_VAR_1",
    "STR_VAR_2",
    "STR_VAR_3",
    "NO",
    "PK",
    "PKMN",
    "POKE",
    "POKEBLOCK",
    "LV",
    "A_BUTTON",
    "B_BUTTON",
    "L_BUTTON",
    "R_BUTTON",
    "START_BUTTON",
    "SELECT_BUTTON",
    "DPAD_UP",
    "DPAD_DOWN",
    "DPAD_LEFT",
    "DPAD_RIGHT",
    "DPAD_UPDOWN",
    "DPAD_LEFTRIGHT",
    "COLOR",
    "HIGHLIGHT",
    "SHADOW",
    "BACKGROUND",
    "ACCENT",
    "PALETTE",
    "FONT",
    "DYNAMIC",
    "PAUSE",
    "CLEAR_TO",
    "SHIFT_RIGHT",
    "SHIFT_DOWN",
    "PLAY_SE",
    "PLAY_BGM",
    "COLOR_HIGHLIGHT_SHADOW",
    "TEXT_COLORS",
    "SPEAKER",
    "MIN_LETTER_SPACING",
    "SKIP",
    "CLEAR",
}

BUTTON_TAGS = (
    "{A_BUTTON}",
    "{B_BUTTON}",
    "{START_BUTTON}",
    "{SELECT_BUTTON}",
    "{L_BUTTON}",
    "{R_BUTTON}",
    "{DPAD_",
)


def normalize_text(text: str, allowed_tags: set[str] | None = None) -> str:
    """Normalize source/dump text into the project-safe comparison form."""
    s = text.strip("\ufeff").rstrip("\n")
    s = s.replace("\\c", "\\p")
    s = s.replace("\\r", "\\n")
    s = s.replace("‥", "…")
    s = s.replace("�", "")
    s = s.replace("{JPN}", "")
    s = s.replace("{ENG}", "")
    for marker in ("[NULL]", "[NONE]", "{NULL}", "{NONE}"):
        s = s.replace(marker, "")

    if allowed_tags is None:
        return s

    def repl_arg(match: re.Match[str]) -> str:
        tag, arg = match.group(1), match.group(2)
        return "{" + tag + " " + arg + "}" if tag in allowed_tags else match.group(0)

    def repl_simple(match: re.Match[str]) -> str:
        tag = match.group(1)
        return "{" + tag + "}" if tag in allowed_tags else match.group(0)

    s = re.sub(r"\[([A-Z0-9_]+) ([A-Z0-9_ ]+)\]", repl_arg, s)
    s = re.sub(r"\[([A-Z0-9_]+)\]", repl_simple, s)
    return s


def visible_text(text: str) -> str:
    without_tags = re.sub(r"\{[^}]*\}", "", text or "")
    return re.sub(r"\\[nlprc]", "", without_tags).strip()


def add_auto_after_placeholders(text: str) -> str:
    for pat in (
        re.compile(r"(\{PLAYER\})(?!\{AUTO\}|\{JPN\})(?=[ぁ-んァ-ヶー一-龯])"),
        re.compile(r"(\{STR_VAR_[123]\})(?!\{AUTO\}|\{JPN\})(?=[ぁ-んァ-ヶー一-龯])"),
        re.compile(r"(\{B_[A-Z0-9_]+\})(?!\{AUTO\}|\{JPN\})(?=[ぁ-んァ-ヶー一-龯])"),
    ):
        text = pat.sub(r"\1{AUTO}", text)
    return text


def wrap_japanese(text: str) -> str:
    if JAPANESE_RE.search(text):
        return "{JPN}" + add_auto_after_placeholders(text) + "{ENG}"
    return text


def collect_allowed_tags(paths: Iterable[Path]) -> set[str]:
    allowed = set(SEEDED_ALLOWED_TAGS)
    for path in paths:
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for match in CURVY_TAG_RE.finditer(text):
            allowed.add(match.group(1))
    return allowed


def read_dump(path: Path, allowed_tags: set[str], wrap_jp: bool = False) -> list[str]:
    if not path.exists():
        return []
    out: list[str] = []
    for line in path.read_text(encoding="utf-8-sig", errors="replace").splitlines():
        if line.rstrip("\n") == "":
            continue
        text = normalize_text(line.rstrip("\n"), allowed_tags)
        out.append(wrap_japanese(text) if wrap_jp else text)
    return out


def build_dump_lookup(en_path: Path, ja_path: Path, allowed_tags: set[str]) -> tuple[dict[str, str], dict[str, set[str]]]:
    en_lines = read_dump(en_path, allowed_tags, wrap_jp=False)
    ja_lines = read_dump(ja_path, allowed_tags, wrap_jp=True)

    pairs: defaultdict[str, set[str]] = defaultdict(set)
    for en, ja in zip(en_lines, ja_lines):
        pairs[en].add(ja)

    unique = {en: next(iter(jas)) for en, jas in pairs.items() if len(jas) == 1}
    duplicate = {en: jas for en, jas in pairs.items() if len(jas) > 1}
    return unique, duplicate


def line_number(text: str, index: int) -> int:
    return text.count("\n", 0, index) + 1


def read_c_string_at(text: str, quote_index: int) -> tuple[str, int]:
    if text[quote_index] != '"':
        raise ValueError("quote_index does not point to a quote")

    i = quote_index + 1
    out: list[str] = []
    escaped = False
    while i < len(text):
        ch = text[i]
        if escaped:
            out.append("\\" + ch)
            escaped = False
        elif ch == "\\":
            escaped = True
        elif ch == '"':
            return "".join(out), i + 1
        else:
            out.append(ch)
        i += 1
    raise ValueError("unterminated string literal")


def read_adjacent_c_strings(text: str, first_quote_index: int) -> tuple[str, int]:
    parts: list[str] = []
    i = first_quote_index
    while True:
        while i < len(text) and text[i].isspace():
            i += 1
        if i >= len(text) or text[i] != '"':
            break
        part, i = read_c_string_at(text, i)
        parts.append(part)
    return "".join(parts), i


def find_symbol_before(text: str, start: int) -> str:
    head = text[max(0, start - 700):start]
    patterns = [
        r"(?:ALIGNED\([0-9]+\)\s+)?(?:static\s+)?const\s+u8\s+([A-Za-z0-9_]+)\s*(?:\[\]|\[\]\[[0-9]+\])\s*=",
        r"\[([A-Z0-9_]+)\]\s*=\s*$",
        r"([A-Za-z0-9_]+)\s*=\s*$",
    ]
    for pat in patterns:
        matches = list(re.finditer(pat, head, re.M))
        if matches:
            return matches[-1].group(1)
    return ""


def extract_strings_from_file(path: Path, allowed_tags: set[str]) -> list[dict[str, str]]:
    text = path.read_text(encoding="utf-8", errors="replace")
    rows: list[dict[str, str]] = []
    call_re = re.compile(r'(_|COMPOUND_STRING)\s*\(\s*"', re.M)

    for match in call_re.finditer(text):
        kind = match.group(1)
        quote_index = match.end() - 1
        try:
            source_text, _end = read_adjacent_c_strings(text, quote_index)
        except ValueError:
            continue

        symbol = find_symbol_before(text, match.start())
        rows.append(
            {
                "file": path.as_posix(),
                "line": str(line_number(text, match.start())),
                "symbol": symbol,
                "kind": kind,
                "source_text": normalize_text(source_text, allowed_tags),
            }
        )
    return rows


def looks_labelish(row: dict[str, str]) -> tuple[bool, str]:
    symbol = row.get("symbol", "")
    source = row.get("source_text", "")
    visible = visible_text(source)

    if symbol in MANUAL_SYMBOL_ALLOW:
        return False, ""
    if symbol in EXPLICIT_SYMBOL_BLACKLIST:
        return True, "explicit_symbol_blacklist"
    if UNSAFE_SYMBOL_RE.search(symbol or ""):
        return True, "unsafe_symbol"
    if ONLY_TAGS_RE.match(source.strip()) and source.strip():
        return True, "tag_only"
    if len(visible) <= 3:
        return True, "very_short_label"
    if len(visible) <= 18 and UPPER_LABEL_RE.match(visible):
        if any(tag in source for tag in BUTTON_TAGS):
            return False, ""
        return True, "uppercase_label_or_identifier"
    return False, ""


def classify(row: dict[str, str], unique_lookup: dict[str, str], duplicate_lookup: dict[str, set[str]]) -> tuple[str, str, str]:
    source = row["source_text"]

    labelish, label_reason = looks_labelish(row)
    if labelish:
        return "", "skip", "label_or_identifier_" + label_reason

    if source.strip() == "":
        return "", "skip", "empty"
    if BRACKET_TAG_RE.search(source):
        return "", "skip", "unknown_or_unconverted_bracket_tag_in_source"
    if RAW_NULL_RE.search(source):
        return "", "skip", "raw_null_or_none_in_source"
    if source in duplicate_lookup:
        return "", "skip", "duplicate_english"
    if source not in unique_lookup:
        return "", "skip", "no_match"

    jp = unique_lookup[source]
    if BRACKET_TAG_RE.search(jp):
        return "", "skip", "unknown_or_unconverted_bracket_tag_in_candidate"
    if RAW_NULL_RE.search(jp):
        return "", "skip", "raw_null_or_none_in_candidate"
    if "�" in jp or "\\r" in jp:
        return "", "skip", "bad_char_or_raw_cr_in_candidate"
    if KANJI_RE.search(jp.replace("円", "")):
        return "", "skip", "kanji_in_candidate"

    return jp, "replace", "unique_match"


def discover_targets(root: Path) -> list[Path]:
    return sorted(path for path in root.rglob("*.c") if is_default_target_file(path))


def main() -> None:
    parser = argparse.ArgumentParser(description="Extract source-verified text candidates.")
    parser.add_argument("--root", default=".", help="Repository root")
    parser.add_argument("--en", default="en_msg.txt", help="English dump text file")
    parser.add_argument("--ja", default="ja-Hrkt_msg.txt", help="Japanese dump text file")
    parser.add_argument("--out", default="source_text_candidates.csv", help="Output CSV")
    parser.add_argument("--all-c-files", action="store_true", help="Scan all src/**/*.c files instead of the default whitelist")
    args = parser.parse_args()

    root = Path(args.root)
    en_path = Path(args.en)
    ja_path = Path(args.ja)
    out_path = Path(args.out)

    source_files = sorted(root.rglob("*.c")) if args.all_c_files else discover_targets(root)
    allowed_tags = collect_allowed_tags(source_files)
    unique_lookup, duplicate_lookup = build_dump_lookup(en_path, ja_path, allowed_tags)

    rows: list[dict[str, str]] = []
    for path in source_files:
        rows.extend(extract_strings_from_file(path, allowed_tags))

    out_fields = [
        "file",
        "line",
        "symbol",
        "kind",
        "source_text",
        "jp_candidate",
        "action",
        "reason",
        "notes",
    ]

    counts: Counter[tuple[str, str]] = Counter()
    out_rows: list[dict[str, str]] = []
    for row in rows:
        jp, action, reason = classify(row, unique_lookup, duplicate_lookup)
        notes: list[str] = []
        if row.get("symbol") in MANUAL_SYMBOL_ALLOW:
            notes.append("manual_fixed_array_ok_requires_c_decl_change")
        if any(tag in jp for tag in BUTTON_TAGS):
            notes.append("button_ui_review")
        if re.search(r"\{(PLAYER|STR_VAR_[123]|B_[A-Z0-9_]+)\}(?!\{AUTO\}|\{JPN\})[ぁ-んァ-ヶー一-龯]", jp):
            notes.append("variable_followed_by_jp_without_auto")

        out = dict(row)
        out.update({"jp_candidate": jp, "action": action, "reason": reason, "notes": ";".join(notes)})
        out_rows.append(out)
        counts[(action, reason)] += 1

    with out_path.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=out_fields)
        writer.writeheader()
        writer.writerows(out_rows)

    print(f"wrote: {out_path}")
    print(f"source files scanned: {len(source_files)}")
    print(f"source strings found: {len(out_rows)}")
    for (action, reason), count in sorted(counts.items()):
        print(f"{action:7s} {reason:45s} {count}")


if __name__ == "__main__":
    main()
