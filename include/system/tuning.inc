#define MAX_TUNING_MODELS 	17

enum E_VEHICLE_STRUCTS {
	VEH_COMPONENT[MAX_TUNING_MODELS]
}

new g_vehicle[MAX_VEHICLES][E_VEHICLE_STRUCTS];

enum E_VEHICLE_TUNING_STRUCT {
	VT_MODEL[MAX_TUNING_MODELS]
}

new g_vehicle_tuning_temp[MAX_VEHICLES][E_VEHICLE_TUNING_STRUCT];

enum E_TUNING_MODELS
{
	E_MODEL_ID,
	E_ITEM_ID[MAX_TUNING_MODELS]
}

new g_tuning_models[][E_TUNING_MODELS] = 
{
	{411, {1196, 1201, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{549, {2712, 2713, 2714, 2715, 2716, 2717, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{15119, {2695, 2697, 2698, 2699, 2700, 2701, 2702, 2703, 2704, 2705, 2706, -1, -1, -1, -1, -1, -1}},
	{15128, {2735, 2736, 2737, 2738, 2739, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{15131, {2740, 2741, 2742, 2743, 2744, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{15120, {2707, 2708, 2709, 2710, 2711, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{15126, {2719, 2720, 2721, 2722, 2723, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},	
	{15125, {2724, 2725, 2726, 2727, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{534, {2728, 2729, 2730, 2731, 2732, 2733, 2734, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{536, {1104, 1105, 1182, 1181, 1184, 1103, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{558, {1088, 1089, 1091, 1163, 1164, 1165, 1166, 1168, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{559, {1067, 1069, 1070, 1159, 1160, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{560, {1026, 1028, 1029, 1030, 1170, 1169, 1140, 1141, 1032, 1033, 1139, -1, -1, -1, -1, -1, -1}},
	{562, {1034, 1035, 1037, 1171, 1172, 1148, 1149, 1036, 1038, 1039, 1146, 1147, -1, -1, -1, -1, -1}},
	{565, {1138, 1150, 1151, 1152, 1153, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{567, {1129, 1132, 1189, 1188, 1187, 1186, 1130, 1102, -1, -1, -1, -1, -1, -1, -1, -1, -1}},	
	{579, {1194, 1195, 2635, 2636, 2645, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{585, {2658, 2659, 2660, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},		
	{613, {1202, 1203, 1204, 2641, 2642, 2643, 2644, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{796, {2637, 2638, 2639, 2640, 2646, 2647, 2648, 2651, -1, -1, -1, -1, -1, -1, -1, -1, -1}},	
	{15080, {2680, 2681, 2682, 2683, 2684, 2685, 2686, 2687, 2688, 2689, 2690, 2691, 2692, 2693, -1, -1, -1}},
	{15107, {2667, 2668, 2669, 2670, 2671, 2672, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{15114, {2676, 2677, 2678, 2679, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
	{15115, {2661, 2662, 2663, 2664, 2665, 2666, 2673, 2674, 2675, -1, -1, -1, -1, -1, -1, -1, -1}},
	{15121, {1205, 1206, 2652, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}																	
};


//--------------------------------------------[Установка тюнинга]---------------------------------------------------//

for(new idx; idx != sizeof g_tuning_models; idx++)
			{
				for(new indexs = 0; indexs != MAX_TUNING_MODELS; indexs ++)
				{
					if(g_tuning_models[idx][E_MODEL_ID] != modelid) continue;
					if(g_tuning_models[idx][E_ITEM_ID][item] == -1) continue;
					if(g_tuning_models[idx][E_MODEL_ID] == GetVehicleModel(vehicleid))
					{
						SendPacket_Tun(playerid, vehicleid, g_tuning_models[idx][E_ITEM_ID][item]);
						g_vehicle_tuning_temp[vehicleid][VT_MODEL][item] = g_tuning_models[idx][E_ITEM_ID][item];
						break;
					}
				}
			}

//-------------------------------------------[Покупка тюнинга]----------------------------------
      new query[256];
			for(new idx = 0; idx != sizeof g_tuning_models; idx ++)
			{
				if(g_tuning_models[idx][E_MODEL_ID] == GetVehicleModel(vehicleid))
				{
					g_vehicle[vehicleid][VEH_COMPONENT][item] = g_tuning_models[idx][E_ITEM_ID][item];
				}
			}
			format(STRING_GLOBAL, sizeof STRING_GLOBAL, "%d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d",
			g_vehicle[vehicleid][VEH_COMPONENT][0], g_vehicle[vehicleid][VEH_COMPONENT][1], g_vehicle[vehicleid][VEH_COMPONENT][2], g_vehicle[vehicleid][VEH_COMPONENT][3], g_vehicle[vehicleid][VEH_COMPONENT][4], g_vehicle[vehicleid][VEH_COMPONENT][5],
			g_vehicle[vehicleid][VEH_COMPONENT][6], g_vehicle[vehicleid][VEH_COMPONENT][7], g_vehicle[vehicleid][VEH_COMPONENT][8], g_vehicle[vehicleid][VEH_COMPONENT][9], g_vehicle[vehicleid][VEH_COMPONENT][10], g_vehicle[vehicleid][VEH_COMPONENT][11],
			g_vehicle[vehicleid][VEH_COMPONENT][12], g_vehicle[vehicleid][VEH_COMPONENT][13], g_vehicle[vehicleid][VEH_COMPONENT][14], g_vehicle[vehicleid][VEH_COMPONENT][15], g_vehicle[vehicleid][VEH_COMPONENT][16]);
			if(IsAOwnableCar(vehicleid))
			{
				mysql_format(mysql, query, sizeof query, "UPDATE ownable_cars SET obves='%s' WHERE id=%d", STRING_GLOBAL,GetOwnableCarData(index, OC_SQL_ID));
				mysql_pquery(mysql, query);
			}
			format(g_ownable_car[index][OC_OBVES], 111, STRING_GLOBAL);
