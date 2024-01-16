#ifndef WMC_0850_NXP_NEW_PITCH_TABLE_H_
#define WMC_0850_NXP_NEW_PITCH_TABLE_H_


#include "TSP_Layout.h"

const float sensor_zero_x__ = 0.00f;
const float sensor_end_x__ = 1928.00f;
const float sensor_zero_y__ = 0.00f;
const float sensor_end_y__ = 1109.00f;

const float aarea_zero_x__ =  28.00f;
const float aarea_end_x__ = 1900.00;
const float aarea_zero_y__ = 28.00f;
const float aarea_end_y__ = 1081.00f;

const float sensor_XPD_Pos__[X_CELL_SIZE] =
{
  12.4f,
  17.6f,
  23.3f,
  29.3f,
  33.5f,
  38.7f,
  46.8f,
  54.7f,
  63.9f,
  72.6f,
  81.2f,
  90.8f,
  108.5f,
  121.0f,
  129.0f,
  141.3f,
  154.3f,
  172.7f,
  179.1f,
  186.3f,
  205.9f,
  220.1f,
  231.4f,
  250.8f,
  263.1f,
  272.9f,
  297.3f,
  318.6f,
  337.2f,
  357.8f,
  376.8f,
  398.4f,
  412.4f,
  434.8f,
  451.9f,
  475.8f,
  481.0f,
  505.9f,
  521.4f,
  542.2f,
  561.4f,
  586.0f,
  599.6f,
  616.7f,
  635.6f,
  657.4f,
  669.7f,
  681.9f,
  699.2f,
  719.2f,
  730.8f,
  739.4f,
  756.8f,
  772.0f,
  794.2f,
  804.2f,
  823.7f,
  831.3f,
  847.8f,
  863.7f,
  879.5f,
  894.4f,
  911.1f,
  925.8f,
  942.4f,
  958.3f,
  973.3f,
  990.0f,
  1006.0f,
  1021.7f,
  1037.5f,
  1053.0f,
  1070.2f,
  1089.9f,
  1111.4f,
  1127.6f,
  1146.4f,
  1154.2f,
  1174.2f,
  1181.0f,
  1200.8f,
  1215.1f,
  1232.2f,
  1252.5f,
  1272.2f,
  1290.0f,
  1308.8f,
  1332.0f,
  1350.5f,
  1366.9f,
  1382.2f,
  1389.9f,
  1414.5f,
  1428.6f,
  1453.6f,
  1475.2f,
  1494.9f,
  1511.2f,
  1528.7f,
  1545.0f,
  1565.4f,
  1579.8f,
  1595.6f,
  1611.6f,
  1628.8f,
  1634.8f,
  1647.4f,
  1667.1f,
  1678.1f,
  1693.4f,
  1707.1f,
  1716.7f,
  1737.4f,
  1749.6f,
  1756.2f,
  1773.2f,
  1784.7f,
  1798.0f,
  1821.0f,
  1825.1f,
  1838.7f,
  1848.4f,
  1856.0f,
  1868.6f,
  1876.5f,
  1881.8f,
  1890.4f,
  1894.7f,
  1900.2f,
  1906.0f,
  1910.5f,
  1915.6f
};

const float sensor_YPD_Pos__[Y_CELL_SIZE] =
{
  12.4f,
  16.4f,
  20.2f,
  24.0f,
  28.1f,
  34.1f,
  43.1f,
  52.6f,
  62.5f,
  72.5f,
  82.4f,
  91.8f,
  100.9f,
  110.1f,
  119.9f,
  131.1f,
  142.4f,
  153.5f,
  164.2f,
  175.6f,
  185.8f,
  196.3f,
  208.8f,
  221.0f,
  233.4f,
  245.8f,
  258.1f,
  271.7f,
  285.2f,
  299.0f,
  312.4f,
  326.2f,
  340.2f,
  354.1f,
  367.2f,
  381.6f,
  392.4f,
  400.4f,
  412.4f,
  419.2f,
  432.0f,
  444.6f,
  459.0f,
  465.8f,
  482.3f,
  487.1f,
  500.3f,
  510.8f,
  521.3f,
  531.8f,
  542.4f,
  552.9f,
  563.4f,
  573.9f,
  580.4f,
  594.9f,
  603.4f,
  619.8f,
  630.6f,
  643.4f,
  655.4f,
  669.4f,
  683.8f,
  697.8f,
  711.7f,
  727.2f,
  740.4f,
  752.5f,
  763.7f,
  776.9f,
  791.0f,
  803.2f,
  817.1f,
  826.2f,
  836.0f,
  846.9f,
  856.2f,
  865.3f,
  877.8f,
  889.4f,
  901.9f,
  914.3f,
  926.6f,
  938.7f,
  950.2f,
  961.8f,
  972.6f,
  984.0f,
  994.9f,
  1005.3f,
  1015.9f,
  1025.5f,
  1034.5f,
  1043.7f,
  1053.1f,
  1062.2f,
  1071.8f,
  1078.5f,
  1083.2f,
  1087.8f,
  1092.6f,
  1096.6f
};

