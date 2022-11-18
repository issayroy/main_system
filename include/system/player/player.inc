

enum PlayerStruct {
	eID,
	eName[MAX_PLAYER_NAME + 1],
	ePassword[16 + 1],
	eEmail[64 + 1],
	eSkin,
	eLevel,
	eSex,
	eAge,
	eAdmin,
	eMoney,
	eAdminPass[32],
	eFraction,
	eRang
}

new ePlayer[MAX_PLAYERS][PlayerStruct];

#define SetPlayerData(%0,%1,%2)		ePlayer[%0][%1] = %2
#define GetPlayerData(%0,%1)			ePlayer[%0][%1]
#define AddPlayerData(%0,%1,%2,%3) 	ePlayer[%0][%1] %2= %3

new const Float:spawn_data[][4] = 
{
	{2501.6062,-2126.6069,23.4458,89.6142}
};

stock InitializeFractionPlayer(playerid)
{
	new frac = ePlayer[playerid][eFraction],
		skin = ePlayer[playerid][eRang];
	if(ePlayer[playerid][eFraction] != 0)
	{
		if(frac == 1) Iter_Add(org_pravo, playerid);
		SetPlayerSkin(playerid, org_data[frac][eOrgSkin][skin - 1]);
		SetPlayerColor(playerid, frac_color[frac]);
	}
	return 1;	
}
