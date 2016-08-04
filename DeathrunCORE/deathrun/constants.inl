// CONSTANS
///////////////////////////////////////////

#define VERSION "1.0.0-dev+"

#define IsPlayer(%1) ( 1 <= %1 <= g_iMaxPlayers )

// Messages prefix
new const g_langPrefix[ ] = "DR_CORE_PREFIX"

enum RandomizeEvents
{
	m_None,
	m_Reset,
	m_Dropped
}

enum _:FORWARDS
{
	DRMCoreFunc:m_Function,
	m_arrIndex,
	Array:m_Array
}

enum _:FORWARD_DATA
{
	m_fwdIndex,
	m_Post,
	bool:m_State
}

enum DRMCoreFunc
{
	DRM_Randomize
}

enum _:PLAYER_DATA
{
	m_szName[ 32 ],
	m_szSteamID[ 32 ]
}

new bool:g_bFirstRound = true, bool:g_RoundStarted = true

new g_iCurrTer, g_PlayerData[ 33 ][ PLAYER_DATA ]

new Trie:TrieReconnected, Array:ArrayForwardRandomize, Array:ArrayForwards

new g_iArg[ 2 ], bool:g_bArgState[ 2 ]

new g_iMaxPlayers