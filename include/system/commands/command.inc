CMD:veh(playerid, params[])
{
	if(CheckAdminLvl(playerid, 4)) return 1;
	extract params -> new model, color1, color2; else return Message(playerid, 0xC0C0C0FF, "[Подсказка] /veh [modelid] [color1] [color2]");
	if(model < 400 || model > 612) return Message(playerid, 0xC0C0C0FF, "Модель транспорта должна быть от 400 до 612");
	new Float:x, Float:y, Float:z, Float: angle;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, angle);
	CreateVehicle(model, x, y + 1.0, z, angle, color1, color2, -1);
	new fmt_text[200];
	format(fmt_text, sizeof fmt_text, "%s %s[%d] создал транспорт. Модель авто - %d", AdminTags[GetPlayerData(playerid, eAdmin)], GetPlayerData(playerid, eName), playerid, model);
	MessageAdmins(fmt_text, 0xC0C0C0FF);
	return 1;
}

CMD:test(playerid) return admin_logged[playerid] = true;

CMD:tp(playerid, params[])
{
	extract params -> new Float:x, Float:y, Float:z, Float:a, interiorid, worldid;
	SetPlayerPos(playerid, x, y, z);
	SetPlayerFacingAngle(playerid,a);
	SetPlayerInterior(playerid,interiorid);
	SetPlayerVirtualWorld(playerid, worldid);
	return 1;
}

CMD:e(playerid)
{
	if(GetPVarInt(playerid, "antiflood") > gettime()) return 1;
	new string[MAX_PLAYER_NAME + 4 + 18];
	new vehicleid = GetPlayerVehicleID(playerid);
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		for(new idx; idx < TotalFracVeh; idx++)
		{
			if(vehicleid == eVehFracData[idx][eVehTemp])
			{
				if(GetPlayerData(playerid, eRang) < eVehFracData[idx][eVehRang]) 
				{
					str_1[0] = EOS;
					format(str_1, sizeof str_1, "[Система безопасности] %s[%d] попытался запустить двигатель транспорта без ключей", 
					GetPlayerData(playerid, eName), playerid);
					SendRadioMessage(GetPlayerData(playerid, eFraction), str_1, 0x33CC66FF);
					RemovePlayerFromVehicle(playerid);
					return Message(playerid, 0xFF4500FF, "{FF4500}[Внимание] {FFFFFF}У вас нету ключей от данного транспорта");
				}
			}
		}
		new engine = (GetVehicleParamEx(vehicleid, V_ENGINE) ^ VEHICLE_PARAM_ON);
		SetVehicleParam(vehicleid, V_ENGINE, engine);
		format(string, sizeof string, "%s[%d] %s", GetPlayerData(playerid, eName), playerid, engine == 1 ? ("запустил(-а) двигатель") : ("заглушил(-а) двигатель"));
		SendMessageInLocal(playerid, string, 0xDD90FFFF, 1);
		SetPVarInt(playerid, "antiflood", gettime() + 5);
	}
	return 1;
}

stock SendVehicleNumberPlate(playerid, vehicleid, plateType, platenumber[]) {
	new BitStream:bs = BS_New();
	BS_WriteValue(bs,
	PR_INT16, vehicleid,
	PR_UINT8, plateType,

	PR_BOOL, true,
	PR_UINT8, strlen(platenumber),
	PR_STRING, platenumber
	);
	PR_SendRPC(bs, playerid, 123, PR_LOW_PRIORITY);
	BS_Delete(bs);
}

CMD:set_veh_numb(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	new plate;
	new number[32];
	sscanf(params, "ds[32]", plate, number);
	SendVehicleNumberPlate(playerid, vehicleid, plate, number);
	return 1;
}

CMD:lights(playerid)
{
	new string[MAX_PLAYER_NAME + 4 + 30];
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		new vehicleid = GetPlayerVehicleID(playerid);
		new lights = (GetVehicleParamEx(vehicleid, V_LIGHTS) ^ VEHICLE_PARAM_ON);
		SetVehicleParam(vehicleid, V_LIGHTS, lights);
		format(string, sizeof string, "%s[%d] %s", GetPlayerData(playerid, eName), playerid, lights == 1 ? ("включил(-а) световые приборы") : ("выключил(-а) световые приборы"));
		SendMessageInLocal(playerid, string, 0xDD90FFFF, 1);
		SetPVarInt(playerid, "antiflood", gettime() + 5);		
	}
}

CMD:create_business(playerid)
{
	if(CheckAdminLvl(playerid, 7)) return 1;
	static const text[] = "Для того чтобы подать заявку на создание собственного предприятия\nВы должны иметь следующие компоненты\n\br1. Наличные деньги - %s\n\br2. Проживание в области не менее 3-х лет";
	new fmt_text[(sizeof text) + 15];
	format(fmt_text, sizeof fmt_text, text, formatInt(PRICE_CREATE_BUSINESS));
	Dialog(playerid, dCreateBusiness, DIALOG_STYLE_MSGBOX, "{FFA500}Создание собственного предприятия", fmt_text, "Далее", "Отмена");
	return 1;
}

CMD:jet(playerid)
{
	SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USEJETPACK);
}

CMD:settime(playerid, get[])
{
	if(CheckAdminLvl(playerid, 4)) return 1;
	extract get -> new time;
	SetWorldTime(time);
	return 1;
}

CMD:application_biz(playerid)
{
	if(CheckAdminLvl(playerid, 7)) return 1;
	new query[128];
	mysql_format(mysql, query, sizeof query, "SELECT * FROM application_business");
	mysql_tquery(mysql, query, "ShowApplicationBusiness", "i", playerid);
	return 1;
}

CMD:setleader(playerid, params[])
{
	if(CheckAdminLvl(playerid, 4)) return 1;
	extract params -> new to_player; else return Message(playerid, 0xC0C0C0FF, "[Подсказка] /setleader [ид игрока]");
	new fmt_text[sizeof(fraction_name) * 30];
	if(!IsPlayerConnected(to_player)) return Message(playerid, 0xC0C0C0FF, "Такого игрока нет на сервере");

	SetPVarInt(playerid, "set_leader_id", to_player);

	for(new idx = 0; idx < sizeof fraction_name; idx++)
	{
		format(fmt_text, sizeof fmt_text, "%s%s\n", fmt_text, fraction_name[idx]);
	}
	Dialog(playerid, dSetLeader, DIALOG_STYLE_LIST, "Выберите фракцию", fmt_text, "Далее", "Отмена");
	return 1;
}