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
#include "constants/vars.h"
#include "data.h"
#include "item.h"
#include "pokedex.h"

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

#define tItemIndex data[1]

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

struct FrontierHubAutoItem
{
    enum Item itemId;
    u16 quantity;
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
static void FrontierCreator_DrawAbilityScreen(u8 taskId);

static void Task_FrontierItemGiver_SelectItem(u8 taskId);
static void FrontierItemGiver_DrawScreen(u8 taskId);
static u16 FrontierItemGiver_GetItemCount(void);
static void FrontierItemGiver_DestroyAndReturn(u8 taskId);
static void FrontierHub_GiveAutoItems(void);

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

static const u8 *const sFrontierCreatorTypeNames[NUMBER_OF_MON_TYPES] =
{
    [TYPE_NONE] = COMPOUND_STRING("----"),
    [TYPE_NORMAL] = COMPOUND_STRING("ノーマル"),
    [TYPE_FIGHTING] = COMPOUND_STRING("かくとう"),
    [TYPE_FLYING] = COMPOUND_STRING("ひこう"),
    [TYPE_POISON] = COMPOUND_STRING("どく"),
    [TYPE_GROUND] = COMPOUND_STRING("じめん"),
    [TYPE_ROCK] = COMPOUND_STRING("いわ"),
    [TYPE_BUG] = COMPOUND_STRING("むし"),
    [TYPE_GHOST] = COMPOUND_STRING("ゴースト"),
    [TYPE_STEEL] = COMPOUND_STRING("はがね"),
    [TYPE_MYSTERY] = COMPOUND_STRING("----"),
    [TYPE_FIRE] = COMPOUND_STRING("ほのお"),
    [TYPE_WATER] = COMPOUND_STRING("みず"),
    [TYPE_GRASS] = COMPOUND_STRING("くさ"),
    [TYPE_ELECTRIC] = COMPOUND_STRING("でんき"),
    [TYPE_PSYCHIC] = COMPOUND_STRING("エスパー"),
    [TYPE_ICE] = COMPOUND_STRING("こおり"),
    [TYPE_DRAGON] = COMPOUND_STRING("ドラゴン"),
    [TYPE_DARK] = COMPOUND_STRING("あく"),
    [TYPE_FAIRY] = COMPOUND_STRING("フェアリー"),
    [TYPE_STELLAR] = COMPOUND_STRING("ステラ"),
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

static const struct FrontierHubAutoItem sFrontierHubAutoItems[] =
{
    { ITEM_HEAT_ROCK, 999 }, // あついいわ
    { ITEM_HEAVY_DUTY_BOOTS, 999 }, // あつぞこブーツ
    { ITEM_LIFE_ORB, 999 }, // いのちのたま
    { ITEM_ELECTRIC_SEED, 999 }, // エレキシード
    { ITEM_BIG_ROOT, 999 }, // おおきなねっこ
    { ITEM_SHELL_BELL, 999 }, // かいがらのすず
    { ITEM_FLAME_ORB, 999 }, // かえんだま
    { ITEM_BLUNDER_POLICY, 999 }, // からぶりほけん
    { ITEM_FOCUS_SASH, 999 }, // きあいのタスキ
    { ITEM_GRASSY_SEED, 999 }, // グラスシード
    { ITEM_TERRAIN_EXTENDER, 999 }, // グランドコート
    { ITEM_IRON_BALL, 999 }, // くろいてっきゅう
    { ITEM_BLACK_SLUDGE, 999 }, // くろいヘドロ
    { ITEM_WIDE_LENS, 999 }, // こうかくレンズ
    { ITEM_CHOICE_SCARF, 999 }, // こだわりスカーフ
    { ITEM_CHOICE_BAND, 999 }, // こだわりハチマキ
    { ITEM_CHOICE_SPECS, 999 }, // こだわりメガネ
    { ITEM_ROCKY_HELMET, 999 }, // ゴツゴツメット
    { ITEM_PSYCHIC_SEED, 999 }, // サイコシード
    { ITEM_SMOOTH_ROCK, 999 }, // さらさらいわ
    { ITEM_DAMP_ROCK, 999 }, // しめったいわ
    { ITEM_WEAKNESS_POLICY, 999 }, // じゃくてんほけん
    { ITEM_WHITE_HERB, 999 }, // しろいハーブ
    { ITEM_EVIOLITE, 999 }, // しんかのきせき
    { ITEM_QUICK_CLAW, 999 }, // せんせいのツメ
    { ITEM_EJECT_PACK, 999 }, // だっしゅつパック
    { ITEM_EJECT_BUTTON, 999 }, // だっしゅつボタン
    { ITEM_EXPERT_BELT, 999 }, // たつじんのおび
    { ITEM_LEFTOVERS, 999 }, // たべのこし
    { ITEM_MUSCLE_BAND, 999 }, // ちからのハチマキ
    { ITEM_ICY_ROCK, 999 }, // つめたいいわ
    { ITEM_LIGHT_BALL, 999 }, // でんきだま
    { ITEM_TOXIC_ORB, 999 }, // どくどくだま
    { ITEM_ASSAULT_VEST, 999 }, // とつげきチョッキ
    { ITEM_THROAT_SPRAY, 999 }, // のどスプレー
    { ITEM_POWER_HERB, 999 }, // パワフルハーブ
    { ITEM_BRIGHT_POWDER, 999 }, // ひかりのこな
    { ITEM_LIGHT_CLAY, 999 }, // ひかりのねんど
    { ITEM_SCOPE_LENS, 999 }, // ピントレンズ
    { ITEM_AIR_BALLOON, 999 }, // ふうせん
    { ITEM_ZOOM_LENS, 999 }, // フォーカスレンズ
    { ITEM_PROTECTIVE_PADS, 999 }, // ぼうごパット
    { ITEM_SAFETY_GOGGLES, 999 }, // ぼうじんゴーグル
    { ITEM_MISTY_SEED, 999 }, // ミストシード
    { ITEM_METRONOME, 999 }, // メトロノーム
    { ITEM_MENTAL_HERB, 999 }, // メンタルハーブ
    { ITEM_ROOM_SERVICE, 999 }, // ルームサービス
    { ITEM_RED_CARD, 999 }, // レッドカード
    { ITEM_KEE_BERRY, 999 }, // アッキのみ
    { ITEM_IAPAPA_BERRY, 999 }, // イアのみ
    { ITEM_PASSHO_BERRY, 999 }, // イトケのみ
    { ITEM_CUSTAP_BERRY, 999 }, // イバンのみ
    { ITEM_WIKI_BERRY, 999 }, // ウイのみ
    { ITEM_PAYAPA_BERRY, 999 }, // ウタンのみ
    { ITEM_OCCA_BERRY, 999 }, // オッカのみ
    { ITEM_CHESTO_BERRY, 999 }, // カゴのみ
    { ITEM_KASIB_BERRY, 999 }, // カシブのみ
    { ITEM_SALAC_BERRY, 999 }, // カムラのみ
    { ITEM_PERSIM_BERRY, 999 }, // キーのみ
    { ITEM_CHERI_BERRY, 999 }, // クラボのみ
    { ITEM_LANSAT_BERRY, 999 }, // サンのみ
    { ITEM_JABOCA_BERRY, 999 }, // ジャポのみ
    { ITEM_SHUCA_BERRY, 999 }, // シュカのみ
    { ITEM_APICOT_BERRY, 999 }, // ズアのみ
    { ITEM_STARF_BERRY, 999 }, // スターのみ
    { ITEM_WACAN_BERRY, 999 }, // ソクノのみ
    { ITEM_MARANGA_BERRY, 999 }, // タラプのみ
    { ITEM_TANGA_BERRY, 999 }, // タンガのみ
    { ITEM_LIECHI_BERRY, 999 }, // チイラのみ
    { ITEM_ENIGMA_BERRY_E_READER, 999 }, // ナゾのみ
    { ITEM_COLBUR_BERRY, 999 }, // ナモのみ
    { ITEM_COBA_BERRY, 999 }, // バコウのみ
    { ITEM_HABAN_BERRY, 999 }, // ハバンのみ
    { ITEM_AGUAV_BERRY, 999 }, // バンジのみ
    { ITEM_KEBIA_BERRY, 999 }, // ビアーのみ
    { ITEM_FIGY_BERRY, 999 }, // フィラのみ
    { ITEM_CHILAN_BERRY, 999 }, // ホズのみ
    { ITEM_MICLE_BERRY, 999 }, // ミクルのみ
    { ITEM_PETAYA_BERRY, 999 }, // ヤタピのみ
    { ITEM_YACHE_BERRY, 999 }, // ヤチェのみ
    { ITEM_CHOPLE_BERRY, 999 }, // ヨプのみ
    { ITEM_CHARTI_BERRY, 999 }, // ヨロギのみ
    { ITEM_GANLON_BERRY, 999 }, // リュガのみ
    { ITEM_RINDO_BERRY, 999 }, // リンドのみ
    { ITEM_ROWAP_BERRY, 999 }, // レンブのみ
    { ITEM_ROSELI_BERRY, 999 }, // ロゼルのみ
    { ITEM_NONE, 0 },
};

static const u8 *FrontierCreator_GetTypeName(u32 type)
{
    if (type >= NUMBER_OF_MON_TYPES)
        return COMPOUND_STRING("----");

    if (sFrontierCreatorTypeNames[type] == NULL)
        return COMPOUND_STRING("----");

    return sFrontierCreatorTypeNames[type];
}

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

static const enum Item sFrontierHubGiveItems[] =
{
    ITEM_BUG_MEMORY,
    ITEM_DARK_MEMORY,
    ITEM_DRAGON_MEMORY,
    ITEM_ELECTRIC_MEMORY,
    ITEM_FAIRY_MEMORY,
    ITEM_FIGHTING_MEMORY,
    ITEM_FIRE_MEMORY,
    ITEM_FLYING_MEMORY,
    ITEM_GHOST_MEMORY,
    ITEM_GRASS_MEMORY,
    ITEM_GROUND_MEMORY,
    ITEM_ICE_MEMORY,
    ITEM_POISON_MEMORY,
    ITEM_PSYCHIC_MEMORY,
    ITEM_ROCK_MEMORY,
    ITEM_STEEL_MEMORY,
    ITEM_WATER_MEMORY,
    ITEM_ABOMASITE,
    ITEM_ABSOLITE,
    ITEM_ABSOLITE_Z,
    ITEM_ADAMANT_CRYSTAL,
    ITEM_ADAMANT_ORB,
    ITEM_AERODACTYLITE,
    ITEM_AGGRONITE,
    ITEM_ALAKAZITE,
    ITEM_ALORAICHIUM_Z,
    ITEM_ALTARIANITE,
    ITEM_AMPHAROSITE,
    ITEM_AUDINITE,
    ITEM_BANETTITE,
    ITEM_BARBARACITE,
    ITEM_BAXCALIBRITE,
    ITEM_BEEDRILLITE,
    ITEM_BLACK_AUGURITE,
    ITEM_BLASTOISINITE,
    ITEM_BLAZIKENITE,
    ITEM_BLUE_ORB,
    ITEM_BUGINIUM_Z,
    ITEM_BUG_GEM,
    ITEM_CAMERUPTITE,
    ITEM_CHANDELURITE,
    ITEM_CHARIZARDITE_X,
    ITEM_CHARIZARDITE_Y,
    ITEM_CHESNAUGHTITE,
    ITEM_CHIMECHITE,
    ITEM_CLEFABLITE,
    ITEM_CORNERSTONE_MASK,
    ITEM_CRABOMINITE,
    ITEM_DARKINIUM_Z,
    ITEM_DARKRANITE,
    ITEM_DARK_GEM,
    ITEM_DECIDIUM_Z,
    ITEM_DELPHOXITE,
    ITEM_DIANCITE,
    ITEM_DRACO_PLATE,
    ITEM_DRAGALGITE,
    ITEM_DRAGONINITE,
    ITEM_DRAGONIUM_Z,
    ITEM_DRAGON_GEM,
    ITEM_DRAMPANITE,
    ITEM_DREAD_PLATE,
    ITEM_EARTH_PLATE,
    ITEM_EELEKTROSSITE,
    ITEM_EEVIUM_Z,
    ITEM_ELECTRIC_GEM,
    ITEM_ELECTRIUM_Z,
    ITEM_EMBOARITE,
    ITEM_EVIOLITE,
    ITEM_EXCADRITE,
    ITEM_FAIRIUM_Z,
    ITEM_FAIRY_GEM,
    ITEM_FALINKSITE,
    ITEM_FERALIGITE,
    ITEM_FIGHTING_GEM,
    ITEM_FIGHTINIUM_Z,
    ITEM_FIRE_GEM,
    ITEM_FIRIUM_Z,
    ITEM_FIST_PLATE,
    ITEM_FLAME_PLATE,
    ITEM_FLOETTITE,
    ITEM_FLYING_GEM,
    ITEM_FLYINIUM_Z,
    ITEM_FROSLASSITE,
    ITEM_GALLADITE,
    ITEM_GARCHOMPITE,
    ITEM_GARCHOMPITE_Z,
    ITEM_GARDEVOIRITE,
    ITEM_GENGARITE,
    ITEM_GHOSTIUM_Z,
    ITEM_GHOST_GEM,
    ITEM_GLALITITE,
    ITEM_GLIMMORANITE,
    ITEM_GOLISOPITE,
    ITEM_GOLURKITE,
    ITEM_GRASSIUM_Z,
    ITEM_GRASS_GEM,
    ITEM_GRENINJITE,
    ITEM_GRISEOUS_CORE,
    ITEM_GRISEOUS_ORB,
    ITEM_GROUNDIUM_Z,
    ITEM_GROUND_GEM,
    ITEM_GYARADOSITE,
    ITEM_HAWLUCHANITE,
    ITEM_HEARTHFLAME_MASK,
    ITEM_HEATRANITE,
    ITEM_HERACRONITE,
    ITEM_HOUNDOOMINITE,
    ITEM_ICE_GEM,
    ITEM_ICICLE_PLATE,
    ITEM_ICIUM_Z,
    ITEM_INCINIUM_Z,
    ITEM_INSECT_PLATE,
    ITEM_IRON_PLATE,
    ITEM_KANGASKHANITE,
    ITEM_KOMMONIUM_Z,
    ITEM_LATIASITE,
    ITEM_LATIOSITE,
    ITEM_LOPUNNITE,
    ITEM_LUCARIONITE,
    ITEM_LUCARIONITE_Z,
    ITEM_LUNALIUM_Z,
    ITEM_LUSTROUS_GLOBE,
    ITEM_LUSTROUS_ORB,
    ITEM_LYCANIUM_Z,
    ITEM_MAGEARNITE,
    ITEM_MALAMARITE,
    ITEM_MANECTITE,
    ITEM_MARSHADIUM_Z,
    ITEM_MAWILITE,
    ITEM_MEADOW_PLATE,
    ITEM_MEDICHAMITE,
    ITEM_MEGANIUMITE,
    ITEM_MEOWSTICITE,
    ITEM_METAGROSSITE,
    ITEM_METEORITE,
    ITEM_MEWNIUM_Z,
    ITEM_MEWTWONITE_X,
    ITEM_MEWTWONITE_Y,
    ITEM_MIMIKIUM_Z,
    ITEM_MIND_PLATE,
    ITEM_NORMALIUM_Z,
    ITEM_NORMAL_GEM,
    ITEM_PIDGEOTITE,
    ITEM_PIKANIUM_Z,
    ITEM_PIKASHUNIUM_Z,
    ITEM_PINSIRITE,
    ITEM_PIXIE_PLATE,
    ITEM_POISONIUM_Z,
    ITEM_POISON_GEM,
    ITEM_PRIMARIUM_Z,
    ITEM_PSYCHIC_GEM,
    ITEM_PSYCHIUM_Z,
    ITEM_PYROARITE,
    ITEM_RAICHUNITE_X,
    ITEM_RAICHUNITE_Y,
    ITEM_RED_ORB,
    ITEM_ROCKIUM_Z,
    ITEM_ROCK_GEM,
    ITEM_RUSTED_SHIELD,
    ITEM_RUSTED_SWORD,
    ITEM_SABLENITE,
    ITEM_SALAMENCITE,
    ITEM_SCEPTILITE,
    ITEM_SCIZORITE,
    ITEM_SCOLIPITE,
    ITEM_SCOVILLAINITE,
    ITEM_SCRAFTINITE,
    ITEM_SHARPEDONITE,
    ITEM_SKARMORITE,
    ITEM_SKY_PLATE,
    ITEM_SLOWBRONITE,
    ITEM_SNORLIUM_Z,
    ITEM_SOLGANIUM_Z,
    ITEM_SOUL_DEW,
    ITEM_SPLASH_PLATE,
    ITEM_SPOOKY_PLATE,
    ITEM_STARAPTITE,
    ITEM_STARMINITE,
    ITEM_STEELIUM_Z,
    ITEM_STEELIXITE,
    ITEM_STEEL_GEM,
    ITEM_STONE_PLATE,
    ITEM_SWAMPERTITE,
    ITEM_TAPUNIUM_Z,
    ITEM_TATSUGIRINITE,
    ITEM_TOXIC_PLATE,
    ITEM_TYRANITARITE,
    ITEM_ULTRANECROZIUM_Z,
    ITEM_VENUSAURITE,
    ITEM_VICTREEBELITE,
    ITEM_WATERIUM_Z,
    ITEM_WATER_GEM,
    ITEM_WELLSPRING_MASK,
    ITEM_ZAP_PLATE,
    ITEM_ZERAORITE,
    ITEM_ZYGARDITE,
    ITEM_Z_POWER_RING,
    ITEM_NONE,
};

static u16 FrontierItemGiver_GetItemCount(void)
{
    u16 count = 0;

    while (sFrontierHubGiveItems[count] != ITEM_NONE)
        count++;

    return count;
}

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

static void FrontierCreator_DrawTeraTypeScreen(u8 taskId);

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

static void FrontierCreator_DrawAbilityScreen(u8 taskId)
{
    u8 windowId = gTasks[taskId].tWindowId;
    u8 slot = gTasks[taskId].tInput;
    enum Ability ability = GetSpeciesAbility(sFrontierCreatorData->species, slot);

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("とくせいを えらぶ{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("スロット: "));
    ConvertIntToDecimalStringN(gStringVar1, slot, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("とくせい: "));
    if (ability == ABILITY_NONE)
        StringAppend(gStringVar4, COMPOUND_STRING("----"));
    else
         StringAppend(gStringVar4, gAbilitiesInfo[ability].name);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    if (ability == ABILITY_NONE)
        StringAppend(gStringVar4, COMPOUND_STRING("これは えらべません{CLEAR_TO 90}\n"));
    else
        StringAppend(gStringVar4, COMPOUND_STRING("このとくせいは OK{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}へんこう"));
    StringAppend(gStringVar4, sFrontierCreatorText_Controls);

    FrontierCreator_PrintWindow(windowId);
}
static void FrontierCreator_DrawTeraTypeScreen(u8 taskId)
{
    u8 windowId = gTasks[taskId].tWindowId;
    u32 type = gTasks[taskId].tInput;

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("テラスタイプを えらぶ{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("No."));
    ConvertIntToDecimalStringN(gStringVar1, type, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("タイプ: "));
    StringAppend(gStringVar4, FrontierCreator_GetTypeName(type));
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    if (type == TYPE_NONE || type == TYPE_MYSTERY)
        StringAppend(gStringVar4, COMPOUND_STRING("これは えらべません{CLEAR_TO 90}\n"));
    else
        StringAppend(gStringVar4, COMPOUND_STRING("このタイプは OK{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("けた: "));
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tDigit, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}へんこう {LEFT_ARROW}{RIGHT_ARROW}けた"));
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

        FrontierCreator_DrawAbilityScreen(taskId);
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
        FrontierCreator_DrawAbilityScreen(taskId);
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

        FrontierCreator_DrawTeraTypeScreen(taskId);
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
        FrontierCreator_DrawTeraTypeScreen(taskId);
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

static void FrontierHub_TryAddItem(enum Item itemId)
{
    if (itemId == ITEM_NONE)
        return;

    if (!CheckBagHasItem(itemId, 1) && CheckBagHasSpace(itemId, 1))
        AddBagItem(itemId, 1);
}

static void FrontierHub_GiveAllKeyItems(void)
{
    enum Item itemId;

    for (itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT; itemId++)
    {
        if (GetItemPocket(itemId) == POCKET_KEY_ITEMS)
            FrontierHub_TryAddItem(itemId);
    }
}

static void FrontierHub_GiveAutoItems(void)
{
    u32 i;

    for (i = 0; sFrontierHubAutoItems[i].itemId != ITEM_NONE; i++)
    {
        if (CheckBagHasSpace(sFrontierHubAutoItems[i].itemId, sFrontierHubAutoItems[i].quantity))
            AddBagItem(sFrontierHubAutoItems[i].itemId, sFrontierHubAutoItems[i].quantity);
    }
}

static void FrontierItemGiver_DrawScreen(u8 taskId)
{
    u8 windowId = gTasks[taskId].tWindowId;
    u16 itemCount = FrontierItemGiver_GetItemCount();
    u16 index = gTasks[taskId].tInput;
    enum Item itemId;

    if (itemCount == 0)
        index = 0;
    else if (index >= itemCount)
        index = itemCount - 1;

    itemId = sFrontierHubGiveItems[index];

    FrontierCreator_ClearWindow(windowId);

    StringCopy(gStringVar4, COMPOUND_STRING("どうぐを えらぶ{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("No."));
    ConvertIntToDecimalStringN(gStringVar1, index + 1, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar1);

    StringAppend(gStringVar4, COMPOUND_STRING(" / "));
    ConvertIntToDecimalStringN(gStringVar1, itemCount, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("なまえ: "));
    StringAppend(gStringVar4, gItemsInfo[itemId].name);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));

    if (CheckBagHasItem(itemId, 1))
        StringAppend(gStringVar4, COMPOUND_STRING("もう もっています{CLEAR_TO 90}\n"));
    else if (!CheckBagHasSpace(itemId, 1))
        StringAppend(gStringVar4, COMPOUND_STRING("バッグが いっぱいです{CLEAR_TO 90}\n"));
    else
        StringAppend(gStringVar4, COMPOUND_STRING("このどうぐを もらえます{CLEAR_TO 90}\n"));

    StringAppend(gStringVar4, COMPOUND_STRING("{UP_ARROW}{DOWN_ARROW}えらぶ"));
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n{A_BUTTON}もらう {B_BUTTON}やめる"));

    FrontierCreator_PrintWindow(windowId);
}

static void FrontierItemGiver_DestroyAndReturn(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    DestroyTask(taskId);

    ScriptContext_Enable();
    UnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

static void Task_FrontierItemGiver_SelectItem(u8 taskId)
{
    u16 itemCount = FrontierItemGiver_GetItemCount();
    enum Item itemId;

    if (itemCount == 0)
    {
        gSpecialVar_Result = FALSE;
        FrontierItemGiver_DestroyAndReturn(taskId);
        return;
    }

    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);

        if (gTasks[taskId].tInput == 0)
            gTasks[taskId].tInput = itemCount - 1;
        else
            gTasks[taskId].tInput--;

        FrontierItemGiver_DrawScreen(taskId);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);

        gTasks[taskId].tInput++;
        if (gTasks[taskId].tInput >= itemCount)
            gTasks[taskId].tInput = 0;

        FrontierItemGiver_DrawScreen(taskId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        itemId = sFrontierHubGiveItems[gTasks[taskId].tInput];

        if (CheckBagHasItem(itemId, 1))
        {
            PlaySE(SE_PC_OFF);
            FrontierItemGiver_DrawScreen(taskId);
            return;
        }

        if (!CheckBagHasSpace(itemId, 1))
        {
            PlaySE(SE_PC_OFF);
            FrontierItemGiver_DrawScreen(taskId);
            return;
        }

        AddBagItem(itemId, 1);
        PlaySE(SE_SELECT);
        gSpecialVar_Result = TRUE;
        FrontierItemGiver_DrawScreen(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_Result = FALSE;
        FrontierItemGiver_DestroyAndReturn(taskId);
    }
}

void Special_SetFrontierAiNormal(void)
{
    VarSet(VAR_FRONTIER_AI_LEVEL, 0);
}

void Special_SetFrontierAiHard(void)
{
    VarSet(VAR_FRONTIER_AI_LEVEL, 1);
}

void Special_SetFrontierAiStrongest(void)
{
    VarSet(VAR_FRONTIER_AI_LEVEL, 2);
}

void Special_SetupFrontierHubState(void)
{
    u32 i;
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    FlagSet(FLAG_BADGE03_GET);
    FlagSet(FLAG_BADGE04_GET);
    FlagSet(FLAG_BADGE05_GET);
    FlagSet(FLAG_BADGE06_GET);
    FlagSet(FLAG_BADGE07_GET);
    FlagSet(FLAG_BADGE08_GET);

    FlagSet(FLAG_SYS_GAME_CLEAR);
    FlagSet(FLAG_SYS_FRONTIER_PASS);
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKENAV_GET);
    FlagSet(FLAG_SYS_B_DASH);
    FlagSet(FLAG_EXPANSION_DYNAMAX_BATTLE);
    FlagSet(FLAG_EXPANSION_TERA_ORB_CHARGED);
    FlagSet(FLAG_EXPANSION_TERA_ORB_NO_COST);
    EnableNationalPokedex();

    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_BULBASAUR), FLAG_SET_SEEN);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_BULBASAUR), FLAG_SET_CAUGHT);

    for (i = 0; i < ARRAY_COUNT(gBadgeFlags); i++)
        FlagSet(gBadgeFlags[i]);

FrontierHub_GiveAllKeyItems();
FrontierHub_GiveAutoItems();
}

void Special_OpenFrontierItemGiver(void)
{
    u8 taskId;
    u8 windowId;

    if (FrontierItemGiver_GetItemCount() == 0)
    {
        gSpecialVar_Result = FALSE;
        return;
    }

    LockPlayerFieldControls();
    FreezeObjectEvents();
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();

    windowId = AddWindow(&sFrontierCreatorWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    taskId = CreateTask(Task_FrontierItemGiver_SelectItem, 3);
    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tInput = 0;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tStatId = 0;

    FrontierItemGiver_DrawScreen(taskId);
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