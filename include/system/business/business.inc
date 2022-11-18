#include <a_mysql>
#include <a_samp>
#define MAX_BUSINESS		50
#define SetBusinessData(%0,%1,%2)		e_business[%0][%1] = %2
#define GetBusinessData(%0,%1)			e_business[%0][%1]
#define AddBusinessData(%0,%1,%2,%3) 	e_business[%0][%1] %2= %3

#define public:%0(%1) \
			forward %0(%1); \
				public %0(%1)

#define MAX_BIZ_NAME	50

enum eBusinessStruct {
	eBizID,
	eBizName[50],
	eBizOwnerName[MAX_PLAYER_NAME],
	eBizOwner,
	Float:eBizPosX,
	Float:eBizPosY,
	Float:eBizPosZ,
	eBizState,
	eBizActiveOrder,
	eBizBalance,
	eBizPerson,
	eBizProduct,
	eBizType,
	eBizPrice,
	eBizRentPrice,
	eBizStatus,
	eBizLock,
	eBizPickup,
	Text3D:eBizLabel,
	eBizMapIcon
}

new e_business[MAX_BUSINESS][eBusinessStruct];
new e_total_biz;

new const g_biz_state[3][30] = 
{
	"Строительство",
	"Подготовка документов",
	"Работает"
};

public: CallBackLoadBusiness()
{
	str_1[0] = EOS;
	format(str_1, sizeof str_1, "SELECT * FROM business");
	mysql_pquery(mysql, str_1, "LoadBusiness");
}


AddBizFunction(playerid, namebiz[], type, price, rentprice, Float:x, Float:y, Float:z)
{
	new query[256], Cache:result;
	new idx = e_total_biz;
	SetBusinessData(idx, eBizPosX, x);
	SetBusinessData(idx, eBizPosY, y);
	SetBusinessData(idx, eBizPosZ, z);
	SetBusinessData(idx, eBizType, type);
	SetBusinessData(idx, eBizPrice, price);
	SetBusinessData(idx, eBizRentPrice, rentprice);

	mysql_format(mysql, query, sizeof query, 
		"INSERT INTO business (name, x, y, z, type, price, rentprice) VALUES ('%s', '%f', '%f', '%f', '%d', '%d', '%d')", namebiz, x, y, z, type, price, rentprice);

	result = mysql_query(mysql, query, true);
	SetPVarInt(playerid, "last_biz", cache_insert_id());
	SetBusinessData(idx, eBizID, cache_insert_id());

	cache_delete(result);
	++e_total_biz;
	str_1[0] = EOS;
	format(str_1, sizeof str_1, "SELECT * FROM business");
	mysql_pquery(mysql, str_1, "LoadBusiness");
	//UpdateBusinessData(idx);
}

CMD:add_biz(playerid, params[])
{
	if(GetPlayerData(playerid, eAdmin) < 7) return Message(playerid, 0xC0C0C0FF, "Вы не можете использовать данное действие");
	extract params -> new string:namebiz[32], type, price, rentprice; else return Message(playerid, 0xC0C0C0FF, "[Подсказка] /add_biz [Тип] [Цена] [Цена аренды]");
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	AddBizFunction(playerid, namebiz, type, price, rentprice, x, y, z);
	return 1;
}

CMD:biz_list(playerid, params[])
{
	new query[128 + 1];
	mysql_format(mysql, query, sizeof query, "SELECT * FROM business WHERE owner = %d", GetPlayerData(playerid, eID));
	mysql_pquery(mysql, query, "ShowPlayerBizList", "d", playerid);
	return 1;
}

public: ShowPlayerBizList(playerid)
{
	new rows, id, biz_name[50], biz_state, query[1024];
	rows = cache_num_rows();
	if(!rows) return Message(playerid, 0xC0C0C0FF, "{FF0000}[Ошибка] {c0c0c0}У вас отсутствуют личные предприятия");
	strcat(query, "Номер\t\tНазвание\t\tСостояние\n");
	for(new i; i < rows; i++)
	{
		cache_get_value_name_int(i, "id", id);
		cache_get_value_name(i, "name", biz_name, 50);
		cache_get_value_name_int(i, "state", biz_state);
		format(query, sizeof query, "%s%d\t\t%s\t\t%s\n", query, id, biz_name, g_biz_state[biz_state]);
		SetPlayerListitemValue(playerid, i, id - 1);
	}
	Dialog(playerid, dBizList, DIALOG_STYLE_TABLIST_HEADERS, "Ваши предприятия", query, "Далее", "Отмена");
	return 1;
}

