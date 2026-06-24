RustboroCity_Gym_MapScripts::
	.byte 0

RustboroCity_Gym_EventScript_Roxanne::
	trainerbattle_single TRAINER_ROXANNE_1, RustboroCity_Gym_Text_RoxanneIntro, RustboroCity_Gym_Text_RoxanneDefeat, RustboroCity_Gym_EventScript_RoxanneDefeated, NO_MUSIC
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_Gym_EventScript_RoxanneRematch
	goto_if_unset FLAG_RECEIVED_TM_ROCK_TOMB, RustboroCity_Gym_EventScript_GiveRockTomb
	msgbox RustboroCity_Gym_Text_RoxannePostBattle, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_RoxanneDefeated::
	message RustboroCity_Gym_Text_ReceivedStoneBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox RustboroCity_Gym_Text_StoneBadgeInfoTakeThis, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_RUSTBORO_GYM
	setflag FLAG_BADGE01_GET
	setvar VAR_RUSTBORO_CITY_STATE, 1
	addvar VAR_PETALBURG_GYM_STATE, 1
	setvar VAR_0x8008, 1
	call Common_EventScript_SetGymTrainers
	call_if_eq VAR_PETALBURG_GYM_STATE, 6, Common_EventScript_ReadyPetalburgGymForBattle
	goto RustboroCity_Gym_EventScript_GiveRockTomb
	end

RustboroCity_Gym_EventScript_GiveRockTomb::
	giveitem ITEM_TM_ROCK_TOMB
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM_ROCK_TOMB
	msgbox RustboroCity_Gym_Text_ExplainRockTomb, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_RoxanneRematch::
	trainerbattle_rematch_double TRAINER_ROXANNE_1, RustboroCity_Gym_Text_RoxannePreRematch, RustboroCity_Gym_Text_RoxanneRematchDefeat, RustboroCity_Gym_Text_RoxanneRematchNeedTwoMons
	msgbox RustboroCity_Gym_Text_RoxannePostRematch, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Josh::
	trainerbattle_single TRAINER_JOSH, RustboroCity_Gym_Text_JoshIntro, RustboroCity_Gym_Text_JoshDefeat
	msgbox RustboroCity_Gym_Text_JoshPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Tommy::
	trainerbattle_single TRAINER_TOMMY, RustboroCity_Gym_Text_TommyIntro, RustboroCity_Gym_Text_TommyDefeat
	msgbox RustboroCity_Gym_Text_TommyPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Marc::
	trainerbattle_single TRAINER_MARC, RustboroCity_Gym_Text_MarcIntro, RustboroCity_Gym_Text_MarcDefeat
	msgbox RustboroCity_Gym_Text_MarcPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_RUSTBORO_GYM, RustboroCity_Gym_EventScript_GymGuidePostVictory
	msgbox RustboroCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_GymGuidePostVictory::
	msgbox RustboroCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_LeftGymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_RightGymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_GymStatueCertified::
	msgbox RustboroCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_GymStatue::
	msgbox RustboroCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_RegisterRoxanne::
	lockall
	pokenavcall RustboroCity_Gym_Text_RoxanneRegisterCall
	waitmessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox RustboroCity_Gym_Text_RegisteredRoxanne, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_ROXANNE_MATCH_CALL
	clearflag FLAG_ENABLE_ROXANNE_FIRST_CALL
	releaseall
	end

RustboroCity_Gym_Text_GymGuideAdvice:
	.string "げんき　しとぉや!!\p"
	.string "いきなり　だが　きみぃ!\n"
	.string "チャンピオンを　めざして　みないか!\p"
	.string "ぼくは　トレーナー　じゃないけど\n"
	.string "かつ　ために　ばっちり\l"
	.string "アドバイス　できる!\p"
	.string "よし　きまった!　いっしょに\n"
	.string "ポケモン　チャンピオン　めざそう!\p"
	.string "トレーナーの　もくてき　といえば\n"
	.string "ジムバッジを　あつめること　だろう\l"
	.string "だが　ジムリーダーは　てごわい!\p"
	.string "そこで　ぼくの　でばん　というわけだ!\n"
	.string "きみに　ばっちり　アドバイス　するよ!\p"
	.string "ここの　ジムリーダー　ツツジは\n"
	.string "いわタイプの　ポケモンの　つかいて!\p"
	.string "いわタイプは　がんじょう　だけど\n"
	.string "みずタイプや　くさタイプの\l"
	.string "こうげきは　にがて　なんだよ!\p"
	.string "ジムリーダーに　かてたら\n"
	.string "また　ぼくに　はなしかけて　くれよ!\p"
	.string "じゃ　きばってこォー!!$"

