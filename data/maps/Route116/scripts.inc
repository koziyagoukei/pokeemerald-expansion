Route116_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route116_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route116_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route116_OnFrame
	.byte 0

Route116_OnTransition:
	call_if_set FLAG_RECOVERED_DEVON_GOODS, Route116_EventScript_SetWandasBoyfriendPos
	call_if_eq VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_HideMapNamePopup
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_NORTH, AbnormalWeather_StartGroudonWeather
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_SOUTH, AbnormalWeather_StartGroudonWeather
	end

Route116_EventScript_SetWandasBoyfriendPos::
	setobjectxyperm LOCALID_ROUTE116_WANDAS_BF, 38, 10
	return

Route116_OnLoad:
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_NORTH, AbnormalWeather_EventScript_PlaceTilesRoute116North
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_SOUTH, AbnormalWeather_EventScript_PlaceTilesRoute116South
	end

Route116_OnFrame:
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route116_EventScript_WandasBoyfriend::
	lock
	faceplayer
	goto_if_set FLAG_RECOVERED_DEVON_GOODS, Route116_EventScript_BoyfriendGruntLeftTunnel
	goto_if_set FLAG_DEVON_GOODS_STOLEN, Route116_EventScript_BoyfriendGruntInTunnel
	msgbox Route116_Text_WantToDigTunnel, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_BoyfriendGruntLeftTunnel::
	msgbox Route116_Text_GoonHightailedItOutOfTunnel, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_BoyfriendGruntInTunnel::
	msgbox Route116_Text_DiggingTunnelWhenGoonOrderedMeOut, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_DevonEmployee::
	lock
	faceplayer
	goto_if_set FLAG_MET_DEVON_EMPLOYEE, Route116_EventScript_TryGiveRepeatBallAgain
	msgbox Route116_Text_ThankYouTokenOfAppreciation, MSGBOX_DEFAULT
	goto Route116_EventScript_GiveRepeatBall
	end

Route116_EventScript_GiveRepeatBall::
	setflag FLAG_MET_DEVON_EMPLOYEE
	giveitem ITEM_REPEAT_BALL
	goto_if_eq VAR_RESULT, FALSE, Route116_EventScript_NoRoomForRepeatBall
	msgbox Route116_Text_NewBallAvailableAtMart, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_FACING, DIR_NORTH, Route116_EventScript_DevonEmployeeExit
	call_if_eq VAR_FACING, DIR_SOUTH, Route116_EventScript_DevonEmployeeExit
	call_if_eq VAR_FACING, DIR_WEST, Route116_EventScript_DevonEmployeeExit
	call_if_eq VAR_FACING, DIR_EAST, Route116_EventScript_DevonEmployeeExitEast
	removeobject VAR_LAST_TALKED
	clearflag FLAG_HIDE_RUSTBORO_CITY_DEVON_CORP_3F_EMPLOYEE
	setflag FLAG_RECEIVED_REPEAT_BALL
	release
	end

Route116_EventScript_DevonEmployeeExit::
	applymovement VAR_LAST_TALKED, Route116_Movement_DevonEmployeeExit
	waitmovement 0
	return

Route116_EventScript_DevonEmployeeExitEast::
	applymovement VAR_LAST_TALKED, Route116_Movement_DevonEmployeeExitEast
	waitmovement 0
	return

Route116_EventScript_TryGiveRepeatBallAgain::
	msgbox Route116_Text_TokenOfAppreciation, MSGBOX_DEFAULT
	goto Route116_EventScript_GiveRepeatBall
	end

Route116_EventScript_NoRoomForRepeatBall::
	msgbox Route116_Text_BagIsJamPacked, MSGBOX_DEFAULT
	release
	end

Route116_Movement_DevonEmployeeExit:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_Movement_DevonEmployeeExitEast:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_EventScript_RouteSignRustboro::
	msgbox Route116_Text_RouteSignRustboro, MSGBOX_SIGN
	end

Route116_EventScript_RusturfTunnelSign::
	msgbox Route116_Text_RusturfTunnelSign, MSGBOX_SIGN
	end

Route116_EventScript_TunnelersRestHouseSign::
	msgbox Route116_Text_TunnelersRestHouse, MSGBOX_SIGN
	end

Route116_EventScript_TrainerTipsBToStopEvolution::
	msgbox Route116_Text_TrainerTipsBToStopEvolution, MSGBOX_SIGN
	end

Route116_EventScript_TrainerTipsBagHasPockets::
	msgbox Route116_Text_TrainerTipsBagHasPockets, MSGBOX_SIGN
	end

