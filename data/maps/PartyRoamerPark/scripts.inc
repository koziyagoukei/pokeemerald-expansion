PartyRoamerPark_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PartyRoamerPark_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PartyRoamerPark_OnFrame
	.byte 0

PartyRoamerPark_OnTransition:
	setflag FLAG_TEMP_HIDE_FOLLOWER
	setvar VAR_TEMP_0, 0
	special Special_SpawnPartyRoamerMons
	end

PartyRoamerPark_OnFrame:
	map_script_2 VAR_TEMP_0, 0, PartyRoamerPark_EventScript_RefreshMons
	.2byte 0

PartyRoamerPark_EventScript_RefreshMons::
	setvar VAR_TEMP_0, 1
	special Special_RefreshPartyRoamerMons
	end

PartyRoamerPark_EventScript_Talk::
	lock
	faceplayer
	special Special_BufferPartyRoamerMessage
	msgbox gStringVar4, MSGBOX_DEFAULT
	release
	end
