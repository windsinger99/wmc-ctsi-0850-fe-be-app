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

#if (MODEL_TYPE == WMC_0750_NXP_VE)
 ledSinkControl_t LedSinkCurrentTbl[LED_SINK_CONTROL_STEPS] = {
		 /*Figure below shows 3rd OPAMP Amplification Configuration*/
   /*case 0 : basic===========================================================*/
  { 0x00,  0.0f, 0.0f }, 	//   0 	 	 ------(Switch=>OFF)---R(1K)-----
  { 0x01,  0.0f, 0.0f }, 	//   1		|				                 |
  { 0x02,  0.0f, 0.0f }, 	//   2		 ------(Switch=>OFF)---R(2K)-----
  { 0x03,  0.0f, 0.0f }, 	//   3		|				                 |
  /*case 1 : amplification====================================================*/
//  { 0x04,  0.0f, 0.0f }, 	//   4 	 	 ------(Switch=>ON)----R(1K)-----
//  { 0x05,  0.0f, 0.0f }, 	//   5		|				                 |
//  { 0x06,  0.0f, 0.0f }, 	//   6		 ------(Switch=>OFF)---R(2K)-----			=> 1K
//  { 0x07,  0.0f, 0.0f }, 	//   7		|				                 |
//  /*case 2 : amplification => synthetic resistance============================*/
//  { 0x12,  0.0f, 0.0f }, 	//   8 	 	 ------(Switch=>ON)----R(1K)-----
//  { 0x13,  0.0f, 0.0f }, 	//   9		|				                 |
//  { 0x14,  0.0f, 0.0f }, 	//  10		 ------(Switch=>ON)----R(2K)-----			=> 1.5K
//  { 0x15,  0.0f, 0.0f }, 	//  11		|				                 |
//  /*case 3 : amplification====================================================*/
//  { 0x08,  0.0f, 0.0f }, 	//  12 	 	 ------(Switch=>OFF)---R(1K)-----
//  { 0x09,  0.0f, 0.0f }, 	//  13		|				                 |
//  { 0x10,  0.0f, 0.0f }, 	//  14		 ------(Switch=>ON)----R(2K)-----			=> 2K
//  { 0x11,  0.0f, 0.0f }, 	//  15		|				                 |
};
#else
 ledSinkControl_t LedSinkCurrentTbl[LED_SINK_CONTROL_STEPS] = {
  { 0x00,  0.0f, 0.0f }, //  0
  { 0x01, 0.0f, 0.0f }, //  1
  { 0x02,  0.0f, 0.0f }, //  2
  { 0x03,  0.0f, 0.0f }, //  3
};
#endif
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
#elif (MODEL_TYPE == SLIM_0750_NXP_VE)
#include "SLIM_0750_VE_NXP_scan_table_3763.h"
#elif (MODEL_TYPE == WMC_0750_NXP_VE)
#include "WMC_0750_VE_NXP_scan_table_3563.h"
#else
#error "not defined MODEL_TYPE"

#endif

