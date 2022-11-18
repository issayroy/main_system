
#define VEHICLE_PARAM_ON	(1)
#define VEHICLE_PARAM_OFF	(0)

#define GetVehicleParamEx(%0,%1) g_vehicle_params[%0][%1]


#define MAX_FDATA	50

new TotalFracVeh = 0;

enum
{
	vehTypeNone = -1,
	vehFractionPravo = 1
}

enum E_VEHICLE_PARAMS_STRUCT
{
	V_ENGINE,
	V_LIGHTS,
	V_ALARM,
	V_LOCK, 
	V_BONNET, 	
	V_BOOT, 	
	V_OBJECTIVE 
};

new g_vehicle_params[MAX_VEHICLES][E_VEHICLE_PARAMS_STRUCT];

enum eVehDataFrac
{
	eVehID,
	eVehColor[2],
	eVehModel,
	eVehRang,
	Float:eVehPos[4],
	eVehTemp,
	eVehFraction
}

new eVehFracData[MAX_FDATA][eVehDataFrac];

enum eVehicleSt
{
	eVehType,
	eVehNumberType,
	eVehNumber[32]
}

new eVehicleData[MAX_VEHICLES][eVehicleSt];

stock n_veh_CreateVehicle(modelid, Float:x, Float:y, Float:z, Float:angle, color1, color2, respawn_delay, type = vehTypeNone)
{
	static n_veh_vehicleid = INVALID_VEHICLE_ID;

	n_veh_vehicleid = CreateVehicle(modelid, x, y, z, angle, color1, color2, respawn_delay);
	eVehicleData[n_veh_vehicleid][eVehType] = type;
	SetVehicleParamsEx(n_veh_vehicleid, VEHICLE_PARAM_OFF, VEHICLE_PARAM_OFF, VEHICLE_PARAM_OFF, VEHICLE_PARAM_OFF, VEHICLE_PARAM_OFF, VEHICLE_PARAM_OFF, VEHICLE_PARAM_OFF);

	return n_veh_vehicleid;
}
#if defined _ALS_CreateVehicle
    #undef CreateVehicle
#else
    #define _ALS_CreateVehicle
#endif
#define CreateVehicle n_veh_CreateVehicle

stock SetVehicleParamsInit(vehicleid)
{
	GetVehicleParamsEx
	(
		vehicleid,
		g_vehicle_params[vehicleid][V_ENGINE],
		g_vehicle_params[vehicleid][V_LIGHTS],
		g_vehicle_params[vehicleid][V_ALARM],
		g_vehicle_params[vehicleid][V_LOCK],
		g_vehicle_params[vehicleid][V_BONNET],
		g_vehicle_params[vehicleid][V_BOOT],
		g_vehicle_params[vehicleid][V_OBJECTIVE]
	);
}

stock GetVehicleParam(vehicleid, E_VEHICLE_PARAMS_STRUCT:paramid)
{
	SetVehicleParamsInit(vehicleid);
	return g_vehicle_params[vehicleid][paramid];
}

stock SetVehicleParam(vehicleid, E_VEHICLE_PARAMS_STRUCT:paramid, set_value)
{
	SetVehicleParamsInit(vehicleid);
	g_vehicle_params[vehicleid][paramid] = bool: set_value;

	SetVehicleParamsEx
	(
		vehicleid,
		g_vehicle_params[vehicleid][V_ENGINE],
		g_vehicle_params[vehicleid][V_LIGHTS],
		g_vehicle_params[vehicleid][V_ALARM],
		g_vehicle_params[vehicleid][V_LOCK],
		g_vehicle_params[vehicleid][V_BONNET],
		g_vehicle_params[vehicleid][V_BOOT],
		g_vehicle_params[vehicleid][V_OBJECTIVE]
	);
}

public: LoadFractionVeh()
{
	new rows;
	rows = cache_num_rows();
	for(new idx; idx != rows; idx++)
	{
		cache_get_value_name_int(idx, "id", eVehFracData[idx][eVehID]);
		cache_get_value_name_int(idx, "model", eVehFracData[idx][eVehModel]);
		cache_get_value_name_int(idx, "rang", eVehFracData[idx][eVehRang]);
		cache_get_value_name_int(idx, "fraction", eVehFracData[idx][eVehFraction]);
		cache_get_value_name_int(idx, "color_1", eVehFracData[idx][eVehColor][0]);
		cache_get_value_name_int(idx, "color_2", eVehFracData[idx][eVehColor][1]);
		cache_get_value_name_float(idx, "X", eVehFracData[idx][eVehPos][0]);
		cache_get_value_name_float(idx, "Y", eVehFracData[idx][eVehPos][1]);
		cache_get_value_name_float(idx, "Z", eVehFracData[idx][eVehPos][2]);
		cache_get_value_name_float(idx, "Angle", eVehFracData[idx][eVehPos][3]);
		TotalFracVeh++;
		eVehFracData[idx][eVehTemp] = CreateVehicle
		(
			eVehFracData[idx][eVehModel],
			eVehFracData[idx][eVehPos][0],
			eVehFracData[idx][eVehPos][1],
			eVehFracData[idx][eVehPos][2],
			eVehFracData[idx][eVehPos][3],
			eVehFracData[idx][eVehColor][0],
			eVehFracData[idx][eVehColor][1],
			-1, eVehFracData[idx][eVehFraction]
		);
		eVehicleData[eVehFracData[idx][eVehTemp]][eVehNumberType] = 4;
		SetVehicleNumberPlate(eVehFracData[idx][eVehTemp], "а777аа");
		format(eVehicleData[eVehFracData[idx][eVehTemp]][eVehNumber], 124, "а777аа");
	}
	printf("[Фракционный транспорт] загружено - %i", TotalFracVeh);
}
