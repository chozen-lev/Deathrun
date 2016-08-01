// Deathrun CORE INCLUDE
///////////////////////////////////////////

#if defined _DeathrunCORE_included
	#endinput
#endif
#define _DeathrunCORE_included

enum RandomizeEvents
{
	m_None,
	m_Reset,
	m_Dropped
};

enum DRMCoreFunc
{
	/*
	* Description:		Called each time a terrorist changes
	* Params:		( index, RandomizeEvents:event )
	*/
	DRM_Randomize
};

native HookChain:DRMCoreForward( DRMCoreFunc:function_id, callback[ ], post = 0 );

native bool:DRMCoreEnableForward( HookChain:hook );
native bool:DRMCoreDisableForward( HookChain:hook );

native SetDRMCoreArg( number, AType:type, any:... );

native dr_set_ter( index );