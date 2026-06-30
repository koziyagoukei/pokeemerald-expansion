import { useMemo, useState } from "react";
import {
  addFrontierDefine,
  addFrontierMon,
  formatType,
  frontierMonBlockFromValues,
  hiddenPowerTypeFromIvs,
  itemAllowedForFrontier,
  itemMatchesCategory,
  itemSearchText,
  monFlags,
  moveLabel,
  moveSearchText,
  nextFrontierKey,
  nextFrontierValue,
  optionLabel,
  parseWorkspace,
  replaceFrontierMon,
  searchText,
} from "./domain/frontier";
import { GitHubClient } from "./domain/github";
import {
  BALL_LABELS,
  CATEGORY_LABELS,
  GENDER_LABELS,
  NATURE_LABELS,
  TARGET_REPO,
  TYPE_LABELS,
} from "./domain/labels";
import type { FrontierMon, FrontierMonValues, ParsedWorkspace, PendingChange, RepoSettings } from "./domain/types";

const MON_PATH = "src/data/battle_frontier/battle_frontier_mons.h";
const CONSTANT_PATH = "include/constants/battle_frontier_mons.h";
const STAT_LABELS = ["HP", "こうげき", "ぼうぎょ", "すばやさ", "とくこう", "とくぼう"];

function initialSettings(): RepoSettings {
  return {
    owner: sessionStorage.getItem("frontier.owner") || TARGET_REPO.owner,
    repo: sessionStorage.getItem("frontier.repo") || TARGET_REPO.repo,
    branch: sessionStorage.getItem("frontier.branch") || TARGET_REPO.branch,
    token: sessionStorage.getItem("frontier.token") || "",
  };
}

