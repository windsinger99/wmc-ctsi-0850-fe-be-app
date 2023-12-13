/*
 * scan_table.c
 *
 *  Created on: 2018. 11. 3.
 *      Author: shj9816
 */


#include "common.h"
#include "backend.h"
#include "scan_table.h"
#include "TSP_Layout.h"


 ledSinkControl_t LedSinkCurrentTbl[LED_SINK_CONTROL_STEPS] = {
  { 0x00,  0.0f, 0.0f }, //  0
  { 0x01, 0.0f, 0.0f }, //  1
  { 0x02,  0.0f, 0.0f }, //  2
  { 0x03,  0.0f, 0.0f }, //  3
};
#if (MODEL_TYPE ==  CTSI_0650_NXP)
#include "CTSI_0650_NXP_scan_table_3133.h"
#elif (MODEL_TYPE ==  WMC_0650_NXP)
#include "WMC_0650_NXP_scan_table_3333.h"
#elif (MODEL_TYPE ==  WMC_0650_VE_NXP)
#include "WMC_0650_VE_NXP_scan_table_3663.h"
 #elif (MODEL_TYPE ==  WMC_0750_NXP)
 #include "WMC_0750_NXP_scan_table_3638.h"
#elif (MODEL_TYPE ==  WMC_0850_NXP)
#include "WMC_0850_NXP_scan_table_4343.h"
#elif (MODEL_TYPE ==  SLIM_0320_NXP)
#include "SLIM_0320_NXP_scan_table_2963.h"
#elif (MODEL_TYPE ==  WMC_0850_NXP_VE)
#include "WMC_0850_VE_NXP_scan_table_3863.h"
#else
#error "not defined MODEL_TYPE"

#endif