Route116_EventScript_Briney::
	lock
	faceplayer
	msgbox Route116_Text_ScoundrelMadeOffWithPeeko, MSGBOX_DEFAULT
	setvar VAR_ROUTE116_STATE, 2
	release
	end

Route116_EventScript_BrineyTrigger::
	lockall
	applymovement LOCALID_ROUTE116_BRINEY, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	msgbox Route116_Text_ScoundrelMadeOffWithPeeko, MSGBOX_DEFAULT
	setvar VAR_ROUTE116_STATE, 2
	releaseall
	end

Route116_EventScript_GlassesMan::
	lock
	faceplayer
	checkitem ITEM_BLACK_GLASSES
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_PlayerHasGlasses
	specialvar VAR_RESULT, FoundBlackGlasses
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_FoundGlassesNotOnPlayer
	msgbox Route116_Text_CanYouHelpMeFindGlasses, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_FoundGlassesNotOnPlayer::
	msgbox Route116_Text_CantFindGlassesNotHere, MSGBOX_DEFAULT
	closemessage
	goto Route116_EventScript_GlassesManExit
	end

Route116_EventScript_PlayerHasGlasses::
	msgbox Route116_Text_CanYouHelpMeFindGlasses, MSGBOX_DEFAULT
	msgbox Route116_Text_MayISeeThoseGlasses, MSGBOX_DEFAULT
	specialvar VAR_RESULT, FoundBlackGlasses
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_FoundGlassesOnPlayer
	msgbox Route116_Text_NotWhatImLookingFor, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_FoundGlassesOnPlayer::
	msgbox Route116_Text_NotWhatImLookingForMaybeTheyArentHere, MSGBOX_DEFAULT
	closemessage
	goto Route116_EventScript_GlassesManExit
	end

Route116_EventScript_GlassesManExit::
	delay 20
	call_if_eq VAR_FACING, DIR_NORTH, Route116_EventScript_GlassesManExitNormal
	call_if_eq VAR_FACING, DIR_SOUTH, Route116_EventScript_GlassesManExitNormal
	call_if_eq VAR_FACING, DIR_WEST, Route116_EventScript_GlassesManExitNormal
	call_if_eq VAR_FACING, DIR_EAST, Route116_EventScript_GlassesManExitEast
	removeobject VAR_LAST_TALKED
	release
	end

Route116_EventScript_GlassesManExitNormal::
	applymovement VAR_LAST_TALKED, Route116_Movement_GlassesManExit
	waitmovement 0
	return

Route116_EventScript_GlassesManExitEast::
	applymovement VAR_LAST_TALKED, Route116_Movement_GlassesManExitEast
	waitmovement 0
	return

Route116_Movement_GlassesManExit:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_Movement_GlassesManExitEast:
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_EventScript_Joey::
	trainerbattle_single TRAINER_JOEY, Route116_Text_JoeyIntro, Route116_Text_JoeyDefeat
	msgbox Route116_Text_JoeyPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Jose::
	trainerbattle_single TRAINER_JOSE, Route116_Text_JoseIntro, Route116_Text_JoseDefeat
	msgbox Route116_Text_JosePostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Jerry::
	trainerbattle_single TRAINER_JERRY_1, Route116_Text_JerryIntro, Route116_Text_JerryDefeat, Route116_EventScript_TryRegisterJerryAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_RematchJerry
	setvar VAR_0x8004, TRAINER_JERRY_1
	specialvar VAR_RESULT, IsTrainerRegistered
	goto_if_eq VAR_RESULT, FALSE, Route116_EventScript_TryRegisterJerry
	msgbox Route116_Text_JerryPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_TryRegisterJerryAfterBattle::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterJerryAfterBattle
	release
	end

Route116_EventScript_RegisterJerryAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route116_Text_JerryRegister2, MSGBOX_DEFAULT
	register_matchcall TRAINER_JERRY_1
	release
	end

Route116_EventScript_TryRegisterJerry::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterJerry
	msgbox Route116_Text_JerryPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_RegisterJerry::
	msgbox Route116_Text_JerryRegister1, MSGBOX_DEFAULT
	register_matchcall TRAINER_JERRY_1
	release
	end

