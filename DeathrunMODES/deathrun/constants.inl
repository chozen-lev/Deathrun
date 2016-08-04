// CONSTANS
///////////////////////////////////////////

#define VERSION "1.0.0-dev+"

new const g_langPrefix[ ] = "DR_MODE_PREFIX"

enum ModeIndexes:iModes
{
	m_None = 0,
	m_Buttons,
	m_Free,
	m_Invis,
	m_Rambo,
	m_Snowballs,
	m_WarmUp,
	m_Duel
}

enum ModeEvents
{
	m_None = 0,
	m_Reset,
	m_Choose,
	m_Auto
}

enum _:ModeData
{
	ModeIndexes:m_Index,
	m_langName[ 32 ],
	bool:m_Hide
}

enum DRMModeFunc
{
	DRM_ShowMenu_ChooseMode,
	DRM_ChangeMode
}

enum _:FORWARDS
{
	DRMModeFunc:m_Function,
	m_arrIndex,
	Array:m_Array
}

enum _:FORWARD_DATA
{
	m_fwdIndex,
	m_Post,
	bool:m_State
}

enum _:TASKS( += 100 )
{
	TASK_AUTOCHOOSE
}

new g_iModesMax = _:iModes, g_iModesNum

new g_CurrMode[ ModeData ], g_NoneMode[ ModeData ] = { ModeIndexes:m_None, "DR_MODE_NONE", true }

new Array:ArrayModes, Array:ArrayModeIndexes, Array:ArrayAvailModes

new g_iCurrTer

new Array:ArrayForwardShowMenu, Array:ArrayForwardChangeMode, Array:ArrayForwards

new g_iModeMenu

new g_iArg[ 3 ], bool:g_bArgState[ 3 ]