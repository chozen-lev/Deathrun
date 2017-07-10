// HOOKS
///////////////////////////////////////////

public client_authorized(id)
{
    // Get some information about player
    get_user_authid(id, g_PlayerData[id][m_szSteamID], charsmax(g_PlayerData[][m_szSteamID]))
    get_user_name(id, g_PlayerData[id][m_szName], charsmax(g_PlayerData[][m_szName]))
}

public client_disconnected(id, bool:drop, message[], maxlen)
{
    // if terrorist left the game
    if (id == g_iCurrTer)
    {
        static iPlayers[32], iNum
        get_players(iPlayers, iNum, "eh", "CT")
        
        if (iNum > 1)
        {
            ReplaceTerrorist(RandomPlayer(iPlayers, iNum))
        }
        else
        {
            static i, Data[FORWARD_DATA], ret, hc_state, RandomizeEvents:event
            
            hc_state = HC_CONTINUE
            
            for (i = 0; i < ArraySize(ArrayForwardRandomize); i++)
            {
                ArrayGetArray(ArrayForwardRandomize, i, Data)
                
                if (Data[m_Post] == 0 && Data[m_State])
                {
                    ExecuteForward(Data[m_fwdIndex], ret, 0, m_Dropped)
                    
                    if (ret == HC_BREAK)
                        return
                    
                    if (ret > hc_state)
                        hc_state = ret
                }
            }
            
            if (hc_state != HC_SUPERCEDE)
            {
                if (!g_bArgState[0])
                    g_iCurrTer = 0
                else
                {
                    g_iCurrTer = g_iArg[0]
                    g_bArgState[0] = false
                }
                
                if (!g_bArgState[1])
                    event = m_Dropped
                else
                {
                    event = RandomizeEvents:g_iArg[1]
                    g_bArgState[1] = false
                }
            }
            
            for (i = 0; i < ArraySize(ArrayForwardRandomize); i++)
            {
                ArrayGetArray(ArrayForwardRandomize, i, Data)
                
                if (Data[m_Post] && Data[m_State])
                {
                    ExecuteForward(Data[m_fwdIndex], ret, g_iCurrTer, event)
                    
                    if (ret == HC_BREAK)
                        return
                }
            }
        }
    }
}

public CSGameRules_RestartRound()
{
    if (!g_bFirstRound)
    {
        if (!get_pcvar_bool(CVAR_ReconnectRespawn)) TrieClear(TrieReconnected)
        
        static iPlayers[32], iNum
        get_players(iPlayers, iNum, "eh", "TERRORIST")
        
        // Transfer all terrorists to the TEAM_CT
        for (new i = 0; i < iNum; i++) rg_set_user_team(iPlayers[i], TEAM_CT)
        
        get_players(iPlayers, iNum, "eh", "CT")
        
        // Selecting next terrorist
        if (iNum > 1)
        {
            static iRandomPlayer, RandomizeEvents:event, i, Data[FORWARD_DATA], ret, hc_state
            iRandomPlayer = RandomPlayer(iPlayers, iNum)
            
            hc_state = HC_CONTINUE
            
            for (i = 0; i < ArraySize(ArrayForwardRandomize); i++)
            {
                ArrayGetArray(ArrayForwardRandomize, i, Data)
                
                if (Data[m_Post] == 0 && Data[m_State])
                {
                    ExecuteForward(Data[m_fwdIndex], ret, iRandomPlayer, m_Reset)
                    
                    if (ret == HC_BREAK)
                        return
                    
                    if (ret > hc_state)
                        hc_state = ret
                }
            }
            
            if (hc_state != HC_SUPERCEDE)
            {
                if (!g_bArgState[0])
                    g_iCurrTer = iRandomPlayer
                else
                {
                    g_iCurrTer = g_iArg[0]
                    g_bArgState[0] = false
                }
                
                if (!g_bArgState[1])
                    event = m_Reset
                else
                {
                    event = RandomizeEvents:g_iArg[1]
                    g_bArgState[1] = false
                }
                
                if (IsPlayer(g_iCurrTer))
                {
                    rg_set_user_team(g_iCurrTer, TEAM_TERRORIST)
                    client_print_color(0, print_team_red, "%L%L", LANG_PLAYER, g_langPrefix, LANG_PLAYER, "DR_CURR_TER", g_PlayerData[g_iCurrTer][m_szName])
                }
            }
            
            for (i = 0; i < ArraySize(ArrayForwardRandomize); i++)
            {
                ArrayGetArray(ArrayForwardRandomize, i, Data)
                
                if (Data[m_Post] && Data[m_State])
                {
                    ExecuteForward(Data[m_fwdIndex], ret, g_iCurrTer, event)
                    
                    if (ret == HC_BREAK)
                        return
                }
            }
        }
    }
}

public CSGameRules_RestartRound_Post()
{
    g_RoundStarted = true
    
    if (g_bFirstRound)
    {
        g_bFirstRound = false
        
        // Restart round and start the game in 10 seconds
        set_cvar_num("sv_restart", 10)
        set_member_game(m_iRoundTime, 11)
    }
}

public CBasePlayer_SetClientUserInfoName_Post(const id, infobuffer[], szNewName[])
{
    // Hook the name change to display correctly
    formatex(g_PlayerData[id][m_szName], charsmax(g_PlayerData[][m_szName]), szNewName)
}

public CSGameRules_FlPlayerFallDamage(id)
{
    if (get_pcvar_bool(CVAR_NoFallDmg) && id == g_iCurrTer)
    {
        // Remove the damage to the terrorist when falling from height
        SetHookChainReturn(ATYPE_FLOAT, 0.0)
        return HC_SUPERCEDE
    }
    
    return HC_CONTINUE
}

// Block unnecessary menus
public ShowVGUIMenu(const id, VGUIMenu:menuType, const bitsSlots, szOldMenu[])
{
    switch (menuType)
    {
        case VGUI_Menu_Team, VGUI_Menu_MapBriefing:return HC_BREAK
        case VGUI_Menu_Buy..VGUI_Menu_Buy_Item:
        {
            if(!get_pcvar_bool(CVAR_BuyMenu))
                return HC_BREAK
        }
    }
    
    return HC_CONTINUE
}

public CSGameRules_FPlayerCanRespawn(id)
{
    if (!get_pcvar_bool(CVAR_ReconnectRespawn) && TrieKeyExists(TrieReconnected, g_PlayerData[id][m_szSteamID]))
    {
        SetHookChainReturn(ATYPE_INTEGER, false)
        return HC_SUPERCEDE
    }
    
    return HC_CONTINUE
}

public CBasePlayer_Spawn_Post(id)
{
    if (is_user_connected(id) && !get_pcvar_bool(CVAR_ReconnectRespawn) && !TrieKeyExists(TrieReconnected, g_PlayerData[id][m_szSteamID]))
        TrieSetCell(TrieReconnected, g_PlayerData[id][m_szSteamID], true)
}

public RoundEnd(WinStatus:status, ScenarioEventEndRound:event, Float:tmDelay)
{
    if (!g_RoundStarted)
    {
        SetHookChainReturn(ATYPE_INTEGER, false)
        return HC_BREAK
    }
    
    return HC_CONTINUE
}

public RoundEnd_Post(WinStatus:status, ScenarioEventEndRound:event, Float:tmDelay)
{
    g_RoundStarted = false
}

public CBasePlayer_AddAccount(const id, amount, RewardType:type, bool:bTrackChange)
{
	if (get_pcvar_bool(CVAR_BlockRewards) && type != RT_NONE) {
		return HC_BREAK
	}
	
	return HC_CONTINUE
}