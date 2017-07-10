// CONSTANS
///////////////////////////////////////////

#define VERSION "1.0.0-dev+"

enum ItemIndexes:iItems
{
    m_None,
    m_HealthArmor,
    m_Speed,
    m_Gravity,
    m_Scout,
    m_Bhop
}

enum _:ItemData
{
    ItemIndexes:m_Index,
    m_langName[32],
    m_Cost
}

enum _:PLAYER_DATA
{
    m_Account,
    Float:m_Discount
}

enum _:CVARS
{
    m_StartMoney,
    m_MaxMoney
}

enum ShopFunc
{
    Shop_ShowMenu,
    Shop_MenuAddItem,
    Shop_BuyItem
}

enum _:FORWARDS
{
    ShopFunc:m_Function,
    m_arrIndex,
    Array:m_Array
}

enum _:FORWARD_DATA
{
    m_fwdIndex,
    m_Post,
    bool:m_State
}

new g_iItemsMax = _:iItems

new Array:ArrayItems, Array:ArrayItemIndexes

new g_PlayerData[33][PLAYER_DATA]

new g_CVAR[CVARS]

new g_iCallback

new g_iMsgStatusValue

new Array:ArrayForwardShowMenu, Array:ArrayForwardMenuAddItem, Array:ArrayForwardBuyItem, Array:ArrayForwards

new g_iArg[3], bool:g_bArgState[3]