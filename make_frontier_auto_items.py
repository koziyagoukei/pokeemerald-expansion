from pathlib import Path
import re
import sys

ITEM_DATA_CANDIDATES = [
    Path("src/data/items.h"),
    Path("src/data/items.c"),
    Path("src/data/item_data.h"),
    Path("src/data/item_data.c"),
]

AUTO_ITEM_JP_NAMES = [
    "あついいわ",
    "あつぞこブーツ",
    "いのちのたま",
    "エレキシード",
    "おおきなねっこ",
    "かいがらのすず",
    "かえんだま",
    "からぶりほけん",
    "きあいのタスキ",
    "グラスシード",
    "グランドコート",
    "くろいてっきゅう",
    "くろいヘドロ",
    "こうかくレンズ",
    "こだわりスカーフ",
    "こだわりハチマキ",
    "こだわりメガネ",
    "ゴツゴツメット",
    "サイコシード",
    "さらさらいわ",
    "しめったいわ",
    "じゃくてんほけん",
    "しろいハーブ",
    "しんかのきせき",
    "せんせいのツメ",
    "だっしゅつパック",
    "だっしゅつボタン",
    "たつじんのおび",
    "たべのこし",
    "ちからのハチマキ",
    "つめたいいわ",
    "でんきだま",
    "どくどくだま",
    "とつげきチョッキ",
    "のどスプレー",
    "パワフルハーブ",
    "ひかりのこな",
    "ひかりのねんど",
    "ピントレンズ",
    "ふうせん",
    "フォーカスレンズ",
    "ぼうごパット",
    "ぼうじんゴーグル",
    "ミストシード",
    "メトロノーム",
    "メンタルハーブ",
    "ルームサービス",
    "レッドカード",

    "アッキのみ",
    "イアのみ",
    "イトケのみ",
    "イバンのみ",
    "ウイのみ",
    "ウタンのみ",
    "オッカのみ",
    "カゴのみ",
    "カシブのみ",
    "カムラのみ",
    "キーのみ",
    "クラボのみ",
    "サンのみ",
    "ジャポのみ",
    "シュカのみ",
    "ズアのみ",
    "スターのみ",
    "ソクノのみ",
    "タラプのみ",
    "タンガのみ",
    "チイラのみ",
    "ナゾのみ",
    "ナモのみ",
    "バコウのみ",
    "ハバンのみ",
    "バンジのみ",
    "ビアーのみ",
    "フィラのみ",
    "ホズのみ",
    "ミクルのみ",
    "ヤタピのみ",
    "ヤチェのみ",
    "ヨプのみ",
    "ヨロギのみ",
    "リュガのみ",
    "リンドのみ",
    "レンブのみ",
    "ロゼルのみ",
]

def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")

def normalize_name(s: str) -> str:
    return (
        s.replace(" ", "")
         .replace("　", "")
         .replace("・", "")
         .strip()
    )

def parse_string_symbols(text: str):
    """
    static const u8 sFoo[] = _("たべのこし");
    static const u8 sFoo[] = COMPOUND_STRING("たべのこし");
    みたいな名前定義を拾う。
    """
    symbols = {}

    patterns = [
        re.compile(
            r"static\s+const\s+u8\s+([A-Za-z0-9_]+)\[\]\s*=\s*_\(\"([^\"]*)\"\)\s*;",
            re.S,
        ),
        re.compile(
            r"static\s+const\s+u8\s+([A-Za-z0-9_]+)\[\]\s*=\s*COMPOUND_STRING\(\"([^\"]*)\"\)\s*;",
            re.S,
        ),
        re.compile(
            r"const\s+u8\s+([A-Za-z0-9_]+)\[\]\s*=\s*_\(\"([^\"]*)\"\)\s*;",
            re.S,
        ),
        re.compile(
            r"const\s+u8\s+([A-Za-z0-9_]+)\[\]\s*=\s*COMPOUND_STRING\(\"([^\"]*)\"\)\s*;",
            re.S,
        ),
    ]

    for pattern in patterns:
        for sym, value in pattern.findall(text):
            symbols[sym] = value

    return symbols

