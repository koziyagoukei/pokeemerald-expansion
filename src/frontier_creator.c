#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_message_box.h"
#include "field_screen_effect.h"
#include "main.h"
#include "malloc.h"
#include "map_name_popup.h"
#include "menu.h"
#include "m4a.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "constants/abilities.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/songs.h"
#include "constants/species.h"

#define FRONTIER_CREATOR_FONT FONT_NORMAL

#define FRONTIER_CREATOR_LEVEL_50  50
#define FRONTIER_CREATOR_LEVEL_100 100
#define FRONTIER_CREATOR_DYNAMAX_LEVEL 10
#define FRONTIER_CREATOR_MAX_TOTAL_EVS 510
#define FRONTIER_CREATOR_MAX_PER_STAT_EVS 252
#define FRONTIER_CREATOR_MAX_IVS 31

#define FRONTIER_CREATOR_SPECIES_DIGITS 4
#define FRONTIER_CREATOR_NATURE_DIGITS  2
#define FRONTIER_CREATOR_ABILITY_DIGITS 1
#define FRONTIER_CREATOR_TYPE_DIGITS    2
#define FRONTIER_CREATOR_IV_DIGITS      2
#define FRONTIER_CREATOR_EV_DIGITS      3

struct FrontierCreatorData
{
    enum Species species;
    u8 level;
    u8 nature;
    u8 abilityNum;
    u8 teraType;
    bool8 gmaxFactor;
    u8 ivs[NUM_STATS];
    u8 evs[NUM_STATS];
};

static EWRAM_DATA struct FrontierCreatorData *sFrontierCreatorData = NULL;

#define tWindowId data[0]
#define tInput    data[1]
#define tDigit    data[2]
#define tStatId   data[3]

static void Task_FrontierCreator_SelectSpecies(u8 taskId);
static void Task_FrontierCreator_SelectLevel(u8 taskId);
static void Task_FrontierCreator_SelectNature(u8 taskId);
static void Task_FrontierCreator_SelectAbility(u8 taskId);
static void Task_FrontierCreator_SelectTeraType(u8 taskId);
static void Task_FrontierCreator_SelectGmax(u8 taskId);
static void Task_FrontierCreator_SelectIVs(u8 taskId);
static void Task_FrontierCreator_SelectEVs(u8 taskId);

static void FrontierCreator_DestroyAndReturn(u8 taskId);
static void FrontierCreator_CreateMonAndGive(void);
static void FrontierCreator_InitDefaultData(void);

static const struct WindowTemplate sFrontierCreatorWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 28,
    .height = 8,
    .paletteNum = 15,
    .baseBlock = 1,
};

static const s32 sFrontierCreatorPowersOfTen[] =
{
    1,
    10,
    100,
    1000,
    10000,
};

static const u8 *const sFrontierCreatorStatNames[NUM_STATS] =
{
    COMPOUND_STRING("HP"),
    COMPOUND_STRING("こうげき"),
    COMPOUND_STRING("ぼうぎょ"),
    COMPOUND_STRING("すばやさ"),
    COMPOUND_STRING("とくこう"),
    COMPOUND_STRING("とくぼう"),
};

static const u8 *const sFrontierCreatorNatureNames[NUM_NATURES + 1] =
{
    COMPOUND_STRING("がんばりや"),
    COMPOUND_STRING("さみしがり"),
    COMPOUND_STRING("ゆうかん"),
    COMPOUND_STRING("いじっぱり"),
    COMPOUND_STRING("やんちゃ"),
    COMPOUND_STRING("ずぶとい"),
    COMPOUND_STRING("すなお"),
    COMPOUND_STRING("のんき"),
    COMPOUND_STRING("わんぱく"),
    COMPOUND_STRING("のうてんき"),
    COMPOUND_STRING("おくびょう"),
    COMPOUND_STRING("せっかち"),
    COMPOUND_STRING("まじめ"),
    COMPOUND_STRING("ようき"),
    COMPOUND_STRING("むじゃき"),
    COMPOUND_STRING("ひかえめ"),
    COMPOUND_STRING("おっとり"),
    COMPOUND_STRING("れいせい"),
    COMPOUND_STRING("てれや"),
    COMPOUND_STRING("うっかりや"),
    COMPOUND_STRING("おだやか"),
    COMPOUND_STRING("おとなしい"),
    COMPOUND_STRING("なまいき"),
    COMPOUND_STRING("しんちょう"),
    COMPOUND_STRING("きまぐれ"),
    COMPOUND_STRING("ランダム"),
};