stock ShowPlayerBizOperationList(playerid, biz_id)
{
	new fmt_text[1024], header[32 + MAX_BIZ_NAME];
	format(header, sizeof header, "Меню управления - {FFA500}%s[%d]", e_business[biz_id][eBizName], biz_id);
	format(fmt_text, sizeof fmt_text, "Наименование\t\t\tСостояние\nУправление дверью\t\t\t%s\nУправление бюджетом\t\t\t%s руб.\nПерсонал\t\t\t%s\nУправление заказами\t\t\t%s\nПродукция\t\t\t%d", 
	(e_business[biz_id][eBizLock] == 0 ? ("{00FF00}Открыто") : ("{FF0000}Закрыто")),
	formatInt(e_business[biz_id][eBizBalance]),
	(e_business[biz_id][eBizPerson] == 0 ? ("Отсутствует") : ("Присутствует")),
	(e_business[biz_id][eBizActiveOrder] == 0 ? ("Отсутствуют") : ("Присутствуют")),
	e_business[biz_id][eBizProduct]);
	Dialog(playerid, dBizOperationSelect, DIALOG_STYLE_TABLIST_HEADERS, header, fmt_text, "Далее", "Отмена");
	return 1;
}

stock SelectBizOperation(playerid, idx)
{
	new biz_id = GetPVarInt(playerid, "select_biz");
	new fmt_text[128];
	switch(idx)
	{
		case 0:
		{
			new lock = (e_business[biz_id][eBizLock] ^ 1);
			e_business[biz_id][eBizLock] = lock;
			UpdateBaseBusiness(biz_id, "lock_status", lock);
			format(fmt_text, sizeof fmt_text, "Двери вашего предприятия были успешно - %s", (lock == 1 ? ("{FF0000}Закрыты") : ("{00FF00}Открыты")));
			Message(playerid, 0xC0C0C0FF, fmt_text);
		//	UpdateBusinessData(biz_id);
			Streamer_Update(playerid, -1);
		}
		case 1:
		{
			Dialog(playerid, dBalanceOperationBiz, DIALOG_STYLE_LIST, "{FFFFFF}Управление бюджетом", "1. Посмотреть последние 20 зачислений\n2. Посмотреть все зачисления", "Далее", "Отмена");
		}
	}
	return 1;
}

stock ShowPlayerBalanceOperation(playerid, index)
{
	new biz_id = GetPVarInt(playerid, "select_biz");
	new query[128];
	if(index == 0)
	{
		mysql_format(mysql, query, sizeof query, "SELECT * FROM business_profit WHERE biz_id=%d ORDER BY id DESC LIMIT 20", biz_id);
		mysql_pquery(mysql, query, "ShowLastBusinessProfit", "d", playerid);
	}
	else
	{
		page_operation[playerid] = 0;
		mysql_format(mysql, query, sizeof query, "SELECT * FROM business_profit WHERE biz_id=%d ORDER BY id ASC", biz_id);
		mysql_pquery(mysql, query, "ShowAllBusinessProfit", "dd", playerid, page_operation[playerid]);
	}
	return 1;
}

public: ShowAllBusinessProfit(playerid, page)
{
	new rows = cache_num_rows(), query[1024], amount, time_and_date[50], id;
	if(!rows) return Message(playerid, 0xC0C0C0FF, "За выбранный период не найдено пополнений бюджета");
	strcat(query, "Транзакция\t\tСумма\t\tВремя\n");
	for(new idx = (page * 10); idx < (rows < ((page + 1) * 10) ? (rows) : ((page + 1) * 10)); idx++)
	{
		cache_get_value_name_int(idx, "id", id);
		cache_get_value_name_int(idx, "amount", amount);
		cache_get_value_name(idx, "time", time_and_date);
		format(query, sizeof query, "%s\t\t%d.\t\t%s\t\t%s\n", query, id, formatInt(amount), time_and_date);
	}
	if(rows >= 10)
	{
		if(page == 0)
		{
			strcat(query, "Следующая страница");
		}
		else if(page > 0 && rows > ((page + 1) * 10))
		{
			strcat(query, "Следующая страница\n");
			strcat(query, "Предыдущая страница");
		}
		else if(rows < ((page + 1) * 10))
		{
			strcat(query, "Предыдущая страница");
		}
	}
	Dialog(playerid, dShowAllOperationBiz, DIALOG_STYLE_TABLIST_HEADERS, "Доходы предприятия за всё время", query, "Далее", "");
	return 1;
}

public: ShowLastBusinessProfit(playerid)
{
	new rows = cache_num_rows(), query[1024], amount, time_and_date[50], id;
	if(!rows) return Message(playerid, 0xC0C0C0FF, "За выбранный период не найдено пополнений бюджета");
	strcat(query, "Транзакция\t\tСумма\t\tВремя\n");
	for(new idx; idx < rows; idx++)
	{
		cache_get_value_name_int(idx, "id", id);
		cache_get_value_name_int(idx, "amount", amount);
		cache_get_value_name(idx, "time", time_and_date);
		format(query, sizeof query, "%s\t\t%d.\t\t%s\t\t%s\n", query, id, formatInt(amount), time_and_date);
	}
	Dialog(playerid, 0, DIALOG_STYLE_TABLIST_HEADERS, "Доходы предприятия", query, "Далее", "");
	return 1;
}

