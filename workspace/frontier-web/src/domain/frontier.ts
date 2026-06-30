import {
  BALL_LABELS,
  CATEGORY_LABELS,
  FRONTIER_MON_FIELDS,
  HIDDEN_POWER_TYPES,
  TYPE_LABELS,
} from "./labels";
import type {
  FrontierMon,
  FrontierMonValues,
  ItemMetadata,
  MoveMetadata,
  ReferenceData,
  RepoFile,
  SourceRecord,
} from "./types";

const DESIGNATOR_COLLISION = /\.[A-Za-z_]+\s*=.*\.[A-Za-z_]+\s*=/;

export function decodeBase64Content(content: string): string {
  const cleaned = content.replace(/\s+/g, "");
  const binary = atob(cleaned);
  const bytes = Uint8Array.from(binary, (char) => char.charCodeAt(0));
  return new TextDecoder("utf-8").decode(bytes);
}

export function encodeBase64Content(content: string): string {
  const bytes = new TextEncoder().encode(content);
  let binary = "";
  for (const byte of bytes) binary += String.fromCharCode(byte);
  return btoa(binary);
}

export function quoteCString(value: string): string {
  return `"${value
    .replace(/\\/g, "\\\\")
    .replace(/"/g, '\\"')
    .replace(/\r/g, "\\r")
    .replace(/\n/g, "\\n")}"`;
}

export function unquoteCString(value: string): string {
  const trimmed = value.trim();
  const match = trimmed.match(/"((?:\\.|[^"\\])*)"/);
  if (!match) return trimmed;
  return match[1].replace(/\\([\\'"nrtt])/g, (_all, escaped: string) => {
    if (escaped === "n") return "\n";
    if (escaped === "r") return "\r";
    if (escaped === "t") return "\t";
    return escaped;
  });
}

export function balancedEnd(source: string, openIndex: number): number | null {
  let depth = 0;
  let inString = false;
  let inChar = false;
  let lineComment = false;
  let blockComment = false;
  for (let index = openIndex; index < source.length; index += 1) {
    const char = source[index];
    const next = source[index + 1] ?? "";
    if (lineComment) {
      if (char === "\n") lineComment = false;
      continue;
    }
    if (blockComment) {
      if (char === "*" && next === "/") {
        blockComment = false;
        index += 1;
      }
      continue;
    }
    if (inString) {
      if (char === "\\") {
        index += 1;
      } else if (char === '"') {
        inString = false;
      }
      continue;
    }
    if (inChar) {
      if (char === "\\") {
        index += 1;
      } else if (char === "'") {
        inChar = false;
      }
      continue;
    }
    if (char === "/" && next === "/") {
      lineComment = true;
      index += 1;
      continue;
    }
    if (char === "/" && next === "*") {
      blockComment = true;
      index += 1;
      continue;
    }
    if (char === '"') {
      inString = true;
      continue;
    }
    if (char === "'") {
      inChar = true;
      continue;
    }
    if (char === "{") depth += 1;
    if (char === "}") {
      depth -= 1;
      if (depth === 0) return index + 1;
    }
  }
  return null;
}

function valueEnd(source: string, start: number): number {
  let depth = 0;
  let inString = false;
  let lineComment = false;
  let blockComment = false;
  for (let index = start; index < source.length; index += 1) {
    const char = source[index];
    const next = source[index + 1] ?? "";
    if (lineComment) {
      if (char === "\n") lineComment = false;
      continue;
    }
    if (blockComment) {
      if (char === "*" && next === "/") {
        blockComment = false;
        index += 1;
      }
      continue;
    }
    if (inString) {
      if (char === "\\") {
        index += 1;
      } else if (char === '"') {
        inString = false;
      }
      continue;
    }
    if (char === "/" && next === "/") {
      lineComment = true;
      index += 1;
      continue;
    }
    if (char === "/" && next === "*") {
      blockComment = true;
      index += 1;
      continue;
    }
    if (char === '"') {
      inString = true;
      continue;
    }
    if (char === "{" || char === "(" || char === "[") depth += 1;
    if (char === "}" || char === ")" || char === "]") {
      if (depth === 0 && char === "}") return index;
      depth = Math.max(0, depth - 1);
      continue;
    }
    if (char === "," && depth === 0) return index;
  }
  return source.length;
}

