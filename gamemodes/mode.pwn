#include <a_samp>
#include <core>
#include <float>
#include <streamer>
#include <a_mysql>
#include <sscanf2>
#include <crashdetect>
#include <foreach>
#include <Pawn.CMD>
#include <Pawn.RakNet>

#if defined MAX_VEHICLES
	#undef MAX_VEHICLES
	#define MAX_VEHICLES 50
#else
	#define MAX_VEHICLES 50
#endif

#include "../include/system/fraction/fraction.inc"
#include "../include/system/player/player.inc"
#include "../include/system/other_components/other.inc"
#include "../include/system/vehicle/veh_function.inc"
#include "../include/system/commands/command.inc"
#include "../include/system/business/business.inc"
#include "../include/system/secondary_system/secondary_system.inc"

main()
{
	print("\n---------------------------------------");
	print("Developer Fernando_Black\n");
	print("---------------------------------------\n");
}



public OnPlayerConnect(playerid)
{
	SetDefaultSettings(playerid);
	SetTimerEx("CheckDataState", 2000, false, "i", playerid);
 	return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
}

stock ShowPlayerTextInLocal(playerid, text[])
{
	new mess[128 + 1];
	format(mess, sizeof mess, "- %s {%06x}(%s)[%d]", text, frac_color[GetPlayerData(playerid, eFraction)] >>> 8, GetPlayerData(playerid, eName), playerid);
	new Float: dist;
	new Float: x, Float: y, Float: z;
	GetPlayerPos(playerid, x, y, z);
	foreach(new idx : Player)
	{
		dist = GetPlayerDistanceFromPoint(idx, x, y, z);
		if(dist > 30.0) continue;
		SendClientMessage(idx, 0x999999FF, mess); 
	}
}

public OnPlayerText(playerid, text[])
{
	ShowPlayerTextInLocal(playerid, text);
}

stock SetDefaultSettings(playerid)
{
	GetPlayerName(playerid, ePlayer[playerid][eName], MAX_PLAYER_NAME + 1);
	admin_logged[playerid] = false;
}

public: CheckDataState(playerid)
{
	Message(playerid, 0xFFD700FF, "Приветствуем вас на "SERVER_NAME" ONLINE");
	static const query[] = "SELECT * FROM profile WHERE name = '%e'";
	new fmt_query[sizeof(query) + MAX_PLAYER_NAME];
	mysql_format(mysql, fmt_query, sizeof fmt_query, query, GetPlayerData(playerid, eName));
	mysql_query(mysql, fmt_query, true);
	if(cache_num_rows())
	{
		cache_get_value_name_int(0, "id", ePlayer[playerid][eID]);
		cache_get_value_name(0, "password", ePlayer[playerid][ePassword], 17);
		ShowLogin(playerid);
	}

}

stock ShowLogin(playerid)
{
	ShowPlayerDialog(playerid, dLogin, DIALOG_STYLE_INPUT, "Авторизация аккаунта", "Мы рады вновь увидеть вас у нас на сервере.\nЧтобы завершить данный этап, введите ваш пароль\nкоторый вы указали при регистрации", "Далее", "Отмена");
	return 1;
}

//----------------------------------------------------------

public OnPlayerSpawn(playerid)
{
}

//----------------------------------------------------------

public OnPlayerDeath(playerid, killerid, reason)
{

}

stock GoToMarkBusiness(playerid, idx) 
{
	new query[128 + 1];
	mysql_format(mysql, query, sizeof query, "SELECT * FROM application_business WHERE id=%d", idx);
	mysql_pquery(mysql, query, "CheckMarkBusiness", "d", playerid);
}

public: CheckMarkBusiness(playerid)
{
	new rows = cache_num_rows();
	new Float:x, Float:y, Float:z;
	if(rows)
	{
		cache_get_value_name_float(0, "x", x);
		cache_get_value_name_float(0, "y", y);
		cache_get_value_name_float(0, "z", z);
		SetPlayerPos(playerid, x, y, z);
	}
}

