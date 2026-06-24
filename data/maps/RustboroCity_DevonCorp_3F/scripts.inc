RustboroCity_DevonCorp_3F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_DevonCorp_3F_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, RustboroCity_DevonCorp_3F_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, RustboroCity_DevonCorp_3F_OnFrame
	.byte 0

RustboroCity_DevonCorp_3F_OnTransition:
	call_if_eq VAR_DEVON_CORP_3F_STATE, 0, RustboroCity_DevonCorp_3F_EventScript_SetEmployeePos
	end

RustboroCity_DevonCorp_3F_EventScript_SetEmployeePos::
	setobjectxyperm LOCALID_DEVON_CORP_3F_EMPLOYEE, 3, 2
	setobjectmovementtype LOCALID_DEVON_CORP_3F_EMPLOYEE, MOVEMENT_TYPE_FACE_LEFT
	return

RustboroCity_DevonCorp_3F_OnWarp:
	map_script_2 VAR_DEVON_CORP_3F_STATE, 0, RustboroCity_DevonCorp_3F_EventScript_PlayerFaceEast
	.2byte 0

RustboroCity_DevonCorp_3F_EventScript_PlayerFaceEast::
	turnobject LOCALID_PLAYER, DIR_EAST
	end

RustboroCity_DevonCorp_3F_OnFrame:
	map_script_2 VAR_DEVON_CORP_3F_STATE, 0, RustboroCity_DevonCorp_3F_EventScript_MeetPresident
	.2byte 0

RustboroCity_DevonCorp_3F_EventScript_MeetPresident::
	lockall
	msgbox RustboroCity_DevonCorp_3F_Text_ThisIs3rdFloorWaitHere, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_EmployeeWalkOffscreen
	waitmovement 0
	delay 80
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_EmployeeReturnToPlayer
	waitmovement 0
	msgbox RustboroCity_DevonCorp_3F_Text_WordWithPresidentComeWithMe, MSGBOX_DEFAULT
	closemessage
	playbgm MUS_FOLLOW_ME, FALSE
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_LeadPlayerToPresident
	applymovement LOCALID_PLAYER, RustboroCity_DevonCorp_3F_Movement_PlayerFollowToPresident
	waitmovement 0
	msgbox RustboroCity_DevonCorp_3F_Text_PleaseGoAhead, MSGBOX_DEFAULT
	closemessage
	fadedefaultbgm
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_EmployeeFaceDesk
	applymovement LOCALID_PLAYER, RustboroCity_DevonCorp_3F_Movement_PlayerApproachDesk
	waitmovement 0
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneIHaveFavor, MSGBOX_DEFAULT
	giveitem ITEM_LETTER
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneWantYouToHaveThis, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message RustboroCity_DevonCorp_3F_Text_ReceivedPokenav
	waitfanfare
	setflag FLAG_SYS_POKENAV_GET
	setflag FLAG_RECEIVED_POKENAV
	setflag FLAG_HIDE_RUSTBORO_CITY_POKEMON_SCHOOL_SCOTT
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneExplainPokenavRestUp, MSGBOX_DEFAULT
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneGoWithCautionAndCare, MSGBOX_DEFAULT
	setflag FLAG_HIDE_ROUTE_116_WANDAS_BOYFRIEND
	clearflag FLAG_HIDE_RUSTURF_TUNNEL_WANDAS_BOYFRIEND
	clearflag FLAG_HIDE_RUSTURF_TUNNEL_WANDA
	clearflag FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY
	clearflag FLAG_HIDE_BRINEYS_HOUSE_PEEKO
	setvar VAR_BRINEY_LOCATION, 1
	clearflag FLAG_HIDE_RUSTBORO_CITY_RIVAL
	setvar VAR_DEVON_CORP_3F_STATE, 1
	setvar VAR_RUSTBORO_CITY_STATE, 6
	releaseall
	end

RustboroCity_DevonCorp_3F_Movement_Unused:
	walk_up
	walk_up
	walk_up
	walk_in_place_faster_left
	step_end

