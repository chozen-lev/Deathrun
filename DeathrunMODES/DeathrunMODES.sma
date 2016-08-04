#include < amxmodx >
#include < reapi >

#include < DeathrunCORE >

#include "deathrun/constants.inl"
#include "deathrun/cvar.inl"

#include "deathrun/deathrun.inl"
#include "deathrun/hooks.inl"
#include "deathrun/modes.inl"

public plugin_init( )
{
	register_plugin( "[ReAPI]Deathrun MODES", VERSION, "ChoZen" )
	
	// hooks
	DRMCoreForward( DRM_Randomize, "DRMCore_Randomize_Post", true )
	RegisterHookChain( RG_CBasePlayer_RoundRespawn, "CBasePlayer_RoundRespawn_Post", true )
	RegisterHookChain( RG_CSGameRules_RestartRound, "CSGameRules_RestartRound" )
	RegisterHookChain( RG_RoundEnd, "RoundEnd_Post", true )
	
	// dictionary
	register_dictionary( "DeathrunMODES.txt" )
}

public plugin_natives( )
{
	ArrayModes = ArrayCreate( ModeData )
	ArrayModeIndexes = ArrayCreate( )
	
	ArrayForwardShowMenu = ArrayCreate( FORWARD_DATA )
	ArrayForwardChangeMode = ArrayCreate( FORWARD_DATA )
	ArrayForwards = ArrayCreate( FORWARDS )
	
	register_library( "DeathrunMODES" )
	register_native( "DRM_RegisterMode", "native_DRM_RegisterMode" )
	register_native( "SetDRMModeArg", "native_set_arg" )
	register_native( "DRMModeForward", "native_register_forward" )
	register_native( "DRMModeEnableForward", "native_enable_forward" )
	register_native( "DRMModeDisableForward", "native_disable_forward" )
	
	register_native( "dr_set_mode", "native_set_mode" )
}