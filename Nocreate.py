from pathlib import Path
import re

SPECIES_H = Path("include/constants/species.h")

# 直接生成禁止にしたい特殊フォーム名パターン
BAN_PATTERNS = [
    "_MEGA",
    "_GMAX",
    "_PRIMAL",
    "_ULTRA",
    "_ORIGIN",
    "_TERA",
    "_WELLSPRING",
    "_HEARTHFLAME",
    "_CORNERSTONE",
    "_ZEN_MODE",
    "_ASH",
    "_BUSTED",
    "_TOTEM",
    "_ETERNAMAX",
    "_CROWNED",
    "_HERO",
    "_COMPLETE",
    "_SCHOOL",
    "_BLADE",
    "_SKY",
    "_THERIAN",
    "_WHITE",
    "_BLACK",
    "_RESOLUTE",
    "_PIROUETTE",
    "_UNBOUND",
    "_DUSK_MANE",
    "_DAWN_WINGS",
    "_LOW_KEY",
    "_ICE_RIDER",
    "_SHADOW_RIDER",
    "_BLOODMOON",
    "_PALDEA_COMBAT",
    "_PALDEA_BLAZE",
    "_PALDEA_AQUA",
]

# これは通常ポケモン名なのにパターンで誤爆しやすいものを救済
ALLOW_EXCEPTIONS = {
    "SPECIES_GIRATINA",     # ORIGINだけ禁止
    "SPECIES_DIALGA",       # ORIGINだけ禁止
    "SPECIES_PALKIA",       # ORIGINだけ禁止
    "SPECIES_OGERPON",      # 仮面フォームだけ禁止
    "SPECIES_NECROZMA",     # ULTRA等だけ禁止
    "SPECIES_ZACIAN",       # CROWNEDだけ禁止
    "SPECIES_ZAMAZENTA",    # CROWNEDだけ禁止
    "SPECIES_KYUREM",       # WHITE/BLACKだけ禁止
    "SPECIES_LANDORUS",     # THERIANだけ禁止
    "SPECIES_THUNDURUS",
    "SPECIES_TORNADUS",
    "SPECIES_ENAMORUS",
}

# そもそも使わせたくないものがあればここへ
MANUAL_BANS = {
    "SPECIES_EGG",
    "SPECIES_NONE",
}

def parse_species():
    text = SPECIES_H.read_text(encoding="utf-8", errors="ignore")
    result = []

    for line in text.splitlines():
        line = line.split("//", 1)[0].strip()
        if not line:
            continue

        m = re.search(r"\b(SPECIES_[A-Z0-9_]+)\b", line)
        if not m:
            continue

        name = m.group(1)
        if name not in result:
            result.append(name)

    return result

def is_allowed(name: str) -> bool:
    if name in MANUAL_BANS:
        return False

    if name in ALLOW_EXCEPTIONS:
        return True

    for pat in BAN_PATTERNS:
        if pat in name:
            return False

    return True

def main():
    species = parse_species()

    allowed = [name for name in species if is_allowed(name)]

    print("static const enum Species sFrontierCreatorAllowedBaseSpecies[] =")
    print("{")
    for name in allowed:
        print(f"    {name},")
    print("    SPECIES_NONE,")
    print("};")
    print()
    print(f"// count: {len(allowed)}")

if __name__ == "__main__":
    main()