#include <amxmodx>
#include <engine>
#include <reapi>
#include <hamsandwich>

#include "deathrun/constants.inl"
#include "deathrun/cvar.inl"

#include "deathrun/deathrun.inl"
#include "deathrun/hooks.inl"
#include "deathrun/randomize.inl"

public plugin_init()
{
    register_plugin("[ReAPI]Deathrun CORE", VERSION, "ChoZen")
    
    // hooks
    RegisterHookChain(RG_CSGameRules_RestartRound, "CSGameRules_RestartRound")
    RegisterHookChain(RG_CSGameRules_RestartRound, "CSGameRules_RestartRound_Post", true)
    RegisterHookChain(RG_CBasePlayer_SetClientUserInfoName, "CBasePlayer_SetClientUserInfoName_Post", true)
    RegisterHookChain(RG_CSGameRules_FlPlayerFallDamage, "CSGameRules_FlPlayerFallDamage")
    RegisterHookChain(RG_ShowVGUIMenu, "ShowVGUIMenu")
    RegisterHookChain(RG_CSGameRules_FPlayerCanRespawn, "CSGameRules_FPlayerCanRespawn")
    RegisterHookChain(RG_CBasePlayer_Spawn, "CBasePlayer_Spawn")
    RegisterHookChain(RG_CBasePlayer_Spawn, "CBasePlayer_Spawn_Post", true)
    RegisterHookChain(RG_RoundEnd, "RoundEnd")
    RegisterHookChain(RG_RoundEnd, "RoundEnd_Post", true)
    RegisterHookChain(RG_CBasePlayer_AddAccount, "CBasePlayer_AddAccount")
    
    // clcmds
    register_clcmd("jointeam", "Cmd_Block_Main")
    register_clcmd("joinclass", "Cmd_Block_Main")
    register_clcmd("showbriefing", "Cmd_Block_Main")
    register_clcmd("radio1", "Cmd_Block_Main")
    register_clcmd("radio2", "Cmd_Block_Main")
    register_clcmd("radio3", "Cmd_Block_Main")
    
    // dictionary
    register_dictionary("DeathrunCORE.txt")
}

public plugin_natives()
{
    ArrayForwardRandomize = ArrayCreate(FORWARD_DATA)
    ArrayForwards = ArrayCreate(FORWARDS)
    ArrayButtons = ArrayCreate()
    
    register_library("DeathrunCORE")
    register_native("SetDRMCoreArg", "native_set_arg")
    register_native("DRMCoreForward", "native_register_forward")
    register_native("DRMCoreEnableForward", "native_enable_forward")
    register_native("DRMCoreDisableForward", "native_disable_forward")
    
    register_native("dr_set_ter", "native_set_ter")
}

public plugin_precache()
{
    RegisterHam(Ham_Spawn, "func_button", "FwdFuncButtonSpawn_Post", true)
}