Route116_EventScript_RematchJerry::
	trainerbattle_rematch TRAINER_JERRY_1, Route116_Text_JerryRematchIntro, Route116_Text_JerryRematchDefeat
	msgbox Route116_Text_JerryPostRematch, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Clark::
	trainerbattle_single TRAINER_CLARK, Route116_Text_ClarkIntro, Route116_Text_ClarkDefeat
	msgbox Route116_Text_ClarkPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Janice::
	trainerbattle_single TRAINER_JANICE, Route116_Text_JaniceIntro, Route116_Text_JaniceDefeat
	msgbox Route116_Text_JanicePostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Karen::
	trainerbattle_single TRAINER_KAREN_1, Route116_Text_KarenIntro, Route116_Text_KarenDefeat, Route116_EventScript_TryRegisterKarenAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_RematchKaren
	setvar VAR_0x8004, TRAINER_KAREN_1
	specialvar VAR_RESULT, IsTrainerRegistered
	goto_if_eq VAR_RESULT, FALSE, Route116_EventScript_TryRegisterKaren
	msgbox Route116_Text_KarenPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_TryRegisterKarenAfterBattle::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterKarenAfterBattle
	release
	end

Route116_EventScript_RegisterKarenAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route116_Text_KarenRegister2, MSGBOX_DEFAULT
	register_matchcall TRAINER_KAREN_1
	release
	end

Route116_EventScript_TryRegisterKaren::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterKaren
	msgbox Route116_Text_KarenPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_RegisterKaren::
	msgbox Route116_Text_KarenRegister1, MSGBOX_DEFAULT
	register_matchcall TRAINER_KAREN_1
	release
	end

Route116_EventScript_RematchKaren::
	trainerbattle_rematch TRAINER_KAREN_1, Route116_Text_KarenRematchIntro, Route116_Text_KarenRematchDefeat
	msgbox Route116_Text_KarenPostRematch, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Sarah::
	trainerbattle_single TRAINER_SARAH, Route116_Text_SarahIntro, Route116_Text_SarahDefeat
	msgbox Route116_Text_SarahPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Dawson::
	trainerbattle_single TRAINER_DAWSON, Route116_Text_DawsonIntro, Route116_Text_DawsonDefeat
	msgbox Route116_Text_DawsonPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Devan::
	trainerbattle_single TRAINER_DEVAN, Route116_Text_DevanIntro, Route116_Text_DevanDefeat
	msgbox Route116_Text_DevanPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Johnson::
	trainerbattle_single TRAINER_JOHNSON, Route116_Text_JohnsonIntro, Route116_Text_JohnsonDefeat
	msgbox Route116_Text_JohnsonPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Text_WeShortenItToDevon:
	.string "{JPN}デボン コーポレーション……\nながいから みんな デボンって よぶけど\pあの かいしゃ では いろいろと\nべんりな ものを つくってるんだ!"
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_SneakyLookingManWentAroundCorner:
	.string "{JPN}ん? あやしい おとこ?\pそういえば にんそうの わるい おとこが\nそこの かどを まがっていったよ"
	.string ""
	.string "$"

RustboroCity_Text_HaveYouChallengedGym:
	.string "{JPN}ポケモンジムに ちょうせん したかい?\pあの キラリと かがやく\nジムバッジを もつと トレーナーとしての\lじかくが めばえて くるんだな!"
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_HeyThatsRustborosGymBadge:
	.string "{JPN}おっ! カナズミの ジムバッジ!\pいくつか ある バッジの なかで\nカナズミの やつが いちばん かっこいいな!"
	.string ""
	.string "$"

RustboroCity_Text_YoureNewAroundHere:
	.string "{JPN}おや? きみは だれだい?\nこの あたりでは みかけない かお だね\pトレーナーズ スクールの てんこうせい?"
	.string ""
	.string "$"

RustboroCity_Text_GymLeaderIsntEasyWithFire:
	.string "{JPN}ジムリーダーに ちょうせん したけど……\pあたしの ほのおタイプの ポケモンじゃ\nちょっと かつのは たいへんそう……\pだって ほのおタイプ って\nいわタイプに あいしょうが わるいもの……"
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_MrBrineyWalksInTheTunnel:
	.string "{JPN}はまべの こやに すんでいる\nふなのりの ハギろうじん\lときどき トンネルに さんぽに いくんだよ"
	.string ""
	.string "$"

RustboroCity_Text_MrBrineyLovesPeeko:
	.string "{JPN}はまべの こやに すんでいる\nふなのりの ハギろうじん\lピーコちゃんの ために カイナに いって\lかいもの するんだってさ\pほんとうに ピーコちゃんが すきなんだな!\nあの じいさん"
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_WowYouHavePokemon:
	.string "{JPN}あなたも ポケモンを つれてるのね\pあたしも おおきくなったら\nポケモンと どこかに いくんだ!"
	.string ""
	.string "$"

