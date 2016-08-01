// RANDOMIZE
///////////////////////////////////////////

RandomPlayer( iPlayers[ ], iNum )
{
	static iRandomPlayer
	
	do
	{
		iRandomPlayer = iPlayers[ random_num( 0, iNum - 1 ) ]
	}
	while( iRandomPlayer == g_iCurrTer )
	
	return iRandomPlayer
}

ReplaceTerrorist( id )
{
	static RandomizeEvents:event, ret, hc_state, i, Data[ FORWARD_DATA ], iNextTer
	
	hc_state = HC_CONTINUE
	
	for( i = 0; i < ArraySize( ArrayForwardRandomize ); i++ )
	{
		ArrayGetArray( ArrayForwardRandomize, i, Data )
		
		if( Data[ m_Post ] == 0 && Data[ m_State ] )
		{
			ExecuteForward( Data[ m_fwdIndex ], ret, id, m_Dropped )
			
			if( ret == HC_BREAK )
				return
			
			if( ret > hc_state )
				hc_state = ret
		}
	}
	
	if( hc_state != HC_SUPERCEDE )
	{
		if( !g_bArgState[ 0 ] )
			iNextTer = id
		else
		{
			iNextTer = g_iArg[ 0 ]
			g_bArgState[ 0 ] = false
		}
		
		if( !g_bArgState[ 1 ] )
			event = m_Dropped
		else
		{
			event = RandomizeEvents:g_iArg[ 1 ]
			g_bArgState[ 1 ] = false
		}
		
		if( IsPlayer( g_iCurrTer ) )
		{
			client_print_color( 0, print_team_red, "%L%L", LANG_PLAYER, g_langPrefix, LANG_PLAYER, "DR_TER_DISCONNECTED", g_PlayerData[ g_iCurrTer ][ m_szName ], g_PlayerData[ iNextTer ][ m_szName ] )
			
			rg_set_user_team( iNextTer, TEAM_TERRORIST )
			
			static Float:vOrigin[ 3 ], Float:vVelocity[ 3 ], Float:vAngles[ 3 ]
			
			get_entvar( g_iCurrTer, var_origin, vOrigin )
			set_entvar( iNextTer, var_origin, vOrigin )
			
			// In case he was flying over the precipice or something else
			get_entvar( g_iCurrTer, var_velocity, vVelocity )
			set_entvar( iNextTer, var_velocity, vVelocity )
			
			get_entvar( g_iCurrTer, var_angles, vAngles )
			set_entvar( iNextTer, var_angles, vAngles )
			set_entvar( iNextTer, var_fixangle, 1 )
			
			// In case he was sitting in a low space
			if( get_entvar( g_iCurrTer, var_bInDuck ) )
				set_entvar( iNextTer, var_bInDuck, true )
		}
		
		g_iCurrTer = iNextTer
	}
	
	for( i = 0; i < ArraySize( ArrayForwardRandomize ); i++ )
	{
		ArrayGetArray( ArrayForwardRandomize, i, Data )
		
		if( Data[ m_Post ] && Data[ m_State ] )
		{
			ExecuteForward( Data[ m_fwdIndex ], ret, g_iCurrTer, event )
			
			if( ret == HC_BREAK )
				return
		}
	}
}

public native_set_ter( CPlugin, CParams )
{
	static i, Data[ FORWARD_DATA ], ret, hc_state, RandomizeEvents:event, iNextTer
	
	hc_state = HC_CONTINUE
	iNextTer = get_param( 1 )
	
	for( i = 0; i < ArraySize( ArrayForwardRandomize ); i++ )
	{
		ArrayGetArray( ArrayForwardRandomize, i, Data )
		
		if( Data[ m_Post ] == 0 && Data[ m_State ] )
		{
			ExecuteForward( Data[ m_fwdIndex ], ret, iNextTer, m_None )
			
			if( ret == HC_BREAK )
				return
			
			if( ret > hc_state )
				hc_state = ret
		}
	}
	
	if( hc_state != HC_SUPERCEDE )
	{
		if( !g_bArgState[ 0 ] )
			g_iCurrTer = iNextTer
		else
		{
			g_iCurrTer = g_iArg[ 0 ]
			g_bArgState[ 0 ] = false
		}
		
		if( !g_bArgState[ 1 ] )
			event = m_None
		else
		{
			event = RandomizeEvents:g_iArg[ 1 ]
			g_bArgState[ 1 ] = false
		}
		
		if( IsPlayer( g_iCurrTer ) ) rg_set_user_team( g_iCurrTer, TEAM_TERRORIST )
	}
	
	for( i = 0; i < ArraySize( ArrayForwardRandomize ); i++ )
	{
		ArrayGetArray( ArrayForwardRandomize, i, Data )
		
		if( Data[ m_Post ] && Data[ m_State ] )
		{
			ExecuteForward( Data[ m_fwdIndex ], ret, g_iCurrTer, event )
			
			if( ret == HC_BREAK )
				return
		}
	}
}