RustboroCity_DevonCorp_3F_Movement_LeadPlayerToPresident:
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_left
	step_end

RustboroCity_DevonCorp_3F_Movement_EmployeeFaceDesk:
	delay_16
	walk_in_place_faster_down
	step_end

RustboroCity_DevonCorp_3F_Movement_EmployeeWalkOffscreen:
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

RustboroCity_DevonCorp_3F_Movement_EmployeeReturnToPlayer:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	walk_left
	walk_left
	walk_left
	step_end

RustboroCity_DevonCorp_3F_Movement_PlayerFollowToPresident:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

RustboroCity_DevonCorp_3F_Movement_PlayerApproachDesk:
	delay_16
	delay_16
	walk_down
	walk_down
	walk_in_place_faster_right
	step_end

RustboroCity_DevonCorp_3F_EventScript_MrStone::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_EXP_SHARE, RustboroCity_DevonCorp_3F_EventScript_MrStoneAfterFavor
	goto_if_set FLAG_DELIVERED_STEVEN_LETTER, RustboroCity_DevonCorp_3F_EventScript_GiveExpShare
	msgbox RustboroCity_DevonCorp_3F_Text_CountingOnYou, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

RustboroCity_DevonCorp_3F_EventScript_GiveExpShare::
	msgbox RustboroCity_DevonCorp_3F_Text_ThankYouForDeliveringLetter, MSGBOX_DEFAULT
	giveitem ITEM_EXP_SHARE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_EXP_SHARE
	msgbox RustboroCity_DevonCorp_3F_Text_ExplainExpShare, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

RustboroCity_DevonCorp_3F_EventScript_MrStoneAfterFavor::
	msgbox RustboroCity_DevonCorp_3F_Text_NotFamiliarWithTrends, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

RustboroCity_DevonCorp_3F_EventScript_Employee::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_REPEAT_BALL, RustboroCity_DevonCorp_3F_EventScript_EmployeeBalls
	msgbox RustboroCity_DevonCorp_3F_Text_VisitCaptSternShipyard, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_3F_EventScript_EmployeeBalls::
	msgbox RustboroCity_DevonCorp_3F_Text_RepeatAndTimerHugelyPopular, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_3F_EventScript_RareRocksDisplay::
	msgbox RustboroCity_DevonCorp_3F_Text_RareRocksDisplay, MSGBOX_SIGN
	end

RustboroCity_DevonCorp_3F_Text_MrStoneIHaveFavor:
	.string "わしが　デボン　コーポレーション\n"
	.string "しゃちょうの　ツワブキ　だ!\p"
	.string "きみの　ことは　さっき　きいたよ!\p"
	.string "なんでも　うちの　けんきゅういんを\n"
	.string "2ども　たすけてくれた　とか\p"
	.string "……で　そんな　すごい　きみに\n"
	.string "たのみごとを　したいのだ!\p"
	.string "きみは　にもつを　とどけに\n"
	.string "カイナの　ぞうせんじょに　いくんだろう?\p"
	.string "その　とちゅうに\n"
	.string "ムロタウンにも　よって　ほしいのだよ!\p"
	.string "いやー　ムロタウンに\n"
	.string "ダイゴという　おとこが　いるはずでね\l"
	.string "ちょいと　てがみを　とどけてほしいのだ!$"

RustboroCity_DevonCorp_3F_Text_MrStoneWantYouToHaveThis:
	.string "ツワブキ『もちろん　わしは\n"
	.string "すごーい　しゃちょうで　あるからな!\p"
	.string "ただで　おねがいすると　いうような\n"
	.string "ケチな　まねは　しないのだ!\p"
	.string "きみに　こいつを　プレゼントしよう!$"

RustboroCity_DevonCorp_3F_Text_ReceivedPokenav:
	.string "{PLAYER}は\n"
	.string "ポケナビを　てにいれた!$"

