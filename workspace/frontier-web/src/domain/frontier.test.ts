import { describe, expect, it } from "vitest";
import {
  addFrontierDefine,
  addFrontierMon,
  assertNoConcatenatedDesignators,
  defaultFrontierValues,
  frontierMonBlockFromValues,
  hiddenPowerTypeFromIvs,
  parseDefineValues,
  parseIndexedRecords,
  replaceFrontierMon,
} from "./frontier";
import { FRONTIER_MON_FIELDS } from "./labels";

const sampleMons = `const struct TrainerMon gBattleFrontierMons[NUM_FRONTIER_MONS] =
{
    [FRONTIER_MON_TEST] = {
        .species = SPECIES_PIKACHU,
        .moves = {MOVE_THUNDERBOLT, MOVE_HIDDEN_POWER, MOVE_NONE, MOVE_NONE},
        .heldItem = ITEM_LIGHT_BALL,
        .iv = TRAINER_PARTY_IVS(31, 30, 31, 30, 31, 30),
        .ability = ABILITY_STATIC,
        .nickname = J_COMPOUND_STRING("ピカ"),
        .ball = BALL_POKE,
    },
};
`;

describe("frontier parser", () => {
  it("parses gBattleFrontierMons records and nickname", () => {
    const records = parseIndexedRecords(sampleMons, "mons.h", "FRONTIER_MON_", FRONTIER_MON_FIELDS);
    expect(records).toHaveLength(1);
    expect(records[0].key).toBe("FRONTIER_MON_TEST");
    const values = defaultFrontierValues(records[0]);
    expect(values.nickname).toBe("ピカ");
    expect(values.species).toBe("SPECIES_PIKACHU");
    expect(values.moves[1]).toBe("MOVE_HIDDEN_POWER");
    expect(values.iv).toEqual([31, 30, 31, 30, 31, 30]);
  });

  it("writes nickname through J_COMPOUND_STRING and keeps field commas", () => {
    const records = parseIndexedRecords(sampleMons, "mons.h", "FRONTIER_MON_", FRONTIER_MON_FIELDS);
    const values = defaultFrontierValues(records[0]);
    values.nickname = "ライ";
    values.isShiny = true;
    const block = frontierMonBlockFromValues(records[0].key, values);
    expect(block).toContain('.nickname = J_COMPOUND_STRING("ライ"),');
    expect(block).toContain(".isShiny = TRUE,");
    expect(() => assertNoConcatenatedDesignators(block)).not.toThrow();
  });

  it("replaces only the selected record range", () => {
    const records = parseIndexedRecords(sampleMons, "mons.h", "FRONTIER_MON_", FRONTIER_MON_FIELDS);
    const values = defaultFrontierValues(records[0]);
    values.ball = "BALL_DUSK";
    const updated = replaceFrontierMon(sampleMons, { record: records[0], values, usageCount: 0, factoryUsage: false });
    expect(updated).toContain(".ball = BALL_DUSK,");
    expect(updated).toContain("const struct TrainerMon");
  });

  it("adds a new mon and define", () => {
    const values = defaultFrontierValues(parseIndexedRecords(sampleMons, "mons.h", "FRONTIER_MON_", FRONTIER_MON_FIELDS)[0]);
    const updatedMons = addFrontierMon(sampleMons, "FRONTIER_MON_NEW", values);
    expect(updatedMons).toContain("[FRONTIER_MON_NEW]");
    const constants = "#define FRONTIER_MON_TEST            0\n#define NUM_FRONTIER_MONS 1\n";
    const updatedConstants = addFrontierDefine(constants, "FRONTIER_MON_NEW", 1);
    expect(updatedConstants).toContain("#define FRONTIER_MON_NEW");
    expect(parseDefineValues(updatedConstants, ["FRONTIER_MON_", "NUM_FRONTIER_MONS"]).NUM_FRONTIER_MONS).toBe(2);
  });

  it("rejects concatenated designated initializers", () => {
    expect(() => assertNoConcatenatedDesignators(".ball = BALL_POKE        .iv = TRAINER_PARTY_IVS(1, 1, 1, 1, 1, 1),")).toThrow();
  });

  it("calculates Hidden Power type", () => {
    expect(hiddenPowerTypeFromIvs([31, 31, 31, 31, 31, 31])).toBe("TYPE_DARK");
  });
});
