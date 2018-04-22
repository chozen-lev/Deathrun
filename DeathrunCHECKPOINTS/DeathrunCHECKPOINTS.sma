#include <amxmodx>
#include <reapi>
#include <fakemeta>

#define VERSION "1.0.0-dev+"

#define ACCESS_FLAG ADMIN_RCON

#define ENT_CLASSNAME "info_point"

// Messages prefix
new const g_langPrefix[] = "DR_CORE_PREFIX"

enum _:PointData
{
    m_Index,
    Float:m_vOrigin[3],
    Float:m_vMins[3],
    Float:m_vMaxs[3],
    m_iPointType
}

enum _:PLAYER_DATA
{
    m_iActiveIndex,
    m_iPropVector,
    m_iPropAxis,
    m_iPropSign,
    m_iPropAmount,
    HookChain:m_iFwdPlayerPostThink,
    m_szName[32]
}

enum Groups (<<= 1)
{
    GROUP_NONE,
    GROUP_INGAME = 1,
    GROUP_STARTED,
    GROUP_FINISHED
}

new Float:PropAmounts[] =
{
    0.1,
    1.0,
    10.0,
    50.0,
    200.0
}

new g_langSigns[][] =
{
    "DR_POINTS_PROPERTIES_PLUS",
    "DR_POINTS_PROPERTIES_MINUS"
}

new g_langPointTypes[][] =
{
    "DR_POINTS_PROPERTIES_START",
    "DR_POINTS_PROPERTIES_FINISH_TRIGGER",
    "DR_POINTS_PROPERTIES_FINISH_WALL"
}

new Array:ArrayPoints
new g_PlayerData[32][PLAYER_DATA]

new g_iPointsCallback, g_iPropertiesCallback

new g_szFilePath[256]

public plugin_natives()
{
    ArrayPoints = ArrayCreate(PointData)
}

public plugin_precache()
{
    register_forward(FM_Spawn, "FwdSpawn_Post", true)
}

public plugin_init()
{
    register_plugin("[ReAPI]Deathrun CHECKPOINTS", VERSION, "ChoZen")
    
    // clcmds
    register_clcmd("points_menu", "Show_MenuPoints")
    register_clcmd("radio3", "Show_MenuPoints")
    
    // menu callbacks
    g_iPointsCallback = menu_makecallback("CallBack_MenuPoints")
    g_iPropertiesCallback = menu_makecallback("CallBack_MenuProperties")
    
    // hooks
    register_forward(FM_Touch, "FwdPointTouch_Post", true)
    RegisterHookChain(RG_CBasePlayer_Spawn, "CBasePlayer_Spawn_Post", true)
    RegisterHookChain(RG_CBasePlayer_SetClientUserInfoName, "CBasePlayer_SetClientUserInfoName_Post", true)
    
    // dictionary
    register_dictionary("DeathrunCHECKPOINTS.txt")
}

