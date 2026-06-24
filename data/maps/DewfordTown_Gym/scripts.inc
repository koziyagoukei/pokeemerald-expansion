DewfordTown_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordTown_Gym_OnTransition
	.byte 0

DewfordTown_Gym_OnTransition:
	call DewfordTown_Gym_EventScript_SetFlashLevel
	end

DewfordTown_Gym_EventScript_SetFlashLevel::
	goto_if_defeated TRAINER_BRAWLY_1, DewfordTown_Gym_EventScript_SetLightsOn
	call DewfordTown_Gym_EventScript_CountTrainersDefeated
	copyvar VAR_0x8001, VAR_0x8000
	goto_if_eq VAR_0x8000, 0, DewfordTown_Gym_EventScript_SetFlashLevel7
	goto_if_eq VAR_0x8000, 1, DewfordTown_Gym_EventScript_SetFlashLevel6
	goto_if_eq VAR_0x8000, 2, DewfordTown_Gym_EventScript_SetFlashLevel5
	goto_if_eq VAR_0x8000, 3, DewfordTown_Gym_EventScript_SetFlashLevel4
	goto_if_eq VAR_0x8000, 4, DewfordTown_Gym_EventScript_SetFlashLevel3
	goto_if_eq VAR_0x8000, 5, DewfordTown_Gym_EventScript_SetFlashLevel2
	goto DewfordTown_Gym_EventScript_SetFlashLevel1

DewfordTown_Gym_EventScript_SetLightsOn::
	setflashlevel 0
	return

@ Brightest
DewfordTown_Gym_EventScript_SetFlashLevel1::
	setflashlevel 1
	return

DewfordTown_Gym_EventScript_SetFlashLevel2::
	setflashlevel 2
	return

DewfordTown_Gym_EventScript_SetFlashLevel3::
	setflashlevel 3
	return

DewfordTown_Gym_EventScript_SetFlashLevel4::
	setflashlevel 4
	return

DewfordTown_Gym_EventScript_SetFlashLevel5::
	setflashlevel 5
	return

DewfordTown_Gym_EventScript_SetFlashLevel6::
	setflashlevel 6
	return

@ Darkest
DewfordTown_Gym_EventScript_SetFlashLevel7::
	setflashlevel 7
	return

DewfordTown_Gym_EventScript_BrightenRoom::
	call DewfordTown_Gym_EventScript_CountTrainersDefeated
	nop1
	goto_if_eq VAR_0x8000, VAR_0x8001, DewfordTown_Gym_EventScript_NoLightChange
	copyvar VAR_0x8001, VAR_0x8000
	goto_if_eq VAR_0x8000, 1, DewfordTown_Gym_EventScript_AnimateFlash1Trainer
	goto_if_eq VAR_0x8000, 2, DewfordTown_Gym_EventScript_AnimateFlash2Trainers
	goto_if_eq VAR_0x8000, 3, DewfordTown_Gym_EventScript_AnimateFlash3Trainers
	goto_if_eq VAR_0x8000, 4, DewfordTown_Gym_EventScript_AnimateFlash4Trainers
	goto_if_eq VAR_0x8000, 5, DewfordTown_Gym_EventScript_AnimateFlash5Trainers
	goto_if_eq VAR_0x8000, 6, DewfordTown_Gym_EventScript_AnimateFlash6Trainers

DewfordTown_Gym_EventScript_NoLightChange::
	return

@ NOTE: A little confusingly, a larger animateflash param value is a smaller flash radius
DewfordTown_Gym_EventScript_AnimateFlash1Trainer::
	playse SE_SWITCH
	animateflash 6
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash2Trainers::
	playse SE_SWITCH
	animateflash 5
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash3Trainers::
	playse SE_SWITCH
	animateflash 4
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash4Trainers::
	playse SE_SWITCH
	animateflash 3
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash5Trainers::
	playse SE_SWITCH
	animateflash 2
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash6Trainers::
	playse SE_SWITCH
	animateflash 1
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlashFullBrightness::
	playse SE_SWITCH
	animateflash 0
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_CountTrainersDefeated::
	setvar VAR_0x8000, 0
	goto_if_not_defeated TRAINER_TAKAO, DewfordTown_Gym_EventScript_CheckJocelyn
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckJocelyn::
	goto_if_not_defeated TRAINER_JOCELYN, DewfordTown_Gym_EventScript_CheckLaura
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckLaura::
	goto_if_not_defeated TRAINER_LAURA, DewfordTown_Gym_EventScript_CheckBrenden
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckBrenden::
	goto_if_not_defeated TRAINER_BRENDEN, DewfordTown_Gym_EventScript_CheckCristian
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckCristian::
	goto_if_not_defeated TRAINER_CRISTIAN, DewfordTown_Gym_EventScript_CheckLilith
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckLilith::
	goto_if_not_defeated TRAINER_LILITH, DewfordTown_Gym_EventScript_StopCountingTrainers
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_StopCountingTrainers::
	return

