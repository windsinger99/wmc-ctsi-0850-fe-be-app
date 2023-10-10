#ifndef WMC_0750_NXP_NEW_PITCH_TABLE_H_
#define WMC_0750_NXP_NEW_PITCH_TABLE_H_


#include "TSP_Layout.h"

const float sensor_zero_x__ = 0.00f;
const float sensor_end_x__ = 1705.24f;
const float sensor_zero_y__ = 0.00f;
const float sensor_end_y__ = 983.26f;

const float aarea_zero_x__ =  27.52f;
const float aarea_end_x__ = 1677.72;
const float aarea_zero_y__ = 27.48f;
const float aarea_end_y__ = 955.78f;

const float sensor_XPD_Pos__[X_CELL_SIZE] =
{
  12.32f,
  17.42f,
  24.12f,
  28.62f,
  33.32f,
  38.62f,
  46.62f,
  54.62f,
  63.62f,
  72.62f,
  81.02f,
  90.62f,
  109.42f,
  122.22f,
  131.82f,
  141.22f,
  154.22f,
  172.42f,
  179.42f,
  186.62f,
  205.82f,
  222.52f,
  232.32f,
  251.02f,
  262.02f,
  272.42f,
  297.22f,
  318.22f,
  336.92f,
  358.02f,
  376.72f,
  396.92f,
  411.82f,
  433.72f,
  451.92f,
  475.22f,
  480.22f,
  505.22f,
  520.72f,
  541.52f,
  560.52f,
  581.92f,
  599.82f,
  615.42f,
  634.52f,
  656.52f,
  668.82f,
  681.02f,
  698.52f,
  718.32f,
  729.92f,
  742.62f,
  755.92f,
  771.42f,
  793.12f,
  803.22f,
  822.72f,
  830.72f,
  846.92f,
  867.72f,
  887.62f,
  905.72f,
  924.22f,
  931.52f,
  951.82f,
  958.82f,
  978.62f,
  995.02f,
  1010.12f,
  1030.32f,
  1050.02f,
  1068.02f,
  1086.62f,
  1109.72f,
  1128.32f,
  1144.92f,
  1159.32f,
  1168.22f,
  1190.22f,
  1209.12f,
  1231.12f,
  1252.32f,
  1272.22f,
  1288.52f,
  1306.52f,
  1322.42f,
  1342.82f,
  1355.22f,
  1372.32f,
  1386.62f,
  1406.22f,
  1412.22f,
  1424.82f,
  1445.52f,
  1456.42f,
  1470.62f,
  1486.52f,
  1495.32f,
  1515.22f,
  1526.82f,
  1533.82f,
  1551.02f,
  1565.92f,
  1575.42f,
  1598.42f,
  1602.42f,
  1615.82f,
  1625.92f,
  1633.22f,
  1643.82f,
  1653.82f,
  1658.82f,
  1667.82f,
  1672.32f,
  1677.52f,
  1683.32f,
  1687.82f,
  1692.92f
};

const float sensor_YPD_Pos__[Y_CELL_SIZE] =
{
  12.28f,
  16.28f,
  20.08f,
  23.88f,
  31.18f,
  38.98f,
  46.28f,
  55.48f,
  63.38f,
  73.38f,
  82.68f,
  92.68f,
  101.78f,
  109.98f,
  121.08f,
  131.98f,
  143.28f,
  154.68f,
  165.08f,
  176.48f,
  187.88f,
  197.18f,
  209.68f,
  221.88f,
  234.28f,
  246.68f,
  258.98f,
  272.58f,
  286.08f,
  299.88f,
  313.28f,
  327.08f,
  341.08f,
  354.98f,
  368.08f,
  382.58f,
  393.38f,
  402.58f,
  413.38f,
  426.18f,
  436.98f,
  445.58f,
  460.08f,
  470.88f,
  479.38f,
  493.88f,
  504.68f,
  517.48f,
  529.48f,
  543.48f,
  557.98f,
  571.98f,
  585.88f,
  600.18f,
  614.68f,
  626.78f,
  637.98f,
  652.48f,
  665.38f,
  678.58f,
  691.48f,
  700.58f,
  710.38f,
  721.28f,
  730.58f,
  739.68f,
  752.18f,
  764.38f,
  776.28f,
  788.58f,
  801.28f,
  813.08f,
  824.88f,
  836.18f,
  847.98f,
  858.38f,
  869.28f,
  879.68f,
  890.28f,
  900.18f,
  909.48f,
  918.08f,
  927.78f,
  936.58f,
  946.18f,
  952.88f,
  957.58f,
  962.18f,
  966.98f,
  970.98f
};

