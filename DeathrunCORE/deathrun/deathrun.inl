// DEATHRUN
///////////////////////////////////////////

public DR_Init( )
{
	g_iMaxPlayers = get_maxplayers( )
	
	TrieReconnected = TrieCreate( )
	
	if( !get_pcvar_bool( CVAR_BuyMenu ) )
	{
		set_member_game( m_bCTCantBuy, true )
		set_member_game( m_bTCantBuy, true )
	}
}

public Cmd_Block_Main( id ) return PLUGIN_HANDLED_MAIN

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
		case DRM_Randomize:
		{
			ForwardData[ m_fwdIndex ] = CreateOneForward( CPlugin, szHandler, FP_CELL, FP_CELL )
			
			Data[ m_Function ] = DRM_Randomize
			Data[ m_Array ] = ArrayForwardRandomize
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

public plugin_end( )
{
	new Data[ FORWARDS ], ForwardData[ FORWARD_DATA ]
	
	for( new i; i < ArraySize( ArrayForwards ); i++ )
	{
		ArrayGetArray( ArrayForwards, i, Data )
		
		ArrayGetArray( Data[ m_Array ], Data[ m_arrIndex ], ForwardData )
		
		DestroyForward( ForwardData[ m_fwdIndex ] )
	}
	
	ArrayDestroy( ArrayForwardRandomize )
	ArrayDestroy( ArrayForwards )
	
	TrieDestroy( TrieReconnected )
}