// SHOP ITEMS INCLUDE
///////////////////////////////////////////

#if defined _ShopAPI_included
    #endinput
#endif
#define _ShopAPI_included

enum ItemIndexes:iItems
{
    m_None,
    m_HealthArmor,
    m_Speed,
    m_Gravity,
    m_Scout,
    m_Bhop
};

enum ShopFunc
{
    /*
    * Description:      -
    * Params:       (index, menu)
    */
    Shop_ShowMenu,
    
    /*
    * Description:      -
    * Params:       (index, menu, item, ItemIndexes:iItem)
    */
    Shop_MenuAddItem,
    
    /*
    * Description:      -
    * Params:       (index, menu, item)
    */
    Shop_BuyItem
};

native HookChain:RegisterShopForward(ShopFunc:function_id, callback[], post = 0);

native bool:ShopEnableForward(HookChain:hook);
native bool:ShopDisableForward(HookChain:hook);

native ShopSetArg(number, AType:type, any:...);

native ItemIndexes:RegisterItem(ItemIndexes:item = ItemIndexes:m_None, langName[], cost = 0);