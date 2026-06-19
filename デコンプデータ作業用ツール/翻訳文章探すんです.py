#!/usr/bin/env python3
"""
Simple translation editor for this pokeemerald-expansion repository.

Run:
    python tools/translation_editor.py

The editor scans src/ and include/ for _("...") and COMPOUND_STRING("...")
entries, shows them in one list, and saves edited entries back to their
source files after creating .bak backups.
"""

from __future__ import annotations

import dataclasses
import csv
import re
import shutil
import sys
import tkinter as tk
from datetime import datetime
from pathlib import Path
from tkinter import filedialog, messagebox, ttk


SOURCE_EXTENSIONS = {".c", ".h", ".inc"}
MACROS = ("COMPOUND_STRING", "_")


@dataclasses.dataclass
class TextEntry:
    entry_id: int
    path: Path
    rel_path: str
    line: int
    macro: str
    symbol: str
    literal_start: int
    literal_end: int
    original_text: str
    current_text: str

    @property
    def is_dirty(self) -> bool:
        return self.current_text != self.original_text


def line_number_at(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def previous_identifier_char(text: str, pos: int) -> bool:
    return pos > 0 and (text[pos - 1].isalnum() or text[pos - 1] == "_")


def skip_ws(text: str, pos: int) -> int:
    while pos < len(text) and text[pos].isspace():
        pos += 1
    return pos


def parse_c_string_sequence(text: str, pos: int):
    """Return (start, end, raw_content) for adjacent C string literals."""
    pos = skip_ws(text, pos)
    first_start = None
    last_end = None
    parts = []

    while pos < len(text) and text[pos] == '"':
        quote_start = pos
        pos += 1
        buf = []

        while pos < len(text):
            ch = text[pos]
            if ch == "\\":
                if pos + 1 < len(text):
                    buf.append(text[pos:pos + 2])
                    pos += 2
                else:
                    buf.append(ch)
                    pos += 1
            elif ch == '"':
                pos += 1
                break
            else:
                buf.append(ch)
                pos += 1

        if first_start is None:
            first_start = quote_start
        last_end = pos
        parts.append("".join(buf))
        pos = skip_ws(text, pos)

    if first_start is None:
        return None

    return first_start, last_end, "".join(parts)


def find_macro_calls(text: str):
    """Yield (macro, open_paren_pos) while skipping comments and strings."""
    i = 0
    n = len(text)
    while i < n:
        if text.startswith("//", i):
            end = text.find("\n", i + 2)
            i = n if end == -1 else end + 1
            continue
        if text.startswith("/*", i):
            end = text.find("*/", i + 2)
            i = n if end == -1 else end + 2
            continue
        if text[i] == '"':
            i += 1
            while i < n:
                if text[i] == "\\":
                    i += 2
                elif text[i] == '"':
                    i += 1
                    break
                else:
                    i += 1
            continue
        if text[i] == "'":
            i += 1
            while i < n:
                if text[i] == "\\":
                    i += 2
                elif text[i] == "'":
                    i += 1
                    break
                else:
                    i += 1
            continue

        for macro in MACROS:
            if not text.startswith(macro, i):
                continue
            after = i + len(macro)
            if previous_identifier_char(text, i):
                continue
            if after < n and (text[after].isalnum() or text[after] == "_"):
                continue
            open_pos = skip_ws(text, after)
            if open_pos < n and text[open_pos] == "(":
                yield macro, i, open_pos
                i = open_pos + 1
                break
        else:
            i += 1


def infer_symbol(text: str, macro_start: int) -> str:
    context = text[max(0, macro_start - 500):macro_start]
    matches = list(re.finditer(r"\b(gText_[A-Za-z0-9_]+)\b", context))
    if matches:
        return matches[-1].group(1)

    matches = list(re.finditer(r"\.([A-Za-z_][A-Za-z0-9_]*)\s*=\s*$", context))
    if matches:
        return "." + matches[-1].group(1)

    matches = list(re.finditer(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]+\])?\s*=\s*$", context))
    if matches:
        return matches[-1].group(1)

    line_start = context.rfind("\n") + 1
    return context[line_start:].strip()[-80:] or "(unknown)"


