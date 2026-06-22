LittlerootTown_BrendansHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, LittlerootTown_BrendansHouse_1F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_BrendansHouse_1F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LittlerootTown_BrendansHouse_1F_OnFrame
	.byte 0

LittlerootTown_BrendansHouse_1F_OnLoad:
	call_if_lt VAR_LITTLEROOT_INTRO_STATE, 6, LittlerootTown_BrendansHouse_1F_EventScript_SetMovingBoxes
	call_if_set FLAG_RECEIVED_RUNNING_SHOES, LittlerootTown_BrendansHouse_1F_EventScript_CheckShowShoesManual
	end

LittlerootTown_BrendansHouse_1F_EventScript_SetMovingBoxes::
	setmetatile 5, 4, METATILE_BrendansMaysHouse_MovingBox_Open, TRUE
	setmetatile 5, 2, METATILE_BrendansMaysHouse_MovingBox_Closed, TRUE
	return

LittlerootTown_BrendansHouse_1F_EventScript_CheckShowShoesManual::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LittlerootTown_BrendansHouse_1F_EventScript_ShowRunningShoesManual
	return

LittlerootTown_BrendansHouse_1F_EventScript_ShowRunningShoesManual::
	setmetatile 3, 7, METATILE_BrendansMaysHouse_BookOnTable, TRUE
	return

LittlerootTown_BrendansHouse_1F_OnTransition:
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 3, LittlerootTown_BrendansHouse_1F_EventScript_MoveMomToDoor
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 5, LittlerootTown_BrendansHouse_1F_EventScript_MoveMomToStairs
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 6, LittlerootTown_BrendansHouse_1F_EventScript_MoveMomToTV
	end

LittlerootTown_BrendansHouse_1F_EventScript_MoveMomToStairs::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 8, 4
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

LittlerootTown_BrendansHouse_1F_EventScript_MoveMomToTV::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 4, 5
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

LittlerootTown_BrendansHouse_1F_EventScript_MoveMomToDoor::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 9, 8
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

@ Many of the below scripts have no gender check because they assume youre in the correct house
@ The below SS Ticket script uses Mays house state by accident(?), but theyre both set identically after the intro
LittlerootTown_BrendansHouse_1F_OnFrame:
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 3, LittlerootTown_BrendansHouse_1F_EventScript_EnterHouseMovingIn
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 5, LittlerootTown_BrendansHouse_1F_EventScript_GoUpstairsToSetClock
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 6, LittlerootTown_BrendansHouse_1F_EventScript_PetalburgGymReport
	map_script_2 VAR_LITTLEROOT_HOUSES_STATE_MAY, 1, LittlerootTown_BrendansHouse_1F_EventScript_YoureNewNeighbor
	map_script_2 VAR_LITTLEROOT_HOUSES_STATE_MAY, 3, PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV
	.2byte 0

LittlerootTown_BrendansHouse_1F_EventScript_GoUpstairsToSetClock::
	lockall
	msgbox PlayersHouse_1F_Text_GoSetTheClock, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, LittlerootTown_BrendansHouse_1F_Movement_PushTowardStairs
	applymovement LOCALID_PLAYERS_HOUSE_1F_MOM, LittlerootTown_BrendansHouse_1F_Movement_PushTowardStairs
	waitmovement 0
	warp MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F, 7, 1
	waitstate
	releaseall
	end

LittlerootTown_BrendansHouse_1F_Movement_PushTowardStairs:
	walk_up
	step_end

LittlerootTown_BrendansHouse_1F_EventScript_EnterHouseMovingIn::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, MALE
	goto PlayersHouse_1F_EventScript_EnterHouseMovingIn
	end

LittlerootTown_BrendansHouse_1F_EventScript_PetalburgGymReport::
	lockall
	setvar VAR_0x8004, MALE
	setvar VAR_0x8005, LOCALID_PLAYERS_HOUSE_1F_MOM
	goto PlayersHouse_1F_EventScript_PetalburgGymReportMale
	end