RustboroCity_Text_CatchRarePokemonIfIGoToSchool:
	.string "{JPN}ぽけもん とれーなーず すくーる かぁ!\pここに にゅうがく すれば\nめずらしい ポケモンでも かんたんに\lつかまえられるように なるのかな?"
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_PokemonCanChangeLookFromExp:
	.string "{JPN}ポケモンって たたかいの けいけんを\nつんでいくと すがたかたち が\lかわっちゃうことが あるんだよ!"
	.string ""
	.string "$"

RustboroCity_Text_PokemonChangeShape:
	.string "{JPN}ポケモンの すがたが かわっちゃうの?\nいきなり かわると ビックリしちゃう!"
	.string "$"

RustboroCity_Text_TradePokemonGrowFast:
	.string "{JPN}ひとと こうかんした ポケモンは\nそだつのが はやい!\p……でも ジムバッジを もっていないと\nいうことを きいて くれないんだ"
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_OutOfTheWay:
	.string "{JPN}どけー! どけどけー!!$"

RustboroCity_Text_WaitDontTakeMyGoods:
	.string "{JPN}まってぇー!\pその にもつを かえしてぇぇぇー!!"
	.string "$"

RustboroCity_Text_HelpMeIWasRobbed:
	.string "{JPN}ああ きみは!\pトウカのもりで たすけてくれた\nすてきな トレーナーじゃ ないですか!\pおねがいです!\pアクアだんに うばわれた\nデボンのにもつ とりかえして ください!\pあれが ないと……\nわたし とても こまるんです"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_ShadyCharacterTookOffTowardsTunnel:
	.string "{JPN}さっきの あやしい やつは\nむこうの トンネルの ほうに\lにげていったと おもうのですよ"
	.string ""
	.string "$"

RustboroCity_Text_YouGotItThankYou:
	.string "{JPN}ああ! どうでした\nデボンのにもつ は……?\pそうですか!\nとりかえして くれたのですか!\pきみは ほんとうに\nすごい トレーナー ですね!\p……そうだ! おれいに また\nスーパーボールを あげちゃおう!"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_YoureLoadedWithItems:
	.string "{JPN}どうぐ いっぱいで\nスーパーボール あげられないね"
	.string "$"

RustboroCity_Text_PleaseComeWithMe:
	.string "{JPN}きみ! きみ!\nわたしに ついてきて ください!"
	.string "$"

RustboroCity_Text_MayHiLetsRegister:
	.string "{JPN}ハルカ『あっ {PLAYER}{KUN}!\pポケナビに エントリーコールを\nつけて もらったのね!\pじゃあ あたしたち\nおたがいに よびだせる ように\lなまえを とうろく しておこうよ!\p…… …… ……"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_RegisteredMay:
	.string "{JPN}ハルカを\nポケナビに とうろく した!"
	.string "$"

RustboroCity_Text_MayPassedBrineyWantToBattle:
	.string "{JPN}ハルカ『そういえば トウカのもりで\nハギろうじんと すれちがったわ\pたぶん…… はまべの こやに\nもどるところ だったんじゃ ないかな!\pえーと…… それより {PLAYER}{KUN}の\nポケモンずかんの ちょうしは どう?\pあたしは かなり いい かんじ!\nどう? ちょっと しょうぶ してみる?"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_MayOhHaventRaisedPokemonEnough:
	.string "{JPN}ハルカ『えー なんでよー\nあんまり ポケモン つかまえたり\lそだてたり してないのかな?\pそれじゃあ トレーナーとして\nまだまだ ね!"
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_MayWantToBattle:
	.string "{JPN}ハルカ『どう? あたしと ここで\nちょっと しょうぶ してみる?"
	.string "$"

RustboroCity_Text_MayImNotGoingToLose:
	.string "{JPN}ハルカ『トレーナーに なった ばかりの\n{PLAYER}{KUN}には まけないよ!"
	.string "$"

RustboroCity_Text_MayDefeat:
	.string "{JPN}うわあ……\nもう こんなに つよいなんて……"
	.string "$"

RustboroCity_Text_MayMrBrineyHint:
	.string "{JPN}ハルカ『そういえば さっき すれちがった\nハギろうじん なんだけどね\p{PLAYER}{KUN}は ひっこして きたばかりで\nしらないかも しれないけど\lむかしは すっごい ふなのり だったのよ!"
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_BrendanHiLetsRegister:
	.string "{JPN}ユウキ『おっ {PLAYER}!\pポケナビに エントリーコールを\nつけて もらったんだな!\pじゃあ オレたち\nおたがいに よびだせる ように\lなまえを とうろく しておこうよ!\p…… …… ……"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_RegisteredBrendan:
	.string "{JPN}ユウキを\nポケナビに とうろく した!"
	.string "$"

