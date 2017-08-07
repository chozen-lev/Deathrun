// HOOKS
///////////////////////////////////////////

public DRMCore_Randomize_Post(id, RandomizeEvents:event)
{ 
    if (task_exists(g_iCurrTer + TASK_AUTOCHOOSE) && event == m_Dropped)
    {
        remove_task(g_iCurrTer + TASK_AUTOCHOOSE)
        
        if (id != 0) {
            CBasePlayer_RoundRespawn_Post(g_iCurrTer = id)
        }
    }
    
    g_iCurrTer = id
}

public CBasePlayer_RoundRespawn_Post(id)
{
    if (id == g_iCurrTer && g_CurrMode[m_Index] == ModeIndexes:m_None)
    {
        static i, access, info[4], name[32], callback, ret, hc_state, fwdData[FORWARD_DATA], AvailModesNum
        ArrayClear(ArrayAvailModes)
        
        for (i = 0; i < menu_items(g_iModeMenu); i++)
        {
            menu_item_getinfo(g_iModeMenu, i, access, info, charsmax(info), name, charsmax(name), callback)
            
            if (callback != -1)
            {
                ExecuteForward(callback, ret, id, g_iModeMenu, i)
                
                if (ret != ITEM_DISABLED) {
                    ArrayPushCell(ArrayAvailModes, str_to_num(info))
                }
            }
            else ArrayPushCell(ArrayAvailModes, str_to_num(info))
        }
        
        hc_state = HC_CONTINUE
        AvailModesNum = ArraySize(ArrayAvailModes)
        
        if (AvailModesNum > 1)
        {
            static iMenu, time
            
            time = get_cvar_num("mp_freezetime")
            
            if (time <= 0) {
                time = 0
            } else if (time > 60) {
                time = 60
            }
            
            for (i = 0; i < ArraySize(ArrayForwardShowMenu); i++)
            {
                ArrayGetArray(ArrayForwardShowMenu, i, fwdData)
                
                if (fwdData[m_Post] == 0 && fwdData[m_State])
                {
                    ExecuteForward(fwdData[m_fwdIndex], ret, id, g_iModeMenu, time)
                    
                    if (ret == HC_BREAK)
                        return
                    
                    if (ret > hc_state)
                        hc_state = ret
                }
            }
            
            if (hc_state != HC_SUPERCEDE)
            {
                if (g_bArgState[0])
                {
                    id = g_iArg[0]
                    g_bArgState[0] = false
                }
                
                if (!g_bArgState[1])
                    iMenu = g_iModeMenu
                else
                {
                    iMenu = g_iArg[1]
                    g_bArgState[1] = false
                }
                
                if (g_bArgState[2])
                {
                    time = g_iArg[2]
                    g_bArgState[2] = false
                }
                
                menu_display(id, iMenu, 0, time)
                
                set_task(float(time), "Task_CloseMenu", id + TASK_AUTOCHOOSE)
            }
            
            for (i = 0; i < ArraySize(ArrayForwardShowMenu); i++)
            {
                ArrayGetArray(ArrayForwardShowMenu, i, fwdData)
                
                if (fwdData[m_Post] && fwdData[m_State])
                {
                    ExecuteForward(fwdData[m_fwdIndex], ret, id, iMenu, time)
                    
                    if (ret == HC_BREAK)
                        return
                }
            }
        }
        else if (AvailModesNum == 1)
        {
            static Data[ModeData], ModeIndexes:iNextMode, ModeEvents:event
            
            menu_item_getinfo(g_iModeMenu, ArrayGetCell(ArrayAvailModes, 0), access, info, charsmax(info), name, charsmax(name), callback)
            
            ArrayGetArray(ArrayModes, ArrayFindValue(ArrayModeIndexes, str_to_num(info)), Data)
            
            for (i = 0; i < ArraySize(ArrayForwardChangeMode); i++)
            {
                ArrayGetArray(ArrayForwardChangeMode, i, fwdData)
                
                if (fwdData[m_Post] == 0 && fwdData[m_State])
                {
                    ExecuteForward(fwdData[m_fwdIndex], ret, Data[m_Index], m_Auto)
                    
                    if (ret == HC_BREAK)
                        return
                    
                    if (ret > hc_state)
                        hc_state = ret
                }
            }
            
            if (hc_state != HC_SUPERCEDE)
            {
                if (!g_bArgState[0])
                    iNextMode = Data[m_Index]
                else
                {
                    iNextMode = ModeIndexes:g_iArg[0]
                    g_bArgState[0] = false
                    
                    ArrayGetArray(ArrayModes, ArrayFindValue(ArrayModeIndexes, iNextMode), Data)
                }
                
                if (!g_bArgState[1])
                    event = m_Auto
                else
                {
                    event = ModeEvents:g_iArg[1]
                    g_bArgState[1] = false
                }
                
                if (Data[m_Index] != ModeIndexes:m_None)
                {
                    client_print_color(0, print_team_default, "%L%L", LANG_PLAYER, g_langPrefix, LANG_PLAYER, "DR_MODE_AUTO", LANG_PLAYER, Data[m_langName])
                }
                
                g_CurrMode = Data
            }
            
            for (i = 0; i < ArraySize(ArrayForwardChangeMode); i++)
            {
                ArrayGetArray(ArrayForwardChangeMode, i, fwdData)
                
                if (fwdData[m_Post] && fwdData[m_State])
                {
                    ExecuteForward(fwdData[m_fwdIndex], ret, iNextMode, event)
                    
                    if (ret == HC_BREAK)
                        return
                }
            }
        }
    }
}