function App() {
  const [settings, setSettings] = useState<RepoSettings>(initialSettings);
  const [workspace, setWorkspace] = useState<ParsedWorkspace | null>(null);
  const [drafts, setDrafts] = useState<Record<string, FrontierMonValues>>({});
  const [selectedKey, setSelectedKey] = useState("");
  const [query, setQuery] = useState("");
  const [itemQuery, setItemQuery] = useState("");
  const [itemCategory, setItemCategory] = useState("all");
  const [moveQuery, setMoveQuery] = useState("");
  const [moveCategory, setMoveCategory] = useState("");
  const [moveType, setMoveType] = useState("");
  const [status, setStatus] = useState("PATを入力して読み込みを実行してください。");
  const [loading, setLoading] = useState(false);
  const [preview, setPreview] = useState("");
  const [previewFresh, setPreviewFresh] = useState(false);
  const [prUrl, setPrUrl] = useState("");

  const selected = useMemo(() => workspace?.frontierMons.find((mon) => mon.record.key === selectedKey) ?? null, [workspace, selectedKey]);
  const selectedValues = selected ? drafts[selected.record.key] ?? selected.values : null;

  const visibleMons = useMemo(() => {
    if (!workspace) return [];
    const needle = query.trim().toLowerCase();
    return workspace.frontierMons.filter((mon) => {
      const values = drafts[mon.record.key] ?? mon.values;
      const speciesName = workspace.reference.speciesNames[values.species] ?? values.species;
      const itemName = workspace.reference.itemNames[values.heldItem] ?? values.heldItem;
      return !needle || searchText(mon.record.key, speciesName, values.species, itemName, values.heldItem).includes(needle);
    });
  }, [workspace, drafts, query]);

  function updateSettings(field: keyof RepoSettings, value: string) {
    setSettings((current) => ({ ...current, [field]: value }));
    sessionStorage.setItem(`frontier.${field}`, value);
  }

  async function loadRepository() {
    setLoading(true);
    setStatus("GitHubから読み込み中...");
    setPreview("");
    setPreviewFresh(false);
    setPrUrl("");
    try {
      const client = new GitHubClient(settings);
      const loaded = await client.readWorkspaceFiles(setStatus);
      const parsed = parseWorkspace(loaded.files, loaded.baseCommitSha);
      setWorkspace(parsed);
      setDrafts({});
      setSelectedKey(parsed.frontierMons[0]?.record.key ?? "");
      setStatus(`${parsed.frontierMons.length}件のフロンティアポケモンを読み込みました。`);
    } catch (error) {
      setStatus(error instanceof Error ? error.message : String(error));
    } finally {
      setLoading(false);
    }
  }

  function setSelectedValues(values: FrontierMonValues) {
    if (!selected) return;
    setDrafts((current) => ({ ...current, [selected.record.key]: values }));
    setPreviewFresh(false);
  }

  function updateValue<K extends keyof FrontierMonValues>(field: K, value: FrontierMonValues[K]) {
    if (!selectedValues) return;
    setSelectedValues({ ...selectedValues, [field]: value });
  }

  function updateArray(field: "ev" | "iv" | "moves", index: number, value: number | string) {
    if (!selectedValues) return;
    const next = [...selectedValues[field]] as Array<number | string>;
    next[index] = value;
    setSelectedValues({ ...selectedValues, [field]: next } as FrontierMonValues);
  }

  function duplicateSelected() {
    if (!workspace || !selectedValues) return;
    const base = selectedValues.species.replace(/^SPECIES_/, "") || "CUSTOM";
    const existing = { ...workspace.reference.frontierValues };
    for (const mon of workspace.frontierMons) {
      if (mon.record.start < 0 && existing[mon.record.key] == null) existing[mon.record.key] = nextFrontierValue(existing);
    }
    const key = nextFrontierKey(base, existing);
    const value = nextFrontierValue(existing);
    const mon: FrontierMon = {
      record: { path: MON_PATH, key, start: -1, end: -1, block: "", values: {} },
      values: { ...selectedValues, nickname: "" },
      usageCount: 0,
      factoryUsage: false,
    };
    setWorkspace({
      ...workspace,
      frontierMons: [...workspace.frontierMons, mon],
      reference: { ...workspace.reference, frontierValues: { ...workspace.reference.frontierValues, [key]: value } },
    });
    setDrafts((current) => ({ ...current, [key]: mon.values }));
    setSelectedKey(key);
    setPreviewFresh(false);
    setStatus(`${key} を追加しました。削除機能はありません。`);
  }

  function buildChanges(): PendingChange[] {
    if (!workspace) return [];
    const changedExisting = workspace.frontierMons
      .filter((mon) => mon.record.start >= 0)
      .filter((mon) => frontierMonBlockFromValues(mon.record.key, drafts[mon.record.key] ?? mon.values) !== mon.record.block);
    const added = workspace.frontierMons.filter((mon) => mon.record.start < 0);
    let monSource = workspace.files[MON_PATH].content;
    for (const mon of [...changedExisting].sort((a, b) => b.record.start - a.record.start)) {
      monSource = replaceFrontierMon(monSource, { ...mon, values: drafts[mon.record.key] ?? mon.values });
    }
    let constantSource = workspace.files[CONSTANT_PATH].content;
    const values = { ...workspace.reference.frontierValues };
    for (const mon of added) {
      const value = values[mon.record.key] ?? nextFrontierValue(values);
      values[mon.record.key] = value;
      monSource = addFrontierMon(monSource, mon.record.key, drafts[mon.record.key] ?? mon.values);
      constantSource = addFrontierDefine(constantSource, mon.record.key, value);
    }
    const changes: PendingChange[] = [];
    if (monSource !== workspace.files[MON_PATH].content) {
      changes.push({ path: MON_PATH, before: workspace.files[MON_PATH].content, after: monSource });
    }
    if (constantSource !== workspace.files[CONSTANT_PATH].content) {
      changes.push({ path: CONSTANT_PATH, before: workspace.files[CONSTANT_PATH].content, after: constantSource });
    }
    return changes;
  }

  function refreshPreview(): PendingChange[] {
    try {
      const changes = buildChanges();
      setPreview(changes.map((change) => makePreview(change)).join("\n\n"));
      setPreviewFresh(true);
      setStatus(changes.length ? `${changes.length}ファイルの差分を生成しました。` : "変更はありません。");
      return changes;
    } catch (error) {
      setPreviewFresh(false);
      setStatus(error instanceof Error ? error.message : String(error));
      return [];
    }
  }

  async function createPullRequest() {
    if (!workspace) return;
    const changes = previewFresh ? buildChanges() : refreshPreview();
    if (!changes.length) return;
    if (!previewFresh) {
      setStatus("差分を確認してから、もう一度PR作成を押してください。");
      return;
    }
    setLoading(true);
    setStatus("PR作成中...");
    try {
      const client = new GitHubClient(settings);
      const result = await client.createPullRequest(
        changes,
        workspace.baseCommitSha,
        "Edit Battle Frontier Pokemon pool",
        [
          "Expansion Frontier Web Editorから作成したPRです。",
          "",
          "変更内容:",
          ...changes.map((change) => `- ${change.path}`),
        ].join("\n"),
      );
      setPrUrl(result.htmlUrl);
      setStatus(`PR #${result.number} を作成しました: ${result.branch}`);
    } catch (error) {
      setStatus(error instanceof Error ? error.message : String(error));
    } finally {
      setLoading(false);
    }
  }

  return (
    <main className="app">
      <header className="toolbar">
        <div className="brand">
          <strong>Frontier Pokémon Web Editor</strong>
          <span>GitHub API / PR作成</span>
        </div>
        <label>
          Owner
          <input value={settings.owner} onChange={(event) => updateSettings("owner", event.target.value)} />
        </label>
        <label>
          Repo
          <input value={settings.repo} onChange={(event) => updateSettings("repo", event.target.value)} />
        </label>
        <label>
          Branch
          <input value={settings.branch} onChange={(event) => updateSettings("branch", event.target.value)} />
        </label>
        <label className="token">
          Fine-grained PAT
          <input type="password" value={settings.token} onChange={(event) => updateSettings("token", event.target.value)} />
        </label>
        <button onClick={loadRepository} disabled={loading}>読み込み</button>
        <button onClick={refreshPreview} disabled={!workspace || loading}>差分プレビュー</button>
        <button onClick={createPullRequest} disabled={!workspace || loading}>PR作成</button>
      </header>

      <div className="status">
        <span>{loading ? "読み込み中..." : status}</span>
        {prUrl && <a href={prUrl} target="_blank" rel="noreferrer">作成したPRを開く</a>}
      </div>

      <section className="workspace">
        <aside className="list-pane">
          <div className="pane-actions">
            <input placeholder="Frontier ID / 種族 / 持ち物" value={query} onChange={(event) => setQuery(event.target.value)} />
            <button onClick={duplicateSelected} disabled={!selected}>複製追加</button>
          </div>
          <div className="mon-table">
            <table>
              <thead>
                <tr>
                  <th>ID</th>
                  <th>種族</th>
                  <th>持ち物</th>
                  <th>使用</th>
                  <th>印</th>
                </tr>
              </thead>
              <tbody>
                {visibleMons.map((mon) => {
                  const values = drafts[mon.record.key] ?? mon.values;
                  const flags = monFlags({ ...mon, values });
                  return (
                    <tr key={mon.record.key} className={mon.record.key === selectedKey ? "selected" : ""} onClick={() => setSelectedKey(mon.record.key)}>
                      <td>{mon.record.key}</td>
                      <td>{workspace?.reference.speciesNames[values.species] ?? values.species}</td>
                      <td>{workspace?.reference.itemNames[values.heldItem] ?? values.heldItem}</td>
                      <td>{mon.usageCount}{mon.factoryUsage ? " / Factory" : ""}</td>
                      <td>{flagText(flags)}</td>
                    </tr>
                  );
                })}
              </tbody>
            </table>
          </div>
        </aside>

        <section className="editor-pane">
          {!workspace || !selected || !selectedValues ? (
            <div className="empty">リポジトリを読み込むと編集できます。</div>
          ) : (
            <FrontierForm
              workspace={workspace}
              mon={selected}
              values={selectedValues}
              itemQuery={itemQuery}
              itemCategory={itemCategory}
              moveQuery={moveQuery}
              moveCategory={moveCategory}
              moveType={moveType}
              onItemQuery={setItemQuery}
              onItemCategory={setItemCategory}
              onMoveQuery={setMoveQuery}
              onMoveCategory={setMoveCategory}
              onMoveType={setMoveType}
              onChange={updateValue}
              onArrayChange={updateArray}
            />
          )}
        </section>
      </section>

      <section className="preview">
        <h2>差分プレビュー</h2>
        <textarea readOnly value={preview || "差分プレビューを生成してください。"} />
      </section>
    </main>
  );
}