stock ConfirmApplicationBusiness(playerid, idx)
{
	new query[128 + 1];
	mysql_format(mysql, query, sizeof query, "SELECT * FROM application_business WHERE id=%d", idx);
	mysql_pquery(mysql, query, "ConfirmAppBusiness", "dd", playerid, idx);	
	return 1;
}

stock UpdateBaseBusiness(idx, field[], content)
{
	new query[128];
	mysql_format(mysql, query, sizeof query, "UPDATE business SET %s=%d WHERE id=%d", field, content, idx + 1);
	mysql_pquery(mysql, query);
}

public: ConfirmAppBusiness(playerid, idx)
{
	new rows = cache_num_rows();
	new Float:x, Float:y, Float:z;
	if(rows)
	{
		cache_get_value_name_float(0, "x", x);
		cache_get_value_name_float(0, "y", y);
		cache_get_value_name_float(0, "z", z);	
		AddBizFunction(playerid, "Строющиеся предприятие", -1, 100000, 10000, x, y, z);
		new last_biz = GetPVarInt(playerid, "last_biz");
		UpdateBaseBusiness(last_biz, "owner", GetPlayerData(playerid, eID));
		UpdateBaseBusiness(last_biz, "status", 1);
		format(e_business[last_biz][eBizOwnerName], MAX_PLAYER_NAME, "%s", GetPlayerData(playerid, eName));
		CheckOwnerNameBiz(last_biz);
		UpdateBusinessData(last_biz);
		DeleteApplicationBusiness(idx);
	}
}

stock DeleteApplicationBusiness(idx)
{
	new query[128];
	mysql_format(mysql, query, sizeof query, "DELETE FROM application_business WHERE id=%d", idx);
	mysql_tquery(mysql, query);
	mysql_tquery(mysql, "SELECT * FROM `business` ORDER BY `business`.`id` ASC ", "LoadBusiness");
}


