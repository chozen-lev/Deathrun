// CVARS
///////////////////////////////////////////

new CVAR_ReconnectRespawn
new CVAR_NoFallDmg
new CVAR_BuyMenu
new CVAR_BlockRewards
new CVAR_BlockDamageOnSpawn
new CVAR_BlockBvelosityOnSpawn

public plugin_cfg()
{
    // CVARs
    CVAR_ReconnectRespawn = create_cvar("mp_reconnectrespawn", "0")
    CVAR_NoFallDmg = create_cvar("mp_terrnfd", "1")
    CVAR_BuyMenu = create_cvar("mp_buymenu", "0")
    CVAR_BlockRewards = create_cvar("dr_rewards", "1")
    CVAR_BlockDamageOnSpawn = create_cvar("dr_blockgamage", "1")
    CVAR_BlockBvelosityOnSpawn = create_cvar("dr_blockbvelocity", "1")
    
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
    
    DR_Init()
    
    // Restart first round and start the game
    set_cvar_num("sv_restart", 1)
}