LittlerootTown_BrendansHouse_1F_EventScript_YoureNewNeighbor::
	lockall
	playse SE_PIN
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, LittlerootTown_BrendansHouse_1F_Movement_RivalMomApproach
	waitmovement 0
	special GetRivalSonDaughterString
	msgbox RivalsHouse_1F_Text_OhYoureTheNewNeighbor, MSGBOX_DEFAULT
	setflag FLAG_MET_RIVAL_MOM
	setvar VAR_LITTLEROOT_HOUSES_STATE_MAY, 2
	releaseall
	end

LittlerootTown_BrendansHouse_1F_Movement_RivalMomApproach:
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

LittlerootTown_BrendansHouse_1F_EventScript_GoSeeRoom::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, MALE
	applymovement VAR_0x8004, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	goto PlayersHouse_1F_EventScript_MomGoSeeRoom
	end

LittlerootTown_BrendansHouse_1F_EventScript_MeetRival0::
	lockall
	setvar VAR_0x8008, 0
	goto LittlerootTown_BrendansHouse_1F_EventScript_MeetRival
	end

LittlerootTown_BrendansHouse_1F_EventScript_MeetRival1::
	lockall
	setvar VAR_0x8008, 1
	goto LittlerootTown_BrendansHouse_1F_EventScript_MeetRival
	end

LittlerootTown_BrendansHouse_1F_EventScript_MeetRival2::
	lockall
	setvar VAR_0x8008, 2
	goto LittlerootTown_BrendansHouse_1F_EventScript_MeetRival
	end

LittlerootTown_BrendansHouse_1F_EventScript_MeetRival::
	playse SE_EXIT
	delay 10
	addobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	delay 30
	playse SE_PIN
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_Delay48
	waitmovement 0
	call_if_ne VAR_0x8008, 1, LittlerootTown_BrendansHouse_1F_EventScript_PlayerFaceBrendan
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	call_if_eq VAR_0x8008, 0, LittlerootTown_BrendansHouse_1F_EventScript_BrendanApproachPlayer0
	call_if_eq VAR_0x8008, 1, LittlerootTown_BrendansHouse_1F_EventScript_BrendanApproachPlayer1
	call_if_eq VAR_0x8008, 2, LittlerootTown_BrendansHouse_1F_EventScript_BrendanApproachPlayer2
	msgbox RivalsHouse_1F_Text_BrendanWhoAreYou, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 0, LittlerootTown_BrendansHouse_1F_EventScript_BrendanGoUpstairs0
	call_if_eq VAR_0x8008, 1, LittlerootTown_BrendansHouse_1F_EventScript_BrendanGoUpstairs1
	call_if_eq VAR_0x8008, 2, LittlerootTown_BrendansHouse_1F_EventScript_BrendanGoUpstairs2
	playse SE_EXIT
	removeobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_BRENDAN
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F_POKE_BALL
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_RIVAL_BEDROOM
	delay 30
	setvar VAR_LITTLEROOT_RIVAL_STATE, 3
	setvar VAR_LITTLEROOT_TOWN_STATE, 1
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

LittlerootTown_BrendansHouse_1F_EventScript_PlayerFaceBrendan::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	return

LittlerootTown_BrendansHouse_1F_EventScript_BrendanApproachPlayer0::
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_BrendansHouse_1F_Movement_BrendanApproachPlayer0
	waitmovement 0
	return

LittlerootTown_BrendansHouse_1F_EventScript_BrendanApproachPlayer1::
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_BrendansHouse_1F_Movement_BrendanApproachPlayer1
	waitmovement 0
	return

LittlerootTown_BrendansHouse_1F_EventScript_BrendanApproachPlayer2::
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_BrendansHouse_1F_Movement_BrendanApproachPlayer2
	waitmovement 0
	return

LittlerootTown_BrendansHouse_1F_Movement_BrendanApproachPlayer0:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_BrendansHouse_1F_Movement_BrendanApproachPlayer1:
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_BrendansHouse_1F_Movement_BrendanApproachPlayer2:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_BrendansHouse_1F_EventScript_BrendanGoUpstairs0::
	applymovement LOCALID_PLAYER, LittlerootTown_BrendansHouse_1F_Movement_PlayerWatchBrendanExit0
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_BrendansHouse_1F_Movement_BrendanGoUpstairs0
	waitmovement 0
	return

