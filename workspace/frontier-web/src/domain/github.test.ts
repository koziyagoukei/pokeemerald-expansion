import { describe, expect, it, vi } from "vitest";
import { GitHubClient } from "./github";

describe("GitHubClient", () => {
  it("stops PR creation when the base branch changed", async () => {
    const client = new GitHubClient({ owner: "o", repo: "r", branch: "main", token: "token" });
    const fetchMock = vi.fn(async () => new Response(JSON.stringify({ object: { sha: "new", type: "commit" } }), { status: 200 }));
    vi.stubGlobal("fetch", fetchMock);
    await expect(client.createPullRequest([{ path: "a.h", before: "a", after: "b" }], "old", "title", "body")).rejects.toThrow("更新されています");
    expect(fetchMock).toHaveBeenCalledTimes(1);
  });
});