public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if(!IsPlayerConnected(playerid)) return 1;
	switch(dialogid)
	{
		case dSetLeader:
		{
			if(!response) return 1;
			new player = GetPVarInt(playerid, "set_leader_id"), frac_name[50];
			ePlayer[player][eFraction] = listitem;
			ePlayer[player][eRang] = 10;
			if(ePlayer[player][eFraction] == 0)
				SetPlayerSkin(playerid, ePlayer[player][eSkin]);
			else SetPlayerSkin(playerid, org_data[listitem][eOrgSkin][ePlayer[player][eRang] - 1]);
			str_1[0] = EOS;
			format(str_1, sizeof str_1, "[A] Администратор %s[%d] %s игрока %s[%d]", GetPlayerData(playerid, eName), playerid, 
			listitem == 0 ? ("снял с лидерского поста") : ("поставил на лидерский пост"), GetPlayerData(player, eName), player);
			if(listitem != 0) 
			{
				format(frac_name, sizeof frac_name, ". Фракция - %s", fraction_name[listitem]);
				strcat(str_1, frac_name);
			}
			MessageAdmins(str_1, 0xFFA500FF);
			str_1[0] = EOS;
			if(listitem == 0)
				format(str_1, sizeof str_1, "Администратор %s[%d] снял вас с лидерского поста", GetPlayerData(playerid, eName), playerid);
			else
				format(str_1, sizeof str_1, "Администратор %s[%d] назначил вас на лидерский пост фракции - %s", GetPlayerData(playerid, eName), playerid, fraction_name[listitem]);
			Message(player, 0xFFFF00FF, str_1);
			UpdateMysqlUserInt(player, "fraction", ePlayer[player][eFraction]);
			UpdateMysqlUserInt(player, "rang", ePlayer[player][eRang]);
		}
		case dAdminLogin:
		{
			if(!response) return SetPVarInt(playerid, "anti_skip_dialog", 0), ShowAdminPass(playerid);
			if(!strcmp(inputtext, ePlayer[playerid][eAdminPass], false))
			{
				admin_logged[playerid] = true;
				Message(playerid, 0x228B22FF, "Вы успешно подтвердили свою личность.");

				str_1[0] = EOS;
				format(str_1, sizeof str_1, "%s %s[%d] успешно подтвердил свою личность в админ-панеле", AdminTags[GetPlayerData(playerid, eAdmin)], GetPlayerData(playerid, eName),
				playerid);
				MessageAdmins(str_1, 0xC0C0C0FF);
				str_1[0] = EOS;
				SetPVarInt(playerid, "anti_skip_dialog", 0);
			}
			else
			{
				SetPVarInt(playerid, "anti_skip_dialog", 0);
				ShowAdminPass(playerid);
				return Message(playerid, 0xFF0000FF, "Ошибка. Попробуйте ещё раз");
			}
		}
		case dShowAllOperationBiz:
		{
			if(!response) return 1;
			new biz_id = GetPVarInt(playerid, "select_biz");
			if(!strcmp(inputtext, "Предыдущая страница")) page_operation[playerid]--;
			else if(!strcmp(inputtext, "Следующая страница")) page_operation[playerid]++;
			new page = page_operation[playerid];
			new query[128];
			mysql_format(mysql, query, sizeof query, "SELECT * FROM business_profit WHERE biz_id=%d ORDER BY id ASC", biz_id);
			mysql_pquery(mysql, query, "ShowAllBusinessProfit", "dd", playerid, page);
		}
		case dBalanceOperationBiz:
		{
			if(!response) return 1;
			ShowPlayerBalanceOperation(playerid, listitem);
		}
		case dBizOperationSelect:
		{
			if(!response) return 1;
			SelectBizOperation(playerid, listitem);
		}
		case dBizList:
		{
			if(!response) return 1;
			new idx = GetPlayerListitemValue(playerid, listitem);
			SetPVarInt(playerid, "select_biz", idx);
			ShowPlayerBizOperationList(playerid, idx);
		}
		case dApplicationBusinessWork:
		{
			if(!response) return 1;
			new idx = GetPVarInt(playerid, "application_business");
			switch(listitem)
			{
				case 0:
				{
					GoToMarkBusiness(playerid, idx);
				}
				case 1:
				{
					ConfirmApplicationBusiness(playerid, idx);
				}
				case 2:
				{
					//RejectApplicationBusiness(playerid, idx);
				}
			}
		}
		case dApplicationBusiness:
		{
			if(!response) return 1;
			new idx = GetPlayerListitemValue(playerid, listitem);
			SetPVarInt(playerid, "application_business", idx);
			Dialog(playerid, dApplicationBusinessWork, DIALOG_STYLE_LIST, "Выберите действие", "1. Посмотреть место\n2. Подтвердить место\n3. Отклонить", "Далее", "Отмена");
		}
		case dCreateBusiness:
		{
			if(!response) return 1;
			new query[128 + 1], 
				Cache:result,
				admin_message[110 + MAX_PLAYER_NAME + 50],
				id,
				Float:X, Float:Y, Float:Z; 
			
			GetPlayerPos(playerid, X, Y, Z);
			mysql_format(mysql, query, sizeof query, "INSERT INTO application_business (owner_id,x,y,z,owner_name) VALUES (%d, %f, %f, %f, '%s')", 
			GetPlayerData(playerid, eID), X, Y, Z, GetPlayerData(playerid, eName));
			result = mysql_query(mysql, query);
			id = cache_insert_id();
			cache_delete(result);
			SendClientMessage(playerid, 0xFFD700FF, "Вы успешно подали заявку на создание предприятия. Ожидайте ответа от администрации");
			format(admin_message, sizeof admin_message, "[APPLICATION] %s [%d] подал заявку на создание предприятия №%d. Рассмотреть заявку - {00FF00}/application_biz",
			GetPlayerData(playerid, eName), playerid, id);
			MessageAdmins(admin_message, 0xFFA500FF);
		}
		case dRentDialog:
		{
			if(!response) return Message(playerid, 0xC0C0C0FF, "Ещё увидимся)");
			new rent_id = GetPVarInt(playerid, "rent_id");
			if(response)
			{
				new vehicleid = CreateVehicle(429, e_rent_data[rent_id][eSpawnPosX], e_rent_data[rent_id][eSpawnPosY], e_rent_data[rent_id][eSpawnPosZ], e_rent_data[rent_id][eSpawnPosA], 1, 1, -1);
				PutPlayerInVehicle(playerid, vehicleid, 0);
				//SetVehicleEngineState(vehicleid, 4);
			}
		}
		case dLogin:
		{
			if(!response) return Kick(playerid);
			if(response)
			{
				if(strlen(inputtext) < 1) return SendClientMessage(playerid, 0xC0C0C0FF, "Некорретная длина пароля"), ShowLogin(playerid);
				if(strcmp(inputtext, GetPlayerData(playerid, ePassword), false) != 0)
				{
					ShowLogin(playerid);
					return SendClientMessage(playerid, 0xC0C0C0FF, "Вы ввели неверный пароль от аккаунта");
				}
				else
				{
					return LoadAccountData(playerid);
				}
			}
		}
	}
	return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
	if(result == -1) return SendClientMessage(playerid, 0xFF4500FF, "{FF4500}[Подсказка] {C0C0C0}Вы ввели неизвестную команду");
	return 1;
}

