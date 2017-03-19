// CVARS
///////////////////////////////////////////

new CVAR_ReconnectRespawn
new CVAR_NoFallDmg
new CVAR_BuyMenu

public plugin_cfg()
{
    // CVARs
    CVAR_ReconnectRespawn = create_cvar("mp_reconnectrespawn", "0")
    CVAR_NoFallDmg = create_cvar("mp_terrnfd", "1")
    CVAR_BuyMenu = create_cvar("mp_buymenu", "0")
    
    // Disable team balancing
    set_cvar_num("mp_limitteams", 0)
    set_cvar_num("mp_autoteambalance", 0)
    
    // Automatically join the CT team
    set_cvar_num("mp_auto_join_team", 1)
    set_cvar_string("humans_join_team", "CT")
    
    // Disable round time out check
    set_cvar_string("mp_round_infinite", "abcdeg")
    
    // Fix jamming on a mobile platform
    set_cvar_num("sv_force_ent_intersection", 1)
    
    // Disable freezetime on round start
    set_cvar_num("mp_freezetime", 0)
    
    DR_Init()
    
    // Restart first round and start the game
    set_cvar_num("sv_restart", 1)
}