type FormProps = {
  workspace: ParsedWorkspace;
  mon: FrontierMon;
  values: FrontierMonValues;
  itemQuery: string;
  itemCategory: string;
  moveQuery: string;
  moveCategory: string;
  moveType: string;
  onItemQuery(value: string): void;
  onItemCategory(value: string): void;
  onMoveQuery(value: string): void;
  onMoveCategory(value: string): void;
  onMoveType(value: string): void;
  onChange<K extends keyof FrontierMonValues>(field: K, value: FrontierMonValues[K]): void;
  onArrayChange(field: "ev" | "iv" | "moves", index: number, value: number | string): void;
};

function FrontierForm(props: FormProps) {
  const { workspace, values } = props;
  const reference = workspace.reference;
  const speciesKeys = Object.keys(reference.speciesNames).sort((a, b) => reference.speciesNames[a].localeCompare(reference.speciesNames[b], "ja"));
  const abilityKeys = [...(reference.speciesAbilities[values.species] ?? ["ABILITY_NONE"])];
  if (values.ability && !abilityKeys.includes(values.ability)) abilityKeys.push(values.ability);
  const itemKeys = Object.values(reference.itemMetadata)
    .filter(itemAllowedForFrontier)
    .filter((item) => itemMatchesCategory(item, props.itemCategory))
    .filter((item) => !props.itemQuery || itemSearchText(item).includes(props.itemQuery.toLowerCase()))
    .sort((a, b) => a.name.localeCompare(b.name, "ja"))
    .map((item) => item.key);
  if (values.heldItem && !itemKeys.includes(values.heldItem) && reference.itemMetadata[values.heldItem]) itemKeys.push(values.heldItem);
  const moveKeys = filteredMoveKeys(workspace, values, props.moveQuery, props.moveType, props.moveCategory);
  const hpType = hiddenPowerTypeFromIvs(values.iv);
  const hpLabel = formatType(hpType);

  return (
    <div className="form-grid">
      <div className="summary">
        <h1>{props.mon.record.key}</h1>
        <p>{reference.speciesNames[values.species] ?? values.species} / {reference.itemNames[values.heldItem] ?? values.heldItem}</p>
        <p>EV合計 {values.ev.reduce((sum, value) => sum + Number(value || 0), 0)} / Hidden Power {hpLabel} [{hpType}]</p>
      </div>

      <section className="card">
        <h2>基本</h2>
        <label>ニックネーム<input value={values.nickname} onChange={(event) => props.onChange("nickname", event.target.value)} placeholder="空欄なら種族名" /></label>
        <label>種族<select value={values.species} onChange={(event) => props.onChange("species", event.target.value)}>
          {speciesKeys.map((key) => <option key={key} value={key}>{reference.speciesNames[key]} [{key}]</option>)}
        </select></label>
        <label>特性<select value={values.ability} onChange={(event) => props.onChange("ability", event.target.value)}>
          {abilityKeys.map((key) => <option key={key} value={key}>{reference.abilityNames[key] ?? key}{reference.speciesAbilities[values.species]?.includes(key) ? "" : " / 既存値"} [{key}]</option>)}
        </select></label>
        <label>性格<select value={values.nature} onChange={(event) => props.onChange("nature", event.target.value)}>
          {Object.keys(NATURE_LABELS).map((key) => <option key={key} value={key}>{optionLabel(key, NATURE_LABELS)}</option>)}
        </select></label>
        <label>ボール<select value={values.ball} onChange={(event) => props.onChange("ball", event.target.value)}>
          {Object.keys(BALL_LABELS).map((key) => <option key={key} value={key}>{optionLabel(key, BALL_LABELS)}</option>)}
        </select></label>
        <label>性別<select value={values.gender} onChange={(event) => props.onChange("gender", event.target.value)}>
          {Object.keys(GENDER_LABELS).map((key) => <option key={key} value={key}>{optionLabel(key, GENDER_LABELS)}</option>)}
        </select></label>
        <div className="number-row">
          <label>Lv<input type="number" min={0} max={100} value={values.lvl} onChange={(event) => props.onChange("lvl", Number(event.target.value))} /></label>
          <label>なつき度<input type="number" min={0} max={255} value={values.friendship} onChange={(event) => props.onChange("friendship", Number(event.target.value))} /></label>
        </div>
      </section>

      <section className="card">
        <h2>持ち物</h2>
        <div className="filters">
          <select value={props.itemCategory} onChange={(event) => props.onItemCategory(event.target.value)}>
            <option value="all">すべて</option>
            <option value="hold">持ち物効果あり</option>
            <option value="berry">きのみ</option>
            <option value="mega">メガストーン</option>
            <option value="z">Z</option>
          </select>
          <input value={props.itemQuery} onChange={(event) => props.onItemQuery(event.target.value)} placeholder="アイテム検索" />
        </div>
        <select value={values.heldItem} onChange={(event) => props.onChange("heldItem", event.target.value)}>
          {itemKeys.map((key) => <option key={key} value={key}>{reference.itemNames[key] ?? key} [{key}]</option>)}
        </select>
      </section>

      <section className="card">
        <h2>能力</h2>
        <StatBoxes title="EV" values={values.ev} max={252} onChange={(index, value) => props.onArrayChange("ev", index, value)} />
        <StatBoxes title="IV" values={values.iv} max={31} onChange={(index, value) => props.onArrayChange("iv", index, value)} />
      </section>

      <section className="card">
        <h2>わざ</h2>
        <div className="filters">
          <select value={props.moveType} onChange={(event) => props.onMoveType(event.target.value)}>
            <option value="">タイプ</option>
            {Object.keys(TYPE_LABELS).filter((key) => key !== "TYPE_NONE").map((key) => <option key={key} value={key}>{TYPE_LABELS[key]}</option>)}
          </select>
          <select value={props.moveCategory} onChange={(event) => props.onMoveCategory(event.target.value)}>
            <option value="">分類</option>
            {Object.keys(CATEGORY_LABELS).map((key) => <option key={key} value={key}>{CATEGORY_LABELS[key]}</option>)}
          </select>
          <input value={props.moveQuery} onChange={(event) => props.onMoveQuery(event.target.value)} placeholder="技名 / MOVE_" />
        </div>
        {values.moves.map((move, index) => (
          <select key={index} value={move} onChange={(event) => props.onArrayChange("moves", index, event.target.value)}>
            {moveKeys.map((key) => <option key={key} value={key}>{moveLabel(key, reference, values.species)}</option>)}
          </select>
        ))}
      </section>

      <section className="card">
        <h2>特殊</h2>
        <label>テラスタイプ<select value={values.teraType} onChange={(event) => props.onChange("teraType", event.target.value)}>
          {Object.keys(TYPE_LABELS).map((key) => <option key={key} value={key}>{optionLabel(key, TYPE_LABELS)}</option>)}
        </select></label>
        <label>ダイマックスLv<input type="number" min={0} max={10} value={values.dynamaxLevel} onChange={(event) => props.onChange("dynamaxLevel", Number(event.target.value))} /></label>
        <label className="check"><input type="checkbox" checked={values.gigantamaxFactor} onChange={(event) => props.onChange("gigantamaxFactor", event.target.checked)} /> キョダイマックス</label>
        <label className="check"><input type="checkbox" checked={values.shouldUseDynamax} onChange={(event) => props.onChange("shouldUseDynamax", event.target.checked)} /> ダイマックス使用</label>
        <label className="check"><input type="checkbox" checked={values.isShiny} onChange={(event) => props.onChange("isShiny", event.target.checked)} /> 色違い</label>
        <label>tags<input value={values.tags} onChange={(event) => props.onChange("tags", event.target.value)} /></label>
      </section>
    </div>
  );
}

