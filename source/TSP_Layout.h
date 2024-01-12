
#ifndef TSP_LAYOUT_H_
#define TSP_LAYOUT_H_


#include "common.h"
#include "backend.h"

//#define X_AXIS                  0
//#define Y_AXIS                  1
typedef enum {
    X_AXIS = 0,
    Y_AXIS = 1,
} axis_type_enum;


/*****************************************************************************/
#if (MODEL_TYPE == CTSI_0650_NXP)
/*****************************************************************************/
#define X_CELL_SIZE             96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             78      // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL	//31  //34      // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL	//33  //55      //

#define CELLS_PER_CS            10  //9       // number of PDs per one chip-select
#define ENABLED_CS_NUM          1       // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       10  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      2   //8       // see last PD board //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            3
#define X_LED_BD1_CELL_NUM      34  //37
#define X_LED_BD2_CELL_NUM      27  //30
#define X_LED_BD3_CELL_NUM      35  //38

#define Y_LED_BD_NUM            2
#define Y_LED_BD1_CELL_NUM      41  //45
#define Y_LED_BD2_CELL_NUM      37  //37

#define X_PD_BD_NUM             3
#define X_PD_BD1_CELL_NUM       34  //36
#define X_PD_BD2_CELL_NUM       28  //31
#define X_PD_BD3_CELL_NUM       34  //38