public: LoadBusiness()
{
	new rows;
	rows = cache_num_rows();
	if(rows > MAX_BUSINESS) return print("Произошла системная ошибка. Обратитесь к разработчику");
	for(new i; i < rows; i++)
	{
		cache_get_value_name_int(i, "id", e_business[i][eBizID]);
		cache_get_value_name(i, "name", e_business[i][eBizName], 50);
		cache_get_value_name_int(i, "owner", e_business[i][eBizOwner]);
		cache_get_value_name_float(i, "X", e_business[i][eBizPosX]);
		cache_get_value_name_float(i, "Y", e_business[i][eBizPosY]);
		cache_get_value_name_float(i, "Z", e_business[i][eBizPosZ]);
		cache_get_value_name_int(i, "state", e_business[i][eBizState]);
		cache_get_value_name_int(i, "type", e_business[i][eBizType]);
		cache_get_value_name_int(i, "status", e_business[i][eBizStatus]);
		cache_get_value_name_int(i, "lock_status", e_business[i][eBizLock]);
		cache_get_value_name_int(i, "active_order", e_business[i][eBizActiveOrder]);
		cache_get_value_name_int(i, "balance", e_business[i][eBizBalance]);
		cache_get_value_name_int(i, "person", e_business[i][eBizPerson]);
		cache_get_value_name_int(i, "product", e_business[i][eBizProduct]);
		cache_get_value_name_int(i, "price", e_business[i][eBizPrice]);
		cache_get_value_name_int(i, "rentprice", e_business[i][eBizRentPrice]);
		CheckOwnerNameBiz(i);
		UpdateBusinessData(i);
	}
	e_total_biz = rows;
	printf("Бизнесов загружено - %d", e_total_biz);
	return 1;
}

UpdateBusinessData(idx)
{
	new lock_text[32];
	new states = e_business[idx][eBizState];
    if(IsValidDynamic3DTextLabel(e_business[idx][eBizLabel]))
		DestroyDynamic3DTextLabel(e_business[idx][eBizLabel]);

	if(e_business[idx][eBizPickup])
		DestroyPickup(e_business[idx][eBizPickup]);

	if(IsValidDynamicMapIcon(e_business[idx][eBizMapIcon]))
		DestroyDynamicMapIcon(e_business[idx][eBizMapIcon]);

	e_business[idx][eBizPickup] = CreatePickup(1274, 23, e_business[idx][eBizPosX], e_business[idx][eBizPosY], e_business[idx][eBizPosZ]);

	static const query[] = "{FFD700}Предприятие {B22222}%s [№%d]\n{FFD700}Владелец: {B22222}%s\n{FFD700}Состояние: {b22222}%s";
	new label[(sizeof query) + MAX_PLAYER_NAME + 50];
	format(label, sizeof label, query, e_business[idx][eBizName], e_business[idx][eBizID], e_business[idx][eBizOwnerName], g_biz_state[states]);
	format(lock_text, sizeof lock_text, "\n{FFD700}Двери: %s", e_business[idx][eBizLock] == 1 ? ("{FF0000}Закрыты") : ("{00FF00}Открыты"));

	if(states < 2) strcat(label, "\n{FF0000}Вход недоступен");
	else strcat(label, lock_text);

	e_business[idx][eBizLabel] = CreateDynamic3DTextLabel(label, -1, e_business[idx][eBizPosX], e_business[idx][eBizPosY], e_business[idx][eBizPosZ] + 1.0, 5.0);
	e_business[idx][eBizMapIcon] = CreateDynamicMapIcon(e_business[idx][eBizPosX], e_business[idx][eBizPosY], e_business[idx][eBizPosZ], 40, -1);
	
}

stock CheckOwnerNameBiz(idx)
{
	new query[128];
	mysql_format(mysql, query, sizeof query, "SELECT name FROM profile WHERE id=%d", e_business[idx][eBizOwner]);
	mysql_pquery(mysql, query, "CallBackCheckOwner", "d", idx);
}

public: CallBackCheckOwner(i)
{
	new rows;
	rows = cache_num_rows();
	if(rows)
	{
		cache_get_value_name(0, "name", e_business[i][eBizOwnerName], MAX_PLAYER_NAME + 1);
	}
	else format(e_business[i][eBizOwnerName], MAX_PLAYER_NAME, "Государство");
	UpdateBusinessData(i);
	return e_business[i][eBizOwnerName];
}


public: ShowApplicationBusiness(playerid)
{
	new rows, fmt_text[2048], id, player_id, name[MAX_PLAYER_NAME];
	strcat(fmt_text, "Номер заявки\t\tИмя игрока\n");
	rows = cache_num_rows();
	if(!rows) return SendClientMessage(playerid, 0xC0C0C0FF, "В данный момент нет доступных заявок для рассмотрения");
	if(rows)
	{
		for(new i = 0; i < rows; i++)
		{
			cache_get_value_name_int(i, "id", id);
			cache_get_value_name_int(i, "owner_id", player_id);
			cache_get_value_name(i, "owner_name", name, MAX_PLAYER_NAME);
			format(fmt_text, sizeof fmt_text, "%s\t%d\t\t%s\n", fmt_text, id, name);
			SetPlayerListitemValue(playerid, i, id);
		}
		ShowPlayerDialog(playerid, dApplicationBusiness, DIALOG_STYLE_TABLIST_HEADERS, "Доступные заявки", fmt_text, "Далее", "Отмена");
	}
	return 1;
}