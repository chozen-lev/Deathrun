#include <amxmodx>
#include <reapi>
#include <engine>
#include <fakemeta>
#include <hamsandwich>

#define VERSION "1.0.0-dev+"

#define ACCESS_FLAG ADMIN_RCON

enum _:ButtonData
{
    m_Index,
    m_szModelName[256],
    m_iType
}

enum _:PLAYER_DATA
{
    m_iActiveIndex,
    bool:m_bActivated
}

new g_langTypes[][] =
{
    "DR_BTNDB_TYPE_ALL",
    "DR_BTNDB_TYPE_T",
    "DR_BTNDB_TYPE_CT"
}

new Array:ArrayButtons, Trie:ArrayButtonsIndexes
new g_PlayerData[32][PLAYER_DATA]

new g_iButtonsCallback

new g_szFilePath[256]

public plugin_natives()
{
    ArrayButtons = ArrayCreate(ButtonData)
    ArrayButtonsIndexes = TrieCreate()
}

public plugin_precache()
{
    register_forward(FM_Spawn, "FwdSpawn_Post", true)
}

public plugin_init()
{
    register_plugin("[ReAPI]Deathrun BTNDB", VERSION, "ChoZen")
    
    // clcmds
    register_clcmd("buttons_menu", "Show_MenuButtons")
    register_clcmd("radio1", "Show_MenuButtons")
    
    // hooks
    RegisterHam(Ham_Use, "func_button", "FwdButtonUse")
    
    // menu callbacks
    g_iButtonsCallback = menu_makecallback("CallBack_MenuButtons")
    
    // dictionary
    register_dictionary("DeathrunBTNDB.txt")
}

public plugin_cfg()
{
    get_localinfo("amxx_datadir", g_szFilePath, charsmax(g_szFilePath))
    formatex(g_szFilePath, charsmax(g_szFilePath),"%s/buttons", g_szFilePath)
    
    if (dir_exists(g_szFilePath) == 0) {
        mkdir(g_szFilePath)
    }
    
    static szMapName[32], pFile
    get_mapname(szMapName, charsmax(szMapName))
    formatex(g_szFilePath, charsmax(g_szFilePath),"%s/%s.dat", g_szFilePath, szMapName)
    
    if ((pFile = fopen(g_szFilePath, "rb")) == 0)
    {
        log_amx("Map %s has any saved buttons", szMapName)
        return PLUGIN_CONTINUE
    }
    
    static Data[ButtonData]
    
    while (fread_blocks(pFile, Data[m_szModelName], sizeof Data - m_szModelName, BLOCK_INT))
    {
        Data[m_Index] = find_ent_by_model(-1, "func_button", Data[m_szModelName])
        if (Data[m_Index]) {
            set_entvar(Data[m_Index], var_iuser1, Data[m_iType])
        }
        
        TrieSetCell(ArrayButtonsIndexes, Data[m_szModelName], ArrayPushArray(ArrayButtons, Data))
    }
    
    fclose(pFile)
    
    if (ArraySize(ArrayButtons) == 0) {
        server_print("[BTNDB] Any buttons have been loaded")
    } else if (ArraySize(ArrayButtons) == 1) {
        server_print("[BTNDB] 1 button has been loaded")
    } else {
        server_print("[BTNDB] %d buttons have been loaded", ArraySize(ArrayButtons))
    }
    
    return PLUGIN_CONTINUE
}

