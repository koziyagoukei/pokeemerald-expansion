RustboroCity_DevonCorp_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_DevonCorp_1F_OnTransition
	.byte 0

RustboroCity_DevonCorp_1F_OnTransition:
	call_if_unset FLAG_RETURNED_DEVON_GOODS, RustboroCity_DevonCorp_1F_EventScript_BlockStairs
	end

RustboroCity_DevonCorp_1F_EventScript_BlockStairs::
	setobjectxyperm LOCALID_DEVON_CORP_STAIR_GUARD, 14, 2
	setobjectmovementtype LOCALID_DEVON_CORP_STAIR_GUARD, MOVEMENT_TYPE_FACE_DOWN
	return

RustboroCity_DevonCorp_1F_EventScript_Employee::
	lock
	faceplayer
	goto_if_set FLAG_RETURNED_DEVON_GOODS, RustboroCity_DevonCorp_1F_EventScript_GoodsRecovered
	goto_if_set FLAG_DEVON_GOODS_STOLEN, RustboroCity_DevonCorp_1F_EventScript_RobberWasntBright
	msgbox RustboroCity_DevonCorp_1F_Text_ThoseShoesAreOurProduct, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_RobberWasntBright::
	msgbox RustboroCity_DevonCorp_1F_Text_RobberWasntVeryBright, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_GoodsRecovered::
	msgbox RustboroCity_DevonCorp_1F_Text_SoundsLikeStolenGoodsRecovered, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_StairGuard::
	lock
	faceplayer
	goto_if_set FLAG_RETURNED_DEVON_GOODS, RustboroCity_DevonCorp_1F_EventScript_AlwaysWelcome
	goto_if_set FLAG_RECOVERED_DEVON_GOODS, RustboroCity_DevonCorp_1F_EventScript_GotRobbed
	goto_if_set FLAG_DEVON_GOODS_STOLEN, RustboroCity_DevonCorp_1F_EventScript_GotRobbed
	msgbox RustboroCity_DevonCorp_1F_Text_OnlyAuthorizedPeopleEnter, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_AlwaysWelcome::
	msgbox RustboroCity_DevonCorp_1F_Text_YoureAlwaysWelcomeHere, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_GotRobbed::
	msgbox RustboroCity_DevonCorp_1F_Text_HowCouldWeGetRobbed, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_Greeter::
	lock
	faceplayer
	goto_if_set FLAG_RETURNED_DEVON_GOODS, RustboroCity_DevonCorp_1F_EventScript_WelcomeToDevonCorp
	goto_if_set FLAG_RECOVERED_DEVON_GOODS, RustboroCity_DevonCorp_1F_EventScript_StaffGotRobbed
	goto_if_set FLAG_DEVON_GOODS_STOLEN, RustboroCity_DevonCorp_1F_EventScript_StaffGotRobbed
	msgbox RustboroCity_DevonCorp_1F_Text_WelcomeToDevonCorp, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_WelcomeToDevonCorp::
	msgbox RustboroCity_DevonCorp_1F_Text_WelcomeToDevonCorp, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_StaffGotRobbed::
	msgbox RustboroCity_DevonCorp_1F_Text_StaffGotRobbed, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_1F_EventScript_RocksMetalDisplay::
	msgbox RustboroCity_DevonCorp_1F_Text_RocksMetalDisplay, MSGBOX_SIGN
	end

RustboroCity_DevonCorp_1F_EventScript_ProductsDisplay::
	msgbox RustboroCity_DevonCorp_1F_Text_ProductDisplay, MSGBOX_SIGN
	end

RustboroCity_DevonCorp_1F_Text_WelcomeToDevonCorp:
	.string "いらっしゃいませ!\n"
	.string "デボン　コーポレーション　です!\p"
	.string "みなさまの　せいかつに　やくだつ\n"
	.string "どうぐや　くすりを　おつくりしてます!$"

RustboroCity_DevonCorp_1F_Text_StaffGotRobbed:
	.string "ドジな　けんきゅういんが\n"
	.string "にもつを　とられちゃって……$"

RustboroCity_DevonCorp_1F_Text_ThoseShoesAreOurProduct:
	.string "おっ!　ランニングシューズ!\p"
	.string "じぶんたちの　かいしゃの　しょうひんを\n"
	.string "じっさいに　つかっている　ひとを　みると\l"
	.string "すごく　うれしくなるよね!$"

RustboroCity_DevonCorp_1F_Text_RobberWasntVeryBright:
	.string "たしかに　あの　にもつは\n"
	.string "だいじな　もの　だけど\l"
	.string "ほかの　ひとが　ぬすんでいっても\l"
	.string "つかえないと　おもうんだよね\p"
	.string "ぼくの　すいりでは　あの　どろぼう……\n"
	.string "たんなる　マヌケ　なんじゃないかな?$"

RustboroCity_DevonCorp_1F_Text_SoundsLikeStolenGoodsRecovered:
	.string "どうやら　デボンのにもつは\n"
	.string "もどってきた　らしいね$"

RustboroCity_DevonCorp_1F_Text_OnlyAuthorizedPeopleEnter:
	.string "おっと　かんけいしゃ　いがいの　かたは\n"
	.string "ここから　さきには　はいれませんよ!$"

RustboroCity_DevonCorp_1F_Text_HowCouldWeGetRobbed:
	.string "どろぼうに　にもつを　うばわれるなんて\n"
	.string "なんて　ドジなんだろう……$"

RustboroCity_DevonCorp_1F_Text_YoureAlwaysWelcomeHere:
	.string "やあ!\n"
	.string "きみなら　いつでも　だいかんげい　だよ!$"

RustboroCity_DevonCorp_1F_Text_RocksMetalDisplay:
	.string "いしや　きんぞくの　サンプルが\n"
	.string "ガラスケースの　なかに　ならんでいる!\p"
	.string "パネルに　せつめいが　かいてあるから\n"
	.string "よんでみよう……\p"
	.string "「デボン　コーポレーションは　もともと\n"
	.string "　やまから　いしを　きりだしたり\p"
	.string "　さてつから　てつざいを　つくるために\n"
	.string "　せつりつ　された　かいしゃ　です」\p"
	.string "「やがて　デボン　コーポレーションは\n"
	.string "　ざいりょう　だけに　とどまらず\p"
	.string "　さまざまな　こうぎょう　せいひんも\n"
	.string "　つくるように　なったのです」$"

RustboroCity_DevonCorp_1F_Text_ProductDisplay:
	.string "いろんな　どうぐの　しさくひんが\n"
	.string "ガラスケースの　なかに　ならんでいる!\p"
	.string "パネルに　せつめいが　かいてあるから\n"
	.string "よんでみよう……\p"
	.string "「げんざい　デボン　コーポレーションでは\n"
	.string "　こうぎょう　せいひんの　ほかにも\l"
	.string "　にちようひんや　くすりなど\p"
	.string "　ひとびとの　くらしに　やくだつ　ものを\n"
	.string "　かいはつ　しています」\p"
	.string "「また　げんざいでは　モンスターボールや\n"
	.string "　ポケナビなどの　トレーナーようひんも\l"
	.string "　てがけるように　なりました」$"