LittlerootTown_BrendansHouse_1F_EventScript_BrendanGoUpstairs1::
	applymovement LOCALID_PLAYER, LittlerootTown_BrendansHouse_1F_Movement_PlayerWatchBrendanExit1
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_BrendansHouse_1F_Movement_BrendanGoUpstairs1
	waitmovement 0
	return

LittlerootTown_BrendansHouse_1F_EventScript_BrendanGoUpstairs2::
	applymovement LOCALID_PLAYER, LittlerootTown_BrendansHouse_1F_Movement_PlayerWatchBrendanExit2
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_BrendansHouse_1F_Movement_BrendanGoUpstairs2
	waitmovement 0
	return

LittlerootTown_BrendansHouse_1F_Movement_PlayerWatchBrendanExit0:
	delay_16
	delay_8
	walk_in_place_faster_right
	step_end

LittlerootTown_BrendansHouse_1F_Movement_PlayerWatchBrendanExit1:
	delay_16
	delay_8
	walk_in_place_faster_right
	delay_16
	delay_16
	walk_in_place_faster_up
	step_end

LittlerootTown_BrendansHouse_1F_Movement_PlayerWatchBrendanExit2:
	delay_16
	delay_8
	walk_in_place_faster_left
	step_end

LittlerootTown_BrendansHouse_1F_Movement_BrendanGoUpstairs0:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

LittlerootTown_BrendansHouse_1F_Movement_BrendanGoUpstairs1:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	step_end

LittlerootTown_BrendansHouse_1F_Movement_BrendanGoUpstairs2:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

PlayersHouse_1F_Text_IsntItNiceInHere:
	.string "ママ『ほら　{PLAYER}!\n"
	.string "おうちの　なかも　すてき　でしょ!$"

PlayersHouse_1F_Text_MoversPokemonGoSetClock:
	.string "おうちの　かたづけは\n"
	.string "ひっこしやさんの　ポケモンが\l"
	.string "てつだって　くれるから　らくちんね!\p"
	.string "{PLAYER}も　2かいに　ある\n"
	.string "じぶんの　おへやに　いってごらん!\p"
	.string "パパが　ひっこしの　おいわいに\n"
	.string "かってくれた　とけいが　あるから\l"
	.string "じかんを　あわせて　おきなさいよ$"

PlayersHouse_1F_Text_ArentYouInterestedInRoom:
	.string "ママ『{PLAYER}\p"
	.string "じぶんの　へやを　みてこないの?$"

PlayersHouse_1F_Text_GoSetTheClock:
	.string "ママ『{PLAYER}\p"
	.string "おへやの　とけい　あわせてきて$"

PlayersHouse_1F_Text_OhComeQuickly:
	.string "ママ『……あ!　{PLAYER}, {PLAYER}!\n"
	.string "はやく　こっちに　いらっしゃい!$"

PlayersHouse_1F_Text_MaybeDadWillBeOn:
	.string "ママ『トウカの　ジムが　うつってるわ\n"
	.string "パパが　でるかもよ!!$"

PlayersHouse_1F_Text_ItsOverWeMissedHim:
	.string "ママ『あらら……　おわっちゃった\p"
	.string "パパが　でてたみたい　だったのに　ざんねんね$"

PlayersHouse_1F_Text_GoIntroduceYourselfNextDoor:
	.string "……あ　そうそう!　このまち　には\n"
	.string "オダマキはかせ　っていう\l"
	.string "パパの　おともだちが　いるの\p"
	.string "はかせの　いえは　おとなり　だから\n"
	.string "きちんと　あいさつ　してくると　いいわ!$"

PlayersHouse_1F_Text_SeeYouHoney:
	.string "ママ『いってらっしゃい!$"

PlayersHouse_1F_Text_DidYouMeetProfBirch:
	.string "ママ『オダマキはかせ　には　あえたの?$"

PlayersHouse_1F_Text_YouShouldRestABit:
	.string "ママ『どう?　{PLAYER}\n"
	.string "すこし　つかれたんじゃ　ないかしら?\p"
	.string "おうちで　やすんでいくと　いいわ$"

PlayersHouse_1F_Text_TakeCareHoney:
	.string "ママ『じゃあ　きをつけてね!$"

PlayersHouse_1F_Text_GotDadsBadgeHeresSomethingFromMom:
	.string "ママ『あら\n"
	.string "その　バッジ　パパから　もらったの!?\p"
	.string "じゃあ　ママも　これ　あげようかな$"