RustboroCity_Gym_Text_GymGuidePostVictory:
	.string "うおォ!　みごとな　しょうり!!\n"
	.string "ぼくの　おうえんが　よかったのか!?\p"
	.string "よし!　きみの\n"
	.string "トレーナーカードを　みたまえ!\p"
	.string "さっき　てにいれた　ストーンバッジが\n"
	.string "きちんと　きろく　されている　だろう\p"
	.string "つまり……\p"
	.string "きみは　チャンピオンへの　かいだんを\n"
	.string "いっぽ　のぼったんだ!\p"
	.string "ごったましィー!!$"

RustboroCity_Gym_Text_JoshIntro:
	.string "ジム　トレーナーを　なめるなよ!\n"
	.string "じつりょくの　ちがいを　みせてやる!$"

RustboroCity_Gym_Text_JoshDefeat:
	.string "じつりょくが　ちがいすぎた……!$"

RustboroCity_Gym_Text_JoshPostBattle:
	.string "いわタイプの　おそろしさ\n"
	.string "まだまだ　こんな　ものじゃない!$"

RustboroCity_Gym_Text_TommyIntro:
	.string "ボクに　かてる　じつりょくが　ないと\n"
	.string "ツツジさん　には　ぜったい　かなわないよ!$"

RustboroCity_Gym_Text_TommyDefeat:
	.string "すごい　じつりょくだ!$"

RustboroCity_Gym_Text_TommyPostBattle:
	.string "ツツジさんは　ボクの　すうばい　つよい!\n"
	.string "ゆだん　しない　ほうが　いいよ!$"

RustboroCity_Gym_Text_MarcIntro:
	.string "こんな　まちなか　でも\n"
	.string "いわタイプで　おおあばれだ!$"

RustboroCity_Gym_Text_MarcDefeat:
	.string "おおーっ!\n"
	.string "いきの　いい　ちょうせんしゃ　だな!$"

RustboroCity_Gym_Text_MarcPostBattle:
	.string "わかくして　いわタイプに\n"
	.string "めを　つける　なんて\l"
	.string "うちの　ジムリーダーは　センスが　いいぜ!$"

RustboroCity_Gym_Text_RoxanneIntro:
	.string "わたくし　カナズミ　ポケモンジム\n"
	.string "リーダーの　ツツジ　といいます\p"
	.string "トレーナーズ　スクールで\n"
	.string "まなんだこと　しょうぶに　いかしたくて\l"
	.string "ジムリーダーに　なったの\p"
	.string "あなた　どのような　ポケモンで\n"
	.string "どんなふうに　たたかうのか\l"
	.string "わたくしに　おしえて　くださるかしら?$"

RustboroCity_Gym_Text_RoxanneDefeat:
	.string "わたくし　まけたのね……\p"
	.string "もっと　おおくの　ことを\n"
	.string "まなぶ　ひつようが　あるみたい……\p"
	.string "わかりましたわ\p"
	.string "ポケモンリーグの　きまりでは\n"
	.string "ジムリーダーに　かった　トレーナーに\l"
	.string "これを　わたすことに　なっています\p"
	.string "どうぞ　ポケモンリーグ　こうにんの\n"
	.string "ストーンバッジ　うけとって　ください$"

RustboroCity_Gym_Text_ReceivedStoneBadge:
	.string "{PLAYER}は　ツツジから\n"
	.string "ストーンバッジを　もらった!$"

