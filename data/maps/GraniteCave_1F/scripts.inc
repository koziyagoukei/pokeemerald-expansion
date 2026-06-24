GraniteCave_1F_MapScripts::
	.byte 0

GraniteCave_1F_EventScript_Hiker::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HM_FLASH, GraniteCave_1F_EventScript_ReceivedFlash
	msgbox GraniteCave_1F_Text_GetsDarkAheadHereYouGo, MSGBOX_DEFAULT
	giveitem ITEM_HM_FLASH
	setflag FLAG_RECEIVED_HM_FLASH
	msgbox GraniteCave_1F_Text_ExplainFlash, MSGBOX_DEFAULT
	release
	end

GraniteCave_1F_EventScript_ReceivedFlash::
	msgbox GraniteCave_1F_Text_ExplainFlash, MSGBOX_DEFAULT
	release
	end

GraniteCave_1F_Text_GetsDarkAheadHereYouGo:
	.string "おい　おまえ\n"
	.string "このさきは　くらいから\l"
	.string "たんけん　するのも　たいへん　だぞ!\p"
	.string "さっきの　おとこ……\n"
	.string "ダイゴって　いったっけか?\p"
	.string "あいつは　フラッシュが　つかえたから\n"
	.string "だいじょうぶ　だろうが……\p"
	.string "うむ!　やまおとこは　であった\n"
	.string "あいてに　やさしく　するのが　モットー!\l"
	.string "これを　ゆずってやると　するか!$"

GraniteCave_1F_Text_ExplainFlash:
	.string "ひでん　フラッシュを\n"
	.string "ポケモンに　おぼえさせて　つかうとだな\p"
	.string "まっくらな　どうくつも\n"
	.string "あかるく　てらせるんだぞ!\p"
	.string "まあ　つかうには　ムロ　ポケモンジムの\n"
	.string "ジムバッジが　ひつよう　だがな$"
