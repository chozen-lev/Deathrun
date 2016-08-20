// CVARS
///////////////////////////////////////////

public plugin_cfg( )
{
	DR_Init( )
}

public OnConfigsExecuted( )
{
	// Mode Menu creating
	new Data[ ModeData ], szText[ 48 ], info[ 4 ]
	
	formatex( szText, charsmax( szText ), "%L\R", LANG_PLAYER, "DR_MODE_TITLE" )
	g_iModeMenu = menu_create( szText, "Handle_MenuModes" )
	
	for( new i; i < ArraySize( ArrayModes ); i++ )
	{
		ArrayGetArray( ArrayModes, i, Data )
		
		if( !Data[ m_Hide ] )
		{
			formatex( szText, charsmax( szText ), "%L", LANG_PLAYER, Data[ m_langName ] )
			num_to_str( _:Data[ m_Index ], info, charsmax( info ) )
			
			menu_additem( g_iModeMenu, szText, info )
		}
	}
	
	formatex( szText, charsmax( szText ), "%L", LANG_PLAYER, "PLAYER_NEXT" )
	menu_setprop( g_iModeMenu, MPROP_NEXTNAME, szText )
	
	formatex( szText, charsmax( szText ), "%L", LANG_PLAYER, "PLAYER_BACK" )
	menu_setprop( g_iModeMenu, MPROP_BACKNAME, szText )
	
	formatex( szText, charsmax( szText ), "%L", LANG_PLAYER, "DR_MODE_RANDOMLY" )
	menu_setprop( g_iModeMenu, MPROP_EXITNAME, szText )
}