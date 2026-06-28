extern const u8 EventScript_FollowerIsShivering[];
extern const u8 EventScript_FollowerNostalgia[];
extern const u8 EventScript_FollowerHopping[];
extern const u8 EventScript_FollowerJumpOnPlayer[];
extern const u8 EventScript_FollowerCuddling[];
extern const u8 EventScript_FollowerShiverCuddling[];
extern const u8 EventScript_FollowerGetCloser[];
extern const u8 EventScript_FollowerPokingPlayer[];
extern const u8 EventScript_FollowerLookAround[];
extern const u8 EventScript_FollowerLookAway[];
extern const u8 EventScript_FollowerLookAwayBark[];
extern const u8 EventScript_FollowerLookAwayPoke[];
extern const u8 EventScript_FollowerPokeGround[];
extern const u8 EventScript_FollowerStartled[];
extern const u8 EventScript_FollowerFastHopping[];
extern const u8 EventScript_FollowerDizzy[];
extern const u8 EventScript_FollowerLookAroundScared[];
extern const u8 EventScript_FollowerDance[];
extern const u8 EventScript_FollowerLookUp[];

// 'Generic', unconditional happy messages
static const u8 sHappyMsg00[] = _("{JPN}{STR_VAR_1}は おなかを ツンツン してきた！");
static const u8 sHappyMsg01[] = _("{JPN}{STR_VAR_1}は うれしくて てれちゃってる！");
static const u8 sHappyMsg02[] = _("{JPN}{STR_VAR_1}は げんきに ついて きてる！");
static const u8 sHappyMsg03[] = _("{JPN}{STR_VAR_1}は よゆう たっぷりだ！");
static const u8 sHappyMsg04[] = _("{JPN}{STR_VAR_1}は きもちよさそうに ついてきてる！");
static const u8 sHappyMsg05[] = _("{JPN}{STR_VAR_1}は げんき いっぱいだ！");
static const u8 sHappyMsg06[] = _("{JPN}{STR_VAR_1}は うれしそうな かおを してる！");
static const u8 sHappyMsg07[] = _("{JPN}{STR_VAR_1}は ググッと ちからを いれた！");
static const u8 sHappyMsg08[] = _("{JPN}{STR_VAR_1}は まわりの においを かいでる");
static const u8 sHappyMsg09[] = _("{JPN}{STR_VAR_1}は うれしくて こおどり してる！");
static const u8 sHappyMsg10[] = _("{JPN}{STR_VAR_1}は まだまだ げんき だ！");
static const u8 sHappyMsg11[] = _("{JPN}クンクン けむりの においを かいでる");
static const u8 sHappyMsg12[] = _("{JPN}{STR_VAR_1}は おなかを ツンツン してきた！");
static const u8 sHappyMsg13[] = _("{JPN}からだを のばして リラックス してる");
static const u8 sHappyMsg14[] = _("{JPN}{STR_VAR_1}は さきに いきたそうだ！");
static const u8 sHappyMsg15[] = _("{JPN}{STR_VAR_1}は いっしょうけんめい ついて きてる！");
static const u8 sHappyMsg16[] = _("{JPN}{STR_VAR_1}は うれしくて すりよってきた！");
static const u8 sHappyMsg17[] = _("{JPN}{STR_VAR_1}は げんき いっぱいだ！");
static const u8 sHappyMsg18[] = _("{JPN}{STR_VAR_1}は とっても うれしそうだ！");
static const u8 sHappyMsg19[] = _("{JPN}{STR_VAR_1}は\nうしくて じっとして いられない！");
static const u8 sHappyMsg20[] = _("{JPN}{STR_VAR_1}は ゆっくりと うなずいた");
static const u8 sHappyMsg21[] = _("{JPN}{STR_VAR_1}は やるき いっぱいだ！");
static const u8 sHappyMsg22[] = _("{JPN}{STR_VAR_1}は\nキョロキョロして おとを きいている");
static const u8 sHappyMsg23[] = _("{JPN}{STR_VAR_1}は きょうみ しんしんだ！");
static const u8 sHappyMsg24[] = _("{JPN}{STR_VAR_1}は なんとか ついて きてる");
static const u8 sHappyMsg25[] = _("{JPN}{STR_VAR_1}は げんきに あいさつした！");
static const u8 sHappyMsg26[] = _("{JPN}{STR_VAR_1}は ニコニコ あいさつした！");
static const u8 sHappyMsg27[] = _("{JPN}クンクン はなの においを かいでる");
static const u8 sHappyMsg28[] = _("{JPN}{STR_VAR_1}は\nうれしそうに あいさつ した！");
static const u8 sHappyMsg29[] = _("{JPN}{STR_VAR_1}は\nこっちを みて ニヤッとした");
static const u8 sHappyMsg30[] = _("{JPN}{STR_VAR_1}は うれしくて すりよってきた！");
// Conditional messages begin here, index 31
static const u8 sHappyMsg31[] = _("{JPN}いい てきんで うれしい みたい！");
static const u8 sHappyMsg32[] = _("{JPN}{STR_VAR_1}は よゆう たっぷりだ！");

