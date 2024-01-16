/*
 * pitch_table.c
 *
 *  Created on: 2018. 11. 3.
 *      Author: shj9816
 */
//------------------------------------------------------------------------------

#include "common.h"

#if (MODEL_TYPE ==  CTSI_0650_NXP) //kang@211015
#include "CTSI_0650_GD_new_pitch_table.h"
#elif ((MODEL_TYPE ==  WMC_0650_NXP))
#include "WMC_0650_NXP_new_pitch_table.h"
#elif ((MODEL_TYPE ==  WMC_0650_VE_NXP))
#include "WMC_0650_VE_NXP_new_pitch_table.h"
#elif ((MODEL_TYPE ==  WMC_0750_NXP))
#include "WMC_0750_NXP_new_pitch_table.h"
#elif (MODEL_TYPE == WMC_0850_NXP)
#include "WMC_0850_NXP_new_pitch_table.h"
#elif (MODEL_TYPE == SLIM_0320_NXP)
#include "SLIM_0320_NXPP_new_pitch_table.h"
#elif (MODEL_TYPE == WMC_0850_NXP_VE)
#include "WMC_0850_VE_NXP_new_pitch_table.h"
#elif (MODEL_TYPE == SLIM_0750_NXP_VE)
#include "SLIM_0750_VE_NXP_new_pitch_table.h"
#elif (MODEL_TYPE == WMC_0750_NXP_VE)
#include "WMC_0750_VE_NXP_new_pitch_table.h"

#else
#error "not defined MODEL_TYPE"

#endif

/* end of file */