DewfordTown_Gym_EventScript_Brawly::
	trainerbattle_single TRAINER_BRAWLY_1, DewfordTown_Gym_Text_BrawlyIntro, DewfordTown_Gym_Text_BrawlyDefeat, DewfordTown_Gym_EventScript_BrawlyDefeated, NO_MUSIC
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, DewfordTown_Gym_EventScript_BrawlyRematch
	goto_if_unset FLAG_RECEIVED_TM_BULK_UP, DewfordTown_Gym_EventScript_GiveBulkUp2
	msgbox DewfordTown_Gym_Text_BrawlyPostBattle, MSGBOX_DEFAULT
	release
	end

DewfordTown_Gym_EventScript_BrawlyDefeated::
	call DewfordTown_Gym_EventScript_AnimateFlashFullBrightness
	message DewfordTown_Gym_Text_ReceivedKnuckleBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox DewfordTown_Gym_Text_KnuckleBadgeInfoTakeThis, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_DEWFORD_GYM
	setflag FLAG_BADGE02_GET
	addvar VAR_PETALBURG_GYM_STATE, 1
	call_if_eq VAR_PETALBURG_GYM_STATE, 6, Common_EventScript_ReadyPetalburgGymForBattle
	setvar VAR_0x8008, 2
	call Common_EventScript_SetGymTrainers
	call DewfordTown_Gym_EventScript_GiveBulkUp
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox DewfordTown_Gym_Text_RegisteredBrawly, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_BRAWLY_MATCH_CALL
	setvar VAR_ROXANNE_CALL_STEP_COUNTER, 0
	setflag FLAG_ENABLE_ROXANNE_FIRST_CALL
	release
	end

DewfordTown_Gym_EventScript_GiveBulkUp::
	giveitem ITEM_TM_BULK_UP
	goto_if_eq VAR_RESULT, 0, Common_EventScript_BagIsFull
	msgbox DewfordTown_Gym_Text_ExplainBulkUp, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_TM_BULK_UP
	return

DewfordTown_Gym_EventScript_GiveBulkUp2:
	giveitem ITEM_TM_BULK_UP
	goto_if_eq VAR_RESULT, 0, Common_EventScript_ShowBagIsFull
	msgbox DewfordTown_Gym_Text_ExplainBulkUp, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_TM_BULK_UP
	release
	end

DewfordTown_Gym_EventScript_BrawlyRematch::
	trainerbattle_rematch_double TRAINER_BRAWLY_1, DewfordTown_Gym_Text_BrawlyPreRematch, DewfordTown_Gym_Text_BrawlyRematchDefeat, DewfordTown_Gym_Text_BrawlyRematchNeedTwoMons
	msgbox DewfordTown_Gym_Text_BrawlyPostRematch, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_Takao::
	trainerbattle_single TRAINER_TAKAO, DewfordTown_Gym_Text_TakaoIntro, DewfordTown_Gym_Text_TakaoDefeat, DewfordTown_Gym_EventScript_TakaoBrightenRoom
	msgbox DewfordTown_Gym_Text_TakaoPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_TakaoBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Jocelyn::
	trainerbattle_single TRAINER_JOCELYN, DewfordTown_Gym_Text_JocelynIntro, DewfordTown_Gym_Text_JocelynDefeat, DewfordTown_Gym_EventScript_JocelynBrightenRoom
	msgbox DewfordTown_Gym_Text_JocelynPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_JocelynBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Laura::
	trainerbattle_single TRAINER_LAURA, DewfordTown_Gym_Text_LauraIntro, DewfordTown_Gym_Text_LauraDefeat, DewfordTown_Gym_EventScript_LauraBrightenRoom
	msgbox DewfordTown_Gym_Text_LauraPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_LauraBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Brenden::
	trainerbattle_single TRAINER_BRENDEN, DewfordTown_Gym_Text_BrendenIntro, DewfordTown_Gym_Text_BrendenDefeat, DewfordTown_Gym_EventScript_BrendenBrightenRoom
	msgbox DewfordTown_Gym_Text_BrendenPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_BrendenBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Cristian::
	trainerbattle_single TRAINER_CRISTIAN, DewfordTown_Gym_Text_CristianIntro, DewfordTown_Gym_Text_CristianDefeat, DewfordTown_Gym_EventScript_CristianBrightenRoom
	msgbox DewfordTown_Gym_Text_CristianPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_CristianBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Lilith::
	trainerbattle_single TRAINER_LILITH, DewfordTown_Gym_Text_LilithIntro, DewfordTown_Gym_Text_LilithDefeat, DewfordTown_Gym_EventScript_LilithBrightenRoom
	msgbox DewfordTown_Gym_Text_LilithPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_LilithBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_DEWFORD_GYM, DewfordTown_Gym_EventScript_GymGuidePostVictory
	msgbox DewfordTown_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

