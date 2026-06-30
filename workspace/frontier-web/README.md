# Frontier Pokémon Web Editor

`koziyagoukei/PokeEm-expansion-CanuseJP` の `frontier-hack` ブランチを、GitHub API経由で編集してPR作成する静的Web UIです。

## 目的

- `src/data/battle_frontier/battle_frontier_mons.h` の `gBattleFrontierMons` を編集する
- 新しい `FRONTIER_MON_...` を追加する
- `include/constants/battle_frontier_mons.h` の定数と `NUM_FRONTIER_MONS` を同時更新する
- GitHub APIで変更ブランチを作成し、`frontier-hack` 宛てにPRを作成する
- 削除機能は実装しない

## 認証

Fine-grained personal access tokenを使用します。

必要権限:

- Repository access: `koziyagoukei/PokeEm-expansion-CanuseJP`
- Contents: Read and write
- Pull requests: Read and write

PATはブラウザの `sessionStorage` にのみ保存します。サーバーには送信せず、GitHub APIへの `Authorization` ヘッダーにだけ使用します。

## 開発

```powershell
cd workspace/frontier-web
npm install
npm run dev
```

ビルド:

```powershell
npm run build
```

テスト:

```powershell
npm test
```

## 実装範囲

- 編集対象は `gBattleFrontierMons` が中心です。
- `battle_frontier_trainer_mons.h` は使用箇所数表示に使います。
- `src/battle_factory.c` が取得できる場合はFactory使用表示に使います。
- 既存レコードは該当ブロックのみを置換します。
- 新規追加は現在選択中の個体を複製して作成します。
- PR作成前に差分プレビューを生成し、base branch更新を検出した場合は停止します。