RustboroCity_Text_BrendanPassedBrineyWantToBattle:
	.string "{JPN}ユウキ『そういえば {PLAYER}\pさっき トウカのもりで\nハギろうじんと すれちがったよ\pたぶん…… はまべの こやに\nもどるところ だったんじゃ ないかな!\pえーと…… それより {PLAYER}の\nポケモンずかんは どう?\lちょっとは ページ うまったか?\pオレは すごいよ!\nどうだ ポケモン しょうぶで\lそのせいかを ためして やろうか?"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_BrendanNoConfidenceInPokemon:
	.string "{JPN}ユウキ『なんだよ\nもってる ポケモンに じしんが ないのか?"
	.string "$"

RustboroCity_Text_BrendanWantToBattle:
	.string "{JPN}ユウキ『どうだい オレと ここで\nしょうぶ してみるか?"
	.string "$"

RustboroCity_Text_BrendanIWontGoEasy:
	.string "{JPN}ユウキ『トレーナーに なりたて だって\nてかげんは しないからな!"
	.string "$"

RustboroCity_Text_BrendanDefeat:
	.string "{JPN}へえ……\nなかなか やるじゃないか"
	.string "$"

RustboroCity_Text_BrendanMrBrineyHint:
	.string "{JPN}ユウキ『そういえば さっき すれちがった\nハギろうじん だけどな\p{PLAYER}は ひっこして きたばかりで\nしらないかも しれないけど\lむかしは すごい ふなのり だったんだ!"
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_TunnelNearingCompletion:
	.string "{JPN}「べんりな トンネル かんせい まぢか!」\p……と かんばんに かかれているが\nその うえに あかい ペンキで\lおおきな バッテンが つけられている"
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_DevonCorpSign:
	.string "{JPN}「デボン コーポレーション」\pみなさんの せいかつの ために\nなんでも つくっています!"
	.string ""
	.string "$"

RustboroCity_Text_GymSign:
	.string "{JPN}カナズミ シティ ポケモンジム\nリーダー ツツジ\lいわに ときめく ゆうとうせい!"
	.string ""
	.string "$"

RustboroCity_Text_DevonCorpBranchOfficeSign:
	.string "{JPN}「デボン コーポレーション ぶんしつ」\pかんけいしゃ いがいの たちいりは\nどうぞ ごえんりょ ください!"
	.string ""
	.string "$"

RustboroCity_Text_CitySign:
	.string "{JPN}ここは カナズミ シティ\pしぜんと かがくの ゆうごうを\nついきゅうする まち"
	.string ""
	.string "$"

RustboroCity_Text_TrainersSchoolSign:
	.string "{JPN}「ポケモン トレーナーズ スクール」\nポケモンのことなら なんでも おしえます!"
	.string "$"

RustboroCity_Text_CuttersHouse:
	.string "{JPN}「いあいぎりオヤジの いえ」$"

RustboroCity_Text_DevelopedNewPokenavFeature:
	.string "{JPN}いま ポケナビの ついかきのうを\nかいはつ していたのですが……\pうまく いきましたよ!\p{PLAYER}さん!\pしゃちょうに いただいた そのポケナビ\nちょっと かしてくださいね!"
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_AddedMatchCallPleaseCallMrStone:
	.string "{JPN}はい! {PLAYER}さんの ポケナビに\nエントリーコール という きのうを\lついか しました!\pポケナビに とうろく されていれば\nとおく はなれた ところの ひとと\lエントリーコールを つうじて\lかいわが できて しまうんです!\p{PLAYER}さんの ポケナビには\nうちの ツワブうちの ツワブキ しゃちょうの なまえが\lとうろく してある はずです\pためしに ちょっと しゃちょうを\nよびだして みてください!"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"

RustboroCity_Text_PleaseSelectPokenav:
	.string "{JPN}ポケナビを えらんで ください!$"

RustboroCity_Text_IdBetterGetBackToWork:
	.string "{JPN}……では わたしは しごとに もどります\p{PLAYER}さん おきをつけて!"
	.string "$"

RustboroCity_Text_YouCanHave2On2Battle:
	.string "{JPN}きみ しってる?\nふたりづれの トレーナーとじゃ なくても\l2たい2の しょうぶが できるんだ!\p2ひき いじょう ポケモンを もってるとき\nふたりの トレーナーと めが あえば\lしょうぶを どうじに いどんで くるよ!\pたった ひとりで ふたりを あいてに\nかてたら カッコいいよねー!"
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string ""
	.string "$"