const struct FollowerMsgInfo gFollowerHappyMessages[] = {
    {sHappyMsg00, EventScript_FollowerPokingPlayer},
    {sHappyMsg01}, {sHappyMsg02}, {sHappyMsg03}, {sHappyMsg04}, {sHappyMsg05}, {sHappyMsg06}, {sHappyMsg07},
    {sHappyMsg08, EventScript_FollowerLookAround},
    {sHappyMsg09, EventScript_FollowerHopping},
    {sHappyMsg10}, {sHappyMsg11},
    {sHappyMsg12, EventScript_FollowerPokingPlayer},
    {sHappyMsg13, EventScript_FollowerLookAround},
    {sHappyMsg14}, {sHappyMsg15},
    {sHappyMsg16, EventScript_FollowerCuddling},
    {sHappyMsg17}, {sHappyMsg18},
    {sHappyMsg19, EventScript_FollowerFastHopping},
    {sHappyMsg20}, {sHappyMsg21}, {sHappyMsg22}, {sHappyMsg23}, {sHappyMsg24}, {sHappyMsg25}, {sHappyMsg26}, {sHappyMsg27}, {sHappyMsg28}, {sHappyMsg29},
    {sHappyMsg30, EventScript_FollowerCuddling},
    {sHappyMsg31}, {sHappyMsg32},
};

// Unconditional neutral messages
static const u8 sNeutralMsg00[] = _("{JPN}{STR_VAR_1}は じめんを\nコツコツ つっついてる");
static const u8 sNeutralMsg01[] = _("{JPN}{STR_VAR_1}は けいかいを している！");
static const u8 sNeutralMsg02[] = _("{JPN}{STR_VAR_1}は\nなにもない ほうを じっと みつめてる……");
static const u8 sNeutralMsg03[] = _("{JPN}{STR_VAR_1}は キョロキョロ してる");
static const u8 sNeutralMsg04[] = _("{JPN}ウワーっと あくびをした！");
static const u8 sNeutralMsg05[] = _("{JPN}まわりを キョロキョロ みまわしてる");
static const u8 sNeutralMsg06[] = _("{JPN}{STR_VAR_1}は ニコッと こっちを みた");
static const u8 sNeutralMsg07[] = _("{JPN}{STR_VAR_1}は\nキョロキョロ あたりを みまわしてる");
static const u8 sNeutralMsg08[] = _("{JPN}{STR_VAR_1}は\nきあいを いれている！");
static const u8 sNeutralMsg09[] = _("{JPN}{STR_VAR_1}は\nすごい おどりを おどった！");
static const u8 sNeutralMsg10[] = _("{JPN}{STR_VAR_1}は やるき いっぱいだ！");
static const u8 sNeutralMsg11[] = _("{JPN}{STR_VAR_1}は とおくを\nじーっと みつめてる");
static const u8 sNeutralMsg12[] = _("{JPN}{STR_VAR_1}は あたりを\nけいかい している！");
static const u8 sNeutralMsg13[] = _("{JPN}{STR_VAR_1}は\nとおくに むかって ほえだした！");