DewfordTown_Gym_EventScript_GymGuidePostVictory::
	msgbox DewfordTown_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

DewfordTown_Gym_EventScript_LeftGymStatue::
	lockall
	goto_if_set FLAG_BADGE02_GET, DewfordTown_Gym_EventScript_GymStatueCertified
	goto DewfordTown_Gym_EventScript_GymStatue
	end

DewfordTown_Gym_EventScript_RightGymStatue::
	lockall
	goto_if_set FLAG_BADGE02_GET, DewfordTown_Gym_EventScript_GymStatueCertified
	goto DewfordTown_Gym_EventScript_GymStatue
	end

DewfordTown_Gym_EventScript_GymStatueCertified::
	msgbox DewfordTown_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

DewfordTown_Gym_EventScript_GymStatue::
	msgbox DewfordTown_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

DewfordTown_Gym_Text_GymGuideAdvice:
	.string "げんき　しとぉや!!\n"
	.string "チャンピオンを　めざす　主!!\p"
	.string "ムロタウン　ジムの　リーダー　トウキは\n"
	.string "かくとうタイプ　ポケモンの　つかいてだ!\p"
	.string "ノーマルタイプで　むかうと\n"
	.string "きびしぃー　いちげきを　くらうから\l"
	.string "きを　つけたほうが　いいぞ!\p"
	.string "しかも!　この　ジムの　なかは\n"
	.string "うみのそこ　みたいに　くらいんだ!\p"
	.string "でも!　トレーナーを　ひとり　たおすごとに\n"
	.string "だんだん　あかるく　なって　いくぞ!\p"
	.string "じゃ　きばってこォー!!$"

DewfordTown_Gym_Text_GymGuidePostVictory:
	.string "うおォ!　ずいぶん　あかるく　なったな!\p"
	.string "きみの　みらいも　あかるいぞ!$"

DewfordTown_Gym_Text_TakaoIntro:
	.string "ちから　こそ　せいぎっ!\n"
	.string "ドリャーッ!!$"

DewfordTown_Gym_Text_TakaoDefeat:
	.string "ちからぶそく　だった……!$"

DewfordTown_Gym_Text_TakaoPostBattle:
	.string "じゅう　よく　ごうを　せいす!\n"
	.string "みごとな　たたかいぶり　だった!$"

DewfordTown_Gym_Text_JocelynIntro:
	.string "トウキさまと　たたかうまでも　ないわ!\n"
	.string "わたしが　たおして　あげる!$"

DewfordTown_Gym_Text_JocelynDefeat:
	.string "まさか……\n"
	.string "こんなに　かんたんに　やられるなんて$"

DewfordTown_Gym_Text_JocelynPostBattle:
	.string "この　まけを　むだに　しないため\n"
	.string "もっと　とっくん　する!\p"
	.string "そして　いつかは　ジムリーダーに……\p"
	.string "いえ　ポケモンリーグの\n"
	.string "チャンピオンに　なるわよ!$"

DewfordTown_Gym_Text_LauraIntro:
	.string "あたしを　あまく　みない　ことね!\n"
	.string "ないても　しらないわよ!$"

DewfordTown_Gym_Text_LauraDefeat:
	.string "わぁーーーん!\n"
	.string "まけたーーーっ!$"

DewfordTown_Gym_Text_LauraPostBattle:
	.string "くらやみの　なかで　たたかうことで\n"
	.string "こころを　きたえるの\p"
	.string "でも　まっくら　だから\n"
	.string "トウキさまの　おかおを　みられない……$"

DewfordTown_Gym_Text_LilithIntro:
	.string "うふふ……!\n"
	.string "くらくて　まよっているの　かな?$"

DewfordTown_Gym_Text_LilithDefeat:
	.string "なによ　もう\n"
	.string "しらない!$"

DewfordTown_Gym_Text_LilithPostBattle:
	.string "あなた　なんて　このまま　まよって\n"
	.string "いりぐちに　もどっちゃえば　いーのよ!$"

DewfordTown_Gym_Text_BrendenIntro:
	.string "うみの　おとこの　いじを\n"
	.string "みせつけて　やるぜぃ!$"

DewfordTown_Gym_Text_BrendenDefeat:
	.string "どうしたぃ!\n"
	.string "おれと　した　ことが……$"