#define Y_PD_BD_NUM             2
#define Y_PD_BD1_CELL_NUM       43  //450
#define Y_PD_BD2_CELL_NUM       35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    10
#define ADC_SIZE    10

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)       // 34/10 => 3 Group(+4)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1) // 28/10 => 3 group(+8 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))     // 34/10 => 3 group(+4 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3)   // 8 group 16


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 43/10 => 4 group(+3 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 35/10 => 4 group(+5 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2)         // 8 group 8

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 8 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 9 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 18

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 5
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3,4
/*****************************************************************************/

#elif (MODEL_TYPE == WMC_0650_NXP)
#define X_CELL_SIZE             102//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             78          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //31  //34      // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //31  //55      //

#define CELLS_PER_CS            10  //9       // number of PDs per one chip-select
#define ENABLED_CS_NUM          1       // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       10  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      2   //8       // see last PD board //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            3
#define X_LED_BD1_CELL_NUM      35  //34  //37
#define X_LED_BD2_CELL_NUM      32  //27  //30
#define X_LED_BD3_CELL_NUM      35  //35  //38

#define Y_LED_BD_NUM            2
#define Y_LED_BD1_CELL_NUM      39  //41  //45
#define Y_LED_BD2_CELL_NUM      39  //37  //37

#define X_PD_BD_NUM             3
#define X_PD_BD1_CELL_NUM       34  //34  //36
#define X_PD_BD2_CELL_NUM       32  //28  //31
#define X_PD_BD3_CELL_NUM       36  //34  //38

#define Y_PD_BD_NUM             2
#define Y_PD_BD1_CELL_NUM       39  //43  //450
#define Y_PD_BD2_CELL_NUM       39  //35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    10
#define ADC_SIZE    10

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 34/10 => 3 group(+4 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 32/10 => 4 group(+2 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))        // 36/10 => 3 group(+6 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3)   // 10 group 12


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 39/10 => 3 group(+9 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 39/10 => 4 group(+9 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2)         // 7 group 18

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 10 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 7 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 15

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 5
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3
/*****************************************************************************/

#elif (MODEL_TYPE == WMC_0650_VE_NXP)
#define X_CELL_SIZE             110//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             78          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //31  //34      // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //31  //55      //

#define CELLS_PER_CS            10  //9       // number of PDs per one chip-select
#define ENABLED_CS_NUM          1       // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       10  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      2   //8       // see last PD board //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            3
#define X_LED_BD1_CELL_NUM      38  //34  //37
#define X_LED_BD2_CELL_NUM      32  //27  //30
#define X_LED_BD3_CELL_NUM      40  //35  //38

#define Y_LED_BD_NUM            2
#define Y_LED_BD1_CELL_NUM      43  //41  //45
#define Y_LED_BD2_CELL_NUM      35  //37  //37

#define X_PD_BD_NUM             3
#define X_PD_BD1_CELL_NUM       38  //34  //36
#define X_PD_BD2_CELL_NUM       32  //28  //31
#define X_PD_BD3_CELL_NUM       40  //34  //38

#define Y_PD_BD_NUM             2
#define Y_PD_BD1_CELL_NUM       42  //43  //450
#define Y_PD_BD2_CELL_NUM       36  //35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    10
#define ADC_SIZE    10

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 34/10 => 3 group(+4 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 32/10 => 4 group(+2 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))        // 36/10 => 3 group(+6 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3)   // 10 group 12


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 39/10 => 3 group(+9 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 39/10 => 4 group(+9 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2)         // 7 group 18

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 10 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 7 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 15

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 5
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3
/*****************************************************************************/


#elif (MODEL_TYPE == WMC_0750_NXP)
#define X_CELL_SIZE             118//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             90          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //38        // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //38        //

#define CELLS_PER_CS            12  //10  //9 // number of PDs per one chip-select
#define ENABLED_CS_NUM          1             // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       12  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      6   //8       // see last PD board(Y(R2)) //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            4
#define X_LED_BD1_CELL_NUM      37  //34  //37
#define X_LED_BD2_CELL_NUM      25  //27  //30
#define X_LED_BD3_CELL_NUM      23  //35  //38
#define X_LED_BD4_CELL_NUM      33  //35  //38

#define Y_LED_BD_NUM            2
#define Y_LED_BD1_CELL_NUM      45  //41  //45
#define Y_LED_BD2_CELL_NUM      45  //37  //37

#define X_PD_BD_NUM             4
#define X_PD_BD1_CELL_NUM       37  //34  //36
#define X_PD_BD2_CELL_NUM       23  //28  //31
#define X_PD_BD3_CELL_NUM       24  //34  //38
#define X_PD_BD4_CELL_NUM       34  //34  //38

#define Y_PD_BD_NUM             2
#define Y_PD_BD1_CELL_NUM       45  //43  //450
#define Y_PD_BD2_CELL_NUM       45  //35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    12
#define ADC_SIZE    12

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 37/12 => 3 group(+7 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 23/10 => 3 group(+3 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS) + 1)    // 24/10 => 3 group(+4 pd)
#define X_PD_SHIFT_BD_4         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))        // 33/10 => 3 group(+3 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3 + X_PD_SHIFT_BD_4)   // 12 group 17


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 45/10 => 4 group(+5 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 45/10 => 5 group(+5 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2)         // 9 group 10

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 12 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 9 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 21

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 6
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3,4,5
/*****************************************************************************/

#elif (MODEL_TYPE == WMC_0850_NXP)
#define X_CELL_SIZE             132//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             102          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //38        // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //38        //

#define CELLS_PER_CS            12  //10  //9 // number of PDs per one chip-select
#define ENABLED_CS_NUM          1             // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       12  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      6   //8       // see last PD board(Y(R2)) //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            4
#define X_LED_BD1_CELL_NUM      37  //34  //37
#define X_LED_BD2_CELL_NUM      28  //27  //30
#define X_LED_BD3_CELL_NUM      28  //35  //38
#define X_LED_BD4_CELL_NUM      39  //35  //38

#define Y_LED_BD_NUM            3
#define Y_LED_BD1_CELL_NUM      40  //41  //45
#define Y_LED_BD2_CELL_NUM      29  //37  //37
#define Y_LED_BD3_CELL_NUM      33  //37  //37

#define X_PD_BD_NUM             4
#define X_PD_BD1_CELL_NUM       37  //34  //36
#define X_PD_BD2_CELL_NUM       29  //28  //31
#define X_PD_BD3_CELL_NUM       28  //34  //38
#define X_PD_BD4_CELL_NUM       38  //34  //38

#define Y_PD_BD_NUM             3
#define Y_PD_BD1_CELL_NUM       44  //43  //450
#define Y_PD_BD2_CELL_NUM       26  //35  //37
#define Y_PD_BD3_CELL_NUM       32  //35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    12
#define ADC_SIZE    12

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 37/12 => 3 group(+7 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 23/10 => 3 group(+3 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS) + 1)    // 24/10 => 3 group(+4 pd)
#define X_PD_SHIFT_BD_4         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))        // 33/10 => 3 group(+3 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3 + X_PD_SHIFT_BD_4)   // 12 group 17


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 45/10 => 4 group(+5 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 45/10 => 5 group(+5 pd)
#define Y_PD_SHIFT_BD_3         ((Y_PD_BD3_CELL_NUM / CELLS_PER_CS) + 1)    // 45/10 => 5 group(+5 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2 + Y_PD_SHIFT_BD_3)         // 9 group 10

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 12 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 9 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 21

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 6
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3,4,5
/*****************************************************************************/


#elif (MODEL_TYPE == SLIM_0320_NXP)
#define X_CELL_SIZE             90//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             60          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //38        // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //38        //

#define CELLS_PER_CS            6  //10  //9 // number of PDs per one chip-select
#define ENABLED_CS_NUM          1             // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       6  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      0   //8       // see last PD board(Y(R2)) //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            2
#define X_LED_BD1_CELL_NUM      45  //34  //37
#define X_LED_BD2_CELL_NUM      45  //27  //30

#define Y_LED_BD_NUM            1
#define Y_LED_BD1_CELL_NUM      60  //41  //45

#define X_PD_BD_NUM             2
#define X_PD_BD1_CELL_NUM       44  //34  //36
#define X_PD_BD2_CELL_NUM       46  //28  //31

#define Y_PD_BD_NUM             1
#define Y_PD_BD1_CELL_NUM       60  //43  //450
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    6
#define ADC_SIZE    6

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 44/6   => 7 group(+2 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 46/6+1 => 8 group(+4 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2)   // 15 group 6


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 60/6 => 10 group(+0 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1)         // 10 group 0

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 15 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 10 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 25

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 6
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3,4,5
/*****************************************************************************/


#elif (MODEL_TYPE == WMC_0850_NXP_VE)
#define X_CELL_SIZE             144	//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             102          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //38        // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //38        //

#define CELLS_PER_CS            12  //10  //9 // number of PDs per one chip-select
#define ENABLED_CS_NUM          1             // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       12  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      6   //8       // see last PD board(Y(R2)) //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            4
#define X_LED_BD1_CELL_NUM      39  //34  //37
#define X_LED_BD2_CELL_NUM      33  //27  //30
#define X_LED_BD3_CELL_NUM      31  //35  //38
#define X_LED_BD4_CELL_NUM      41  //35  //38

#define Y_LED_BD_NUM            3
#define Y_LED_BD1_CELL_NUM      41  //41  //45
#define Y_LED_BD2_CELL_NUM      22  //37  //37
#define Y_LED_BD3_CELL_NUM      39  //37  //37

#define X_PD_BD_NUM             4
#define X_PD_BD1_CELL_NUM       40  //34  //36
#define X_PD_BD2_CELL_NUM       33  //28  //31
#define X_PD_BD3_CELL_NUM       31  //34  //38
#define X_PD_BD4_CELL_NUM       40  //34  //38

#define Y_PD_BD_NUM             3
#define Y_PD_BD1_CELL_NUM       46  //43  //450
#define Y_PD_BD2_CELL_NUM       19  //35  //37
#define Y_PD_BD3_CELL_NUM       37  //35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    12
#define ADC_SIZE    12

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 37/12 => 3 group(+7 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 23/10 => 3 group(+3 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS) + 1)    // 24/10 => 3 group(+4 pd)
#define X_PD_SHIFT_BD_4         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))        // 33/10 => 3 group(+3 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3 + X_PD_SHIFT_BD_4)   // 12 group 17


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 45/10 => 4 group(+5 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 45/10 => 5 group(+5 pd)
#define Y_PD_SHIFT_BD_3         (Y_PD_BD3_CELL_NUM / CELLS_PER_CS)    // 45/10 => 5 group(+5 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2 + Y_PD_SHIFT_BD_3)         // 9 group 10

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 12 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 9 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 21

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 6
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3,4,5
/*****************************************************************************/

#elif (MODEL_TYPE == SLIM_0750_NXP_VE)
#define X_CELL_SIZE             132	//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             92          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //38        // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //38        //

#define CELLS_PER_CS            12  //10  //9 // number of PDs per one chip-select
#define ENABLED_CS_NUM          1             // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       12  //9       // number of output of PD signals
#define ADC_GROUP_NUM           2   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      4   //8       // see last PD board(Y(R2)) //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            4
#define X_LED_BD1_CELL_NUM      41  //34  //37
#define X_LED_BD2_CELL_NUM      28  //27  //30
#define X_LED_BD3_CELL_NUM      25  //35  //38
#define X_LED_BD4_CELL_NUM      38  //35  //38

#define Y_LED_BD_NUM            2
#define Y_LED_BD1_CELL_NUM      47  //41  //45
#define Y_LED_BD2_CELL_NUM      45  //37  //37

#define X_PD_BD_NUM             4
#define X_PD_BD1_CELL_NUM       40  //34  //36
#define X_PD_BD2_CELL_NUM       27  //28  //31
#define X_PD_BD3_CELL_NUM       27  //34  //38
#define X_PD_BD4_CELL_NUM       38  //34  //38

#define Y_PD_BD_NUM             2
#define Y_PD_BD1_CELL_NUM       46  //43  //450
#define Y_PD_BD2_CELL_NUM       46  //35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    12
#define ADC_SIZE    12

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 37/12 => 3 group(+7 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 23/10 => 3 group(+3 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS) + 1)    // 24/10 => 3 group(+4 pd)
#define X_PD_SHIFT_BD_4         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))        // 33/10 => 3 group(+3 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3 + X_PD_SHIFT_BD_4)   // 12 group 17


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 45/10 => 4 group(+5 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 45/10 => 5 group(+5 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2)         // 9 group 10

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 12 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 9 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 21

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 6
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3,4,5
/*****************************************************************************/

#elif (MODEL_TYPE == WMC_0750_NXP_VE)
#define X_CELL_SIZE             129	//96     // No. of devices on X-Axis (Top or Bottom B'd)
#define Y_CELL_SIZE             90          // No. of devices on Y-Axis (Right or Left B'd)
#define X_MAX_OFFSET            MAX_X_SLOPE_VAL //38        // x tilt
#define Y_MAX_OFFSET            MAX_Y_SLOPE_VAL //38        //

#define CELLS_PER_CS            12  //10  //9 // number of PDs per one chip-select
#define ENABLED_CS_NUM          1             // number of enabled chip-select for PD
#define ADC_NUM                 2   //3       // number of ADCs to simultaneously read the PD signal level
#define PD_SIGNAL_OUT_NUM       12  //9       // number of output of PD signals
#define ADC_GROUP_NUM           6   //3       // PD_SIGNAL_OUT_NUM / ADC_NUM
#define NUM_PD_NOT_ALIGNED      6   //8       // see last PD board(Y(R2)) //nsmoon@210708, FIXME!!

#define X_LED_BD_NUM            4
#define X_LED_BD1_CELL_NUM      39  //34  //37
#define X_LED_BD2_CELL_NUM      28  //27  //30
#define X_LED_BD3_CELL_NUM      26  //35  //38
#define X_LED_BD4_CELL_NUM      36  //35  //38

#define Y_LED_BD_NUM            2
#define Y_LED_BD1_CELL_NUM      47  //41  //45
#define Y_LED_BD2_CELL_NUM      43  //37  //37

#define X_PD_BD_NUM             4
#define X_PD_BD1_CELL_NUM       35  //34  //36
#define X_PD_BD2_CELL_NUM       29  //28  //31
#define X_PD_BD3_CELL_NUM       30  //34  //38
#define X_PD_BD4_CELL_NUM       35  //34  //38

#define Y_PD_BD_NUM             2
#define Y_PD_BD1_CELL_NUM       47  //43  //450
#define Y_PD_BD2_CELL_NUM       43  //35  //37
//------------------------------------------------------------------------------

#define ADC_DMA_SIZE    12
#define ADC_SIZE    12

/*****************************************************************************/
//
/*****************************************************************************/

#define TOTAL_CELL_NUM          (X_CELL_SIZE + Y_CELL_SIZE)   // Total No. of devices (LED or PD)
#define X_TOTAL_OFFSET          (X_MAX_OFFSET * 2 + 1)
#define Y_TOTAL_OFFSET          (Y_MAX_OFFSET * 2 + 1)

/* TOTAL_X_PD_SHIFT_MAX */
#define X_PD_SHIFT_BD_1         (X_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 37/12 => 3 group(+7 pd)
#define X_PD_SHIFT_BD_2         ((X_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 23/10 => 3 group(+3 pd)
#define X_PD_SHIFT_BD_3         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS) + 1)    // 24/10 => 3 group(+4 pd)
#define X_PD_SHIFT_BD_4         ((X_PD_BD3_CELL_NUM / CELLS_PER_CS))        // 33/10 => 3 group(+3 pd)
#define TOTAL_X_PD_SHIFT_MAX    (X_PD_SHIFT_BD_1 + X_PD_SHIFT_BD_2 + X_PD_SHIFT_BD_3 + X_PD_SHIFT_BD_4)   // 12 group 17


/* TOTAL_Y_PD_SHIFT_MAX */
#define Y_PD_SHIFT_BD_1         (Y_PD_BD1_CELL_NUM / CELLS_PER_CS)          // 45/10 => 4 group(+5 pd)
#define Y_PD_SHIFT_BD_2         ((Y_PD_BD2_CELL_NUM / CELLS_PER_CS) + 1)    // 45/10 => 5 group(+5 pd)
#define TOTAL_Y_PD_SHIFT_MAX    (Y_PD_SHIFT_BD_1 + Y_PD_SHIFT_BD_2)         // 9 group 10

/* miss calculation */
#define X_PD_SHIFT_MAX          (TOTAL_X_PD_SHIFT_MAX)  // 12 group
#define Y_PD_SHIFT_MAX          (TOTAL_Y_PD_SHIFT_MAX)  // 9 group
#define TOTAL_PD_SHIFT_MAX      (X_PD_SHIFT_MAX + Y_PD_SHIFT_MAX + 1)   // 21

/* */
#define TOTAL_LED_BD_NUM        (X_LED_BD_NUM + Y_LED_BD_NUM)   // 6
#define LED_BD_INDEX_END        (TOTAL_LED_BD_NUM - 1)          // 0,1,2,3,4,5
/*****************************************************************************/


#else
#   error "MODEL_TYPE is not defined"
#endif



// Signal level
#define EXTL_LOW_SIG            64


/*****************************************************************************/
//Corner Cell Range Setting (for valid signal for light reflection)
/*****************************************************************************/
#define Y_CORNER_CELL_SIZE      12//8
#define X_CORNER_CELL_SIZE      12//8


/*****************************************************************************/
//Threshold ratio Setting (max 100) : Drop percent for check if blocked or not
/*****************************************************************************/
#define X_THRESHOLD_DROP_PERCENT                     50	//50
#define Y_THRESHOLD_DROP_PERCENT                     40	//50	//50
#define THRESHOLD_DROP_PERCENT_ENABLE              TRUE
#define THRESHOLD_DROP_PERCENT_SATURATION            40 //30 //nsmoon@220330

#define Y_CORNER_THRESHOLD_DROP_PERCENT              30//40	//50
#define X_CORNER_THRESHOLD_DROP_PERCENT              30	//50
/*****************************************************************************/




#endif /* TSP_LAYOUT_H_ */
