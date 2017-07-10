// HOOKS
///////////////////////////////////////////

public CBasePlayer_AddAccount(const id, amount, RewardType:type, bool:bTrackChange)
{
    if (g_PlayerData[id][m_Account] + amount > g_CVAR[m_MaxMoney])
    {
        SetHookChainArg(2, ATYPE_INTEGER, g_CVAR[m_MaxMoney] - g_PlayerData[id][m_Account])
    }
    else if (g_PlayerData[id][m_Account] + amount < 0)
    {
        SetHookChainArg(2, ATYPE_INTEGER, g_PlayerData[id][m_Account])
    }
}

public CBasePlayer_AddAccount_Post(const id, amount, RewardType:type, bool:bTrackChange)
{
    if (type == RT_PLAYER_BOUGHT_SOMETHING)
    {
        g_PlayerData[id][m_Account] = get_member(id, m_iAccount)
    }
    else if (type == RT_PLAYER_RESET)
    {
        g_PlayerData[id][m_Account] = g_CVAR[m_StartMoney]
    }
    else 
    {
        g_PlayerData[id][m_Account] += amount
    }
    
    if (!is_user_alive(id))
    {
        message_begin(MSG_ONE, g_iMsgStatusValue, _, id)
        write_long(2)
        write_short(id)
        message_end()
    }
}

public client_disconnected(id, bool:drop, message[], maxlen)
{
    g_PlayerData[id][m_Account] = 0
    g_PlayerData[id][m_Discount] = 0.0
}