export function rawField(block: string, fieldName: string): string | null {
  const pattern = new RegExp(`\\.${fieldName}\\s*=`, "m");
  const match = pattern.exec(block);
  if (!match) return null;
  const start = match.index + match[0].length;
  const end = valueEnd(block, start);
  return block.slice(start, end).trim();
}

function stringField(block: string, fieldName: string): string | null {
  const raw = rawField(block, fieldName);
  return raw ? unquoteCString(raw) : null;
}

export function firstToken(value: string, prefix: string, fallback: string): string {
  const escaped = prefix.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
  const match = value.match(new RegExp(`\\b(${escaped}[A-Z0-9_]+)\\b`));
  return match ? match[1] : fallback;
}

export function numericArgs(value: string, count: number, fallback = 0): number[] {
  const start = value.indexOf("(") >= 0 ? value.indexOf("(") : value.indexOf("{");
  const end = value.lastIndexOf(")") >= 0 ? value.lastIndexOf(")") : value.lastIndexOf("}");
  if (start < 0 || end <= start) return Array(count).fill(fallback);
  const parsed = value
    .slice(start + 1, end)
    .split(",")
    .map((part) => {
      const cleaned = part.trim();
      return /^-?\d+$/.test(cleaned) ? Number(cleaned) : fallback;
    });
  return [...parsed, ...Array(count).fill(fallback)].slice(0, count);
}

export function parseIndexedRecords(source: string, path: string, prefix: string, fields: string[]): SourceRecord[] {
  const records: SourceRecord[] = [];
  const pattern = new RegExp(`\\[\\s*(${prefix}[A-Z0-9_]+)\\s*\\]\\s*=\\s*\\{`, "g");
  let match: RegExpExecArray | null;
  while ((match = pattern.exec(source))) {
    const open = source.indexOf("{", match.index);
    const close = balancedEnd(source, open);
    if (close == null) continue;
    let end = close;
    while (/[ \t]/.test(source[end] ?? "")) end += 1;
    if (source[end] === ",") end += 1;
    const block = source.slice(match.index, end);
    const values: Record<string, string> = {};
    for (const field of fields) {
      const raw = rawField(block, field);
      if (raw != null) values[field] = raw;
    }
    records.push({ path, key: match[1], start: match.index, end, block, values });
    pattern.lastIndex = end;
  }
  return records;
}