DewfordTown_Gym_Text_BrendenPostBattle:
	.string "おう!　あんたなら\n"
	.string "ジムリーダーに　いどむに　ふさわしいぜぃ!$"

DewfordTown_Gym_Text_CristianIntro:
	.string "ここを　とおる　なら\n"
	.string "この　わしを　たおしてから　じゃ!$"

DewfordTown_Gym_Text_CristianDefeat:
	.string "むうーん!\n"
	.string "すごく　つよい……!$"

DewfordTown_Gym_Text_CristianPostBattle:
	.string "わしの　まけじゃ!\n"
	.string "がんばって　バッジ　とってこいよ!$"

DewfordTown_Gym_Text_BrawlyIntro:
	.string "ボクは　ムロ　ポケモンジム\n"
	.string "リーダーの　トウキ!\p"
	.string "この　あたりの\n"
	.string "はげしい　なみに　もまれ……\p"
	.string "くらい　どうくつで\n"
	.string "しゅぎょう　して　いるのさ!\p"
	.string "ボクに　ちょうせん　しよう　という\n"
	.string "キミの　じつりょく……\l"
	.string "どれほどの　ものか　みせて　もらうよ!$"

DewfordTown_Gym_Text_BrawlyDefeat:
	.string "ううーん　よそう　していたより\n"
	.string "キミは　ビッグウェーブ!\p"
	.string "わかった!\n"
	.string "この　バッジを　もっていきな!$"

DewfordTown_Gym_Text_ReceivedKnuckleBadge:
	.string "{PLAYER}は　トウキから\n"
	.string "ナックルバッジを　もらった!$"

DewfordTown_Gym_Text_KnuckleBadgeInfoTakeThis:
	.string "ナックルバッジには\n"
	.string "レベル30までの　ポケモンなら\l"
	.string "ひとから　もらった　ポケモンでも\l"
	.string "すなおに　する　こうかが　ある!\p"
	.string "それに　たたかって　いなくても\n"
	.string "ひでんわざの　フラッシュを　つかえる!\p"
	.string "この　わざマシン\n"
	.string "よかったら　つかいな!$"

DewfordTown_Gym_Text_ExplainBulkUp:
	.string "その　わざマシン08　には\n"
	.string "ビルドアップが　はいっている!\p"
	.string "こうげきりょくと　ぼうぎょりょくを\n"
	.string "あげる　わざ　なのさ!\p"
	.string "……　……　……$"

DewfordTown_Gym_Text_RegisteredBrawly:
	.string "ジムリーダー　トウキを\n"
	.string "ポケナビに　とうろく　した!$"

DewfordTown_Gym_Text_BrawlyPostBattle:
	.string "キミの　さいのうは\n"
	.string "いつのひか　おおきな　なみと　なって\p"
	.string "ポケモン　トレーナーたちの　あいだに\n"
	.string "おどろきの　あらしを　まきおこすだろう!$"

DewfordTown_Gym_Text_GymStatue:
	.string "ムロタウン　ポケモンジム$"

DewfordTown_Gym_Text_GymStatueCertified:
	.string "ムロタウン　ポケモンジム\p"
	.string "トウキ　にんてい　トレーナー!\n"
	.string "{PLAYER}$"

DewfordTown_Gym_Text_BrawlyPreRematch:
	.string "トウキ『なみというのは　いちど　ひいても\n"
	.string "いつか　かならず　はまべに　もどってくる\p"
	.string "キミほどの　ビッグウェーブだ\n"
	.string "きっと　また　くると　おもっていたよ!\p"
	.string "さあ　どれほど　たかさを　ましたのか\n"
	.string "ボクに　みせてくれ!$"

DewfordTown_Gym_Text_BrawlyRematchDefeat:
	.string "わーおっ!\n"
	.string "キミに　のみこまれそうだ!$"

DewfordTown_Gym_Text_BrawlyPostRematch:
	.string "トウキ『やっぱり　ポケモン　しょうぶは\n"
	.string "そこが　みえないほど　ふかく　ひろい!\l"
	.string "まるで　ホウエンの　うみの　ようだね!$"

DewfordTown_Gym_Text_BrawlyRematchNeedTwoMons:
	.string "トウキ『なみというのは　いちど　ひいても\n"
	.string "いつか　かならず　はまべに　もどってくる\p"
	.string "キミほどの　ビッグウェーブだ\n"
	.string "きっと　また　くると　おもっていたよ!\p"
	.string "……あれ?　たたかえる　ポケモンを\n"
	.string "1ひきしか　もっていないんだね\p"
	.string "すまないけど　2ひき　いじょうの\n"
	.string "ポケモンを　もって　また　きてくれよ$"