public plugin_cfg()
{
    get_localinfo("amxx_datadir", g_szFilePath, charsmax(g_szFilePath))
    formatex(g_szFilePath, charsmax(g_szFilePath),"%s/checkpoints", g_szFilePath)
    
    if (dir_exists(g_szFilePath) == 0) {
        mkdir(g_szFilePath)
    }
    
    static szMapName[32], pFile
    get_mapname(szMapName, charsmax(szMapName))
    formatex(g_szFilePath, charsmax(g_szFilePath),"%s/%s.dat", g_szFilePath, szMapName)
    
    if ((pFile = fopen(g_szFilePath, "rb")) == 0)
    {
        log_amx("Map %s has any checkpoints", szMapName)
        return PLUGIN_CONTINUE
    }
    
    static Data[PointData], Float:vSize[3]
    new iFinishNum
    
    while (fread_blocks(pFile, Data[m_vOrigin], sizeof Data - m_vOrigin, BLOCK_INT))
    {
        Data[m_Index] = rg_create_entity("info_target")
        set_entvar(Data[m_Index], var_classname, ENT_CLASSNAME)
        set_entvar(Data[m_Index], var_movetype, MOVETYPE_FLY)
        
        set_entvar(Data[m_Index], var_origin, Data[m_vOrigin])
        
        set_entvar(Data[m_Index], var_mins, Data[m_vMins])    
        set_entvar(Data[m_Index], var_maxs, Data[m_vMaxs])
        
        vSize[0] = Data[m_vMaxs][0] - Data[m_vMins][0]
        vSize[1] = Data[m_vMaxs][1] - Data[m_vMins][1]
        vSize[2] = Data[m_vMaxs][2] - Data[m_vMins][2]
        
        set_entvar(Data[m_Index], var_size, vSize)
        
        if (Data[m_iPointType] == 2)
        {
            set_entvar(Data[m_Index], var_classname, "func_wall")
            set_entvar(Data[m_Index], var_solid, SOLID_BBOX)
            set_entvar(Data[m_Index], var_groupinfo, GROUP_FINISHED)
        }
        else
        {
            if (Data[m_iPointType] == 1) {
                iFinishNum++
            }
            
            set_entvar(Data[m_Index], var_classname, ENT_CLASSNAME)
            set_entvar(Data[m_Index], var_solid, SOLID_TRIGGER)
            set_entvar(Data[m_Index], var_groupinfo, GROUP_STARTED)
            set_entvar(Data[m_Index], var_iuser1, Data[m_iPointType])
        }
        
        ArrayPushArray(ArrayPoints, Data)
    }
    
    fclose(pFile)
    
    if (ArraySize(ArrayPoints) == 1) {
        server_print("[CHECKPOINTS] 1 checkpoint has been loaded")
    } else {
        server_print("[CHECKPOINTS] %d checkpoints have been loaded", ArraySize(ArrayPoints))
    }
    
    if (iFinishNum == 0) {
        server_print("[CHECKPOINTS] Map %s has any FINISH checkpoints", szMapName)
        log_amx("Map %s has any FINISH checkpoints", szMapName)
    }
    
    return PLUGIN_CONTINUE
}

public client_authorized(id)
{
    get_user_name(id, g_PlayerData[id][m_szName], charsmax(g_PlayerData[][m_szName]))
}

