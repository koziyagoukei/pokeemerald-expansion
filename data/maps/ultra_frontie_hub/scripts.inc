ultra_frontie_hub_MapScripts::
	.byte 0

UltraFrontieHub_EventScript_FacilityWarpMenu::
	lock
	faceplayer
	msgbox UltraFrontieHub_Text_WhereGo, MSGBOX_DEFAULT
	goto UltraFrontieHub_EventScript_FacilityWarpMenuChoice
	end

UltraFrontieHub_EventScript_FacilityWarpMenuChoice::
	dynmultipush UltraFrontieHub_Text_BattleTower, 0
	dynmultipush UltraFrontieHub_Text_BattleDome, 1
	dynmultipush UltraFrontieHub_Text_BattleFactory, 2
	dynmultipush UltraFrontieHub_Text_BattlePalace, 3
	dynmultipush UltraFrontieHub_Text_BattleArena, 4
	dynmultipush UltraFrontieHub_Text_BattlePike, 5
	dynmultipush UltraFrontieHub_Text_BattlePyramid, 6
	dynmultipush UltraFrontieHub_Text_Cancel, 7
	dynmultistack 0, 0, FALSE, 8, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, UltraFrontieHub_EventScript_WarpBattleTower
	case 1, UltraFrontieHub_EventScript_WarpBattleDome
	case 2, UltraFrontieHub_EventScript_WarpBattleFactory
	case 3, UltraFrontieHub_EventScript_WarpBattlePalace
	case 4, UltraFrontieHub_EventScript_WarpBattleArena
	case 5, UltraFrontieHub_EventScript_WarpBattlePike
	case 6, UltraFrontieHub_EventScript_WarpBattlePyramid
	case 7, UltraFrontieHub_EventScript_FacilityWarpCancel
	release
	end

UltraFrontieHub_EventScript_WarpBattleTower::
	warp MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY, 255, 13, 8
	waitstate
	end

UltraFrontieHub_EventScript_WarpBattleDome::
	warp MAP_BATTLE_FRONTIER_BATTLE_DOME_LOBBY, 255, 11, 15
	waitstate
	end

UltraFrontieHub_EventScript_WarpBattleFactory::
	warp MAP_BATTLE_FRONTIER_BATTLE_FACTORY_LOBBY, 255, 9, 10
	waitstate
	end

UltraFrontieHub_EventScript_WarpBattlePalace::
	warp MAP_BATTLE_FRONTIER_BATTLE_PALACE_LOBBY, 255, 11, 10
	waitstate
	end

UltraFrontieHub_EventScript_WarpBattleArena::
	warp MAP_BATTLE_FRONTIER_BATTLE_ARENA_LOBBY, 255, 7, 11
	waitstate
	end

UltraFrontieHub_EventScript_WarpBattlePike::
	warp MAP_BATTLE_FRONTIER_BATTLE_PIKE_LOBBY, 255, 5, 11
	waitstate
	end

UltraFrontieHub_EventScript_WarpBattlePyramid::
	warp MAP_BATTLE_FRONTIER_BATTLE_PYRAMID_LOBBY, 255, 7, 15
	waitstate
	end

UltraFrontieHub_EventScript_FacilityWarpCancel::
	msgbox UltraFrontieHub_Text_ComeAgain, MSGBOX_DEFAULT
	release
	end

UltraFrontieHub_Text_WhereGo::
	.string "どこへ　いきますか？$"

UltraFrontieHub_Text_BattleTower::
	.string "バトルタワー$"

UltraFrontieHub_Text_BattleDome::
	.string "バトルドーム$"

UltraFrontieHub_Text_BattleFactory::
	.string "バトルファクトリー$"

UltraFrontieHub_Text_BattlePalace::
	.string "バトルパレス$"

UltraFrontieHub_Text_BattleArena::
	.string "バトルアリーナ$"

UltraFrontieHub_Text_BattlePike::
	.string "バトルチューブ$"

UltraFrontieHub_Text_BattlePyramid::
	.string "バトルピラミッド$"

UltraFrontieHub_Text_Cancel::
	.string "やめる$"

UltraFrontieHub_Text_ComeAgain::
	.string "またどうぞ。$"

UltraFrontieHub_EventScript_FrontierMoveTutor::
	lock
	faceplayer
	msgbox UltraFrontieHub_Text_FrontierMoveTutor, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq UltraFrontieHub_EventScript_StartFrontierMoveTutor
	msgbox UltraFrontieHub_Text_Cancel, MSGBOX_DEFAULT
	release
	end

UltraFrontieHub_EventScript_StartFrontierMoveTutor::
	closemessage
	goto MoveRelearner_EventScript_TeachFrontierFullMoves
	end

UltraFrontieHub_Text_FrontierMoveTutor::
	.string "わざを\nおもいだしますか？$"

UltraFrontieHub_EventScript_FrontierPokemonCreator::
	lock
	faceplayer
	msgbox UltraFrontieHub_Text_FrontierPokemonCreator, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq UltraFrontieHub_EventScript_OpenFrontierPokemonCreator
	msgbox UltraFrontieHub_Text_Cancel, MSGBOX_DEFAULT
	release
	end

UltraFrontieHub_EventScript_OpenFrontierPokemonCreator::
	closemessage
	special Special_OpenFrontierPokemonCreator
	waitstate
	release
	end

UltraFrontieHub_Text_FrontierPokemonCreator::
	.string "フロンティアようの\nポケモンを つくりますか？$"