const float sensor_XLED_Pos__[X_CELL_SIZE] =
{
  12.4f,
  17.6f,
  23.1f,
  26.9f,
  31.7f,
  39.6f,
  45.6f,
  53.2f,
  62.2f,
  71.2f,
  80.6f,
  93.8f,
  117.1f,
  127.0f,
  133.0f,
  149.5f,
  167.5f,
  179.5f,
  184.1f,
  196.5f,
  205.2f,
  223.9f,
  234.4f,
  253.8f,
  266.3f,
  288.6f,
  309.1f,
  329.6f,
  348.4f,
  368.4f,
  383.7f,
  401.5f,
  421.4f,
  443.8f,
  466.9f,
  473.4f,
  491.1f,
  515.3f,
  526.7f,
  551.0f,
  568.2f,
  587.8f,
  612.8f,
  618.1f,
  643.0f,
  656.0f,
  670.0f,
  682.4f,
  695.4f,
  706.8f,
  728.5f,
  746.4f,
  765.9f,
  779.2f,
  784.6f,
  809.6f,
  833.7f,
  849.3f,
  868.8f,
  885.7f,
  901.5f,
  919.0f,
  932.1f,
  948.9f,
  961.7f,
  980.3f,
  996.2f,
  1011.9f,
  1027.3f,
  1043.7f,
  1059.5f,
  1075.3f,
  1091.2f,
  1109.7f,
  1129.0f,
  1141.6f,
  1161.5f,
  1181.1f,
  1198.1f,
  1210.2f,
  1221.9f,
  1241.6f,
  1264.6f,
  1284.1f,
  1304.7f,
  1325.0f,
  1330.6f,
  1342.0f,
  1363.0f,
  1384.8f,
  1393.8f,
  1412.0f,
  1428.7f,
  1450.1f,
  1468.1f,
  1477.9f,
  1492.4f,
  1514.0f,
  1522.2f,
  1545.5f,
  1564.2f,
  1582.5f,
  1591.3f,
  1612.2f,
  1626.9f,
  1642.3f,
  1653.4f,
  1665.5f,
  1679.5f,
  1698.8f,
  1709.6f,
  1722.1f,
  1734.5f,
  1745.6f,
  1767.5f,
  1778.2f,
  1786.4f,
  1807.7f,
  1814.8f,
  1824.8f,
  1836.8f,
  1844.8f,
  1854.4f,
  1864.3f,
  1871.3f,
  1882.1f,
  1888.9f,
  1892.8f,
  1899.3f,
  1905.3f,
  1910.6f,
  1915.6f
};

const float sensor_YLED_Pos__[Y_CELL_SIZE] =
{
  12.4f,
  16.5f,
  20.4f,
  24.2f,
  29.7f,
  38.5f,
  46.9f,
  55.8f,
  64.4f,
  73.3f,
  82.9f,
  91.8f,
  101.7f,
  110.7f,
  120.5f,
  130.8f,
  141.7f,
  152.7f,
  163.9f,
  175.4f,
  186.8f,
  198.7f,
  210.9f,
  223.4f,
  235.8f,
  247.3f,
  259.5f,
  272.4f,
  284.7f,
  298.3f,
  311.4f,
  324.8f,
  338.5f,
  352.2f,
  365.1f,
  379.5f,
  390.3f,
  402.3f,
  415.5f,
  420.3f,
  435.5f,
  446.4f,
  457.2f,
  465.0f,
  475.1f,
  483.6f,
  498.1f,
  508.6f,
  519.1f,
  529.6f,
  540.0f,
  550.5f,
  561.0f,
  571.5f,
  578.0f,
  592.5f,
  601.0f,
  616.8f,
  627.6f,
  640.4f,
  652.5f,
  666.2f,
  679.6f,
  694.0f,
  707.9f,
  721.1f,
  735.6f,
  749.8f,
  763.8f,
  778.0f,
  791.9f,
  804.9f,
  816.1f,
  827.9f,
  837.2f,
  847.5f,
  856.7f,
  866.8f,
  878.4f,
  890.6f,
  902.8f,
  914.8f,
  926.9f,
  938.8f,
  950.5f,
  962.0f,
  973.2f,
  984.3f,
  995.2f,
  1005.6f,
  1015.5f,
  1025.2f,
  1035.1f,
  1044.8f,
  1053.7f,
  1062.9f,
  1070.9f,
  1079.8f,
  1084.5f,
  1088.4f,
  1092.6f,
  1096.6f
};



#endif /* WMC_0850_NXP_NEW_PITCH_TABLE_H_ */
