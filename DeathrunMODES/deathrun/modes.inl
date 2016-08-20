// MODES
///////////////////////////////////////////

public ModeIndexes:native_DRM_RegisterMode( CPlugin, CParams )
{
	static Data[ ModeData ]
	
	Data[ m_Index ] = ModeIndexes:get_param( 1 )
	
	if( Data[ m_Index ] == ModeIndexes:m_None )
	{
		Data[ m_Index ] = ModeIndexes:g_iModesMax
		g_iModesMax++
	}
	
	get_string( 2, Data[ m_langName ], charsmax( Data[ m_langName ] ) )
	
	Data[ m_Hide ] = bool:get_param( 3 )
	
	ArrayPushArray( ArrayModes, Data )
	ArrayPushCell( ArrayModeIndexes, Data[ m_Index ] )
	
	return Data[ m_Index ]
}

public Handle_MenuModes( id, menu, item )
{
	if( task_exists( id + TASK_AUTOCHOOSE ) )
	{
		static i, ret, hc_state, fwdData[ FORWARD_DATA ], Data[ ModeData ], ModeIndexes:iNextMode, ModeEvents:event
		
		if( item == MENU_EXIT || item == MENU_TIMEOUT )
		{
			ArrayGetArray( ArrayModes, random( ArraySize( ArrayAvailModes ) - 1 ), Data )
		}
		else
		{
			remove_task( id + TASK_AUTOCHOOSE )
			
			static access, info[ 4 ], name[ 32 ], callback
			menu_item_getinfo( menu, item, access, info, charsmax( info ), name, charsmax( name ), callback )
			
			ArrayGetArray( ArrayModes, ArrayFindValue( ArrayModeIndexes, str_to_num( info ) ), Data )
		}
		
		for( i = 0; i < ArraySize( ArrayForwardChangeMode ); i++ )
		{
			ArrayGetArray( ArrayForwardChangeMode, i, fwdData )
			
			if( fwdData[ m_Post ] == 0 && fwdData[ m_State ] )
			{
				ExecuteForward( fwdData[ m_fwdIndex ], ret, Data[ m_Index ], m_Choose )
				
				if( ret == HC_BREAK )
					return
				
				if( ret > hc_state )
					hc_state = ret
			}
		}
		
		if( hc_state != HC_SUPERCEDE )
		{
			if( !g_bArgState[ 0 ] )
				iNextMode = Data[ m_Index ]
			else
			{
				iNextMode = ModeIndexes:g_iArg[ 0 ]
				g_bArgState[ 0 ] = false
				
				ArrayGetArray( ArrayModes, ArrayFindValue( ArrayModeIndexes, iNextMode ), Data )
			}
			
			if( !g_bArgState[ 1 ] )
				event = m_Choose
			else
			{
				event = ModeEvents:g_iArg[ 1 ]
				g_bArgState[ 1 ] = false
			}
			
			if( Data[ m_Index ] != ModeIndexes:m_None )
			{
				if( item == MENU_EXIT || item == MENU_TIMEOUT )
					client_print_color( 0, print_team_default, "%L%L", LANG_PLAYER, g_langPrefix, LANG_PLAYER, "DR_MODE_RANDOM", LANG_PLAYER, Data[ m_langName ] )
				else
					client_print_color( 0, print_team_default, "%L%L", LANG_PLAYER, g_langPrefix, LANG_PLAYER, "DR_MODE_TERCC", LANG_PLAYER, Data[ m_langName ] )
			}
			
			g_CurrMode = Data
		}
		
		for( i = 0; i < ArraySize( ArrayForwardChangeMode ); i++ )
		{
			ArrayGetArray( ArrayForwardChangeMode, i, fwdData )
			
			if( fwdData[ m_Post ] && fwdData[ m_State ] )
			{
				ExecuteForward( fwdData[ m_fwdIndex ], ret, g_CurrMode[ m_Index ], event )
				
				if( ret == HC_BREAK )
					return
			}
		}
	}
}

public native_set_mode( CPlugin, CParams )
{
	static i, fwdData[ FORWARD_DATA ], ret, hc_state, RandomizeEvents:event, Data[ ModeData ]
	
	hc_state = HC_CONTINUE
	Data[ m_Index ] = ModeIndexes:get_param( 1 )
	
	for( i = 0; i < ArraySize( ArrayForwardChangeMode ); i++ )
	{
		ArrayGetArray( ArrayForwardChangeMode, i, fwdData )
		
		if( fwdData[ m_Post ] == 0 && fwdData[ m_State ] )
		{
			ExecuteForward( fwdData[ m_fwdIndex ], ret, Data[ m_Index ], m_None )
			
			if( ret == HC_BREAK )
				return
			
			if( ret > hc_state )
				hc_state = ret
		}
	}
	
	if( hc_state != HC_SUPERCEDE )
	{
		if( g_bArgState[ 0 ] )
		{
			Data[ m_Index ] = ModeIndexes:g_iArg[ 0 ]
			g_bArgState[ 0 ] = false
		}
		
		ArrayGetArray( ArrayModes, ArrayFindValue( ArrayModeIndexes, Data[ m_Index ] ), Data )
		
		if( !g_bArgState[ 1 ] )
			event = m_None
		else
		{
			event = RandomizeEvents:g_iArg[ 1 ]
			g_bArgState[ 1 ] = false
		}
		
		g_CurrMode = Data
	}
	
	for( i = 0; i < ArraySize( ArrayForwardChangeMode ); i++ )
	{
		ArrayGetArray( ArrayForwardChangeMode, i, fwdData )
		
		if( fwdData[ m_Post ] && fwdData[ m_State ] )
		{
			ExecuteForward( fwdData[ m_fwdIndex ], ret, g_CurrMode[ m_Index ], event )
			
			if( ret == HC_BREAK )
				return
		}
	}
}