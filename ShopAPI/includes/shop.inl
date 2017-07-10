// SHOP
///////////////////////////////////////////

public ItemIndexes:native_RegisterItem(CPlugin, CParams)
{
    static Data[ItemData]
    
    Data[m_Index] = ItemIndexes:get_param(1)
    
    if (Data[m_Index] == ItemIndexes:m_None)
    {
        Data[m_Index] = ItemIndexes:g_iItemsMax
        g_iItemsMax++
    }
    
    get_string(2, Data[m_langName], charsmax(Data[m_langName]))
    Data[m_Cost] = get_param(3)
    
    ArrayPushArray(ArrayItems, Data)
    ArrayPushCell(ArrayItemIndexes, Data[m_Index])
    
    return Data[m_Index]
}

public native_register_forward(CPlugin, CParams)
{
    static Data[FORWARDS], ForwardData[FORWARD_DATA], szHandler[128]
    
    get_string(2, szHandler, charsmax(szHandler))
    ForwardData[m_Post] = get_param(3)
    ForwardData[m_State] = true
    
    switch(get_param(1))
    {
        case Shop_ShowMenu:
        {
            ForwardData[m_fwdIndex] = CreateOneForward(CPlugin, szHandler, FP_CELL, FP_CELL)
            
            Data[m_Function] = Shop_ShowMenu
            Data[m_Array] = ArrayForwardShowMenu
        }
        case Shop_MenuAddItem:
        {
            ForwardData[m_fwdIndex] = CreateOneForward(CPlugin, szHandler, FP_CELL, FP_CELL, FP_CELL, FP_CELL)
            
            Data[m_Function] = Shop_MenuAddItem
            Data[m_Array] = ArrayForwardMenuAddItem
        }
        case Shop_BuyItem:
        {
            ForwardData[m_fwdIndex] = CreateOneForward(CPlugin, szHandler, FP_CELL, FP_CELL, FP_CELL)
            
            Data[m_Function] = Shop_BuyItem
            Data[m_Array] = ArrayForwardBuyItem
        }
    }
    
    Data[m_arrIndex] = ArrayPushArray(Data[m_Array], ForwardData)
    
    return ArrayPushArray(ArrayForwards, Data)
}

public native_set_arg(CPlugin, CParams)
{
    if (AType:get_param(2) == ATYPE_INTEGER)
    {
        static number; number = get_param(1) - 1
        
        g_iArg[number] = get_param(3)
        g_bArgState[number] = true
    }
}

public bool:native_enable_forward(CPlugin, CParams)
{
    static Data[FORWARDS], ForwardData[FORWARD_DATA]
    
    if (!ArrayGetArray(ArrayForwards, get_param(1), Data))
        return false
    
    ArrayGetArray(Data[m_Array], Data[m_arrIndex], ForwardData)
    
    ForwardData[m_State] = true
    
    ArraySetArray(Data[m_Array], Data[m_arrIndex], ForwardData)
    
    return true
}

public bool:native_disable_forward(CPlugin, CParams)
{
    static Data[FORWARDS], ForwardData[FORWARD_DATA]
    
    if (!ArrayGetArray(ArrayForwards, get_param(1), Data))
        return false
    
    ArrayGetArray(Data[m_Array], Data[m_arrIndex], ForwardData)
    
    ForwardData[m_State] = false
    
    ArraySetArray(Data[m_Array], Data[m_arrIndex], ForwardData)
    
    return true
}