public Show_MenuButtons(id, bool:activated)
{
    if(!(get_user_flags(id) & ACCESS_FLAG)) {
        return PLUGIN_CONTINUE
    }
    
    g_PlayerData[id][m_bActivated] = activated
    
    if (g_PlayerData[id][m_iActiveIndex] == -1 && ArraySize(ArrayButtons)) {
        g_PlayerData[id][m_iActiveIndex] = 0
    } else if (g_PlayerData[id][m_iActiveIndex] >= ArraySize(ArrayButtons)) {
        g_PlayerData[id][m_iActiveIndex] = ArraySize(ArrayButtons) - 1
    }
    
    static iMenu, szText[256], Data[ButtonData]
    
    if (g_PlayerData[id][m_iActiveIndex] >= 0) {
        ArrayGetArray(ArrayButtons, g_PlayerData[id][m_iActiveIndex], Data)
    }
    
    formatex(szText, charsmax(szText), "%L^n\w%L", LANG_PLAYER, "DR_BTNDB_TITLE", LANG_PLAYER, "DR_BTNDB_NUMBER", ArraySize(ArrayButtons))
    
    if (g_PlayerData[id][m_iActiveIndex] > -1) {
        formatex(szText, charsmax(szText), "%s \d| \w%L", szText, LANG_PLAYER, "DR_BTNDB_ACTIVE", g_PlayerData[id][m_iActiveIndex] + 1)
    }
    
    iMenu = menu_create(szText, "Handle_MenuButtons")
    menu_setprop(iMenu, MPROP_PERPAGE, 0)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, g_PlayerData[id][m_bActivated] ? "DR_BTNDB_WAIT" : "DR_BTNDB_ADD")
    menu_additem(iMenu, szText, "1", ACCESS_FLAG, g_iButtonsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_BTNDB_REMOVE")
    menu_additem(iMenu, szText, "2", ACCESS_FLAG, g_iButtonsCallback)
    
    formatex(szText, charsmax(szText), "%L: \y%L", LANG_PLAYER, "DR_BTNDB_TYPE", LANG_PLAYER, g_PlayerData[id][m_iActiveIndex] >= 0 ? g_langTypes[get_entvar(Data[m_Index], var_iuser1)] : "DR_BTNDB_TYPE_UNDEFINED")
    menu_additem(iMenu, szText, "3", ACCESS_FLAG, g_iButtonsCallback)
    
    menu_addblank2(iMenu)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_BTNDB_SAVE")
    menu_additem(iMenu, szText, "5", ACCESS_FLAG, g_iButtonsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_BTNDB_LOAD")
    menu_additem(iMenu, szText, "6", ACCESS_FLAG, g_iButtonsCallback)

    menu_addblank2(iMenu)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_BTNDB_PREVIOUS")
    menu_additem(iMenu, szText, "8", ACCESS_FLAG, g_iButtonsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "DR_BTNDB_NEXT")
    menu_additem(iMenu, szText, "9", ACCESS_FLAG, g_iButtonsCallback)
    
    formatex(szText, charsmax(szText), "%L", LANG_PLAYER, "PLAYER_EXIT")
    menu_additem(iMenu, szText, "0")
    
    menu_display(id, iMenu)
    
    return PLUGIN_HANDLED
}

public CallBack_MenuButtons(id, menu, item)
{
    static access, info[8], name[128], callback, iKey
    menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
    
    iKey = str_to_num(info)
    
    if (iKey == 1 && g_PlayerData[id][m_bActivated]) {
        return ITEM_DISABLED
    }
    
    if ((iKey == 2 || iKey == 3) && g_PlayerData[id][m_iActiveIndex] == -1) {
        return ITEM_DISABLED
    }
    
    if ((iKey == 8 || iKey == 9) && ArraySize(ArrayButtons) <= 1) {
        return ITEM_DISABLED
    }
    
    if (iKey == 8 && g_PlayerData[id][m_iActiveIndex] <= 0) {
        return ITEM_DISABLED
    }
    
    if (iKey == 9 && g_PlayerData[id][m_iActiveIndex] >= ArraySize(ArrayButtons) - 1) {
        return ITEM_DISABLED
    }
    
    return ITEM_IGNORE
}

public Handle_MenuButtons(id, menu, item)
{
    if (item == MENU_EXIT)
    {
        menu_destroy(menu)
        g_PlayerData[id][m_bActivated] = false
        return PLUGIN_CONTINUE
    }
    
    static access, info[8], name[128], callback
    menu_item_getinfo(menu, item, access, info, charsmax(info), name, charsmax(name), callback)
    
    switch (str_to_num(info))
    {
        case 1:
        {
            g_PlayerData[id][m_bActivated] = true
        }
        case 2:
        {
            static Data[ButtonData]
            ArrayGetArray(ArrayButtons, g_PlayerData[id][m_iActiveIndex], Data)
            TrieDeleteKey(ArrayButtonsIndexes, Data[m_szModelName])
            ArrayDeleteItem(ArrayButtons, g_PlayerData[id][m_iActiveIndex])
        }
        case 3:
        {
            static Data[ButtonData]
            ArrayGetArray(ArrayButtons, g_PlayerData[id][m_iActiveIndex], Data)
            
            Data[m_iType] = get_entvar(Data[m_Index], var_iuser1)
            set_entvar(Data[m_Index], var_iuser1, (Data[m_iType] == sizeof(g_langTypes) - 1) ? 0 : Data[m_iType] + 1)
            
            ArraySetArray(ArrayButtons, g_PlayerData[id][m_iActiveIndex], Data)
        }
        case 5:
        {
            static pFile
            
            if (ArraySize(ArrayButtons) == 0) {
                delete_file(g_szFilePath)
            } else if ((pFile = fopen(g_szFilePath, "wb")) == 0) {
                log_amx("Read file error(%s)", g_szFilePath)
            }
            else
            {
                static Data[ButtonData]
                
                for (new i = 0; i < ArraySize(ArrayButtons); i++)
                {
                    ArrayGetArray(ArrayButtons, i, Data)
                    
                    get_entvar(Data[m_Index], var_iuser1, Data[m_iType])
                    
                    fwrite_blocks(pFile, Data[m_szModelName], sizeof Data - m_szModelName, BLOCK_INT)
                }
                
                fclose(pFile)
            }
        }
        case 6:
        {
            
        }
        case 8: {
            g_PlayerData[id][m_iActiveIndex]--
        }
        case 9: {
            g_PlayerData[id][m_iActiveIndex]++
        }
        default:
        {
            menu_destroy(menu)
            g_PlayerData[id][m_bActivated] = false
            return PLUGIN_CONTINUE
        }
    }
    
    menu_destroy(menu)
    Show_MenuButtons(id, g_PlayerData[id][m_bActivated])
    
    return PLUGIN_CONTINUE
}

public FwdButtonUse(id, idcaller, idactivator, use_type, Float:value)
{
    if (g_PlayerData[idactivator][m_bActivated])
    {
        static Data[ButtonData]
        
        Data[m_Index] = id
        get_entvar(id, var_model, Data[m_szModelName], charsmax(Data[m_szModelName]))
        
        g_PlayerData[idactivator][m_bActivated] = false
        
        if (TrieKeyExists(ArrayButtonsIndexes, Data[m_szModelName])) {
            TrieGetCell(ArrayButtonsIndexes, Data[m_szModelName], g_PlayerData[idactivator][m_iActiveIndex])
        }
        else
        {
            Data[m_iType] = 0
            g_PlayerData[idactivator][m_iActiveIndex] = ArrayPushArray(ArrayButtons, Data)
            TrieSetCell(ArrayButtonsIndexes, Data[m_szModelName], g_PlayerData[idactivator][m_iActiveIndex])
        }
        
        Show_MenuButtons(idactivator, false)
        return HAM_SUPERCEDE
    }
    
    return HAM_IGNORED
}

public FwdSpawn_Post(iEnt)
{
    static szModelName[256], arrIndex, Data[ButtonData]
    get_entvar(iEnt, var_model, szModelName, charsmax(szModelName))
    
    if (TrieGetCell(ArrayButtonsIndexes, szModelName, arrIndex))
    {
        ArrayGetArray(ArrayButtons, arrIndex, Data)
        set_entvar(iEnt, var_iuser1, Data[m_iType])
        
        return FMRES_IGNORED
    }
    
    return FMRES_IGNORED
}