const struct FollowerMsgInfo gFollowerNeutralMessages[] = {
    {sNeutralMsg00, EventScript_FollowerPokeGround},
    {sNeutralMsg01},
    {sNeutralMsg02, EventScript_FollowerLookAway},
    {sNeutralMsg03, EventScript_FollowerLookAround},
    {sNeutralMsg04},
    {sNeutralMsg05, EventScript_FollowerLookAround},
    {sNeutralMsg06}, {sNeutralMsg07}, {sNeutralMsg08},
    {sNeutralMsg09, EventScript_FollowerDance},
    {sNeutralMsg10},
    {sNeutralMsg11, EventScript_FollowerLookAway},
    {sNeutralMsg12},
    {sNeutralMsg13, EventScript_FollowerLookAwayBark},
};

// Unconditional sad messages
static const u8 sSadMsg00[] = _("{JPN}{STR_VAR_1}は\nふらふら してる！");
static const u8 sSadMsg01[] = _("{JPN}{STR_VAR_1}は\nあしを ふんできた！");
static const u8 sSadMsg02[] = _("{JPN}{STR_VAR_1}は すこし つかれてる みたい");
// Conditional messages begin, index 3
static const u8 sSadMsg03[] = _("{JPN}{STR_VAR_1}は うれしく なさそう……");
static const u8 sSadMsg04[] = _("{JPN}{STR_VAR_1}は ころびそうに なった！");
static const u8 sSadMsg05[] = _("{JPN}{STR_VAR_1}は いまにも たおれそうだ！");
static const u8 sSadMsg06[] = _("{JPN}{STR_VAR_1}は\nがんばって ついてきてる……");
static const u8 sSadMsg07[] = _("{JPN}{STR_VAR_1}は きんちょう してる！");

const struct FollowerMsgInfo gFollowerSadMessages[] = {
    {sSadMsg00, EventScript_FollowerDizzy},
    {sSadMsg01}, {sSadMsg02},
    {sSadMsg03}, {sSadMsg04}, {sSadMsg05}, {sSadMsg06}, {sSadMsg07},
};

// Unconditional upset messages
static const u8 sUpsetMsg00[] = _("{JPN}{STR_VAR_1}は\nちょっと いやそうな かおをした……");
static const u8 sUpsetMsg01[] = _("{JPN}{STR_VAR_1}は\nちょっと いやそうなかおを してる……");
static const u8 sUpsetMsg02[] = _("{JPN}…… ちょっと さむそうに してる");
// Conditional messages, index 3
static const u8 sUpsetMsg03[] = _("{JPN}{STR_VAR_1}は くさむらで\nあまやどり してる");

const struct FollowerMsgInfo gFollowerUpsetMessages[] = {
    {sUpsetMsg00}, {sUpsetMsg01},
    {sUpsetMsg02, EventScript_FollowerIsShivering},
    {sUpsetMsg03},
};

// Unconditional angry messages
static const u8 sAngryMsg00[] = _("{JPN}{STR_VAR_1}は うなりごえを あげた！");
static const u8 sAngryMsg01[] = _("{JPN}{STR_VAR_1}は\nおこったかおで うなってる！");
static const u8 sAngryMsg02[] = _("{JPN}{STR_VAR_1}は なんだか おこってる！");
static const u8 sAngryMsg03[] = _("{JPN}ブスッとした かおで そっぽむいてる……");
static const u8 sAngryMsg04[] = _("{JPN}{STR_VAR_1}は ほえだした！");

const struct FollowerMsgInfo gFollowerAngryMessages[] = {
    {sAngryMsg00}, {sAngryMsg01}, {sAngryMsg02},
    {sAngryMsg03, EventScript_FollowerLookAway},
    {sAngryMsg04},
};