function StatBoxes(props: { title: string; values: number[]; max: number; onChange(index: number, value: number): void }) {
  return (
    <div className="stats">
      <h3>{props.title}</h3>
      {STAT_LABELS.map((label, index) => (
        <label key={label}>{label}<input type="number" min={0} max={props.max} value={props.values[index] ?? 0} onChange={(event) => props.onChange(index, Number(event.target.value))} /></label>
      ))}
    </div>
  );
}

function filteredMoveKeys(workspace: ParsedWorkspace, values: FrontierMonValues, query: string, type: string, category: string): string[] {
  const reference = workspace.reference;
  const preferred = reference.speciesLearnsets[values.species] ?? new Set<string>();
  const ordered = ["MOVE_NONE", ...Array.from(preferred).sort((a, b) => (reference.moveNames[a] ?? a).localeCompare(reference.moveNames[b] ?? b, "ja"))];
  for (const selected of values.moves) if (selected && !ordered.includes(selected)) ordered.push(selected);
  for (const move of Object.keys(reference.moveNames).sort((a, b) => (reference.moveNames[a] ?? a).localeCompare(reference.moveNames[b] ?? b, "ja"))) {
    if (!ordered.includes(move)) ordered.push(move);
  }
  const needle = query.trim().toLowerCase();
  const filtered = ordered.filter((move) => {
    const meta = reference.moveMetadata[move];
    if (type && meta?.type !== type) return false;
    if (category && meta?.category !== category) return false;
    if (needle && !moveSearchText(move, reference).includes(needle)) return false;
    return true;
  });
  for (const selected of values.moves) if (selected && !filtered.includes(selected)) filtered.push(selected);
  return filtered.slice(0, 700);
}

function flagText(flags: Record<string, boolean>): string {
  const labels: Record<string, string> = { mega: "M", z: "Z", dmax: "D", tera: "T", shiny: "S", unused: "未" };
  return Object.entries(flags).filter(([, value]) => value).map(([key]) => labels[key]).join(" ");
}

function makePreview(change: PendingChange): string {
  const before = change.before.split(/\r?\n/);
  const after = change.after.split(/\r?\n/);
  const lines = [`--- ${change.path}`, `+++ ${change.path}`];
  const limit = Math.max(before.length, after.length);
  let emitted = 0;
  for (let index = 0; index < limit; index += 1) {
    if (before[index] === after[index]) continue;
    lines.push(`@@ line ${index + 1} @@`);
    if (before[index] != null) lines.push(`- ${before[index]}`);
    if (after[index] != null) lines.push(`+ ${after[index]}`);
    emitted += 1;
    if (emitted >= 300) {
      lines.push("... preview truncated ...");
      break;
    }
  }
  return lines.join("\n");
}

export default App;