public CSGameRules_RestartRound()
{
    if (get_cvar_num("mp_freezetime"))
    {
        EnableHookChain(g_iFwdRoundFreezeEnd)
        EnableHamForward(g_iFwdTriggerPushTouch)
    }
    
    if (g_CurrMode[m_Index] != ModeIndexes:m_None)
    {
        static Data[ModeData], i, ModeEvents:event, ret, hc_state, fwdData[FORWARD_DATA]
        
        hc_state = HC_CONTINUE
        
        for (i = 0; i < ArraySize(ArrayForwardChangeMode); i++)
        {
            ArrayGetArray(ArrayForwardChangeMode, i, fwdData)
            
            if (fwdData[m_Post] == 0 && fwdData[m_State])
            {
                ExecuteForward(fwdData[m_fwdIndex], ret, g_NoneMode[m_Index], m_Reset)
                
                if (ret == HC_BREAK)
                    return
                
                if (ret > hc_state)
                    hc_state = ret
            }
        }
        
        if (hc_state != HC_SUPERCEDE)
        {
            if (!g_bArgState[0])
                Data = g_NoneMode
            else
            {
                Data[m_Index] = ModeIndexes:g_iArg[0]
                g_bArgState[0] = false
                
                ArrayGetArray(ArrayModes, ArrayFindValue(ArrayModeIndexes, Data[m_Index]), Data)
            }
            
            if (!g_bArgState[1])
                event = ModeEvents:m_Reset
            else
            {
                event = ModeEvents:g_iArg[1]
                g_bArgState[1] = false
            }
            
            g_CurrMode = Data
        }
        
        for (i = 0; i < ArraySize(ArrayForwardChangeMode); i++)
        {
            ArrayGetArray(ArrayForwardChangeMode, i, fwdData)
            
            if (fwdData[m_Post] && fwdData[m_State])
            {
                ExecuteForward(fwdData[m_fwdIndex], ret, g_CurrMode[m_Index], event)
                
                if (ret == HC_BREAK)
                    return
            }
        }
    }
}

public CSGameRules_RestartRound_Post()
{
    if (get_cvar_num("mp_freezetime") && g_iCurrTer == 0)
    {
        set_member_game(m_iRoundTimeSecs, 0)
        set_member_game(m_fRoundStartTime, halflife_time())
        set_member_game(m_fRoundStartTimeReal, halflife_time())
    }
}

public RoundEnd_Post(WinStatus:status, ScenarioEventEndRound:event, Float:tmDelay)
{
    if (task_exists(g_iCurrTer + TASK_AUTOCHOOSE))
    {
        remove_task(g_iCurrTer + TASK_AUTOCHOOSE)
        show_menu(g_iCurrTer, 0, "^n")
    }
}

public CSGameRules_OnRoundFreezeEnd_Post()
{
    DisableHookChain(g_iFwdRoundFreezeEnd)
    DisableHamForward(g_iFwdTriggerPushTouch)
}

public FwdTriggerPushTouch(touched, toucher)
{
    return HAM_SUPERCEDE
}