// Unconditional pensive messages
static const u8 sPensiveMsg00[] = _("{JPN}{STR_VAR_1}は\nじーっと したを むいてる");
static const u8 sPensiveMsg01[] = _("{JPN}{STR_VAR_1}は\nあたりを みまわしている！");
static const u8 sPensiveMsg02[] = _("{JPN}{STR_VAR_1}は したを\nじーっと のぞいている");
static const u8 sPensiveMsg03[] = _("{JPN}{STR_VAR_1}は ねむいのを\nなんとか こらえてる……");
static const u8 sPensiveMsg04[] = _("{JPN}{STR_VAR_1}は なにか\nキョロキョロ してる みたい");
static const u8 sPensiveMsg05[] = _("{JPN}{STR_VAR_1}は まわりをみて\nぽかーんと してる");
static const u8 sPensiveMsg06[] = _("{JPN}{STR_VAR_1}は\nおおきな あくびを した！");
static const u8 sPensiveMsg07[] = _("{JPN}{STR_VAR_1}は\nゆったり リラックス してる");
static const u8 sPensiveMsg08[] = _("{JPN}{STR_VAR_1}は じーっと\n{PLAYER}の かおを みつめてる");
static const u8 sPensiveMsg09[] = _("{JPN}{STR_VAR_1}は\n{PLAYER}の かおを じーっとみてる");
static const u8 sPensiveMsg10[] = _("{JPN}{STR_VAR_1}は こちらを じーっと みてる ");
static const u8 sPensiveMsg11[] = _("{JPN}{STR_VAR_1}は おくを\nじーっと みてる……");
static const u8 sPensiveMsg12[] = _("{JPN}{STR_VAR_1}は じめんの においを\nクンクン かいでる");
static const u8 sPensiveMsg13[] = _("{JPN}なにも ないところを じーっと みつめてる……");
static const u8 sPensiveMsg14[] = _("{JPN}{STR_VAR_1}は めを キリッと させた！");
static const u8 sPensiveMsg15[] = _("{JPN}{STR_VAR_1}は しゅうちゅう している……");
static const u8 sPensiveMsg16[] = _("{JPN}{STR_VAR_1}は こっちをみて うなずいた");
static const u8 sPensiveMsg17[] = _("{JPN}{STR_VAR_1}は すこし きんちょう してる みたい");
static const u8 sPensiveMsg18[] = _("{JPN}{STR_VAR_1}は {PLAYER}の\nあしあとを みてる");
static const u8 sPensiveMsg19[] = _("{JPN}{STR_VAR_1}は {PLAYER}の めを\nじーっと みつめてる");

const struct FollowerMsgInfo gFollowerPensiveMessages[] = {
    {sPensiveMsg00},
    {sPensiveMsg01, EventScript_FollowerLookAround},
    {sPensiveMsg02}, {sPensiveMsg03}, {sPensiveMsg04},
    {sPensiveMsg05, EventScript_FollowerLookAround},
    {sPensiveMsg06}, {sPensiveMsg07}, {sPensiveMsg08}, {sPensiveMsg09}, {sPensiveMsg10},
    {sPensiveMsg11, EventScript_FollowerLookAway},
    {sPensiveMsg12, EventScript_FollowerPokeGround},
    {sPensiveMsg13, EventScript_FollowerLookAway},
    {sPensiveMsg14}, {sPensiveMsg15}, {sPensiveMsg16}, {sPensiveMsg17}, {sPensiveMsg18}, {sPensiveMsg19},
};

// All 'love' messages are unconditional
static const u8 sLoveMsg00[] = _("{JPN}{STR_VAR_1}は とつぜん\nあしに すりよってきた！");
static const u8 sLoveMsg01[] = _("{JPN}{STR_VAR_1}の\nほっぺが あかく なってる！");
static const u8 sLoveMsg02[] = _("{JPN}わ！ {STR_VAR_1}が いきなり だきついてきた！");
static const u8 sLoveMsg03[] = _("{JPN}わ！ {STR_VAR_1}が\nきゅうに じゃれてきた！");
static const u8 sLoveMsg04[] = _("{JPN}{STR_VAR_1}は\nうれしくて すりよってきた！");
static const u8 sLoveMsg05[] = _("{JPN}{STR_VAR_1}は ほっぺを あかくした！");
static const u8 sLoveMsg06[] = _("{JPN}わ！ {STR_VAR_1}が\nだきついてきた！");
static const u8 sLoveMsg07[] = _("{JPN}{STR_VAR_1}は あまえためで\nこっちを みている！");
static const u8 sLoveMsg08[] = _("{JPN}{STR_VAR_1}は\n{PLAYER}に くっついてきた！");
static const u8 sLoveMsg09[] = _("{JPN}{STR_VAR_1}は からだに\nピトッと くっついた");

const struct FollowerMsgInfo gFollowerLoveMessages[] = {
    {sLoveMsg00, EventScript_FollowerGetCloser},
    {sLoveMsg01},
    {sLoveMsg02, EventScript_FollowerCuddling},
    {sLoveMsg03},
    {sLoveMsg04, EventScript_FollowerCuddling},
    {sLoveMsg05},
    {sLoveMsg06, EventScript_FollowerCuddling},
    {sLoveMsg07},
    {sLoveMsg08, EventScript_FollowerGetCloser},
    {sLoveMsg09},
};