public: LoadAccountData(playerid)
{
	static const query[] = "SELECT * FROM profile WHERE id = %d LIMIT 1";
	new fmt_text[sizeof(query) + 50];
	mysql_format(mysql, fmt_text, sizeof fmt_text, query, GetPlayerData(playerid, eID));
	mysql_tquery(mysql, fmt_text, "LoginCallBack", "d", playerid);
	return 1;
}

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle)
{
	printf("%d | %s | %s | %s", errorid, error, callback, query);
}

public: LoginCallBack(playerid)
{
	new rows;
	cache_get_row_count(rows);
	cache_get_value_name(0, "email", ePlayer[playerid][eEmail], 65);
	cache_get_value_name_int(0, "skin", ePlayer[playerid][eSkin]);
	cache_get_value_name_int(0, "age", ePlayer[playerid][eAge]);
	cache_get_value_name_int(0, "admin", ePlayer[playerid][eAdmin]);
	cache_get_value_name_int(0, "level", ePlayer[playerid][eLevel]);
	cache_get_value_name_int(0, "money", ePlayer[playerid][eMoney]);
	cache_get_value_name_int(0, "fraction", ePlayer[playerid][eFraction]);
	cache_get_value_name_int(0, "rang", ePlayer[playerid][eRang]);
	cache_get_value_name(0, "admin_pass", ePlayer[playerid][eAdminPass]);
	if(GetPlayerData(playerid, eAdmin) != 0)
	{
		static const text[] = "Ваши полномочия больше чем просто игрок. Подтвердите свои права";
		Message(playerid, 0xFFD700FF, text);
		ShowAdminPass(playerid);
		SetPlayerSkillLevel(playerid, 24, 100);
	}
	SetAccountDefaultSettings(playerid);
	SetAccountMoneySettings(playerid);
	SetAccountScoreSettings(playerid);
	GivePlayerWeapon(playerid, 24, 1000);
	InitializeFractionPlayer(playerid);
}


stock SetAccountMoneySettings(playerid)
{
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, GetPlayerData(playerid, eMoney));
}

stock SetAccountScoreSettings(playerid)
{
	SetPlayerScore(playerid, GetPlayerData(playerid, eLevel));
}

stock SetAccountDefaultSettings(playerid)
{
	new spawn_id = random(sizeof spawn_data);
	SetSpawnInfo(playerid, 0, ePlayer[playerid][eSkin], spawn_data[spawn_id][0], spawn_data[spawn_id][1], spawn_data[spawn_id][2], spawn_data[spawn_id][3], 
	0, 0, 0, 0, 0, 0);
	SpawnPlayer(playerid);
}


public OnPlayerRequestClass(playerid, classid)
{
	return 0;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	SetPlayerPos(playerid, fX, fY, fZ);
}

//----------------------------------------------------------

