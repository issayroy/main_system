const MAX_ORG_SKIN = 10;

enum eOrgData
{
    eOrgID,
    eOrgSkin[MAX_ORG_SKIN],
    Float:eOrgSpawn[4],
    eOrgColor
}

new Iterator:org_pravo<MAX_PLAYERS>;

new const org_data[][eOrgData] = 
{
    {0, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0.0, 0.0, 0.0, 0.0}},
    {1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {1822.1677,2095.8022,16.1631,272.0798}}
};

new const
    frac_color[] = 
{
    0xFFFFFF11,
    0xCCFF00FF
};

new const 
    fraction_name[][30] = 
{
    "Снятие с лидерского поста",
    "Городская администрация"
};