def extract_block(text: str, start_pos: int):
    """
    [ITEM_X] = { ... } のブロックを雑に中括弧カウントで抜く。
    正規表現一発よりは少しだけ文明的。
    """
    brace_start = text.find("{", start_pos)
    if brace_start < 0:
        return ""

    depth = 0
    for i in range(brace_start, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[brace_start:i + 1]

    return ""

def get_name_from_block(block: str, symbols: dict):
    """
    .name = _("..."),
    .name = COMPOUND_STRING("..."),
    .name = ITEM_NAME("..."),
    .name = "..."
    .name = sSomeName,
    を拾う。
    """
    patterns = [
        re.compile(r"\.name\s*=\s*_\(\"([^\"]*)\"\)"),
        re.compile(r"\.name\s*=\s*COMPOUND_STRING\(\"([^\"]*)\"\)"),
        re.compile(r"\.name\s*=\s*ITEM_NAME\(\"([^\"]*)\"\)"),
        re.compile(r"\.name\s*=\s*\"([^\"]*)\""),
    ]

    for pattern in patterns:
        m = pattern.search(block)
        if m:
            return m.group(1)

    m = re.search(r"\.name\s*=\s*([A-Za-z0-9_]+)", block)
    if m:
        sym = m.group(1)
        return symbols.get(sym)

    return None

def parse_items():
    item_by_jp_name = {}
    debug_name_lines = []

    for path in ITEM_DATA_CANDIDATES:
        if not path.exists():
            continue

        text = read_text(path)
        symbols = parse_string_symbols(text)

        for m in re.finditer(r"\[(ITEM_[A-Z0-9_]+)\]\s*=", text):
            item_const = m.group(1)
            block = extract_block(text, m.end())
            if not block:
                continue

            jp_name = get_name_from_block(block, symbols)
            if jp_name:
                item_by_jp_name[normalize_name(jp_name)] = item_const
            elif ".name" in block:
                first_line = next((line.strip() for line in block.splitlines() if ".name" in line), "")
                debug_name_lines.append((item_const, first_line, str(path)))

    return item_by_jp_name, debug_name_lines

def main():
    item_by_jp_name, debug_name_lines = parse_items()

    if not item_by_jp_name:
        print("ERROR: アイテムデータから名前を拾えませんでした。", file=sys.stderr)
        print("`.name` 行の例:", file=sys.stderr)
        for item_const, line, path in debug_name_lines[:30]:
            print(f"  {path}: {item_const}: {line}", file=sys.stderr)
        print("", file=sys.stderr)
        print("次を実行して .name の形式を見せてください:", file=sys.stderr)
        print("  grep -RIn \"\\.name\" src/data/items.h | head -40", file=sys.stderr)
        sys.exit(1)

    selected = []
    missing = []

    for jp_name in AUTO_ITEM_JP_NAMES:
        item_const = item_by_jp_name.get(normalize_name(jp_name))
        if item_const is None:
            missing.append(jp_name)
        else:
            selected.append((jp_name, item_const))

    print("/*")
    print(" * Generated frontier auto item list.")
    print(f" * matched: {len(selected)} / {len(AUTO_ITEM_JP_NAMES)}")
    print(" */")
    print()
    print("struct FrontierHubAutoItem")
    print("{")
    print("    enum Item itemId;")
    print("    u16 quantity;")
    print("};")
    print()
    print("static const struct FrontierHubAutoItem sFrontierHubAutoItems[] =")
    print("{")

    for jp_name, item_const in selected:
        print(f"    {{ {item_const}, 999 }}, // {jp_name}")

    print("    { ITEM_NONE, 0 },")
    print("};")
    print()

    if missing:
        print("/*")
        print(" * missing item names:")
        for name in missing:
            print(f" * - {name}")
        print(" */")
        print()
        print("/* grep candidates */")
        for name in missing:
            key = name.replace("のみ", "").replace(" ", "").replace("　", "")
            print(f"/* grep -RIn \"{key}\" src/data include/constants | head -30 */")

if __name__ == "__main__":
    main()