public OnGameModeInit()
{
	new server_version[32], year, month, day;
	getdate(year, month, day);
	format(server_version, sizeof server_version, "Build %02d.%02d.%04d", day, month, year);
	SetGameModeText(server_version);
	SendRconCommand("hostname "SERVER_NAME" ONLINE | DEVELOPMENT");
	mysql = mysql_connect(DATABASE_HOST, DATABASE_USER, DATABASE_PASSWORD, DATABASE_DB);
	mysql_set_charset("cp1251");
	switch(mysql_errno())
	{
		case 0: print("Подключение к базе данных удалось");
	    case 1044: return print("Подключение к базе данных не удалось [Указано несуществующее имя пользователя]");
	    case 1045: return print("Подключение к базе данных не удалось [Указан неправильный пароль]");
	    case 1049: return print("Подключение к базе данных не удалось [Указана несуществующая база данных]");
	    case 2003: return print("Подключение к базе данных не удалось [Хостинг с базой данных недоступен]");
	    case 2005: return print("Подключение к базе данных не удалось [Указан неправильный адрес хостинга]");
	    default:   return printf("Подключение к базе данных не удалось [Неизвестная ошибка. Код ошибки: %d]", mysql_errno());
	}
	printf("Загрузка игрового процесса - прошла успешно");
	new hour;
	gettime(hour);
	SetWorldTime(hour);
	ShowNameTags(true);
	DisableInteriorEnterExits();
	EnableStuntBonusForAll(false);
	ManualVehicleEngineAndLights();
	ShowPlayerMarkers(PLAYER_MARKERS_MODE_STREAMED);
	SetNameTagDrawDistance(30.0);

	Streamer_SetVisibleItems(STREAMER_TYPE_MAP_ICON, 98);
	Streamer_SetVisibleItems(STREAMER_TYPE_OBJECT, 700); // Default: 500

	Streamer_ToggleErrorCallback(true);
	Streamer_SetTickRate(30); // Default: 50

	LoadingOtherComponent();
	
	return 1;
}
stock LoadingOtherComponent()
{
	//---------------------------[Подгрузка компонентов]-------------------------//
	mysql_tquery(mysql, "SELECT * FROM `business` ORDER BY `business`.`id` ASC ", "LoadBusiness");
	mysql_tquery(mysql, "SELECT * FROM `fraction_veh` ORDER BY `fraction_veh`.`id` ASC", "LoadFractionVeh");
	//--------------------
	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(IsPlayerInAnyVehicle(playerid))
	{
		if(newkeys == 131072)
		{
			callcmd::e(playerid);
		}
		else if(PRESSED(KEY_FIRE)) callcmd::lights(playerid);
	}
	if(newkeys == 1024)
	{
		for(new idx; idx < e_total_biz; idx++)
		{
			if(IsPlayerInRangeOfPoint(playerid, 1.5, e_business[idx][eBizPosX], e_business[idx][eBizPosY], e_business[idx][eBizPosZ]))
			{
				if(0 <= e_business[idx][eBizState] <= 1) return Message(playerid, 0xC0C0C0FF, "* Данное предприятие находится на этапе подготовки");
				else Debug(playerid, "Вход");
			}
		}
		for(new idx; idx < MAX_RENT_DATA; idx++)
		{
			if(IsPlayerInRangeOfPoint(playerid, 1.5, e_rent_data[idx][ePosX], e_rent_data[idx][ePosY], e_rent_data[idx][ePosZ]))
			{
				new fmt_text[64 + 10];
				format(fmt_text, sizeof fmt_text, "Стоимость аренды скутера составляет - %d рублей.\nАрендовать?", PRICE_RENT);
				Dialog(playerid, dRentDialog, DIALOG_STYLE_MSGBOX, "Прокат лёгкого транспорта", fmt_text, "Далее", "Отмена");
				SetPVarInt(playerid, "rent_id", idx);
			}
		}
	}
	return 1;
}

public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid)
{
	return 1;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	return 1;
}

public OnPlayerUpdate(playerid)
{
	return 1;
}

public OnOutgoingRPC(playerid, rpcid, BitStream:bs)
{
	if(rpcid == 123)
	{
		new vehicleid;
		BS_ReadValue(bs, PR_UINT16, vehicleid);
		new string [124];
		format(string, sizeof string, "%s", eVehicleData[vehicleid][eVehNumber]);
		SendVehicleNumberPlate(playerid, vehicleid, eVehicleData[vehicleid][eVehNumberType], string);
		return 0;
	}
	return 1;
}