public Show_MenuShop(id)
{
    static iMenu, szText[128], info[8], i, j, Data[ItemData], ret, hc_state, fwdData[FORWARD_DATA]
    
    if (g_PlayerData[id][m_Discount] <= 0)
    {
        formatex(szText, charsmax(szText), "%L\R\d", LANG_PLAYER, "SHOP_TITLE", LANG_PLAYER)
    }
    else
    {
        formatex(szText, charsmax(szText), "%L^n\w%L\R\d", LANG_PLAYER, "SHOP_TITLE", LANG_PLAYER, "PLAYER_DISCOUNT", g_PlayerData[id][m_Discount])
    }
    
    iMenu = menu_create(szText, "Handle_MenuShop")
    
    for (i = 0; i < ArraySize(ArrayItems); i++)
    {
        ArrayGetArray(ArrayItems, i, Data)
        
        hc_state = HC_CONTINUE
        
        for (j = 0; j < ArraySize(ArrayForwardMenuAddItem); j++)
        {
            ArrayGetArray(ArrayForwardMenuAddItem, j, fwdData)
            
            if (fwdData[m_Post] == 0 && fwdData[m_State])
            {
                ExecuteForward(fwdData[m_fwdIndex], ret, id, iMenu, i, Data[m_Index])
                
                if (ret == HC_BREAK)
                    break
                
                if (ret > hc_state)
                    hc_state = ret
            }
        }
        
        if (ret == HC_BREAK)
        {
            continue
        }
        else if (hc_state != HC_SUPERCEDE)
        {
            if (g_bArgState[0])
            {
                id = g_iArg[0]
                g_bArgState[0] = false
            }
            
            if (g_bArgState[1])
            {
                iMenu = g_iArg[1]
                g_bArgState[1] = false
            }
            
            if (g_bArgState[2])
            {
                Data[m_Index] = ItemIndexes:g_iArg[2]
                g_bArgState[2] = false
            }
            
            formatex(szText, charsmax(szText), "%L \y- $%d", LANG_PLAYER, Data[m_langName], Data[m_Cost])
            num_to_str(_:Data[m_Index], info, charsmax(info))
            
            menu_additem(iMenu, szText, info, 0, g_iCallback)
        }
        
        for (j = 0; j < ArraySize(ArrayForwardMenuAddItem); j++)
        {
            ArrayGetArray(ArrayForwardMenuAddItem, j, fwdData)
            
            if (fwdData[m_Post] && fwdData[m_State])
            {
                ExecuteForward(fwdData[m_fwdIndex], ret, id, iMenu, i, Data[m_Index])
                
                if (ret == HC_BREAK)
                    break
            }
        }
    }
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "PLAYER_NEXT")
    menu_setprop(iMenu, MPROP_NEXTNAME, szText)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "PLAYER_BACK")
    menu_setprop(iMenu, MPROP_BACKNAME, szText)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "PLAYER_EXIT")
    menu_setprop(iMenu, MPROP_EXITNAME, szText)
    
    hc_state = HC_CONTINUE
    
    for (i = 0; i < ArraySize(ArrayForwardShowMenu); i++)
    {
        ArrayGetArray(ArrayForwardShowMenu, i, fwdData)
        
        if (fwdData[m_Post] == 0 && fwdData[m_State])
        {
            ExecuteForward(fwdData[m_fwdIndex], ret, id, iMenu)
            
            if (ret == HC_BREAK)
                break
            
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
        
        if (g_bArgState[1])
        {
            iMenu = g_iArg[1]
            g_bArgState[1] = false
        }
        
        menu_display(id, iMenu)
    }
    
    for (i = 0; i < ArraySize(ArrayForwardShowMenu); i++)
    {
        ArrayGetArray(ArrayForwardShowMenu, i, fwdData)
        
        if (fwdData[m_Post] && fwdData[m_State])
        {
            ExecuteForward(fwdData[m_fwdIndex], ret, id, iMenu)
            
            if (ret == HC_BREAK)
                break
        }
    }
    
    return PLUGIN_HANDLED_MAIN
}

public CallBack_MenuShop(id, menu, item)
{
    static Data[ItemData], access, info[8], name[128], callback
    menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
    
    ArrayGetArray(ArrayItems, ArrayFindValue(ArrayItemIndexes, str_to_num(info)), Data)
    
    if (g_PlayerData[id][m_Account] < Data[m_Cost])
        return ITEM_DISABLED
    
    return ITEM_ENABLED
}

public Handle_MenuShop(id, menu, item)
{
    if (item == MENU_EXIT)
    {
        menu_destroy(menu)
    }
    else
    {
        static access, info[8], name[128], callback, ret
        menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
        
        ExecuteForward(callback, ret, id, menu, item)
        
        if (ret == ITEM_DISABLED)
        {
            menu_destroy(menu)
            
            Show_MenuShop(id)
        }
        else
        {
            static i, Data[ItemData], hc_state, fwdData[FORWARD_DATA]
            
            ArrayGetArray(ArrayItems, ArrayFindValue(ArrayItemIndexes, str_to_num(info)), Data)
            
            hc_state = HC_CONTINUE
            
            for (i = 0; i < ArraySize(ArrayForwardBuyItem); i++)
            {
                ArrayGetArray(ArrayForwardBuyItem, i, fwdData)
                
                if (fwdData[m_Post] == 0 && fwdData[m_State])
                {
                    ExecuteForward(fwdData[m_fwdIndex], ret, id, menu, item)
                    
                    if (ret == HC_BREAK)
                        break
                    
                    if (ret > hc_state)
                        hc_state = ret
                }
            }
            
            if (hc_state != HC_SUPERCEDE)
            {
                rg_add_account(id, -Data[m_Cost])
                
                menu_destroy(menu)
            }

            for (i = 0; i < ArraySize(ArrayForwardBuyItem); i++)
            {
                ArrayGetArray(ArrayForwardBuyItem, i, fwdData)
                
                if (fwdData[m_Post] && fwdData[m_State])
                {
                    ExecuteForward(fwdData[m_fwdIndex], ret, id, menu, item)
                    
                    if (ret == HC_BREAK)
                        break
                }
            }
        }
    }
}