def quote_source_text(raw_text: str) -> str:
    out = ['"']
    i = 0
    while i < len(raw_text):
        ch = raw_text[i]
        if ch == "\r":
            if i + 1 < len(raw_text) and raw_text[i + 1] == "\n":
                i += 1
            out.append(r"\n")
        elif ch == "\n":
            out.append(r"\n")
        elif ch == '"':
            slash_count = 0
            j = i - 1
            while j >= 0 and raw_text[j] == "\\":
                slash_count += 1
                j -= 1
            if slash_count % 2 == 0:
                out.append(r"\"")
            else:
                out.append(ch)
        else:
            out.append(ch)
        i += 1
    out.append('"')
    return "".join(out)


class TranslationEditor(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Pokemon Translation Editor")
        self.geometry("1250x760")

        self.repo_root = tk.StringVar(value=str(Path.cwd()))
        self.search_text = tk.StringVar()
        self.file_filter = tk.StringVar()
        self.show_untranslated_only = tk.BooleanVar(value=False)
        self.show_dirty_only = tk.BooleanVar(value=False)
        self.char_count_var = tk.StringVar(value="Chars: 0")
        self.entries: list[TextEntry] = []
        self.filtered_ids: list[int] = []
        self.file_texts: dict[Path, str] = {}
        self.current_entry_id: int | None = None
        self.loading_editor = False

        self.create_widgets()
        self.search_text.trace_add("write", lambda *_: self.apply_filter())
        self.file_filter.trace_add("write", lambda *_: self.apply_filter())
        self.show_untranslated_only.trace_add("write", lambda *_: self.apply_filter())
        self.show_dirty_only.trace_add("write", lambda *_: self.apply_filter())

    def create_widgets(self):
        root_frame = ttk.Frame(self, padding=8)
        root_frame.pack(fill=tk.X)

        ttk.Label(root_frame, text="Repo root").pack(side=tk.LEFT)
        ttk.Entry(root_frame, textvariable=self.repo_root).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=6)
        ttk.Button(root_frame, text="Browse", command=self.choose_root).pack(side=tk.LEFT)
        ttk.Button(root_frame, text="Scan", command=self.scan).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Button(root_frame, text="Save changes", command=self.save_changes).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Button(root_frame, text="Export CSV", command=self.export_csv).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Button(root_frame, text="Import CSV", command=self.import_csv).pack(side=tk.LEFT, padx=(6, 0))

        search_frame = ttk.Frame(self, padding=(8, 0, 8, 8))
        search_frame.pack(fill=tk.X)
        ttk.Label(search_frame, text="Search").pack(side=tk.LEFT)
        ttk.Entry(search_frame, textvariable=self.search_text).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=6)
        ttk.Label(search_frame, text="File").pack(side=tk.LEFT)
        ttk.Entry(search_frame, textvariable=self.file_filter, width=28).pack(side=tk.LEFT, padx=6)
        ttk.Checkbutton(search_frame, text="Untranslated only", variable=self.show_untranslated_only).pack(side=tk.LEFT)
        ttk.Checkbutton(search_frame, text="Dirty only", variable=self.show_dirty_only).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Button(search_frame, text="Clear", command=self.clear_filters).pack(side=tk.LEFT, padx=(6, 0))

        main = ttk.PanedWindow(self, orient=tk.HORIZONTAL)
        main.pack(fill=tk.BOTH, expand=True, padx=8, pady=(0, 8))

        list_frame = ttk.Frame(main)
        main.add(list_frame, weight=3)

        columns = ("dirty", "symbol", "macro", "text", "file", "line")
        self.tree = ttk.Treeview(list_frame, columns=columns, show="headings", selectmode="browse")
        self.tree.heading("dirty", text="*")
        self.tree.heading("symbol", text="Symbol")
        self.tree.heading("macro", text="Kind")
        self.tree.heading("text", text="Text")
        self.tree.heading("file", text="File")
        self.tree.heading("line", text="Line")
        self.tree.column("dirty", width=32, anchor=tk.CENTER, stretch=False)
        self.tree.column("symbol", width=220)
        self.tree.column("macro", width=130, stretch=False)
        self.tree.column("text", width=420)
        self.tree.column("file", width=260)
        self.tree.column("line", width=70, anchor=tk.E, stretch=False)

        yscroll = ttk.Scrollbar(list_frame, orient=tk.VERTICAL, command=self.tree.yview)
        self.tree.configure(yscrollcommand=yscroll.set)
        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        yscroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.tree.bind("<<TreeviewSelect>>", self.on_select)

        edit_frame = ttk.Frame(main, padding=(8, 0, 0, 0))
        main.add(edit_frame, weight=2)

        self.detail_var = tk.StringVar(value="No entry selected.")
        ttk.Label(edit_frame, textvariable=self.detail_var, wraplength=420, justify=tk.LEFT).pack(fill=tk.X)

        ttk.Label(edit_frame, text="Original").pack(anchor=tk.W, pady=(8, 0))
        self.original_box = tk.Text(edit_frame, height=8, wrap=tk.WORD)
        self.original_box.pack(fill=tk.BOTH, expand=True)
        self.original_box.configure(state=tk.DISABLED)

        ttk.Label(edit_frame, text="Edited").pack(anchor=tk.W, pady=(8, 0))
        self.editor_box = tk.Text(edit_frame, height=12, wrap=tk.WORD, undo=True)
        self.editor_box.pack(fill=tk.BOTH, expand=True)
        self.editor_box.bind("<<Modified>>", self.on_editor_modified)

        button_row = ttk.Frame(edit_frame)
        button_row.pack(fill=tk.X, pady=(8, 0))
        ttk.Button(button_row, text="Revert item", command=self.revert_current).pack(side=tk.LEFT)
        ttk.Button(button_row, text="Copy file path", command=self.copy_current_path).pack(side=tk.LEFT, padx=6)
        ttk.Label(button_row, textvariable=self.char_count_var).pack(side=tk.RIGHT)

        self.status_var = tk.StringVar(value="Choose a repository root and scan.")
        ttk.Label(self, textvariable=self.status_var, anchor=tk.W, padding=(8, 0, 8, 8)).pack(fill=tk.X)

    def choose_root(self):
        selected = filedialog.askdirectory(initialdir=self.repo_root.get() or str(Path.cwd()))
        if selected:
            self.repo_root.set(selected)

    def scan(self):
        root = Path(self.repo_root.get()).resolve()
        if not root.exists():
            messagebox.showerror("Invalid root", "Repository root does not exist.")
            return

        self.entries.clear()
        self.file_texts.clear()
        self.current_entry_id = None

        paths = []
        for subdir in ("src", "include"):
            base = root / subdir
            if base.exists():
                paths.extend(p for p in base.rglob("*") if p.suffix in SOURCE_EXTENSIONS and p.is_file())

        entry_id = 0
        for path in sorted(paths):
            try:
                text = path.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                continue
            if "COMPOUND_STRING" not in text and "_(" not in text and "_ (" not in text:
                continue
            self.file_texts[path] = text
            rel_path = str(path.relative_to(root)).replace("\\", "/")

            seen_ranges = set()
            for macro, macro_start, open_pos in find_macro_calls(text):
                parsed = parse_c_string_sequence(text, open_pos + 1)
                if parsed is None:
                    continue
                literal_start, literal_end, raw_content = parsed
                if (literal_start, literal_end) in seen_ranges:
                    continue
                seen_ranges.add((literal_start, literal_end))

                symbol = infer_symbol(text, macro_start)
                self.entries.append(TextEntry(
                    entry_id=entry_id,
                    path=path,
                    rel_path=rel_path,
                    line=line_number_at(text, literal_start),
                    macro=macro,
                    symbol=symbol,
                    literal_start=literal_start,
                    literal_end=literal_end,
                    original_text=raw_content,
                    current_text=raw_content,
                ))
                entry_id += 1

        self.apply_filter()
        self.clear_editor()
        self.status_var.set(f"Scanned {len(self.entries)} text entries from {len(paths)} files.")

    def apply_filter(self):
        query = self.search_text.get().casefold().strip()
        file_query = self.file_filter.get().casefold().strip()
        self.tree.delete(*self.tree.get_children())
        self.filtered_ids.clear()

        for entry in self.entries:
            haystack = "\n".join((entry.symbol, entry.macro, entry.current_text, entry.rel_path)).casefold()
            if query and query not in haystack:
                continue
            if file_query and file_query not in entry.rel_path.casefold():
                continue
            if self.show_untranslated_only.get() and not self.is_untranslated(entry):
                continue
            if self.show_dirty_only.get() and not entry.is_dirty:
                continue
            self.filtered_ids.append(entry.entry_id)
            preview = entry.current_text.replace("\n", r"\n")
            if len(preview) > 180:
                preview = preview[:177] + "..."
            self.tree.insert("", tk.END, iid=str(entry.entry_id), values=(
                "*" if entry.is_dirty else "",
                entry.symbol,
                entry.macro,
                preview,
                entry.rel_path,
                entry.line,
            ))

        dirty = sum(1 for entry in self.entries if entry.is_dirty)
        untranslated = sum(1 for entry in self.entries if self.is_untranslated(entry))
        self.status_var.set(
            f"Showing {len(self.filtered_ids)} / {len(self.entries)} entries. "
            f"Untranslated: {untranslated}. Dirty: {dirty}."
        )

    def clear_filters(self):
        self.search_text.set("")
        self.file_filter.set("")
        self.show_untranslated_only.set(False)
        self.show_dirty_only.set(False)

    def is_untranslated(self, entry: TextEntry) -> bool:
        return entry.current_text == entry.original_text and self.looks_untranslated(entry.current_text)

    @staticmethod
    def looks_untranslated(text: str) -> bool:
        return any(("A" <= ch <= "Z") or ("a" <= ch <= "z") for ch in text)

    def get_entry(self, entry_id: int) -> TextEntry:
        return self.entries[entry_id]

    def on_select(self, _event=None):
        selected = self.tree.selection()
        if not selected:
            return
        self.load_entry(int(selected[0]))

    def load_entry(self, entry_id: int):
        self.current_entry_id = entry_id
        entry = self.get_entry(entry_id)
        self.detail_var.set(f"{entry.rel_path}:{entry.line}\n{entry.symbol}  [{entry.macro}]")

        self.loading_editor = True
        self.original_box.configure(state=tk.NORMAL)
        self.original_box.delete("1.0", tk.END)
        self.original_box.insert("1.0", entry.original_text)
        self.original_box.configure(state=tk.DISABLED)

        self.editor_box.delete("1.0", tk.END)
        self.editor_box.insert("1.0", entry.current_text)
        self.editor_box.edit_modified(False)
        self.loading_editor = False
        self.update_char_count()

    def clear_editor(self):
        self.detail_var.set("No entry selected.")
        self.original_box.configure(state=tk.NORMAL)
        self.original_box.delete("1.0", tk.END)
        self.original_box.configure(state=tk.DISABLED)
        self.editor_box.delete("1.0", tk.END)
        self.update_char_count()

    def on_editor_modified(self, _event=None):
        if self.loading_editor:
            self.editor_box.edit_modified(False)
            return
        if self.current_entry_id is None:
            self.editor_box.edit_modified(False)
            return

        entry = self.get_entry(self.current_entry_id)
        value = self.editor_box.get("1.0", "end-1c")
        entry.current_text = value
        self.editor_box.edit_modified(False)
        self.update_char_count()
        self.refresh_tree_item(entry)

    def refresh_tree_item(self, entry: TextEntry):
        if not self.tree.exists(str(entry.entry_id)):
            self.apply_filter()
            return
        preview = entry.current_text.replace("\n", r"\n")
        if len(preview) > 180:
            preview = preview[:177] + "..."
        self.tree.item(str(entry.entry_id), values=(
            "*" if entry.is_dirty else "",
            entry.symbol,
            entry.macro,
            preview,
            entry.rel_path,
            entry.line,
        ))
        dirty = sum(1 for item in self.entries if item.is_dirty)
        untranslated = sum(1 for item in self.entries if self.is_untranslated(item))
        self.status_var.set(
            f"Showing {len(self.filtered_ids)} / {len(self.entries)} entries. "
            f"Untranslated: {untranslated}. Dirty: {dirty}."
        )

    def update_char_count(self):
        if self.current_entry_id is None:
            self.char_count_var.set("Chars: 0")
            return
        text = self.editor_box.get("1.0", "end-1c")
        self.char_count_var.set(f"Chars: {len(text)}")

    def revert_current(self):
        if self.current_entry_id is None:
            return
        entry = self.get_entry(self.current_entry_id)
        entry.current_text = entry.original_text
        self.load_entry(entry.entry_id)
        self.refresh_tree_item(entry)

    def copy_current_path(self):
        if self.current_entry_id is None:
            return
        entry = self.get_entry(self.current_entry_id)
        self.clipboard_clear()
        self.clipboard_append(str(entry.path))
        self.status_var.set(f"Copied path: {entry.path}")

    def save_changes(self):
        dirty_entries = [entry for entry in self.entries if entry.is_dirty]
        if not dirty_entries:
            self.status_var.set("No changes to save.")
            return

        by_path: dict[Path, list[TextEntry]] = {}
        for entry in dirty_entries:
            by_path.setdefault(entry.path, []).append(entry)

        try:
            for path, entries in by_path.items():
                current_disk_text = path.read_text(encoding="utf-8")
                self.create_backup(path)

                base_text = self.file_texts[path]
                if current_disk_text != base_text:
                    raise RuntimeError(f"{path} changed on disk after scan. Rescan before saving.")

                new_text = base_text
                for entry in sorted(entries, key=lambda item: item.literal_start, reverse=True):
                    replacement = quote_source_text(entry.current_text)
                    new_text = new_text[:entry.literal_start] + replacement + new_text[entry.literal_end:]

                with path.open("w", encoding="utf-8", newline="") as handle:
                    handle.write(new_text)
        except Exception as exc:
            messagebox.showerror("Save failed", str(exc))
            return

        saved_count = len(dirty_entries)
        self.scan()
        self.status_var.set(f"Saved {saved_count} entries. Backups were created before writing.")

    def create_backup(self, path: Path):
        backup_path = path.with_name(path.name + ".bak")
        if backup_path.exists():
            timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
            backup_path = path.with_name(path.name + f".bak.{timestamp}")
        shutil.copy2(path, backup_path)

    def export_csv(self):
        if not self.entries:
            self.status_var.set("No entries to export.")
            return

        output_path = filedialog.asksaveasfilename(
            title="Export CSV",
            defaultextension=".csv",
            filetypes=(("CSV files", "*.csv"), ("All files", "*.*")),
        )
        if not output_path:
            return

        entries = [self.get_entry(entry_id) for entry_id in self.filtered_ids]
        try:
            with open(output_path, "w", encoding="utf-8-sig", newline="") as handle:
                writer = csv.DictWriter(handle, fieldnames=[
                    "id", "file", "line", "macro", "symbol", "original", "current", "dirty"
                ])
                writer.writeheader()
                for entry in entries:
                    writer.writerow({
                        "id": entry.entry_id,
                        "file": entry.rel_path,
                        "line": entry.line,
                        "macro": entry.macro,
                        "symbol": entry.symbol,
                        "original": entry.original_text,
                        "current": entry.current_text,
                        "dirty": "1" if entry.is_dirty else "0",
                    })
        except Exception as exc:
            messagebox.showerror("Export failed", str(exc))
            return

        self.status_var.set(f"Exported {len(entries)} entries to {output_path}.")

    def import_csv(self):
        if not self.entries:
            self.status_var.set("Scan before importing CSV.")
            return

        input_path = filedialog.askopenfilename(
            title="Import CSV",
            filetypes=(("CSV files", "*.csv"), ("All files", "*.*")),
        )
        if not input_path:
            return

        index = {
            self.csv_key(entry): entry
            for entry in self.entries
        }
        imported = 0
        skipped = 0

        try:
            with open(input_path, "r", encoding="utf-8-sig", newline="") as handle:
                reader = csv.DictReader(handle)
                required = {"file", "line", "macro", "symbol", "original", "current"}
                if not reader.fieldnames or not required.issubset(set(reader.fieldnames)):
                    raise RuntimeError("CSV must contain file,line,macro,symbol,original,current columns.")

                for row in reader:
                    try:
                        line = int(row["line"])
                    except (TypeError, ValueError):
                        skipped += 1
                        continue
                    key = (
                        row["file"],
                        line,
                        row["macro"],
                        row["symbol"],
                        row["original"],
                    )
                    entry = index.get(key)
                    if entry is None:
                        skipped += 1
                        continue
                    entry.current_text = row["current"]
                    imported += 1
        except Exception as exc:
            messagebox.showerror("Import failed", str(exc))
            return

        if self.current_entry_id is not None:
            self.load_entry(self.current_entry_id)
        self.apply_filter()
        self.status_var.set(f"Imported {imported} entries from CSV. Skipped: {skipped}.")

    @staticmethod
    def csv_key(entry: TextEntry):
        return (
            entry.rel_path,
            entry.line,
            entry.macro,
            entry.symbol,
            entry.original_text,
        )


def main() -> int:
    app = TranslationEditor()
    app.mainloop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