public Show_MenuPoints(id)
{
    if(!(get_user_flags(id) & ACCESS_FLAG)) {
        return PLUGIN_CONTINUE
    }
    
    if (g_PlayerData[id][m_iActiveIndex] == -1 && ArraySize(ArrayPoints)) {
        g_PlayerData[id][m_iActiveIndex] = 0
    } else if (g_PlayerData[id][m_iActiveIndex] >= ArraySize(ArrayPoints)) {
        g_PlayerData[id][m_iActiveIndex] = ArraySize(ArrayPoints) - 1
    }

    if (ArraySize(ArrayPoints) > 0 && g_PlayerData[id][m_iFwdPlayerPostThink] == INVALID_HOOKCHAIN) {
        g_PlayerData[id][m_iFwdPlayerPostThink] = RegisterHookChain(RG_CBasePlayer_PostThink, "CBasePlayer_PostThink_Post", true)
    }
    
    static iMenu, szText[128]
    
    formatex(szText, charsmax(szText), "%L^n\w%L", LANG_PLAYER, "DR_POINTS_TITLE", LANG_PLAYER, "DR_POINTS_NUMBER", ArraySize(ArrayPoints))
    
    if (g_PlayerData[id][m_iActiveIndex] > -1) {
        formatex(szText, charsmax(szText), "%s \d| \w%L", szText, LANG_PLAYER, "DR_POINTS_ACTIVE", g_PlayerData[id][m_iActiveIndex] + 1)
    }
    
    iMenu = menu_create(szText, "Handle_MenuPoints")
    menu_setprop(iMenu, MPROP_PERPAGE, 0)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_CREATE")
    menu_additem(iMenu, szText, "1", ACCESS_FLAG, g_iPointsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_REMOVE")
    menu_additem(iMenu, szText, "2", ACCESS_FLAG, g_iPointsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_EDIT")
    menu_additem(iMenu, szText, "3", ACCESS_FLAG, g_iPointsCallback)
    
    menu_addblank2(iMenu)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_SAVE")
    menu_additem(iMenu, szText, "5", ACCESS_FLAG, g_iPointsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_LOAD")
    menu_additem(iMenu, szText, "6", ACCESS_FLAG, g_iPointsCallback)

    menu_addblank2(iMenu)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_PREVIOUS")
    menu_additem(iMenu, szText, "8", ACCESS_FLAG, g_iPointsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_NEXT")
    menu_additem(iMenu, szText, "9", ACCESS_FLAG, g_iPointsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "PLAYER_EXIT")
    menu_additem(iMenu, szText, "0")
    
    menu_display(id, iMenu)
    
    return PLUGIN_HANDLED
}

public CallBack_MenuPoints(id, menu, item)
{
    static access, info[8], name[128], callback, iKey
    menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
    
    iKey = str_to_num(info)
    
    if ((iKey == 2 || iKey == 3) && g_PlayerData[id][m_iActiveIndex] == -1) {
        return ITEM_DISABLED
    }
    
    if ((iKey == 8 || iKey == 9) && ArraySize(ArrayPoints) <= 1) {
        return ITEM_DISABLED
    }
    
    if (iKey == 8 && g_PlayerData[id][m_iActiveIndex] <= 0) {
        return ITEM_DISABLED
    }
    
    if (iKey == 9 && g_PlayerData[id][m_iActiveIndex] >= ArraySize(ArrayPoints) - 1) {
        return ITEM_DISABLED
    }
    
    return ITEM_IGNORE
}

public Handle_MenuPoints(id, menu, item)
{
    if (item == MENU_EXIT)
    {
        if (g_PlayerData[id][m_iFwdPlayerPostThink] != INVALID_HOOKCHAIN)
        {
            DisableHookChain(g_PlayerData[id][m_iFwdPlayerPostThink])
            g_PlayerData[id][m_iFwdPlayerPostThink] = INVALID_HOOKCHAIN
        }
        
        menu_destroy(menu)
        return PLUGIN_CONTINUE
    }
    
    static access, info[8], name[128], callback
    menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
    
    switch (str_to_num(info))
    {
        case 1:
        {
            static Data[PointData], Float:vOrigin[3]
            
            Data[m_Index] = rg_create_entity("info_target")
            set_entvar(Data[m_Index], var_classname, ENT_CLASSNAME)
            set_entvar(Data[m_Index], var_movetype, MOVETYPE_FLY)
            
            get_entvar(id, var_origin, vOrigin)
            set_entvar(Data[m_Index], var_origin, vOrigin)
            
            set_entvar(Data[m_Index], var_mins, Float:{-8.0,-8.0,-32.0})    
            set_entvar(Data[m_Index], var_maxs, Float:{8.0,8.0,32.0})
            set_entvar(Data[m_Index], var_size, Float:{16.0,16.0,64.0})
            
            set_entvar(Data[m_Index], var_solid, SOLID_TRIGGER)
            
            set_entvar(Data[m_Index], var_groupinfo, GROUP_STARTED)
            
            g_PlayerData[id][m_iActiveIndex] = ArrayPushArray(ArrayPoints, Data)
        }
        case 2:
        {
            static Data[PointData]
            ArrayGetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
            
            set_entvar(Data[m_Index], var_flags, FL_KILLME)
            set_entvar(Data[m_Index], var_nextthink, get_gametime())
            
            ArrayDeleteItem(ArrayPoints, g_PlayerData[id][m_iActiveIndex])
            
            if (ArraySize(ArrayPoints) == 0)
            {
                DisableHookChain(g_PlayerData[id][m_iFwdPlayerPostThink])
                g_PlayerData[id][m_iFwdPlayerPostThink] = INVALID_HOOKCHAIN
            }
            
            if (g_PlayerData[id][m_iActiveIndex] != 0 || ArraySize(ArrayPoints) < 1) {
                g_PlayerData[id][m_iActiveIndex]--
            }
        }
        case 3:
        {
            menu_destroy(menu)
            Show_MenuProperties(id)
            return PLUGIN_CONTINUE
        }
        case 5:
        {
            static pFile
            
            if (ArraySize(ArrayPoints) == 0) {
                delete_file(g_szFilePath)
            } else if ((pFile = fopen(g_szFilePath, "wb")) == 0) {
                log_amx("Read file error(%s)", g_szFilePath)
            }
            else
            {
                static Data[PointData]
                
                for (new i = 0; i < ArraySize(ArrayPoints); i++)
                {
                    ArrayGetArray(ArrayPoints, i, Data)
                    
                    get_entvar(Data[m_Index], var_origin, Data[m_vOrigin])
                    get_entvar(Data[m_Index], var_mins, Data[m_vMins])
                    get_entvar(Data[m_Index], var_maxs, Data[m_vMaxs])
                    
                    fwrite_blocks(pFile, Data[m_vOrigin], sizeof Data - m_vOrigin, BLOCK_INT)
                }
                
                fclose(pFile)
            }
        }
        case 6:
        {
            static pFile
            
            ArrayClear(ArrayPoints)
            
            if ((pFile = fopen(g_szFilePath, "rb")) != 0)
            {
                static Data[PointData], Float:vSize[3]
                
                while (fread_blocks(pFile, Data[m_vOrigin], sizeof Data - m_vOrigin, BLOCK_INT))
                {
                    Data[m_Index] = rg_create_entity("info_target")
                    set_entvar(Data[m_Index], var_classname, ENT_CLASSNAME)
                    set_entvar(Data[m_Index], var_movetype, MOVETYPE_FLY)
                    
                    set_entvar(Data[m_Index], var_origin, Data[m_vOrigin])
                    
                    set_entvar(Data[m_Index], var_mins, Data[m_vMins])    
                    set_entvar(Data[m_Index], var_maxs, Data[m_vMaxs])
                    
                    vSize[0] = Data[m_vMaxs][0] - Data[m_vMins][0]
                    vSize[1] = Data[m_vMaxs][1] - Data[m_vMins][1]
                    vSize[2] = Data[m_vMaxs][2] - Data[m_vMins][2]
                    
                    set_entvar(Data[m_Index], var_size, vSize)
                    
                    if (Data[m_iPointType] == 2)
                    {
                        set_entvar(Data[m_Index], var_classname, "func_wall")
                        set_entvar(Data[m_Index], var_solid, SOLID_BBOX)
                        set_entvar(Data[m_Index], var_groupinfo, GROUP_FINISHED)
                    }
                    else
                    {
                        set_entvar(Data[m_Index], var_classname, ENT_CLASSNAME)
                        set_entvar(Data[m_Index], var_solid, SOLID_TRIGGER)
                        set_entvar(Data[m_Index], var_groupinfo, GROUP_STARTED)
                        set_entvar(Data[m_Index], var_iuser1, Data[m_iPointType])
                    }
                    
                    ArrayPushArray(ArrayPoints, Data)
                }
                
                fclose(pFile)
            }
        }
        case 8: {
            g_PlayerData[id][m_iActiveIndex]--
        }
        case 9: {
            g_PlayerData[id][m_iActiveIndex]++
        }
        default:
        {
            if (g_PlayerData[id][m_iFwdPlayerPostThink] != INVALID_HOOKCHAIN)
            {
                DisableHookChain(g_PlayerData[id][m_iFwdPlayerPostThink])
                g_PlayerData[id][m_iFwdPlayerPostThink] = INVALID_HOOKCHAIN
            }
            
            menu_destroy(menu)
            return PLUGIN_CONTINUE
        }
    }
    
    menu_destroy(menu)
    Show_MenuPoints(id)
    
    return PLUGIN_CONTINUE
}

public Show_MenuProperties(id)
{
    static iMenu, szText[128], Data[PointData]
    ArrayGetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
    
    formatex(szText, charsmax(szText), "%L\R\d^n\w%L\d", LANG_PLAYER, "DR_POINTS_PROPERTIES_TITLE", LANG_PLAYER, "DR_POINTS_ACTIVE", g_PlayerData[id][m_iActiveIndex] + 1)
    
    iMenu = menu_create(szText, "Handle_MenuProperties")
    menu_setprop(iMenu, MPROP_PERPAGE, 0)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_PROPERTIES_VECTOR", g_PlayerData[id][m_iPropVector] ? "maxs" : "mins")
    menu_additem(iMenu, szText, "1", ACCESS_FLAG, g_iPropertiesCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_PROPERTIES_AXIS", (g_PlayerData[id][m_iPropAxis] == 0) ? "X" : ((g_PlayerData[id][m_iPropAxis] == 1) ? "Y" : "Z"))
    menu_additem(iMenu, szText, "2", ACCESS_FLAG, g_iPropertiesCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_PROPERTIES_SIGN", LANG_PLAYER, g_langSigns[g_PlayerData[id][m_iPropSign]])
    menu_additem(iMenu, szText, "3", ACCESS_FLAG, g_iPropertiesCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_PROPERTIES_AMOUNT", PropAmounts[g_PlayerData[id][m_iPropAmount]])
    menu_additem(iMenu, szText, "4", ACCESS_FLAG, g_iPropertiesCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_PROPERTIES_APPLY")
    menu_additem(iMenu, szText, "5", ACCESS_FLAG, g_iPropertiesCallback)
    
    menu_addblank2(iMenu)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_POINTS_PROPERTIES_TYPE", LANG_PLAYER, g_langPointTypes[Data[m_iPointType]])
    menu_additem(iMenu, szText, "7", ACCESS_FLAG, g_iPointsCallback)
    
    menu_addblank2(iMenu)
    menu_addblank2(iMenu)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "PLAYER_BACK")
    menu_additem(iMenu, szText, "0")
    
    menu_display(id, iMenu)
    
    return PLUGIN_HANDLED
}

public CallBack_MenuProperties(id, menu, item)
{
    static access, info[8], name[128], callback
    menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
    
    if (str_to_num(info) == 5 && g_PlayerData[id][m_iPropSign])
    {
        static Data[PointData], Float:vMins[3], Float:vMaxs[3]        
        ArrayGetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
        
        get_entvar(Data[m_Index], var_mins, vMins)
        get_entvar(Data[m_Index], var_maxs, vMaxs)
        
        if (g_PlayerData[id][m_iPropVector] == 0 && vMins[g_PlayerData[id][m_iPropAxis]] >= -0.1) {
            return ITEM_DISABLED
        }
        
        if (g_PlayerData[id][m_iPropVector] == 1 && vMaxs[g_PlayerData[id][m_iPropAxis]] <= 0.1) {
            return ITEM_DISABLED
        }
    }

    return ITEM_IGNORE
}

public Handle_MenuProperties(id, menu, item)
{
    if (item == MENU_EXIT)
    {
        if (g_PlayerData[id][m_iFwdPlayerPostThink] != INVALID_HOOKCHAIN)
        {
            DisableHookChain(g_PlayerData[id][m_iFwdPlayerPostThink])
            g_PlayerData[id][m_iFwdPlayerPostThink] = INVALID_HOOKCHAIN
        }
        
        menu_destroy(menu)
        return PLUGIN_CONTINUE
    }
    
    static access, info[8], name[128], callback
    menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
    
    switch (str_to_num(info))
    {
        case 1: {
            g_PlayerData[id][m_iPropVector] = g_PlayerData[id][m_iPropVector] ? 0 : 1
        }
        case 2: {
            g_PlayerData[id][m_iPropAxis] = (g_PlayerData[id][m_iPropAxis] == 2) ? 0 : g_PlayerData[id][m_iPropAxis] + 1
        }
        case 3: {
            g_PlayerData[id][m_iPropSign] = g_PlayerData[id][m_iPropSign] ? 0 : 1
        }
        case 4: {
            g_PlayerData[id][m_iPropAmount] = (g_PlayerData[id][m_iPropAmount] == sizeof(PropAmounts) - 1) ? 0 : g_PlayerData[id][m_iPropAmount] + 1
        }
        case 5:
        {
            static Data[PointData], Float:vMins[3], Float:vMaxs[3], Float:vSize[3]
            
            ArrayGetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
            
            get_entvar(Data[m_Index], var_mins, vMins)
            get_entvar(Data[m_Index], var_maxs, vMaxs)
            
            if (g_PlayerData[id][m_iPropVector])
            {
                vMaxs[g_PlayerData[id][m_iPropAxis]] -= g_PlayerData[id][m_iPropSign] ? PropAmounts[g_PlayerData[id][m_iPropAmount]] : -PropAmounts[g_PlayerData[id][m_iPropAmount]]
                
                if (vMaxs[g_PlayerData[id][m_iPropAxis]] < 0.1) {
                    vMaxs[g_PlayerData[id][m_iPropAxis]] = 0.1
                }
            }
            else
            {
                vMins[g_PlayerData[id][m_iPropAxis]] += g_PlayerData[id][m_iPropSign] ? PropAmounts[g_PlayerData[id][m_iPropAmount]] : -PropAmounts[g_PlayerData[id][m_iPropAmount]]
                
                if (vMins[g_PlayerData[id][m_iPropAxis]] > -0.1) {
                    vMins[g_PlayerData[id][m_iPropAxis]] = -0.1
                }
            }
            
            vSize[0] = vMaxs[0] - vMins[0]
            vSize[1] = vMaxs[1] - vMins[1]
            vSize[2] = vMaxs[2] - vMins[2]
            
            set_entvar(Data[m_Index], var_mins, vMins)    
            set_entvar(Data[m_Index], var_maxs, vMaxs)
            set_entvar(Data[m_Index], var_size, vSize)
            
            client_print(id, print_center, "mins: %.1f %.1f %.1f maxs: %.1f %.1f %.1f", vMins[0], vMins[1], vMins[2], vMaxs[0], vMaxs[1], vMaxs[2])
            
            ArraySetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
        }
        case 7:
        {
            static Data[PointData]
            ArrayGetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
            
            Data[m_iPointType] = (Data[m_iPointType] == 2) ? 0 : Data[m_iPointType] + 1
            
            if (task_exists(Data[m_Index])) {
                remove_task(Data[m_Index])
            }
            
            if (Data[m_iPointType] == 2)
            {
                set_entvar(Data[m_Index], var_classname, "func_wall")
                set_entvar(Data[m_Index], var_solid, SOLID_BBOX)
                set_entvar(Data[m_Index], var_groupinfo, GROUP_FINISHED)
            }
            else
            {
                set_entvar(Data[m_Index], var_classname, ENT_CLASSNAME)
                set_entvar(Data[m_Index], var_groupinfo, GROUP_STARTED)               
                set_entvar(Data[m_Index], var_iuser1, Data[m_iPointType])
                
                // "Trigger in clipping list" fix
                set_entvar(Data[m_Index], var_solid, SOLID_NOT)
                set_task(0.1, "TaskSetSolidTrigger", Data[m_Index])
            }
            
            ArraySetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
        }
        case 0:
        {
            menu_destroy(menu)
            Show_MenuPoints(id)
            return PLUGIN_CONTINUE
        }
    }
    
    menu_destroy(menu)
    Show_MenuProperties(id)
    
    return PLUGIN_CONTINUE
}

public TaskSetSolidTrigger(id)
{
    if (get_entvar(id, var_solid) == SOLID_NOT) {
        set_entvar(id, var_solid, SOLID_TRIGGER)
    }
}

public CBasePlayer_PostThink_Post(id)
{
    static Float:players[32]
    
    if (get_gametime() - players[id] <= 0.1) {
        return HC_CONTINUE
    }
    
    players[id] = get_gametime()
    
    static Data[PointData], Float:vOrigin[3], Float:vMins[3], Float:vMaxs[3]
    ArrayGetArray(ArrayPoints, g_PlayerData[id][m_iActiveIndex], Data)
    
    get_entvar(Data[m_Index], var_origin, vOrigin)
    get_entvar(Data[m_Index], var_mins, vMins)
    get_entvar(Data[m_Index], var_maxs, vMaxs)
    
    engfunc(EngFunc_MessageBegin, MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, vOrigin, id)
    write_byte(TE_BOX)
    engfunc(EngFunc_WriteCoord, vOrigin[0] + vMins[0])
    engfunc(EngFunc_WriteCoord, vOrigin[1] + vMins[1])
    engfunc(EngFunc_WriteCoord, vOrigin[2] + vMins[2])
    engfunc(EngFunc_WriteCoord, vOrigin[0] + vMaxs[0])
    engfunc(EngFunc_WriteCoord, vOrigin[1] + vMaxs[1])
    engfunc(EngFunc_WriteCoord, vOrigin[2] + vMaxs[2])
    write_short(1)
    write_byte(255)
    write_byte(0)
    write_byte(0)
    message_end()
    
    return HC_CONTINUE
}

public FwdPointTouch_Post(touched, toucher)
{
    static szClassname[16]
    get_entvar(touched, var_classname, szClassname, charsmax(szClassname))
    
    if (!equali(szClassname, ENT_CLASSNAME)) {
        return FMRES_IGNORED
    }
    
    get_entvar(toucher, var_classname, szClassname, charsmax(szClassname))
    
    if (!equali(szClassname, "player")) {
        return FMRES_SUPERCEDE
    }
    
    if (get_entvar(touched, var_iuser1) == 1)
    {
        set_entvar(toucher, var_groupinfo, get_entvar(toucher, var_groupinfo) & ~GROUP_STARTED | GROUP_FINISHED)
        
        client_print_color(0, print_team_default, "%L%L", LANG_PLAYER, g_langPrefix, LANG_PLAYER, "DR_POINTS_FINISHED", g_PlayerData[toucher][m_szName])
    }
    
    return FMRES_IGNORED
}

public CBasePlayer_Spawn_Post(id)
{
    if (is_user_alive(id))
    {
        if (get_member(id, m_iTeam) == TEAM_TERRORIST) {
            set_entvar(id, var_groupinfo, get_entvar(id, var_groupinfo) & ~GROUP_STARTED | GROUP_FINISHED | GROUP_INGAME)
        } else if (get_member(id, m_iTeam) == TEAM_CT) {
            set_entvar(id, var_groupinfo, get_entvar(id, var_groupinfo) & ~GROUP_FINISHED | GROUP_STARTED | GROUP_INGAME)
        }
    }
}

public FwdSpawn_Post(iEnt)
{
    static szTargetname[16]
    get_entvar(iEnt, var_targetname, szTargetname, charsmax(szTargetname))
    
    if (!equali(szTargetname, "groupinfo4")) {
        return FMRES_IGNORED
    }
    
    set_entvar(iEnt, var_groupinfo, GROUP_FINISHED)
    
    return FMRES_IGNORED
}

public CBasePlayer_SetClientUserInfoName_Post(const id, infobuffer[], szNewName[])
{
    formatex(g_PlayerData[id][m_szName], charsmax(g_PlayerData[][m_szName]), szNewName)
}