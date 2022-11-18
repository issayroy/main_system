#define DATABASE_HOST						"127.0.0.1"
#define DATABASE_USER 						"root"
#define DATABASE_PASSWORD 					""
#define DATABASE_DB 						"mode"

#define SERVER_NAME							"НАЗВАНИЕ"
#define PRICE_CREATE_BUSINESS				100000

#define IsTestServer


#define f format

#define MAX_RENT_DATA 						1
#define PRICE_RENT							5000


#define Message								SendClientMessage
#define showPlayerDialog					Dialog

#define GetPlayerListitemValue(%0,%1) 		g_player_listitem[%0][%1]
#define SetPlayerListitemValue(%0,%1,%2) 	g_player_listitem[%0][%1] = %2

#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))


new str_1[2048] = EOS;

new bool:admin_logged[MAX_PLAYERS] = false;


new MySQL:mysql;

new g_player_listitem[MAX_PLAYERS][32];

enum dialogs {
	dLogin = 1,
	dRentDialog = 2,
	dCreateBusiness = 3,
	dApplicationBusiness = 4,
	dApplicationBusinessWork = 5,
	dBizList = 6,
	dBizOperationSelect = 7,
	dBalanceOperationBiz = 8,
	dShowAllOperationBiz = 9,
	dAdminLogin = 10,
	dSetLeader = 11
}


new page_operation[MAX_PLAYERS] = 0;

#define public:%0(%1) \
			forward %0(%1); \
				public %0(%1)

new
const AdminTags[][36] =
{
	"Неизвестно",
	"Агент Поддержки",
	"Младший Администратор",
	"Администратор",
	"Старший Администратор",
	"Заместитель Главного Администратора",
	"Главный Администратор",
	"Администратор седьмого уровня"
};


stock formatInt(amount, delimiter[2] = ".")
{
	new int_string[15];
	format(int_string, sizeof(int_string), "%d", amount >= 0 ? amount : -amount);

	new value = strlen(int_string);
	switch(value)
	{
		case 4..6:
			strins(int_string, delimiter, value - 3, 1);
		case 7..9:
			strins(int_string, delimiter, value - 3, 1),
			strins(int_string, delimiter, value - 6, 1);
		case 10..12:
			strins(int_string, delimiter, value - 3, 1),
			strins(int_string, delimiter, value - 6, 1),
			strins(int_string, delimiter, value - 9, 1);
	}

	if(amount < 0) strins(int_string, "-", 0);

	new money[16];
	format(money, sizeof(money), "%s", int_string);
	return money;
}

stock MessageAdmins(text[], color)
{
	foreach(new playerid : Player)
	{
		if(GetPlayerData(playerid, eAdmin) < 1) continue;
		SendClientMessage(playerid, color, text);
	}
	return 1;
}

stock SendMessageInLocal(playerid, message[], color, chat_buble = 0)
{
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	foreach(new i: Player)
	{
		if(!IsPlayerConnected(i)) continue;
		if(!IsPlayerInRangeOfPoint(playerid, 5.0, pos[0], pos[1], pos[2])) continue;
		if(chat_buble != 0) SetPlayerChatBubble(i, message, 0xfeca57FF, 10.0, 1600);
		SendClientMessage(i, color, message);
	}
}

stock ShowAdminPass(playerid)
{
	Dialog(
		playerid, dAdminLogin, DIALOG_STYLE_PASSWORD, 
		"Авторизация в админ-панель",
		"В поле ниже введите сгенерированый для вас админ-пароль",
		"Далее", "Отмена"
	);
	SetPVarInt(playerid, "anti_skip_dialog", 1);
	return 0;
}

stock Debug(playerid, const message[])
{
	str_1[0] = EOS;
	format(str_1, sizeof str_1, "{FF0000}[DEBUG] {FFFFFF}%s", message);
	#if defined IsTestServer
		Message(playerid, -1, str_1);
	#endif
	str_1[0] = EOS;
}

stock CheckAdminLvl(playerid, lvl = 1)
{
	if(GetPlayerData(playerid, eAdmin) == 0) return 1;
	if(admin_logged[playerid] == false) return Message(playerid, 0xC0C0C0FF, "Вы не авторизованы в админ-панель");
	if(GetPlayerData(playerid, eAdmin) < lvl) return SendClientMessage(playerid, 0xC0C0C0FF, "Ваш уровень прав недостаточно высок для совершения данного действия");
	return 0;
}

stock Dialog(playerid, dialogid, style, caption[], info[], button1[], button2[])
{
	if(GetPVarInt(playerid, "anti_skip_dialog") == 1) return 1;
	return ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
}

stock UpdateMysqlUserInt(playerid, field[], value)
{
	new query[128];
	mysql_format(mysql, query, sizeof query, "UPDATE profile SET %s=%d WHERE id=%d", field, value, GetPlayerData(playerid, eID));
	mysql_pquery(mysql, query);
}

stock SendRadioMessage(fraction, const text[], color)
{
	foreach(new idx : Player)
	{
		if(!IsPlayerConnected(idx)) continue;
		else if(GetPlayerData(idx, eFraction) != fraction) continue;
		SendClientMessage(idx, color, text);
	}
}