static const u8 sFrontierCreatorText_Controls[] = _("{CLEAR_TO 90}\n{A_BUTTON}けってい {B_BUTTON}やめる");

static const u16 sFrontierCreatorGmaxCapableSpecies[] =
{
    SPECIES_VENUSAUR,
    SPECIES_CHARIZARD,
    SPECIES_BLASTOISE,
    SPECIES_BUTTERFREE,
    SPECIES_PIKACHU,
    SPECIES_MEOWTH,
    SPECIES_MACHAMP,
    SPECIES_GENGAR,
    SPECIES_KINGLER,
    SPECIES_LAPRAS,
    SPECIES_EEVEE,
    SPECIES_SNORLAX,
    SPECIES_GARBODOR,
    SPECIES_MELMETAL,
    SPECIES_RILLABOOM,
    SPECIES_CINDERACE,
    SPECIES_INTELEON,
    SPECIES_CORVIKNIGHT,
    SPECIES_ORBEETLE,
    SPECIES_DREDNAW,
    SPECIES_COALOSSAL,
    SPECIES_FLAPPLE,
    SPECIES_APPLETUN,
    SPECIES_SANDACONDA,
    SPECIES_CENTISKORCH,
    SPECIES_HATTERENE,
    SPECIES_GRIMMSNARL,
    SPECIES_ALCREMIE,
    SPECIES_COPPERAJAH,
    SPECIES_DURALUDON,
    SPECIES_NONE,
};

static bool32 FrontierCreator_CanGmax(enum Species species)
{
    u32 i;

    for (i = 0; sFrontierCreatorGmaxCapableSpecies[i] != SPECIES_NONE; i++)
    {
        if (sFrontierCreatorGmaxCapableSpecies[i] == species)
            return TRUE;
    }

    return FALSE;
}

static bool32 FrontierCreator_IsBannedSpecies(enum Species species)
{
    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return TRUE;

    if (!IsSpeciesEnabled(species))
        return TRUE;

    if (species >= SPECIES_VENUSAUR_MEGA && species <= SPECIES_GROUDON_PRIMAL)
        return TRUE;

    if (species >= SPECIES_VENUSAUR_GMAX && species <= SPECIES_URSHIFU_RAPID_STRIKE_GMAX)
        return TRUE;

    if (species >= SPECIES_CLEFABLE_MEGA && species <= SPECIES_RAICHU_MEGA_Y)
        return TRUE;

    return FALSE;
}

static void FrontierCreator_ClearWindow(u8 windowId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
}

static void FrontierCreator_PrintWindow(u8 windowId)
{
    AddTextPrinterParameterized(windowId, FRONTIER_CREATOR_FONT, gStringVar4, 0, 0, 0, NULL);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void FrontierCreator_DrawNatureScreen(u8 taskId)
{
    u8 windowId = gTasks[taskId].tWindowId;
    u8 nature = gTasks[taskId].tInput;

    if (nature > NUM_NATURES)
        nature = NUM_NATURES;

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("せいかくを えらぶ{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("No."));
    ConvertIntToDecimalStringN(gStringVar1, nature, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("せいかく: "));
    StringAppend(gStringVar4, sFrontierCreatorNatureNames[nature]);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("けた: "));
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tDigit, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}へんこう {LEFT_ARROW}{RIGHT_ARROW}けた"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierCreator_DrawNumberScreen(u8 windowId, const u8 *title, u32 value, u32 digit, u32 max)
{
    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, title);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("あたい: "));
    ConvertIntToDecimalStringN(gStringVar1, value, STR_CONV_MODE_LEADING_ZEROS, 4);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("さいだい: "));
    ConvertIntToDecimalStringN(gStringVar1, max, STR_CONV_MODE_LEADING_ZEROS, 4);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("けた: "));
    ConvertIntToDecimalStringN(gStringVar1, digit, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}へんこう {LEFT_ARROW}{RIGHT_ARROW}けた"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierCreator_DrawBooleanScreen(u8 windowId, const u8 *title, bool32 value)
{
    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, title);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("せってい: "));
    if (value)
        StringAppend(gStringVar4, COMPOUND_STRING("オン"));
    else
        StringAppend(gStringVar4, COMPOUND_STRING("オフ"));

    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));
    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}きりかえ"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierCreator_HandleNumericInput(u8 taskId, s32 min, s32 max, u8 digits)
{
    s32 value = gTasks[taskId].tInput;

    if (JOY_NEW(DPAD_UP))
    {
        value += sFrontierCreatorPowersOfTen[gTasks[taskId].tDigit];
        if (value > max)
            value = max;
    }

    if (JOY_NEW(DPAD_DOWN))
    {
        value -= sFrontierCreatorPowersOfTen[gTasks[taskId].tDigit];
        if (value < min)
            value = min;
    }

    if (JOY_NEW(DPAD_LEFT))
    {
        if (gTasks[taskId].tDigit > 0)
            gTasks[taskId].tDigit--;
    }

    if (JOY_NEW(DPAD_RIGHT))
    {
        if (gTasks[taskId].tDigit < digits - 1)
            gTasks[taskId].tDigit++;
    }

    gTasks[taskId].tInput = value;
}

