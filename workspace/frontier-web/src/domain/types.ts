export type RepoSettings = {
  owner: string;
  repo: string;
  branch: string;
  token: string;
};

export type RepoFile = {
  path: string;
  sha: string;
  content: string;
};

export type SourceRecord = {
  path: string;
  key: string;
  start: number;
  end: number;
  block: string;
  values: Record<string, string>;
};

export type NamedConstant = {
  key: string;
  value: number;
  label: string;
  search: string;
};

export type MoveMetadata = {
  key: string;
  name: string;
  type: string;
  category: string;
};

export type ItemMetadata = {
  key: string;
  name: string;
  pocket: string;
  holdEffect: string;
};

export type FrontierMonValues = {
  nickname: string;
  species: string;
  moves: string[];
  heldItem: string;
  ev: number[];
  iv: number[];
  ability: string;
  lvl: number;
  ball: string;
  friendship: number;
  nature: string;
  gender: string;
  isShiny: boolean;
  teraType: string;
  gigantamaxFactor: boolean;
  shouldUseDynamax: boolean;
  dynamaxLevel: number;
  tags: string;
};

export type FrontierMon = {
  record: SourceRecord;
  values: FrontierMonValues;
  usageCount: number;
  factoryUsage: boolean;
};

export type ReferenceData = {
  speciesNames: Record<string, string>;
  speciesAbilities: Record<string, string[]>;
  moveNames: Record<string, string>;
  moveMetadata: Record<string, MoveMetadata>;
  itemNames: Record<string, string>;
  itemMetadata: Record<string, ItemMetadata>;
  abilityNames: Record<string, string>;
  frontierValues: Record<string, number>;
  speciesLearnsets: Record<string, Set<string>>;
  speciesMoveSources: Record<string, Record<string, Set<string>>>;
  usageCounts: Record<string, number>;
  factoryUsage: Set<string>;
};

export type ParsedWorkspace = {
  files: Record<string, RepoFile>;
  frontierMons: FrontierMon[];
  reference: ReferenceData;
  baseCommitSha: string;
};

export type PendingChange = {
  path: string;
  before: string;
  after: string;
};

export type PullRequestResult = {
  number: number;
  htmlUrl: string;
  branch: string;
};
