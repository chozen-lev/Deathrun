// DEATHRUN
///////////////////////////////////////////

public DR_Init( )
{
	ArrayAvailModes = ArrayCreate( )
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

public native_set_arg( CPlugin, CParams )
{
	if( AType:get_param( 2 ) == ATYPE_INTEGER )
	{
		static number; number = get_param( 1 ) - 1
		
		g_iArg[ number ] = get_param( 3 )
		g_bArgState[ number ] = true
	}
}

public native_register_forward( CPlugin, CParams )
{
	static Data[ FORWARDS ], ForwardData[ FORWARD_DATA ], szHandler[ 128 ]
	
	get_string( 2, szHandler, charsmax( szHandler ) )
	ForwardData[ m_Post ] = get_param( 3 )
	ForwardData[ m_State ] = true
	
	switch( get_param( 1 ) )
	{
		case DRM_ShowMenu_ChooseMode:
		{
			ForwardData[ m_fwdIndex ] = CreateOneForward( CPlugin, szHandler, FP_CELL, FP_CELL, FP_CELL )
			
			Data[ m_Function ] = DRM_ShowMenu_ChooseMode
			Data[ m_Array ] = ArrayForwardShowMenu
		}
		case DRM_ChangeMode:
		{
			ForwardData[ m_fwdIndex ] = CreateOneForward( CPlugin, szHandler, FP_CELL, FP_CELL )
			
			Data[ m_Function ] = DRM_ChangeMode
			Data[ m_Array ] = ArrayForwardChangeMode
		}
	}
	
	Data[ m_arrIndex ] = ArrayPushArray( Data[ m_Array ], ForwardData )
	
	return ArrayPushArray( ArrayForwards, Data )
}

public bool:native_enable_forward( CPlugin, CParams )
{
	static Data[ FORWARDS ], ForwardData[ FORWARD_DATA ]
	
	if( !ArrayGetArray( ArrayForwards, get_param( 1 ), Data ) )
		return false
	
	ArrayGetArray( Data[ m_Array ], Data[ m_arrIndex ], ForwardData )
	
	ForwardData[ m_State ] = true
	
	ArraySetArray( Data[ m_Array ], Data[ m_arrIndex ], ForwardData )
	
	return true
}

public bool:native_disable_forward( CPlugin, CParams )
{
	static Data[ FORWARDS ], ForwardData[ FORWARD_DATA ]
	
	if( !ArrayGetArray( ArrayForwards, get_param( 1 ), Data ) )
		return false
	
	ArrayGetArray( Data[ m_Array ], Data[ m_arrIndex ], ForwardData )
	
	ForwardData[ m_State ] = false
	
	ArraySetArray( Data[ m_Array ], Data[ m_arrIndex ], ForwardData )
	
	return true
}

public Task_CloseMenu( tid )
{
	show_menu( tid - TASK_AUTOCHOOSE, 0, "^n" )
}

public plugin_end( )
{
	new Data[ FORWARDS ], ForwardData[ FORWARD_DATA ]
	
	for( new i; i < ArraySize( ArrayForwards ); i++ )
	{
		ArrayGetArray( ArrayForwards, i, Data )
		
		ArrayGetArray( Data[ m_Array ], Data[ m_arrIndex ], ForwardData )
		
		DestroyForward( ForwardData[ m_fwdIndex ] )
	}
	
	ArrayDestroy( ArrayAvailModes )
	ArrayDestroy( ArrayModes )
	ArrayDestroy( ArrayModeIndexes )
	ArrayDestroy( ArrayForwardShowMenu )
	ArrayDestroy( ArrayForwardChangeMode )
	ArrayDestroy( ArrayForwards )
}