// Unconditional surprised messages
static const u8 sSurpriseMsg00[] = _("{JPN}{STR_VAR_1}は\nあやうく ころびそうに なった！");
static const u8 sSurpriseMsg01[] = _("{JPN}{STR_VAR_1}は すべって ぶつかった！");
static const u8 sSurpriseMsg02[] = _("{JPN}{STR_VAR_1}は まだ\nじぶんのなまえに なれてない みたい");
static const u8 sSurpriseMsg03[] = _("{JPN}{STR_VAR_1}は したを\nじーっと のぞいている");
static const u8 sSurpriseMsg04[] = _("{JPN}つまずいて ころびそうに なった！");
static const u8 sSurpriseMsg05[] = _("{JPN}{STR_VAR_1}は なにかを\nかんじて うなっている！");
static const u8 sSurpriseMsg06[] = _("{JPN}{STR_VAR_1}は からだを しゃきっと させた！");
static const u8 sSurpriseMsg07[] = _("{JPN}{STR_VAR_1}は いきなり\nうしろに むかって ほえだした！");
static const u8 sSurpriseMsg08[] = _("{JPN}{STR_VAR_1}は いきなり\nサッと うしろを むいた！");
static const u8 sSurpriseMsg09[] = _("{JPN}いきなり はなしかけられて\nびっくり したみたい！");
static const u8 sSurpriseMsg10[] = _("{JPN}クンクン！ なんだか いいにおいが するみたい！");
static const u8 sSurpriseMsg11[] = _("{JPN}{STR_VAR_1}は\nからだを しゃきっと させた！");
static const u8 sSurpriseMsg12[] = _("{JPN}{STR_VAR_1}は\nゆらゆら ゆれて ころびそう！");
static const u8 sSurpriseMsg13[] = _("{JPN}{STR_VAR_1}は\nあやうく ころびそうに なった！");
static const u8 sSurpriseMsg14[] = _("{JPN}{STR_VAR_1}は\nしんちょうに ついてきている！");
static const u8 sSurpriseMsg15[] = _("{JPN}{STR_VAR_1}は\nきんちょうして かたくなってる");
static const u8 sSurpriseMsg16[] = _("{JPN}{STR_VAR_1}は なにかの けはいを\nかんじて びっくりしている！");
static const u8 sSurpriseMsg17[] = _("{JPN}{STR_VAR_1}は\nこわくて すりよってきた！");
static const u8 sSurpriseMsg18[] = _("{JPN}{STR_VAR_1}は\nただならぬ けはいを かんじている……");
static const u8 sSurpriseMsg19[] = _("{JPN}{STR_VAR_1}は\nきんちょうして かたくなってる");
// Conditional messages, index 20
static const u8 sSurpriseMsg20[] = _("{JPN}{STR_VAR_1}は\nあめに びっくり したみたい！");

const struct FollowerMsgInfo gFollowerSurpriseMessages[] = {
    {sSurpriseMsg00},
    {sSurpriseMsg01, EventScript_FollowerPokingPlayer},
    {sSurpriseMsg02}, {sSurpriseMsg03}, {sSurpriseMsg04}, {sSurpriseMsg05}, {sSurpriseMsg06},
    {sSurpriseMsg07, EventScript_FollowerLookAwayBark},
    {sSurpriseMsg08, EventScript_FollowerLookAway},
    {sSurpriseMsg09},
    {sSurpriseMsg10, EventScript_FollowerLookAround},
    {sSurpriseMsg11}, {sSurpriseMsg12}, {sSurpriseMsg13}, {sSurpriseMsg14}, {sSurpriseMsg15}, {sSurpriseMsg16},
    {sSurpriseMsg17, EventScript_FollowerCuddling},
    {sSurpriseMsg18},
    {sSurpriseMsg19, EventScript_FollowerLookAround},
    {sSurpriseMsg20},
};

