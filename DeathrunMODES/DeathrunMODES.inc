// Deathrun MODES INCLUDE
///////////////////////////////////////////

#if defined _DeathrunMODES_included
	#endinput
#endif
#define _DeathrunMODES_included

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
	m_None,
	m_Reset,
	m_Choose,
	m_Auto
};

enum DRMModeFunc
{
	/*
	* Description:		Called each time menu displayes
	* Params:		( index, menu, time )
	*/
	DRM_ShowMenu_ChooseMode,
	
	/*
	* Description:		Called each time mode changes
	* Params:		( ModeIndexes:mode, ModeEvents:event )
	*/
	DRM_ChangeMode
};

native HookChain:DRMModeForward( {DRMModeFunc,_}:function_id, callback[ ], post = 0 );

native bool:DRMModeEnableForward( HookChain:hook );
native bool:DRMModeDisableForward( HookChain:hook );

native SetDRMModeArg( number, AType:type, any:... );

native ModeIndexes:DRM_RegisterMode( {ModeIndexes,_}:mode = ModeIndexes:m_None, langName[ ], bool:hide );

native dr_set_mode( ModeIndexes:mode );