export function parseDefineValues(source: string, prefixes: string[]): Record<string, number> {
  const values: Record<string, number> = {};
  for (const line of source.split(/\r?\n/)) {
    const match = line.match(/^\s*#define\s+([A-Z0-9_]+)\s+(.+?)(?:\s*\/\/.*)?$/);
    if (!match || !prefixes.some((prefix) => match[1].startsWith(prefix))) continue;
    const raw = match[2].trim();
    if (/^\d+$/.test(raw)) values[match[1]] = Number(raw);
  }
  return values;
}

function boolLiteral(value: string | undefined): boolean {
  return (value ?? "").trim() === "TRUE" || (value ?? "").trim() === "1";
}

function displayInteger(value: string | undefined): number {
  const trimmed = (value ?? "").trim();
  if (/^-?\d+$/.test(trimmed)) return Number(trimmed);
  const conditional = trimmed.match(/\?\s*(-?\d+)/);
  return conditional ? Number(conditional[1]) : 0;
}

export function defaultFrontierValues(record: SourceRecord): FrontierMonValues {
  const moveTokens = (record.values.moves ?? "").match(/\bMOVE_[A-Z0-9_]+\b/g) ?? [];
  const moves = [...moveTokens, "MOVE_NONE", "MOVE_NONE", "MOVE_NONE", "MOVE_NONE"].slice(0, 4);
  return {
    nickname: stringField(record.block, "nickname") ?? "",
    species: firstToken(record.values.species ?? "", "SPECIES_", "SPECIES_NONE"),
    moves,
    heldItem: firstToken(record.values.heldItem ?? "", "ITEM_", "ITEM_NONE"),
    ev: numericArgs(record.values.ev ?? "", 6),
    iv: numericArgs(record.values.iv ?? "", 6),
    ability: firstToken(record.values.ability ?? "", "ABILITY_", "ABILITY_NONE"),
    lvl: displayInteger(record.values.lvl),
    ball: firstToken(record.values.ball ?? "", "BALL_", "BALL_POKE"),
    friendship: displayInteger(record.values.friendship),
    nature: firstToken(record.values.nature ?? "", "NATURE_", "NATURE_HARDY"),
    gender: firstToken(record.values.gender ?? "", "TRAINER_MON_", "0"),
    isShiny: boolLiteral(record.values.isShiny),
    teraType: firstToken(record.values.teraType ?? "", "TYPE_", "TYPE_NONE"),
    gigantamaxFactor: boolLiteral(record.values.gigantamaxFactor),
    shouldUseDynamax: boolLiteral(record.values.shouldUseDynamax),
    dynamaxLevel: displayInteger(record.values.dynamaxLevel),
    tags: (record.values.tags ?? "0").trim() || "0",
  };
}

function evText(values: number[]): string {
  return `TRAINER_PARTY_EVS(${values.slice(0, 6).map((value) => clampInt(value, 0, 252)).join(", ")})`;
}

function ivText(values: number[]): string {
  return `TRAINER_PARTY_IVS(${values.slice(0, 6).map((value) => clampInt(value, 0, 31)).join(", ")})`;
}

export function clampInt(value: number, min: number, max: number): number {
  if (!Number.isFinite(value)) return min;
  return Math.max(min, Math.min(max, Math.trunc(value)));
}

export function frontierMonBlockFromValues(key: string, values: FrontierMonValues): string {
  const lines = [`    [${key}] = {`];
  if (values.nickname.trim()) {
    lines.push(`        .nickname = J_COMPOUND_STRING(${quoteCString(values.nickname.trim())}),`);
  }
  lines.push(`        .species = ${values.species || "SPECIES_NONE"},`);
  lines.push(`        .moves = {${values.moves.slice(0, 4).map((move) => move || "MOVE_NONE").join(", ")}},`);
  lines.push(`        .heldItem = ${values.heldItem || "ITEM_NONE"},`);
  if (values.ev.some((value) => value !== 0)) lines.push(`        .ev = ${evText(values.ev)},`);
  if (values.iv.some((value) => value !== 0)) lines.push(`        .iv = ${ivText(values.iv)},`);
  if (values.ability && values.ability !== "ABILITY_NONE") lines.push(`        .ability = ${values.ability},`);
  if (values.lvl) lines.push(`        .lvl = ${clampInt(values.lvl, 0, 100)},`);
  lines.push(`        .ball = ${values.ball || "BALL_POKE"},`);
  if (values.friendship) lines.push(`        .friendship = ${clampInt(values.friendship, 0, 255)},`);
  lines.push(`        .nature = ${values.nature || "NATURE_HARDY"},`);
  if (values.gender && values.gender !== "0") lines.push(`        .gender = ${values.gender},`);
  if (values.isShiny) lines.push("        .isShiny = TRUE,");
  if (values.teraType && values.teraType !== "TYPE_NONE") lines.push(`        .teraType = ${values.teraType},`);
  if (values.gigantamaxFactor) lines.push("        .gigantamaxFactor = TRUE,");
  if (values.shouldUseDynamax) lines.push("        .shouldUseDynamax = TRUE,");
  if (values.dynamaxLevel) lines.push(`        .dynamaxLevel = ${clampInt(values.dynamaxLevel, 0, 10)},`);
  if (values.tags && values.tags !== "0") lines.push(`        .tags = ${values.tags},`);
  lines.push("    },");
  return lines.join("\n");
}

export function assertNoConcatenatedDesignators(source: string): void {
  const bad = source.split(/\r?\n/).find((line) => DESIGNATOR_COLLISION.test(line));
  if (bad) throw new Error(`1行に複数の .field = が連結されています: ${bad.trim()}`);
}

export function replaceFrontierMon(source: string, mon: FrontierMon): string {
  const replacement = frontierMonBlockFromValues(mon.record.key, mon.values);
  const updated = source.slice(0, mon.record.start) + replacement + source.slice(mon.record.end);
  assertNoConcatenatedDesignators(updated);
  return updated;
}

export function addFrontierMon(source: string, key: string, values: FrontierMonValues): string {
  const insertAt = source.lastIndexOf("\n};");
  if (insertAt < 0) throw new Error("gBattleFrontierMons array end was not found.");
  let prefix = source.slice(0, insertAt).trimEnd();
  if (!prefix.endsWith(",")) prefix += ",";
  const updated = `${prefix}\n${frontierMonBlockFromValues(key, values)}${source.slice(insertAt)}`;
  assertNoConcatenatedDesignators(updated);
  return updated;
}

export function addFrontierDefine(source: string, key: string, value: number): string {
  const match = source.match(/^#define\s+NUM_FRONTIER_MONS\s+(\d+)\s*$/m);
  if (!match || match.index == null) throw new Error("NUM_FRONTIER_MONS was not found.");
  const oldNum = Number(match[1]);
  const newNum = Math.max(oldNum, value + 1);
  const defineLine = `#define ${key.padEnd(28, " ")} ${value}\n`;
  const numLine = source.slice(match.index, match.index + match[0].length).replace(/\d+/, String(newNum));
  return source.slice(0, match.index) + defineLine + numLine + source.slice(match.index + match[0].length);
}

export function nextFrontierKey(input: string, existing: Record<string, number>): string {
  let name = input.trim().toUpperCase().replace(/[^A-Z0-9_]+/g, "_").replace(/^_+|_+$/g, "");
  if (!name) name = "CUSTOM";
  if (!name.startsWith("FRONTIER_MON_")) name = `FRONTIER_MON_${name}`;
  const base = name;
  let suffix = 2;
  while (existing[name] != null) {
    name = `${base}_${suffix}`;
    suffix += 1;
  }
  return name;
}

export function nextFrontierValue(existing: Record<string, number>): number {
  const nums = Object.entries(existing)
    .filter(([key]) => key.startsWith("FRONTIER_MON_"))
    .map(([, value]) => value);
  return Math.max(-1, ...nums) + 1;
}

export function hiddenPowerTypeFromIvs(ivs: number[]): string {
  const values = [...ivs, 0, 0, 0, 0, 0, 0].slice(0, 6).map((value) => clampInt(value, 0, 31));
  const bits = ((values[0] & 1) << 0) |
    ((values[1] & 1) << 1) |
    ((values[2] & 1) << 2) |
    ((values[3] & 1) << 3) |
    ((values[4] & 1) << 4) |
    ((values[5] & 1) << 5);
  const index = Math.floor(((HIDDEN_POWER_TYPES.length - 1) * bits) / 63);
  return HIDDEN_POWER_TYPES[index];
}

function labelSearch(key: string, label: string): string {
  return `${key} ${label}`.toLowerCase();
}

function recordLabel(record: SourceRecord, fieldName: string): string {
  return stringField(record.block, fieldName) ?? record.values[fieldName] ?? "";
}

function buildReference(recordsByPath: Record<string, SourceRecord[]>, files: Record<string, RepoFile>): ReferenceData {
  const speciesRecords = Object.entries(recordsByPath)
    .filter(([path]) => path.startsWith("src/data/pokemon/species_info"))
    .flatMap(([, records]) => records);
  const moveRecords = recordsByPath["src/data/moves_info.h"] ?? [];
  const itemRecords = recordsByPath["src/data/items.h"] ?? [];
  const abilityRecords = recordsByPath["src/data/abilities.h"] ?? [];
  const frontierConstants = parseDefineValues(files["include/constants/battle_frontier_mons.h"]?.content ?? "", ["FRONTIER_MON_", "FRONTIER_MONS_", "NUM_FRONTIER_MONS"]);
  const speciesNames = Object.fromEntries(speciesRecords.map((record) => [record.key, recordLabel(record, "speciesName") || record.key]));
  const speciesAbilities = Object.fromEntries(speciesRecords.map((record) => {
    const abilities = [...new Set((record.values.abilities ?? "").match(/\bABILITY_[A-Z0-9_]+\b/g) ?? [])].filter((ability) => ability !== "ABILITY_NONE");
    return [record.key, abilities.length ? abilities : ["ABILITY_NONE"]];
  }));
  const moveNames = Object.fromEntries(moveRecords.map((record) => [record.key, recordLabel(record, "name") || record.key]));
  const moveMetadata = Object.fromEntries(moveRecords.map((record): [string, MoveMetadata] => [record.key, {
    key: record.key,
    name: recordLabel(record, "name") || record.key,
    type: firstToken(record.values.type ?? "", "TYPE_", "TYPE_NONE"),
    category: firstToken(record.values.category ?? "", "DAMAGE_CATEGORY_", ""),
  }]));
  const itemNames = Object.fromEntries(itemRecords.map((record) => [record.key, recordLabel(record, "name") || record.key]));
  const itemMetadata = Object.fromEntries(itemRecords.map((record): [string, ItemMetadata] => [record.key, {
    key: record.key,
    name: recordLabel(record, "name") || record.key,
    pocket: record.values.pocket ?? "",
    holdEffect: record.values.holdEffect ?? "HOLD_EFFECT_NONE",
  }]));
  const abilityNames = Object.fromEntries(abilityRecords.map((record) => [record.key, recordLabel(record, "name") || record.key]));
  const usageCounts: Record<string, number> = {};
  const trainerMons = files["src/data/battle_frontier/battle_frontier_trainer_mons.h"]?.content ?? "";
  for (const token of trainerMons.match(/\bFRONTIER_MON_[A-Z0-9_]+\b/g) ?? []) usageCounts[token] = (usageCounts[token] ?? 0) + 1;
  const factoryUsage = new Set(files["src/battle_factory.c"]?.content.match(/\bFRONTIER_MON_[A-Z0-9_]+\b/g) ?? []);
  const { speciesLearnsets, speciesMoveSources } = buildSpeciesLearnsets(speciesRecords, files);
  return {
    speciesNames,
    speciesAbilities,
    moveNames,
    moveMetadata,
    itemNames,
    itemMetadata,
    abilityNames,
    frontierValues: frontierConstants,
    speciesLearnsets,
    speciesMoveSources,
    usageCounts,
    factoryUsage,
  };
}

function parseMoveArray(source: string): Record<string, Set<string>> {
  const arrays: Record<string, Set<string>> = {};
  const pattern = /\b([A-Za-z0-9_]+)\s*\[\]\s*=\s*\{/g;
  let match: RegExpExecArray | null;
  while ((match = pattern.exec(source))) {
    const open = source.indexOf("{", match.index);
    const close = balancedEnd(source, open);
    if (close == null) continue;
    const moves = new Set((source.slice(open, close).match(/\bMOVE_[A-Z0-9_]+\b/g) ?? []).filter((move) => move !== "MOVE_NONE" && move !== "MOVE_UNAVAILABLE"));
    arrays[match[1]] = moves;
    pattern.lastIndex = close;
  }
  return arrays;
}

function buildSpeciesLearnsets(speciesRecords: SourceRecord[], files: Record<string, RepoFile>): {
  speciesLearnsets: Record<string, Set<string>>;
  speciesMoveSources: Record<string, Record<string, Set<string>>>;
} {
  const frontierFull = parseMoveArray(files["src/data/pokemon/frontier_full_learnsets.h"]?.content ?? "");
  const speciesLearnsets: Record<string, Set<string>> = {};
  const speciesMoveSources: Record<string, Record<string, Set<string>>> = {};
  for (const record of speciesRecords) {
    const titleName = speciesTitleName(record.key);
    const candidates = [
      `s${titleName}FrontierFullLearnset`,
      `s${titleName}FullLearnset`,
      `${record.values.levelUpLearnset ?? ""}FrontierFullLearnset`,
      record.values.levelUpLearnset ?? "",
      record.values.eggMoveLearnset ?? "",
      record.values.teachableLearnset ?? "",
    ];
    const full = candidates.map((name) => frontierFull[name]).find((set) => set && set.size);
    const level = frontierFull[record.values.levelUpLearnset ?? ""] ?? new Set<string>();
    const egg = frontierFull[record.values.eggMoveLearnset ?? ""] ?? new Set<string>();
    const teachable = frontierFull[record.values.teachableLearnset ?? ""] ?? new Set<string>();
    const fallback = new Set([...level, ...egg, ...teachable]);
    speciesMoveSources[record.key] = { full: full ?? new Set<string>(), level, egg, teachable };
    speciesLearnsets[record.key] = full ?? fallback;
  }
  return { speciesLearnsets, speciesMoveSources };
}

function speciesTitleName(species: string): string {
  return species
    .replace(/^SPECIES_/, "")
    .toLowerCase()
    .split("_")
    .filter(Boolean)
    .map((part) => part.charAt(0).toUpperCase() + part.slice(1))
    .join("");
}

export function parseWorkspace(files: Record<string, RepoFile>, baseCommitSha: string) {
  const recordsByPath: Record<string, SourceRecord[]> = {};
  for (const [path, file] of Object.entries(files)) {
    if (path.startsWith("src/data/pokemon/species_info")) {
      recordsByPath[path] = parseIndexedRecords(file.content, path, "SPECIES_", ["speciesName", "abilities", "levelUpLearnset", "eggMoveLearnset", "teachableLearnset"]);
    } else if (path === "src/data/moves_info.h") {
      recordsByPath[path] = parseIndexedRecords(file.content, path, "MOVE_", ["name", "type", "category"]);
    } else if (path === "src/data/items.h") {
      recordsByPath[path] = parseIndexedRecords(file.content, path, "ITEM_", ["name", "pocket", "holdEffect"]);
    } else if (path === "src/data/abilities.h") {
      recordsByPath[path] = parseIndexedRecords(file.content, path, "ABILITY_", ["name"]);
    }
  }
  const reference = buildReference(recordsByPath, files);
  const monPath = "src/data/battle_frontier/battle_frontier_mons.h";
  const frontierRecords = parseIndexedRecords(files[monPath]?.content ?? "", monPath, "FRONTIER_MON_", FRONTIER_MON_FIELDS);
  const frontierMons = frontierRecords.map((record): FrontierMon => ({
    record,
    values: defaultFrontierValues(record),
    usageCount: reference.usageCounts[record.key] ?? 0,
    factoryUsage: reference.factoryUsage.has(record.key),
  }));
  return { files, frontierMons, reference, baseCommitSha };
}

export function itemAllowedForFrontier(item: ItemMetadata): boolean {
  if (item.key === "ITEM_NONE") return true;
  if (item.pocket === "POCKET_TM_HM" || item.pocket === "POCKET_KEY_ITEMS") return false;
  if (item.pocket === "POCKET_BERRIES") return true;
  return item.holdEffect !== "" && item.holdEffect !== "HOLD_EFFECT_NONE";
}

export function itemMatchesCategory(item: ItemMetadata, category: string): boolean {
  if (item.key === "ITEM_NONE") return true;
  if (category === "berry") return item.pocket === "POCKET_BERRIES";
  if (category === "z") return item.holdEffect === "HOLD_EFFECT_Z_CRYSTAL" || item.key.includes("_Z") || item.key.includes("Z_CRYSTAL");
  if (category === "mega") return item.holdEffect === "HOLD_EFFECT_MEGA_STONE";
  if (category === "hold") return itemAllowedForFrontier(item);
  return true;
}

export function moveLabel(move: string, reference: ReferenceData, species: string): string {
  if (move === "MOVE_NONE") return "なし [MOVE_NONE]";
  const meta = reference.moveMetadata[move];
  const name = reference.moveNames[move] ?? move;
  const details = [];
  if (meta?.type) details.push(TYPE_LABELS[meta.type] ?? meta.type);
  if (meta?.category) details.push(CATEGORY_LABELS[meta.category] ?? meta.category);
  const sources = reference.speciesMoveSources[species] ?? {};
  const sourceNames = [];
  if (sources.full?.has(move)) sourceNames.push("Full");
  if (sources.level?.has(move)) sourceNames.push("Lv");
  if (sources.egg?.has(move)) sourceNames.push("Egg");
  if (sources.teachable?.has(move)) sourceNames.push("TM");
  if (sourceNames.length) details.push(sourceNames.join("/"));
  return `${name}${details.length ? ` (${details.join(" / ")})` : ""} [${move}]`;
}

export function monFlags(mon: FrontierMon): Record<string, boolean> {
  const item = mon.values.heldItem;
  const moves = mon.values.moves;
  return {
    mega: item.includes("ITE") || item.includes("MEGA"),
    z: item.includes("_Z") || item.includes("Z_CRYSTAL"),
    dmax: mon.values.shouldUseDynamax,
    tera: mon.values.teraType !== "TYPE_NONE",
    shiny: mon.values.isShiny,
    unused: mon.usageCount === 0 && !mon.factoryUsage,
  };
}

export function optionLabel(key: string, labels: Record<string, string>): string {
  return `${labels[key] ?? key} [${key}]`;
}

export function searchText(...parts: string[]): string {
  return parts.join(" ").toLowerCase();
}

export function moveSearchText(key: string, reference: ReferenceData): string {
  const meta = reference.moveMetadata[key];
  return labelSearch(key, `${reference.moveNames[key] ?? ""} ${meta?.type ?? ""} ${TYPE_LABELS[meta?.type ?? ""] ?? ""} ${meta?.category ?? ""} ${CATEGORY_LABELS[meta?.category ?? ""] ?? ""}`);
}

export function itemSearchText(item: ItemMetadata): string {
  return labelSearch(item.key, `${item.name} ${item.pocket} ${item.holdEffect}`);
}

export function formatType(type: string): string {
  return TYPE_LABELS[type] ?? type;
}

export function formatBall(ball: string): string {
  return BALL_LABELS[ball] ?? ball;
}
