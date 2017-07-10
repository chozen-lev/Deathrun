// CVARS
///////////////////////////////////////////

public OnConfigsExecuted()
{
    g_CVAR[m_StartMoney] = get_cvar_num("mp_startmoney")
    g_CVAR[m_MaxMoney] = get_cvar_num("mp_maxmoney")
}