RustboroCity_DevonCorp_3F_Text_MrStoneExplainPokenavRestUp:
	.string "ツワブキ『そいつは……\p"
	.string "ポケモン　ナビゲーター!\n"
	.string "りゃくして　ポケナビ　といってな\p"
	.string "ぼうけんを　する　トレーナーの\n"
	.string "ひつじゅひん　なのだよ!\p"
	.string "そいつで　ホウエンマップを　みれば\n"
	.string "ムロや　カイナの　ばしょ　だって\l"
	.string "ばっちり　わかると　いうものだ!\p"
	.string "……さいきんは　アクアだの　マグマだの\n"
	.string "といった　あやしげな　れんちゅうが\l"
	.string "あちこちで　わるさを　してるらしい\p"
	.string "まずは　ひとやすみ　していきなさい!$"

RustboroCity_DevonCorp_3F_Text_MrStoneGoWithCautionAndCare:
	.string "ツワブキ『それでは　{PLAYER}{KUN}\n"
	.string "じゅうぶん　きをつけて　いくのだよ!$"

RustboroCity_DevonCorp_3F_Text_CountingOnYou:
	.string "ツワブキ『よろしく　たのむよ!$"

RustboroCity_DevonCorp_3F_Text_ThankYouForDeliveringLetter:
	.string "ツワブキ『てがみを　とどけて　くれたんだね\n"
	.string "いやいや　ありがとう!\p"
	.string "こいつは　おれい　だよ\n"
	.string "トレーナーの　きみに　やくだつ　はずだ!$"

RustboroCity_DevonCorp_3F_Text_ExplainExpShare:
	.string "ツワブキ『がくしゅうそうちを　もたせた\n"
	.string "ポケモンは　しょうぶを　しなくても\l"
	.string "けいけんちを　わけて　もらえる!\p"
	.string "よわい　ポケモンを　そだてるのに\n"
	.string "なかなか　べんりな　どうぐ　なのだよ!$"

RustboroCity_DevonCorp_3F_Text_NotFamiliarWithTrends:
	.string "ツワブキ『わたしは　わかいころ　から\n"
	.string "しごと　ばかり　だったから\l"
	.string "いまいち　わからんのだが……\p"
	.string "わかものは　やはり　トレーナーとして\n"
	.string "ポケモンリーグを　めざす　もの　なのかね$"

RustboroCity_DevonCorp_3F_Text_ThisIs3rdFloorWaitHere:
	.string "ここが　デボン　コーポレーション　3かい\n"
	.string "しゃちょうの　おへや　なのよね!\p"
	.string "いやー!　きみには\n"
	.string "かんしゃ　かんげき　あめ　あられ　なのです\p"
	.string "ところで……　きみが\n"
	.string "とりもどしてくれた　にもつ\p"
	.string "そいつを　カイナシティの　ぞうせんじょ\n"
	.string "つまり　ふねの　こうじょうへ\l"
	.string "とどけて　ほしいのです!\p"
	.string "また　おかしな　やつらに\n"
	.string "うばわれては　たまりません　からね\p"
	.string "えへへ……\n"
	.string "よろしく　おねがい　しますよ\p"
	.string "ちょっと　まっていて　くださいね$"

RustboroCity_DevonCorp_3F_Text_WordWithPresidentComeWithMe:
	.string "しゃちょうが　ぜひ\n"
	.string "おはなし　したいそうです\p"
	.string "わたしに　ついて　きてください$"

RustboroCity_DevonCorp_3F_Text_PleaseGoAhead:
	.string "ささ　どうぞ!$"

RustboroCity_DevonCorp_3F_Text_VisitCaptSternShipyard:
	.string "カイナの　ぞうせんじょ　には\n"
	.string "クスノキさん　という　ひとが　いるから\l"
	.string "かれを　たずねていくと　いいですよ!$"

RustboroCity_DevonCorp_3F_Text_RepeatAndTimerHugelyPopular:
	.string "デボンの　しんせいひん\n"
	.string "リピートボールと　タイマーボール\l"
	.string "トレーナーに　だいにんき　なのよね!$"

RustboroCity_DevonCorp_3F_Text_RareRocksDisplay:
	.string "しゃちょうの　あつめた\n"
	.string "めずらしい　いしが　かざってある!$"