RustboroCity_Gym_Text_StoneBadgeInfoTakeThis:
	.string "ストーンバッジ　には　あなたの\n"
	.string "ポケモンの　こうげきりょくを\l"
	.string "たかめる　こうかが　あるの\p"
	.string "あと　たたかっていない　ときでも\n"
	.string "ひでんわざの　いあいぎり　が\l"
	.string "つかえるようにも　なるわ\p"
	.string "それから……\n"
	.string "これも　もっていって　ください$"

RustboroCity_Gym_Text_ExplainRockTomb:
	.string "その　わざマシン39の　なかには\n"
	.string "がんせきふうじが　はいっているわ\p"
	.string "いわを　あいてに　むかって　おとして\n"
	.string "ダメージを　あたえる　だけでなくて\l"
	.string "すばやさも　さげる　わざ　なの\p"
	.string "ちなみに\n"
	.string "わざマシンを　つかえば　ポケモンに\l"
	.string "いっしゅんで　わざを　おしえられるの\p"
	.string "ただし　つかいすて　だから\n"
	.string "つかうときは　よく　かんがえてね$"

RustboroCity_Gym_Text_RoxannePostBattle:
	.string "それだけ　つよいなら\n"
	.string "ほかの　ポケモンジムにも\l"
	.string "ちょうせん　したら　どうかしら?\p"
	.string "おおくの　トレーナーと\n"
	.string "しょうぶを　することで\l"
	.string "さまざまなこと　まなべるはずよ$"

RustboroCity_Gym_Text_GymStatue:
	.string "カナズミシティ　ポケモンジム$"

RustboroCity_Gym_Text_GymStatueCertified:
	.string "カナズミシティ　ポケモンジム\p"
	.string "ツツジ　にんてい　トレーナー!\n"
	.string "{PLAYER}$"

RustboroCity_Gym_Text_RoxanneRegisterCall:
	.string "……　……　……\n"
	.string "……　……　……　ピッ!\p"
	.string "ツツジ『あっ　主さん　ですか?\n"
	.string "わたくし　カナズミの　ツツジです!\p"
	.string "トウキさんから　ききましたよぉ\n"
	.string "あなた　すごい　いきおいで\l"
	.string "かちすすんで　いるらしい　ですね!\p"
	.string "こちらも　まけっぱなしでは　いられません\n"
	.string "もういちど　ジムを　きたえなおして\l"
	.string "いつかまた　ちょうせんを　うけますよ!\p"
	.string "そのひが　くるのを\n"
	.string "どうぞ　まっていて　くださいね!\p"
	.string "……　……　……\n"
	.string "……　……　……　ツー$"

RustboroCity_Gym_Text_RegisteredRoxanne:
	.string "ジムリーダー　ツツジを\n"
	.string "ポケナビに　とうろく　した!$"

RustboroCity_Gym_Text_RoxannePreRematch:
	.string "ツツジ『また　おめにかかれましたね\n"
	.string "ジムリーダーの　ツツジ　です\p"
	.string "あれから　おたがいに\n"
	.string "いくつもの　しょうぶを　けいけんし\l"
	.string "どれほど　つよく　なれたのか……\p"
	.string "さあ　おてあわせ　ねがいます!$"

RustboroCity_Gym_Text_RoxanneRematchDefeat:
	.string "くうぅ……\n"
	.string "またしても　わたくしの　まけ……$"

RustboroCity_Gym_Text_RoxannePostRematch:
	.string "ツツジ『まだまだ　ポケモン　しょうぶは\n"
	.string "まなぶことが　たくさん　あるのですね\p"
	.string "わたくし……　ポケモン　しょうぶが\n"
	.string "ますます　すきに　なりましたよ!$"

RustboroCity_Gym_Text_RoxanneRematchNeedTwoMons:
	.string "ツツジ『また　おめにかかれましたね\n"
	.string "ジムリーダーの　ツツジ　です\p"
	.string "あれから　おたがいに\n"
	.string "いくつもの　しょうぶを　けいけんし\l"
	.string "どれほど　つよく　なれたのか……\p"
	.string "……あら?　たたかえる　ポケモンを\n"
	.string "1ひきしか　もっていませんね\p"
	.string "どうぞ　2ひき　いじょうの\n"
	.string "ポケモンを　もって　きてください$"