// Unconditional curious messages
static const u8 sCuriousMsg00[] = _("{JPN}キョロキョロ なにか さがしてる みたい");
static const u8 sCuriousMsg01[] = _("{JPN}まえが みえなくて ぶつかった！");
static const u8 sCuriousMsg02[] = _("{JPN}クンクン！ なにか ちかくに あるのかな？");
static const u8 sCuriousMsg03[] = _("{JPN}{STR_VAR_1}は いしころを\nコロコロ ころがして あそんでる");
static const u8 sCuriousMsg04[] = _("{JPN}{STR_VAR_1}は\nキョロキョロ なにかを さがしてる");
static const u8 sCuriousMsg05[] = _("{JPN}{STR_VAR_1}は {PLAYER}の\nにおいを クンクン かいでる");
static const u8 sCuriousMsg06[] = _("{JPN}{STR_VAR_1}は ちょっと\nまよってる みたい……");

const struct FollowerMsgInfo gFollowerCuriousMessages[] = {
    {sCuriousMsg00, EventScript_FollowerLookAround},
    {sCuriousMsg01, EventScript_FollowerPokingPlayer},
    {sCuriousMsg02}, {sCuriousMsg03},
    {sCuriousMsg04, EventScript_FollowerLookAround},
    {sCuriousMsg05}, {sCuriousMsg06},
};

// Unconditional music messages
static const u8 sMusicMsg00[] = _("{JPN}{STR_VAR_1}は\nササッと みがるに うごいた！");
static const u8 sMusicMsg01[] = _("{JPN}{STR_VAR_1}は フンフンっと\nげんきに うごいた！");
static const u8 sMusicMsg02[] = _("{JPN}わ！ {STR_VAR_1}は\nうれしくて おどっちゃってる！");
static const u8 sMusicMsg03[] = _("{JPN}{STR_VAR_1}は\nたのしそうに ついてきてる！");
static const u8 sMusicMsg04[] = _("{JPN}{STR_VAR_1}は\n{PLAYER}と あそびたそうだ！");
static const u8 sMusicMsg05[] = _("{JPN}{STR_VAR_1}は\nうれしくて スキップ なんて してる！");
static const u8 sMusicMsg06[] = _("{JPN}{STR_VAR_1}は\nはなうた なんて うたってる！");
static const u8 sMusicMsg07[] = _("{JPN}{STR_VAR_1}は\nあしもとを つついて きた");
static const u8 sMusicMsg08[] = _("{JPN}{STR_VAR_1}は {PLAYER}の\nかおを みつめた");
static const u8 sMusicMsg09[] = _("{JPN}{STR_VAR_1}は\nちからいっぱいに アピールした！");
static const u8 sMusicMsg10[] = _("{JPN}わ！ {STR_VAR_1}は\nうれしくて おどっちゃった！");
static const u8 sMusicMsg11[] = _("{JPN}{STR_VAR_1}は ウキウキしてる！");
static const u8 sMusicMsg12[] = _("{JPN}{STR_VAR_1}は ピョンっと\nみがるに ジャンプした！");
static const u8 sMusicMsg13[] = _("{JPN}なんだか なつかしい かおりがする みたい……");
// Conditional music messages, index 14
static const u8 sMusicMsg14[] = _("{JPN}{STR_VAR_1}は あめで\nとっても よろこんでる！");

const struct FollowerMsgInfo gFollowerMusicMessages[] = {
    {sMusicMsg00, EventScript_FollowerLookAround},
    {sMusicMsg01},
    {sMusicMsg02, EventScript_FollowerDance},
    {sMusicMsg03},
    {sMusicMsg04, EventScript_FollowerHopping},
    {sMusicMsg05, EventScript_FollowerHopping},
    {sMusicMsg06}, {sMusicMsg07}, {sMusicMsg08}, {sMusicMsg09},
    {sMusicMsg10, EventScript_FollowerDance},
    {sMusicMsg11},
    {sMusicMsg12, EventScript_FollowerHopping},
    {sMusicMsg13, EventScript_FollowerNostalgia},
    {sMusicMsg14}
};


static const u8 sPoisonedMsg00[] = _("{JPN}{STR_VAR_1}は\nどくで ぶるぶる してる……");

const struct FollowerMsgInfo gFollowerPoisonedMessages[] = {
    {sPoisonedMsg00, EventScript_FollowerIsShivering},
};
