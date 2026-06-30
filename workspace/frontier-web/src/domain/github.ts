import { OPTIONAL_PATHS, REQUIRED_PATHS } from "./labels";
import { decodeBase64Content, encodeBase64Content } from "./frontier";
import type { PendingChange, PullRequestResult, RepoFile, RepoSettings } from "./types";

const API_ROOT = "https://api.github.com";

type GitHubContentResponse = {
  path: string;
  sha: string;
  type: string;
};

type GitHubBlobResponse = {
  sha: string;
  encoding: "base64" | "utf-8";
  content: string;
};

type GitHubTreeItem = {
  path: string;
  type: "blob" | "tree" | "commit";
  sha: string;
  size?: number;
};

type GitHubRefResponse = {
  object: {
    sha: string;
    type: string;
  };
};

type GitHubCommitResponse = {
  sha: string;
  tree: {
    sha: string;
  };
};

type GitHubPullResponse = {
  number: number;
  html_url: string;
};

export class GitHubClient {
  readonly settings: RepoSettings;

  constructor(settings: RepoSettings) {
    this.settings = settings;
  }

  private repoPath(path: string): string {
    const clean = path.replace(/^\/+/, "");
    return `${API_ROOT}/repos/${this.settings.owner}/${this.settings.repo}/${clean}`;
  }

  private async request<T>(url: string, init: RequestInit = {}): Promise<T> {
    const headers = new Headers(init.headers);
    headers.set("Accept", "application/vnd.github+json");
    headers.set("X-GitHub-Api-Version", "2022-11-28");
    if (this.settings.token.trim()) headers.set("Authorization", `Bearer ${this.settings.token.trim()}`);
    const response = await fetch(url, { ...init, headers });
    if (!response.ok) {
      const body = await response.text();
      throw new Error(`GitHub API ${response.status}: ${body || response.statusText}`);
    }
    return response.json() as Promise<T>;
  }

  async getBranchHead(branch = this.settings.branch): Promise<string> {
    const ref = await this.request<GitHubRefResponse>(this.repoPath(`git/ref/heads/${encodeURIComponent(branch)}`));
    return ref.object.sha;
  }

  async getTreePaths(branch = this.settings.branch): Promise<GitHubTreeItem[]> {
    const data = await this.request<{ tree: GitHubTreeItem[] }>(this.repoPath(`git/trees/${encodeURIComponent(branch)}?recursive=1`));
    return data.tree.filter((entry) => entry.type === "blob");
  }

  async readFile(path: string, branch = this.settings.branch): Promise<RepoFile> {
    const encodedPath = path.split("/").map(encodeURIComponent).join("/");
    const info = await this.request<GitHubContentResponse>(this.repoPath(`contents/${encodedPath}?ref=${encodeURIComponent(branch)}`));
    if (info.type !== "file") throw new Error(`${path} is not a file.`);
    const blob = await this.request<GitHubBlobResponse>(this.repoPath(`git/blobs/${info.sha}`));
    const content = blob.encoding === "base64" ? decodeBase64Content(blob.content) : blob.content;
    return { path, sha: info.sha, content };
  }

  async readWorkspaceFiles(onProgress?: (message: string) => void): Promise<{ files: Record<string, RepoFile>; baseCommitSha: string }> {
    const baseCommitSha = await this.getBranchHead();
    onProgress?.(`base commit: ${baseCommitSha.slice(0, 12)}`);
    const tree = await this.getTreePaths();
    const speciesPaths = tree
      .map((entry) => entry.path)
      .filter((path) => path === "src/data/pokemon/species_info.h" || /^src\/data\/pokemon\/species_info\/.*\.h$/.test(path));
    const wanted = [...REQUIRED_PATHS, ...OPTIONAL_PATHS, ...speciesPaths];
    const unique = [...new Set(wanted)];
    const files: Record<string, RepoFile> = {};
    for (const path of unique) {
      const exists = tree.some((entry) => entry.path === path);
      if (!exists && OPTIONAL_PATHS.includes(path)) continue;
      onProgress?.(`loading ${path}`);
      files[path] = await this.readFile(path);
    }
    return { files, baseCommitSha };
  }

  async createPullRequest(changes: PendingChange[], baseCommitSha: string, title: string, body: string): Promise<PullRequestResult> {
    if (!changes.length) throw new Error("変更がありません。");
    const currentHead = await this.getBranchHead();
    if (currentHead !== baseCommitSha) {
      throw new Error(`base branch が読み込み後に更新されています。再読み込みしてください。old=${baseCommitSha.slice(0, 12)} current=${currentHead.slice(0, 12)}`);
    }
    const baseCommit = await this.request<GitHubCommitResponse>(this.repoPath(`git/commits/${baseCommitSha}`));
    const treeEntries = [];
    for (const change of changes) {
      const blob = await this.request<{ sha: string }>(this.repoPath("git/blobs"), {
        method: "POST",
        body: JSON.stringify({
          content: encodeBase64Content(change.after),
          encoding: "base64",
        }),
      });
      treeEntries.push({
        path: change.path,
        mode: "100644",
        type: "blob",
        sha: blob.sha,
      });
    }
    const tree = await this.request<{ sha: string }>(this.repoPath("git/trees"), {
      method: "POST",
      body: JSON.stringify({
        base_tree: baseCommit.tree.sha,
        tree: treeEntries,
      }),
    });
    const commit = await this.request<{ sha: string }>(this.repoPath("git/commits"), {
      method: "POST",
      body: JSON.stringify({
        message: title,
        tree: tree.sha,
        parents: [baseCommitSha],
      }),
    });
    const branch = `frontier-editor/${timestamp()}`;
    await this.request(this.repoPath("git/refs"), {
      method: "POST",
      body: JSON.stringify({
        ref: `refs/heads/${branch}`,
        sha: commit.sha,
      }),
    });
    const pull = await this.request<GitHubPullResponse>(this.repoPath("pulls"), {
      method: "POST",
      body: JSON.stringify({
        title,
        head: branch,
        base: this.settings.branch,
        body,
      }),
    });
    return { number: pull.number, htmlUrl: pull.html_url, branch };
  }
}

function timestamp(): string {
  const now = new Date();
  const pad = (value: number) => String(value).padStart(2, "0");
  return `${now.getFullYear()}${pad(now.getMonth() + 1)}${pad(now.getDate())}-${pad(now.getHours())}${pad(now.getMinutes())}${pad(now.getSeconds())}`;
}
