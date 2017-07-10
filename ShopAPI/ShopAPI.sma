#include <amxmodx>
#include <reapi>
#include <fakemeta>

#include "includes/constants.inl"
#include "includes/cvar.inl"

#include "includes/hooks.inl"
#include "includes/shop.inl"

public plugin_init()
{
    register_plugin("[ReAPI]Shop ITEMS", VERSION, "ChoZen")

    // hooks
    RegisterHookChain(RG_CBasePlayer_AddAccount, "CBasePlayer_AddAccount")
    RegisterHookChain(RG_CBasePlayer_AddAccount, "CBasePlayer_AddAccount_Post", true)

    // clcmds
    register_clcmd("shop_menu", "Show_MenuShop")
    register_clcmd("radio2", "Show_MenuShop")
    register_clcmd("radio1", "a")

    g_iMsgStatusValue = get_user_msgid("StatusValue")

    g_iCallback = menu_makecallback("CallBack_MenuShop")

    // dictionary
    register_dictionary("ShopAPI.txt")
}

public a(id)
{
    rg_add_account(id, 5000)
}

public plugin_natives()
{
    ArrayItems = ArrayCreate(ItemData)
    ArrayItemIndexes = ArrayCreate()
    
    ArrayForwardShowMenu = ArrayCreate(FORWARD_DATA)
    ArrayForwardMenuAddItem = ArrayCreate(FORWARD_DATA)
    ArrayForwardBuyItem = ArrayCreate(FORWARD_DATA)
    ArrayForwards = ArrayCreate(FORWARDS)
    
    register_library("ShopAPI")
    register_native("RegisterItem", "native_RegisterItem")
    register_native("RegisterShopForward", "native_register_forward")
    register_native("ShopSetArg", "native_set_arg")
    register_native("ShopEnableForward", "native_enable_forward")
    register_native("ShopDisableForward", "native_disable_forward")
}