PlayersHouse_1F_Text_DontPushYourselfTooHard:
	.string "あんまり　むりは　しないでね\n"
	.string "なにか　あれば　かえって　くればいいから\p"
	.string "じゃあ　がんばってね!$"

PlayersHouse_1F_Text_IsThatAPokenav:
	.string "ママ『なあに　それ?　ポケナビ?\n"
	.string "デボンの　ひとから　いただいたの?\p"
	.string "じゃあ　ママの　ことも\n"
	.string "とうろくして　もらっちゃおうかな!\p"
	.string "……　……　……$"

PlayersHouse_1F_Text_RegisteredMom:
	.string "ママを\n"
	.string "ポケナビに　とうろく　した!$"

PlayersHouse_1F_Text_Vigoroth1:
	.string "ふぎーっ$"

PlayersHouse_1F_Text_Vigoroth2:
	.string "ふごっ　ふご　ふご……$"

PlayersHouse_1F_Text_ReportFromPetalburgGym:
	.string "インタビュアー『……　いじょう\n"
	.string "トウカジムの　まえ　からでした!$"

PlayersHouse_1F_Text_TheresAMovieOnTV:
	.string "テレビで　えいがを　やってる!\p"
	.string "おとなと　こどもが　ふたり　ならんで\n"
	.string "おおきな　ピアノの　うえで　おどってる!\p"
	.string "もう　いかなきゃ!$"

PlayersHouse_1F_Text_RunningShoesManual:
	.string "ランニングシューズの\n"
	.string "せつめいしょが　ある!\p"
	.string "「ランニングシューズは\n"
	.string "　Bボタンを　おすことで　いままでよりも\l"
	.string "　はやく　はしれるように　なります!\p"
	.string "　ランニングシューズを　はいて\n"
	.string "　おもいっきり　そとを　はしろう!」$"

PlayersHouse_1F_Text_TicketFromBrineyCameForYou:
	.string "パパ『ん?\n"
	.string "{PLAYER}じゃ　ないか!\p"
	.string "ひさしぶりに　かおを　みたが\n"
	.string "どこか　たくましくなった……\l"
	.string "そんな　かんじ　だな\p"
	.string "……おっと　そうだった\n"
	.string "ハギさん　という　ひとから\l"
	.string "おまえあてに　なにか　とどいていたぞ!$"

PlayersHouse_1F_Text_PortsInSlateportLilycove:
	.string "パパ『ふねのチケットか……\p"
	.string "ふねに　のるなら　カイナか　ミナモに\n"
	.string "ふねのりばが　あったよな$"

PlayersHouse_1F_Text_BetterGetBackToGym:
	.string "では　わたしは\n"
	.string "トウカの　ジムに　もどるよ\p"
	.string "ママ\n"
	.string "いえのこと　たのんだよ$"

PlayersHouse_1F_Text_DadShouldStayLonger:
	.string "ママ『パパったら\n"
	.string "ひさしぶりに　かえって　きたのに\l"
	.string "ポケモンの　はなし　ばっかり!\p"
	.string "もっと　ゆっくり　すれば　いいのにね!$"

PlayersHouse_1F_Text_IsThatABreakingStory:
	.string "ママ『なにかの　ニュース　かしら?$"

PlayersHouse_1F_Text_LatiEmergencyNewsFlash:
	.string "りんじ　ニュース　です\p"
	.string "ホウエンちほうの　かくちで\n"
	.string "……いろの　ポケモンが　とんでいた\l"
	.string "という　じょうほうが　よせられています\p"
	.string "いまのところ　しょうたいは\n"
	.string "わかって　いない　ようです……\p"
	.string "では　ひきつづき\n"
	.string "えいがを　おたのしみ　ください$"

PlayersHouse_1F_Text_WhatColorDidTheySay:
	.string "ママ『ねえ　ねえ　{PLAYER}\n"
	.string "いまの　ニュース\l"
	.string "なにいろの　ポケモンって　いってたの?$"

PlayersHouse_1F_Text_StillUnknownPokemon:
	.string "ママ『へえー　まだまだ\n"
	.string "しられていない　ポケモンは　いるのねえ$"