const float sensor_XLED_Pos__[X_CELL_SIZE] =
{
  12.32f,
  17.32f,
  23.32f,
  27.82f,
  31.62f,
  39.32f,
  45.32f,
  53.02f,
  62.22f,
  71.12f,
  80.42f,
  93.82f,
  117.52f,
  126.22f,
  136.22f,
  149.72f,
  168.92f,
  179.92f,
  184.42f,
  196.62f,
  205.32f,
  223.82f,
  233.02f,
  254.12f,
  265.22f,
  288.82f,
  309.02f,
  329.42f,
  348.42f,
  368.02f,
  384.42f,
  400.72f,
  421.32f,
  442.22f,
  466.72f,
  473.42f,
  491.12f,
  515.12f,
  527.02f,
  550.42f,
  567.92f,
  584.42f,
  607.42f,
  618.12f,
  642.52f,
  655.52f,
  669.52f,
  681.72f,
  695.02f,
  706.12f,
  727.92f,
  741.82f,
  765.42f,
  778.82f,
  783.52f,
  808.52f,
  832.22f,
  847.72f,
  868.72f,
  887.62f,
  904.32f,
  919.62f,
  939.12f,
  959.12f,
  975.72f,
  987.82f,
  999.52f,
  1020.62f,
  1042.32f,
  1061.52f,
  1082.22f,
  1102.62f,
  1108.12f,
  1119.52f,
  1140.52f,
  1162.12f,
  1173.42f,
  1188.72f,
  1207.12f,
  1227.52f,
  1245.52f,
  1256.62f,
  1271.82f,
  1291.42f,
  1299.62f,
  1322.82f,
  1343.52f,
  1359.12f,
  1368.42f,
  1389.72f,
  1404.02f,
  1419.62f,
  1430.72f,
  1443.22f,
  1458.02f,
  1476.02f,
  1486.92f,
  1500.02f,
  1511.72f,
  1523.02f,
  1544.52f,
  1555.52f,
  1564.72f,
  1585.02f,
  1592.02f,
  1602.12f,
  1614.22f,
  1621.02f,
  1631.62f,
  1644.42f,
  1651.42f,
  1659.22f,
  1666.22f,
  1672.62f,
  1676.62f,
  1682.62f,
  1687.92f,
  1692.92f
};

const float sensor_YLED_Pos__[Y_CELL_SIZE] =
{
  12.28f,
  16.38f,
  20.28f,
  24.08f,
  30.58f,
  38.38f,
  47.08f,
  55.68f,
  64.28f,
  73.18f,
  82.48f,
  91.68f,
  101.58f,
  110.58f,
  120.68f,
  130.68f,
  141.58f,
  152.28f,
  163.78f,
  175.28f,
  186.38f,
  198.58f,
  210.78f,
  223.28f,
  234.68f,
  247.18f,
  259.38f,
  272.28f,
  284.58f,
  298.18f,
  311.28f,
  324.68f,
  338.38f,
  352.08f,
  364.98f,
  379.48f,
  390.28f,
  401.08f,
  415.58f,
  424.78f,
  435.58f,
  446.48f,
  457.28f,
  468.08f,
  477.18f,
  490.98f,
  501.78f,
  514.58f,
  526.68f,
  540.38f,
  553.78f,
  568.28f,
  582.18f,
  596.68f,
  609.98f,
  624.18f,
  638.18f,
  652.08f,
  666.28f,
  680.28f,
  690.48f,
  702.28f,
  711.58f,
  721.88f,
  731.08f,
  740.18f,
  751.78f,
  764.38f,
  776.88f,
  789.18f,
  801.28f,
  813.18f,
  824.98f,
  836.38f,
  847.58f,
  858.68f,
  869.58f,
  879.98f,
  889.88f,
  899.88f,
  909.78f,
  919.18f,
  928.38f,
  937.28f,
  945.28f,
  954.18f,
  958.88f,
  962.78f,
  966.98f,
  970.98f
};



#endif /* WMC_0750_NXP_NEW_PITCH_TABLE_H_ */