static u16 FrontierCreator_GetEVTotalExcept(u8 statId)
{
    u16 total = 0;
    u32 i;

    for (i = 0; i < NUM_STATS; i++)
    {
        if (i != statId)
            total += sFrontierCreatorData->evs[i];
    }

    return total;
}

static u16 FrontierCreator_GetMaxEVForStat(u8 statId)
{
    u16 totalExcept = FrontierCreator_GetEVTotalExcept(statId);
    u16 remaining;

    if (totalExcept >= FRONTIER_CREATOR_MAX_TOTAL_EVS)
        return 0;

    remaining = FRONTIER_CREATOR_MAX_TOTAL_EVS - totalExcept;

    if (remaining > FRONTIER_CREATOR_MAX_PER_STAT_EVS)
        return FRONTIER_CREATOR_MAX_PER_STAT_EVS;

    return remaining;
}

static void FrontierCreator_DrawSpeciesScreen(u8 taskId)
{
    u8 windowId = gTasks[taskId].tWindowId;
    enum Species species = gTasks[taskId].tInput;

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("しゅるいを えらぶ{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("No."));
    ConvertIntToDecimalStringN(gStringVar1, species, STR_CONV_MODE_LEADING_ZEROS, 4);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    if (!FrontierCreator_IsBannedSpecies(species))
    {
        StringAppend(gStringVar4, COMPOUND_STRING("なまえ: "));
        StringAppend(gStringVar4, GetSpeciesName(species));
        StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));
    }
    else
    {
        StringAppend(gStringVar4, COMPOUND_STRING("なまえ: ----{CLEAR_TO 90}\n"));
    }

    if (FrontierCreator_IsBannedSpecies(species))
        StringAppend(gStringVar4, COMPOUND_STRING("このしゅるいは つくれません{CLEAR_TO 90}\n"));
    else
        StringAppend(gStringVar4, COMPOUND_STRING("このしゅるいは OK{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("けた: 10 "));
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tDigit, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}へんこう {LEFT_ARROW}{RIGHT_ARROW}けた"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierCreator_DrawLevelScreen(u8 taskId)
{
    u8 windowId = gTasks[taskId].tWindowId;

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("レベルを えらぶ{CLEAR_TO 90}\n"));
    StringAppend(gStringVar4, COMPOUND_STRING("レベル: "));

    ConvertIntToDecimalStringN(gStringVar1, sFrontierCreatorData->level, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW} 50/100"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierCreator_DrawIVScreen(u8 taskId)
{
    u8 statId = gTasks[taskId].tStatId;
    u8 windowId = gTasks[taskId].tWindowId;

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("こたいちを えらぶ{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, sFrontierCreatorStatNames[statId]);
    StringAppend(gStringVar4, COMPOUND_STRING(": "));

    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING(" / 31{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("けた: 10 "));
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tDigit, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}へんこう {LEFT_ARROW}{RIGHT_ARROW}けた"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierCreator_DrawEVScreen(u8 taskId)
{
    u8 statId = gTasks[taskId].tStatId;
    u8 windowId = gTasks[taskId].tWindowId;
    u16 max = FrontierCreator_GetMaxEVForStat(statId);
    u16 totalExcept = FrontierCreator_GetEVTotalExcept(statId);

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("きそポイントを えらぶ{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, sFrontierCreatorStatNames[statId]);
    StringAppend(gStringVar4, COMPOUND_STRING(": "));

    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar1);

    StringAppend(gStringVar4, COMPOUND_STRING(" / "));
    ConvertIntToDecimalStringN(gStringVar1, max, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("ほかのごうけい: "));
    ConvertIntToDecimalStringN(gStringVar1, totalExcept, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("ごうけい: "));
    ConvertIntToDecimalStringN(gStringVar1, totalExcept + gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING(" / 510{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}へんこう {LEFT_ARROW}{RIGHT_ARROW}けた"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierCreator_InitDefaultData(void)
{
    u32 i;

    sFrontierCreatorData->species = SPECIES_BULBASAUR;
    sFrontierCreatorData->level = FRONTIER_CREATOR_LEVEL_50;
    sFrontierCreatorData->nature = NATURE_RANDOM;
    sFrontierCreatorData->abilityNum = 0;
    sFrontierCreatorData->teraType = TYPE_NORMAL;
    sFrontierCreatorData->gmaxFactor = FALSE;

    for (i = 0; i < NUM_STATS; i++)
    {
        sFrontierCreatorData->ivs[i] = FRONTIER_CREATOR_MAX_IVS;
        sFrontierCreatorData->evs[i] = 0;
    }
}

static void FrontierCreator_CreateMonAndGive(void)
{
    struct Pokemon mon;
    u32 personality;
    u8 dmaxLevel = FRONTIER_CREATOR_DYNAMAX_LEVEL;
    u8 gmaxFactor = sFrontierCreatorData->gmaxFactor;
    u32 teraType = sFrontierCreatorData->teraType;
    u8 abilityNum = sFrontierCreatorData->abilityNum;
    u8 isShiny = FALSE;
    u32 i;
    u8 ivVal;
    u8 evVal;

    if (!FrontierCreator_CanGmax(sFrontierCreatorData->species))
        gmaxFactor = FALSE;

    personality = GetMonPersonality(
        sFrontierCreatorData->species,
        MON_GENDER_RANDOM,
        sFrontierCreatorData->nature,
        RANDOM_UNOWN_LETTER
    );

    CreateMon(
        &mon,
        sFrontierCreatorData->species,
        sFrontierCreatorData->level,
        personality,
        OTID_STRUCT_PLAYER_ID
    );

    SetMonData(&mon, MON_DATA_IS_SHINY, &isShiny);
    SetMonData(&mon, MON_DATA_GIGANTAMAX_FACTOR, &gmaxFactor);
    SetMonData(&mon, MON_DATA_DYNAMAX_LEVEL, &dmaxLevel);

    if (teraType == TYPE_NONE || teraType == TYPE_MYSTERY || teraType >= NUMBER_OF_MON_TYPES)
        teraType = GetTeraTypeFromPersonality(&mon);

    SetMonData(&mon, MON_DATA_TERA_TYPE, &teraType);

    for (i = 0; i < NUM_STATS; i++)
    {
        ivVal = sFrontierCreatorData->ivs[i];
        SetMonData(&mon, MON_DATA_HP_IV + i, &ivVal);
    }

    for (i = 0; i < NUM_STATS; i++)
    {
        evVal = sFrontierCreatorData->evs[i];
        SetMonData(&mon, MON_DATA_HP_EV + i, &evVal);
    }

    GiveMonInitialMoveset(&mon);

    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);

    CalculateMonStats(&mon);

    GiveScriptedMonToPlayer(&mon, PARTY_SIZE);
    FlagSet(FLAG_SYS_POKEMON_GET);

    gSpecialVar_Result = TRUE;
}

static void FrontierCreator_DestroyAndReturn(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    DestroyTask(taskId);

    if (sFrontierCreatorData != NULL)
    {
        Free(sFrontierCreatorData);
        sFrontierCreatorData = NULL;
    }

    ScriptContext_Enable();
    UnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

static void Task_FrontierCreator_SelectSpecies(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        FrontierCreator_HandleNumericInput(taskId, 1, NUM_SPECIES - 1, FRONTIER_CREATOR_SPECIES_DIGITS);
        FrontierCreator_DrawSpeciesScreen(taskId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        if (FrontierCreator_IsBannedSpecies(gTasks[taskId].tInput))
        {
            PlaySE(SE_PC_OFF);
            return;
        }

        PlaySE(SE_SELECT);
        sFrontierCreatorData->species = gTasks[taskId].tInput;

        FrontierCreator_DrawLevelScreen(taskId);
        gTasks[taskId].func = Task_FrontierCreator_SelectLevel;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

static void Task_FrontierCreator_SelectLevel(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (sFrontierCreatorData->level == FRONTIER_CREATOR_LEVEL_50)
            sFrontierCreatorData->level = FRONTIER_CREATOR_LEVEL_100;
        else
            sFrontierCreatorData->level = FRONTIER_CREATOR_LEVEL_50;

        FrontierCreator_DrawLevelScreen(taskId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].tInput = NUM_NATURES;
        gTasks[taskId].tDigit = 0;

        FrontierCreator_DrawNatureScreen(taskId);
        gTasks[taskId].func = Task_FrontierCreator_SelectNature;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

static void Task_FrontierCreator_SelectNature(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        FrontierCreator_HandleNumericInput(taskId, 0, NUM_NATURES, FRONTIER_CREATOR_NATURE_DIGITS);
        FrontierCreator_DrawNatureScreen(taskId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);

        if (gTasks[taskId].tInput >= NUM_NATURES)
            sFrontierCreatorData->nature = NATURE_RANDOM;
        else
            sFrontierCreatorData->nature = gTasks[taskId].tInput;

        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;

        FrontierCreator_DrawNumberScreen(
            gTasks[taskId].tWindowId,
            COMPOUND_STRING("とくせいスロットを えらぶ"),
            gTasks[taskId].tInput,
            gTasks[taskId].tDigit,
            NUM_ABILITY_SLOTS - 1
        );
        gTasks[taskId].func = Task_FrontierCreator_SelectAbility;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

static void Task_FrontierCreator_SelectAbility(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        FrontierCreator_HandleNumericInput(taskId, 0, NUM_ABILITY_SLOTS - 1, FRONTIER_CREATOR_ABILITY_DIGITS);
        FrontierCreator_DrawNumberScreen(
            gTasks[taskId].tWindowId,
            COMPOUND_STRING("とくせいスロットを えらぶ"),
            gTasks[taskId].tInput,
            gTasks[taskId].tDigit,
            NUM_ABILITY_SLOTS - 1
        );
    }

    if (JOY_NEW(A_BUTTON))
    {
        if (GetSpeciesAbility(sFrontierCreatorData->species, gTasks[taskId].tInput) == ABILITY_NONE)
        {
            PlaySE(SE_PC_OFF);
            return;
        }

        PlaySE(SE_SELECT);
        sFrontierCreatorData->abilityNum = gTasks[taskId].tInput;
        gTasks[taskId].tInput = TYPE_NORMAL;
        gTasks[taskId].tDigit = 0;

        FrontierCreator_DrawNumberScreen(
            gTasks[taskId].tWindowId,
            COMPOUND_STRING("テラスタイプを えらぶ"),
            gTasks[taskId].tInput,
            gTasks[taskId].tDigit,
            NUMBER_OF_MON_TYPES - 1
        );
        gTasks[taskId].func = Task_FrontierCreator_SelectTeraType;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

static void Task_FrontierCreator_SelectTeraType(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        FrontierCreator_HandleNumericInput(taskId, 0, NUMBER_OF_MON_TYPES - 1, FRONTIER_CREATOR_TYPE_DIGITS);
        FrontierCreator_DrawNumberScreen(
            gTasks[taskId].tWindowId,
            COMPOUND_STRING("テラスタイプを えらぶ"),
            gTasks[taskId].tInput,
            gTasks[taskId].tDigit,
            NUMBER_OF_MON_TYPES - 1
        );
    }

    if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tInput == TYPE_NONE || gTasks[taskId].tInput == TYPE_MYSTERY)
        {
            PlaySE(SE_PC_OFF);
            return;
        }

        PlaySE(SE_SELECT);
        sFrontierCreatorData->teraType = gTasks[taskId].tInput;

        if (FrontierCreator_CanGmax(sFrontierCreatorData->species))
        {
            gTasks[taskId].tInput = FALSE;
            FrontierCreator_DrawBooleanScreen(
                gTasks[taskId].tWindowId,
                COMPOUND_STRING("キョダイマックス"),
                gTasks[taskId].tInput
            );
            gTasks[taskId].func = Task_FrontierCreator_SelectGmax;
        }
        else
        {
            sFrontierCreatorData->gmaxFactor = FALSE;
            gTasks[taskId].tStatId = 0;
            gTasks[taskId].tInput = FRONTIER_CREATOR_MAX_IVS;
            gTasks[taskId].tDigit = 0;
            FrontierCreator_DrawIVScreen(taskId);
            gTasks[taskId].func = Task_FrontierCreator_SelectIVs;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

static void Task_FrontierCreator_SelectGmax(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].tInput = !gTasks[taskId].tInput;
        FrontierCreator_DrawBooleanScreen(
            gTasks[taskId].tWindowId,
            COMPOUND_STRING("キョダイマックス"),
            gTasks[taskId].tInput
        );
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sFrontierCreatorData->gmaxFactor = gTasks[taskId].tInput;

        gTasks[taskId].tStatId = 0;
        gTasks[taskId].tInput = FRONTIER_CREATOR_MAX_IVS;
        gTasks[taskId].tDigit = 0;
        FrontierCreator_DrawIVScreen(taskId);
        gTasks[taskId].func = Task_FrontierCreator_SelectIVs;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

static void Task_FrontierCreator_SelectIVs(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        FrontierCreator_HandleNumericInput(taskId, 0, FRONTIER_CREATOR_MAX_IVS, FRONTIER_CREATOR_IV_DIGITS);
        FrontierCreator_DrawIVScreen(taskId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sFrontierCreatorData->ivs[gTasks[taskId].tStatId] = gTasks[taskId].tInput;

        if (gTasks[taskId].tStatId < NUM_STATS - 1)
        {
            gTasks[taskId].tStatId++;
            gTasks[taskId].tInput = FRONTIER_CREATOR_MAX_IVS;
            gTasks[taskId].tDigit = 0;
            FrontierCreator_DrawIVScreen(taskId);
        }
        else
        {
            gTasks[taskId].tStatId = 0;
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;
            FrontierCreator_DrawEVScreen(taskId);
            gTasks[taskId].func = Task_FrontierCreator_SelectEVs;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

static void Task_FrontierCreator_SelectEVs(u8 taskId)
{
    u16 max = FrontierCreator_GetMaxEVForStat(gTasks[taskId].tStatId);

    if (gTasks[taskId].tInput > max)
        gTasks[taskId].tInput = max;

    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        FrontierCreator_HandleNumericInput(taskId, 0, max, FRONTIER_CREATOR_EV_DIGITS);
        FrontierCreator_DrawEVScreen(taskId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sFrontierCreatorData->evs[gTasks[taskId].tStatId] = gTasks[taskId].tInput;

        if (gTasks[taskId].tStatId < NUM_STATS - 1)
        {
            gTasks[taskId].tStatId++;
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;
            FrontierCreator_DrawEVScreen(taskId);
        }
        else
        {
            FrontierCreator_CreateMonAndGive();
            PlaySE(MUS_LEVEL_UP);
            FrontierCreator_DestroyAndReturn(taskId);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierCreator_DestroyAndReturn(taskId);
    }
}

void Special_OpenFrontierPokemonCreator(void)
{
    u8 taskId;
    u8 windowId;

    if (sFrontierCreatorData != NULL)
    {
        gSpecialVar_Result = FALSE;
        return;
    }

    sFrontierCreatorData = AllocZeroed(sizeof(*sFrontierCreatorData));
    if (sFrontierCreatorData == NULL)
    {
        gSpecialVar_Result = FALSE;
        return;
    }

    FrontierCreator_InitDefaultData();

    LockPlayerFieldControls();
    FreezeObjectEvents();
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();

    windowId = AddWindow(&sFrontierCreatorWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    taskId = CreateTask(Task_FrontierCreator_SelectSpecies, 3);
    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tInput = SPECIES_BULBASAUR;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tStatId = 0;

    FrontierCreator_DrawSpeciesScreen(taskId);
}

#undef tWindowId
#undef tInput
#undef tDigit
#undef tStatId