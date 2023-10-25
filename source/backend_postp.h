/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#ifndef _BACKEND_POSTP_H    /* Guard against multiple inclusion */
#define _BACKEND_POSTP_H
#include "backend.h"
#include "bezier.h"
/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef SHOW_DEBUG_SIZE
extern int s_debug_minX, s_debug_maxX;
extern int s_debug_minY, s_debug_maxY;
extern unsigned int s_debug_minXY, s_debug_maxXY;
extern int s_debug_minX2, s_debug_maxX2;
extern int s_debug_minY2, s_debug_maxY2;
extern int s_debug_cnt_total;
extern int s_debug_cnt_true;
extern int s_debug_cnt_false;
#endif
#ifdef SHOW_DEBUG_TRACKING
extern uint32_t s_debug_frameperiod;
extern int32_t s_debug_diff_x, s_debug_diff_y;
#endif
#ifdef SHOW_DEBUG_SIZE_VAR
extern int s_debug_down_skip_cnt;
extern int s_debug_up_skip_cnt;
extern float s_debug_sizeSq, s_debug_sizeX, s_debug_sizeY;
extern float s_debug_sizeSqCur, s_debug_sizeXcur, s_debug_sizeYcur;
extern float s_debug_posXcur, s_debug_posYcur;
extern int s_debug_size_type, s_debug_curCnt, s_debug_curStat;
#endif
#if 0 //def SHOW_DEBUG_SIZE_DOWN //for test
extern float s_debug_sizeSq, s_debug_sizeX, s_debug_sizeY;
extern float s_debug_sizeSqCur, s_debug_sizeXcur, s_debug_sizeYcur;
extern float s_debug_posXcur, s_debug_posYcur;
extern int s_debug_size_type, s_debug_curCnt, s_debug_curStat;
extern float s_debug_sizeSqMax, s_debug_sizeSqMin;
extern float s_debug_sizeXMax, s_debug_sizeXMin;
extern float s_debug_sizeYMax, s_debug_sizeYMin;
extern float s_th50RatioX, s_th50RatioY;
extern float s_sizeMin;
extern int s_th10CntMin;
extern float s_sizeMulMin;
extern float s_sizeMulMin_min;
extern float s_sizeMulMin_max;
extern float s_debug_sizeSq_initial, s_debug_sizeSq_initial_max;
extern int s_debug_th50X, s_debug_th50Y, s_debug_th10X, s_debug_th10Y;
extern int s_debug_th50X_used, s_debug_th50Y_used, s_debug_th10X_used, s_debug_th10Y_used;
extern int s_debug_insideArea;
extern float s_debug_th10sizeXcur, s_debug_th10sizeYcur;
extern float s_debug_th10sizeXMin, s_debug_th10sizeXMax;
extern float s_debug_th10sizeYMin, s_debug_th10sizeYMax;
extern float s_debug_sizeWeight;
#endif
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
extern float s_debug_posXcur, s_debug_posYcur;
extern int s_debug_th50X, s_debug_th10X, s_debug_th50Y, s_debug_th10Y;
extern int s_debug_th50X_used, s_debug_th10X_used, s_debug_th50Y_used, s_debug_th10Y_used;
extern float s_debug_sizeXcur, s_debug_th10sizeXcur, s_debug_sizeYcur, s_debug_th10sizeYcur;
extern float s_debug_sizeXMin, s_debug_sizeXMax, s_debug_sizeYMin, s_debug_sizeYMax;
extern float s_debug_th10sizeXMin, s_debug_th10sizeXMax, s_debug_th10sizeYMin, s_debug_th10sizeYMax;
extern float s_debug_X10Y50, s_debug_X50Y10;
extern int s_debug_size_type, s_debug_edge;
#endif

///////////////////////////////////////////////
// Tunning Parameters
///////////////////////////////////////////////
#define SMOOTHING_FILTER_ENABLE     //filter enable
//#define CHECK_INITIAL_MOVEMENT //nsmoon@191220
#ifdef BZ_FILTER_USE
#define A0_BZ_OUT_FINE				0.7f
#define A0_ERASE_OUT_FINE			0.9f
#define A0_OUT_FINE					0.35f //0.35f//1.0f//0.3f
#else
#define A0_OUT_FINE					0.35f//1.0f//0.3f
#define A0_ERASE_OUT_FINE			0.35f
#endif
#define A0_OUT_FINE_SIZE			0.5f
#define A0_OUT_FINE_ERASER			0.5f
#define A0_OUT_BRUSH_SIZE			0.5f
#define A1_OUT_BRUSH_SIZE			0.5f	//2nd Size Filter
//#define RELEASE_LIMIT_MFINGER		1
//#define START_LIMIT_MFINGER			2 //1
#define RELEASE_LIMIT_PEN			2 	//1
#define START_LIMIT_PEN				4	//2 //3 //2
#define START_LIMIT_PEN_SHORT		2 //3 //2
#define UNALLOC_LIMIT				5
#define PREDICTION_LIMIT			0

#define STAY_FRAME_LIMIT			10
#define START_FRAME_LIMIT_ERASER	6	//5	//10		//YJ1127

#define D2MIN_LIMIT_INITIAL			8000 //1500
#define D2MIN_LIMIT_PEN				80000 //15000
#define D2MIN_ACCEL_MFINGER			5000
#define ACCEL_LIMIT_PEN				3 //5
//#define ACCEL_LIMIT_MFINGER			20

#define SIZE_DEFINE_BORDER			//YJ@231011

#if (MODEL_SPT == MODEL_CTKS_750_V130) || (MODEL_SPT == MODEL_CTKS_750_V140)  || (MODEL_SPT == MODEL_CTSK_850_V100) || (MODEL_SPT ==MODEL_CTSK_650_V100) || (MODEL_SPT == MODEL_CTSK_N650_V100)|| (MODEL_SPT == MODEL_CTSK_550_V100) || (MODEL_SPT == MODEL_CTSK_N750_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100)//nsmoon@201103
#if 0
#define TOUCH_AREA_EDGE_TOP     (s_aarea_end_y - 85.0f)
#define TOUCH_AREA_EDGE_BOT     (s_aarea_zero_y + 85.0f)
#define TOUCH_AREA_EDGE_LFT     (s_aarea_end_x - 85.0f)
#define TOUCH_AREA_EDGE_RHT     (s_aarea_zero_x + 85.0f)
#endif
#define MIN_SIZE_ERASER         120.0f		//110.0f	YJ20200318
#define MIN_SIZE_B_ERASER       200.0f		//2500.0f == Real Size
#define ERASER_MID_SIZE			35.0f
#define ERASER_BIG_SIZE			50.0f
#define BIG_ERASER_ADJ			0.75f

///////////Size Pen_marker Mode////////////////////////////////////
#define MAX_SIZE_PEN_1          5.0f				//  < w_min  1
#define MAX_SIZE_PEN_1_EDGE     4.0f
#define MAX_SIZE_PEN_1_SIDE     4.0f

#if (MODEL_SPT == MODEL_CTSK_850_V100)
#define START_FRAME_LIMIT		7		//YJ@210323	//10 //2105	//4  //750 PV	//3	//4 R0004 YJ200428
#define START_FRAME_MULTI3		2
#define START_FRAME_ERASE		2

#define MAX_SIZE_PEN_2          16.0f	//12.0f	//8.0f	//10.0f	//8.0f R0004	//7.5f					//  < w_max  2
#define MAX_SIZE_MARKER_1       17.0f	//13.0f 	//10.0f//11.0f	//8.0f R0004	//7.5f					//  > w_min  4

#define MAX_SIZE_PEN_E2         8.0f	//12.5f			//  < w_max  2
#define MAX_SIZE_MARKER_E1      8.5f	//13.0f			//  > w_min  4

#define MAX_SIZE_MARKER_2       25.0f	//16.0f	//14.0f R0004				//  > w_max  31

#define MAX_SIZE_PEN_2_EDGE     16.0f	//12.0f	//14.0f	//16.0f	//17.0f	//11.0f R0004
#define MAX_SIZE_MARKER_1_EDGE  17.0f	//13.0f	//17.0f	//18.0f	//11.0f	R0004
#define MAX_SIZE_MARKER_2_EDGE  25.0f	//12.0f R0004

#define MAX_SIZE_PEN_2_SIDE     16.0f	//8.0f   //8.0f	//9.0f	//10.0f	//8.0f R0004
#define MAX_SIZE_MARKER_1_SIDE  17.0f	//9.0f	//11.0f	//8.0f R0004
#define MAX_SIZE_MARKER_2_SIDE  10.0f  //15.0f	//11.0f	R004
//75/85
#elif (MODEL_SPT == MODEL_CTKS_750_V140)
#define START_FRAME_LIMIT		7		//YJ@210323	//10 //2105	//4  //750 PV	//3	//4 R0004 YJ200428
#define START_FRAME_MULTI3		2
#define START_FRAME_ERASE		2

#define MAX_SIZE_PEN_2          14.0f //1204	//12.5f	//8.0f R0004	//7.5f					//  < w_max  2
#define MAX_SIZE_MARKER_1       15.0f //1204	//13.0f	//8.0f R0004	//7.5f					//  > w_min  4

#define MAX_SIZE_PEN_E2         7.0f	//12.5f			//  < w_max  2
#define MAX_SIZE_MARKER_E1      7.5f	//13.0f			//  > w_min  4

#define MAX_SIZE_MARKER_2       22.0f	//15.0f			//  > w_max  31

#define MAX_SIZE_PEN_2_EDGE     13.0f	//11.0f R0004
#define MAX_SIZE_MARKER_1_EDGE  14.0f	//11.0f	R0004
#define MAX_SIZE_MARKER_2_EDGE  15.0f	//12.0f R0004

#define MAX_SIZE_PEN_2_SIDE     11.0f	//8.0f R0004
#define MAX_SIZE_MARKER_1_SIDE  12.0f	//8.0f R0004
#define MAX_SIZE_MARKER_2_SIDE  13.0f	//11.0f	R004
#elif (MODEL_SPT == MODEL_CTSK_650_V100)||(MODEL_SPT == MODEL_CTSK_N650_V100)||(MODEL_SPT == MODEL_CTSK_N750_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100)
#define START_FRAME_LIMIT			7	//7	//10//7		//YJ@210323	//10 //2105	//4  //750 PV	//3	//4 R0004 YJ200428
#define START_FRAME_MULTI3			3	//5
#define START_FRAME_ERASE			3	//5
#define LIB_0418		1
#ifdef LIB_0418
#define MAX_SIZE_PEN_2          11.0f	//15.0f //11.0f	//14.0f //1204	//12.5f	//8.0f R0004	//7.5f					//  < w_max  2
#define MAX_SIZE_MARKER_1      	12.0f	//16.0f // 12.0f	//15.0f //1204	//13.0f	//8.0f R0004	//7.5f					//  > w_min  4

#define MAX_SIZE_PEN_E2         9.0f	//6.0f	//7.0f	//12.5f			//  < w_max  2
#define MAX_SIZE_MARKER_E1      9.5f	//7.0f	//7.5f	//13.0f			//  > w_min  4

#define MAX_SIZE_MARKER_2       24.0f	//15.0f			//  > w_max  31

#define MAX_SIZE_PEN_2_EDGE     11.0f	//11.0f R0004
#define MAX_SIZE_MARKER_1_EDGE  15.0f	//11.0f	R0004
#define MAX_SIZE_MARKER_2_EDGE  26.0f	//12.0f R0004

#define MAX_SIZE_PEN_2_SIDE     11.0f	//8.0f R0004
#define MAX_SIZE_MARKER_1_SIDE  15.0f	//8.0f R0004
#define MAX_SIZE_MARKER_2_SIDE  26.0f	//11.0f	R004
#ifdef MARKER_TOP_AAREA
#define MAX_SIZE_PEN_1_MTOP     2.5f
#define MAX_SIZE_PEN_2_MTOP     11.0f	//8.0f R0004
#define MAX_SIZE_MARKER_1_MTOP  15.0f	//8.0f R0004
#define MAX_SIZE_MARKER_2_MTOP  26.0f	//11.0f	R004
#define MAX_SIZE_ERASER_1_MTOP  95.0f				//80.0f	YJ20200318
#endif

#elif LIB_0411
#define MAX_SIZE_PEN_2          14.0f	//15.0f //11.0f	//14.0f //1204	//12.5f	//8.0f R0004	//7.5f					//  < w_max  2
#define MAX_SIZE_MARKER_1      	15.0f	//16.0f // 12.0f	//15.0f //1204	//13.0f	//8.0f R0004	//7.5f					//  > w_min  4

#define MAX_SIZE_PEN_E2         12.0f	//7.0f	//12.5f			//  < w_max  2
#define MAX_SIZE_MARKER_E1      13.0f	//7.5f	//13.0f			//  > w_min  4

#define MAX_SIZE_MARKER_2       24.0f	//15.0f			//  > w_max  31

#define MAX_SIZE_PEN_2_EDGE     13.0f	//11.0f R0004
#define MAX_SIZE_MARKER_1_EDGE  14.0f	//11.0f	R0004
#define MAX_SIZE_MARKER_2_EDGE  22.0f	//12.0f R0004

#define MAX_SIZE_PEN_2_SIDE     13.0f	//8.0f R0004
#define MAX_SIZE_MARKER_1_SIDE  14.0f	//8.0f R0004
#define MAX_SIZE_MARKER_2_SIDE  22.0f	//11.0f	R004
#endif

#elif (MODEL_SPT == MODEL_CTSK_550_V100)
#define START_FRAME_LIMIT			7		//YJ@210323	//10 //2105	//4  //750 PV	//3	//4 R0004 YJ200428
#define START_FRAME_MULTI3			5
#define START_FRAME_ERASE			5

#define MAX_SIZE_PEN_2          13.0f //1204	//12.5f	//8.0f R0004	//7.5f					//  < w_max  2
#define MAX_SIZE_MARKER_1       14.0f //1204	//13.0f	//8.0f R0004	//7.5f					//  > w_min  4

#define MAX_SIZE_PEN_E2         10.0f	//12.5f			//  < w_max  2
#define MAX_SIZE_MARKER_E1      12.0f	//13.0f			//  > w_min  4

#define MAX_SIZE_MARKER_2       22.0f	//15.0f			//  > w_max  31

#define MAX_SIZE_PEN_2_EDGE     18.0f	//11.0f R0004
#define MAX_SIZE_MARKER_1_EDGE  19.0f	//11.0f	R0004
#define MAX_SIZE_MARKER_2_EDGE  20.0f	//12.0f R0004

#define MAX_SIZE_PEN_2_SIDE     13.0f	//8.0f R0004
#define MAX_SIZE_MARKER_1_SIDE  14.0f	//8.0f R0004
#define MAX_SIZE_MARKER_2_SIDE  20.0f	//11.0f	R004
#else
#ERROR MODEL_SPT NOT DEFINED!
#endif


#define MAX_SIZE_ERASER_1       95.0f				//80.0f	YJ20200318
#define MAX_SIZE_ERASER_1_EDGE  95.0f				//80.0f	YJ20200318
#define MAX_SIZE_ERASER_1_SIDE  95.0f				//80.0f	YJ20200318

///////////end of Pen_marker Mode////////////////////////////////////

#define TOUCH_SIZESQ_ERASER     MIN_SIZE_ERASER
#define DOWN_THR10_50_RATIO		0.15f //0.5f
#define UP_THR10_50_RATIO       0.1f //0.1f
#define HOLD_THR10_50_RATIO     0.2f //0.2f
#define HOLD_TO_UP_COUNT        2
#endif

#define TOUCH_LARGE_ERASE_SIZE			900.0f //30*30
#define TOUCH_LARGE_ERASE_SIZE2			100.0f //10*10
#define TOUCH_LARGE_ERASE_SIZE3			5000.0f
#define TOUCH_FINGER_ERASE_SIZE			64.0f  //8*8
#define MULTI_MAX_LARGE_ERASE_GAP		20.0f //30.0f //40.0f  //80.0f
#define FINE_MULTI_MIN_LARGE_ERASE_DIST	20.0f
#define FINE_MULTI_MAX_LARGE_ERASE_DIST	300.0f
#define FINE_MULTI_ERASER_DIST          120.0f //120mm
#define FINE_MULTI_GROUP_DIST			250.0f
#define FINE_MULTI_GROUP_DIST_SQ		(FINE_MULTI_GROUP_DIST * FINE_MULTI_GROUP_DIST)

#define ALLOWABLE_TOUCH_DATA_IO		(ALLOWABLE_TOUCH_BE * 2)   //20 //s_touchPosDataIn[]
#define MAX_TOUCH_ID_ALLOCTBL		ALLOWABLE_TOUCH_DATA_IO //20 //0x14, s_allocIDTbl[]
#define DEBUG_TRACKING_TOUCH_ID     MAX_TOUCH_ID_ALLOCTBL

#ifndef LOBYTE_16
#define LOBYTE_16(x)  ((uint8_t)(x & 0x00FF))
#endif
#ifndef HIBYTE_16
#define HIBYTE_16(x)  ((uint8_t)((x & 0xFF00) >> 8))
#endif

//s_scroll_State
#define SCROLL_STATE_OFF		0
#define SCROLL_STATE_ON			1
#define SCROLL_STATE_DRAW		2
#define SCROLL_STATE_WAIT		3
#define SCROLL_STATE_ON_DELAY 	4

#define TOUCH_NO_STATE		0x00
#define TOUCH_START_STATE	0x11
#define TOUCH_DOWN_STATE	0x13
#define TOUCH_UP_STATE		0x10

//#define TOUCH_SIG_START		0x07
#define TOUCH_SIG_DOWN		0x07
#define TOUCH_SIG_UP		0x04

#if 1 //shj@191119
#define PRESSURE_RESOLUTION		2048

#define BRUSH_MAX_ID_NUM		1
#if (PRESSURE_RESOLUTION == 1024)
#define BRUSH_MAX_OBJECT_SIZE	350	//50mm
#define BRUSH_MAX_OBJECT_SIZE_F	(float)(BRUSH_MAX_OBJECT_SIZE / 10)
#define BRUSH_MIN_OBJECT_SIZE	10	//1mm
#define BRUSH_MIN_OBJECT_SIZE_F	(float)(BRUSH_MIN_OBJECT_SIZE / 10)
#define BRUSH_PRESSURE_MIN	27
#define BRUSH_PRESSURE_MAX	(PRESSURE_RESOLUTION-1)
#else
#define BRUSH_MAX_OFFSET_OBJECT_SIZE	60
#define BRUSH_MAX_OBJECT_SIZE	500	//50mm
#define BRUSH_MAX_OBJECT_SIZE_F	(float)((BRUSH_MAX_OBJECT_SIZE + BRUSH_MAX_OFFSET_OBJECT_SIZE) / 10)
#define BRUSH_MIN_OBJECT_SIZE	10	//1mm
#define BRUSH_MIN_OBJECT_SIZE_F	(float)(BRUSH_MIN_OBJECT_SIZE / 10)
#define BRUSH_PRESSURE_MIN	55
#define BRUSH_PRESSURE_MAX	(PRESSURE_RESOLUTION-1)
#endif

#define BRUSH_NORMAL_SIZE_FRAME_CNT	5
#define BRUSH_BIG_SIZE_FRAME_CNT	15
#define BRUSH_RELEASE_FRAME_CNT		5
#define BRUSH_NO_OBJECT_FRAME_CNT	3

#define BRUSH_RESIZE_OFFSET_DEFAULT	((float)1.0f)	//for display size
#define BRUSH_RESIZE_OFFSET1	((float)0.5f)	//for display size
#define BRUSH_RESIZE_OFFSET2	((float)0.7f)	//for display size

#define BRUSH_NORMAL_SIZE_OFFSET	((float)5.0f)	//for display size

#define BRUSH_ERASER_SIZE_OFFSET	((float)30.0f)	//for display size
#define BRUSH_ERASER_PRESSURE_START_VALUE	800
#define BRUSH_ERASER_PRESSURE_STEP_CNT	200
#define BURSH_ERASER_SIZE_MAX	255
#define BURSH_ERASER_SIZE_MIN	35


#define BRUSH_REAL_SIZE_STEP		(BRUSH_MAX_OBJECT_SIZE - BRUSH_MIN_OBJECT_SIZE)
#define BRUSH_VIRTUAL_SIZE_STEP		((BRUSH_REAL_SIZE_STEP * BRUSH_PRESSURE_MIN) / 10)

#define BRUSH_PRESSURE_RESCALE_VAL   (float)((float)(1 - ((float)PRESSURE_RESOLUTION / (float)BRUSH_VIRTUAL_SIZE_STEP)) / (float)BRUSH_REAL_SIZE_STEP)
#endif

#if 0
typedef struct {
	uint16_t xCord;
	uint16_t yCord;
} touch_pos_t;
#endif

typedef enum
{
    ENUM_SIZE_ONE = 0x01, //not-used
    ENUM_SIZE_PEN = 0x02,
    ENUM_SIZE_MARKER = 0x03,
    ENUM_SIZE_ERASER = 0x04,
	ENUM_SIZE_S_ERASER = 0x05,
    ENUM_SIZE_UNKNOWN = 0xFF
} size_type_t;

#define TOUCH_MAX_CNT			0xFF //uint8_t: pressedCnt, releaseCnt
#define TOUCH_ID_UNALLOC		0xFF //un-alloc
typedef struct {
	uint8_t contactID;
	uint8_t status;
	uint8_t pressedCnt;
	uint8_t releaseCnt;
	float xCord;
	float yCord;
	float xSize;
	float ySize;
	float distSquare;
	float xVel;
	float yVel;
	int8_t used;
    int8_t cxRef;
    size_type_t sizeType;
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
    uint8_t th50CntX;
    uint8_t th10CntX;
    uint8_t th50CntY;
    uint8_t th10CntY;
	float th10WidthX;
	float th10WidthY;
#endif
    int8_t multi_fine; //for debugging
#if	1	//KYJ20200228
  //  uint8_t penCnt;
  //  uint8_t markerCnt;
    uint16_t eraseSize;
#endif
} touchDataSt_t;

//for Brush
typedef enum {
	MOVE_NONE = 0,
	MOVE_X_POS = 1,
	MOVE_Y_POS = 2,
	MOVE_XY_POS = 3
} move_pos_status_t;

typedef enum {
	NORMAL_SIZE = 0,
	ERASER_SIZE	= 1
} brush_size_mode_status_t;

typedef enum {
	NORMAL_SIZE_PROCESS = 0,
	ERASER_SIZE_PROCESS	= 1
} brush_process_t;

#if 0
typedef struct {
    float distSq;
    float accelX, accelY;
} dist_sq_t;
#endif

#if 1 //shj@20200421 for window drawing (multi pen) & bursh
typedef enum {
	NOT_IN_USE_ID = 0x00,
	IN_USE_ID = 0x01,
	NEW_ID = 0x02,
	ID_STATUS_ERROR
}TP_ID_STATUS_T;

typedef struct {
	uint8_t *tp_ID_Status_buf;
	uint8_t *tp_ID_idx_buf;
	uint8_t *tp_Big_Size_ID_buf;
	//uint8_t in_use_ID;
} TP_ID_INFO_T;

typedef struct {
	uint32_t *data_stream_cnt;
	uint32_t tp_send_start_cnt;
} DEF_TOUCH_DATA_FRAME;
#endif

#ifdef HOR_EDGE_TOUCH_ENABLE
extern float s_touch_data_hor_edge_save;
extern int s_wheel_delta;
extern int s_cal_hor_scroll(vec_t edgeData_in);
#endif


#ifdef TOUCH_SCROLL_INFO_ENABLE //nsmoon@200611
#define START_SCROLL_NONE	0x00
#define START_SCROLL_TOP	0x01
#define START_SCROLL_BOT	0x02
#define START_NON_STATE     0x03

typedef struct {
    uint8_t out_of_scroll_area;
    uint8_t start_from_scroll_area;
    uint16_t reserved;
    uint32_t touch_elapsed_time;
} touch_scroll_info_t;
#endif
//////////////////////////////////////////////////////
// Exported Global variable prototypes
extern int s_prevTouchCnt, s_touchReportCnt;
extern int s_prevTouchCntPen;
extern DEF_TOUCH_OUT s_touch_data_out[ALLOWABLE_TOUCH_DATA_IO];
extern float s_ratioXforP2L, s_ratioYforP2L; // ratio for physical coordinates to logical coordinates
extern int s_touch_count;
extern vec_t *s_touch_data;
extern tp_size_t *s_touch_size;
extern tp_line_cnt_t *s_touch_lineCnt;
extern int8_t *s_multi_fine; //for debugging
extern uint8_t curBrushId;
//////////////////////////////////////////////////////
// Exported Global function prototypes
extern void s_coordinates_conversion(DEF_DATA_INFO *pOutBuf);
extern void s_init_post_processing(void);
extern void s_smooth_filter(void);
#ifdef SMOOTH_FILTER_AVG //for test
extern void s_smooth_filter_avg(void);
#endif
extern void s_touch_point_tracking(void);

extern void tp_init_post_processing(void);
extern void s_coordinates_conversion_brush(DEF_DATA_INFO *pOutBuf);
extern void s_coordinates_conversion_brush_report_delay(DEF_DATA_INFO *pOutBuf);
extern void s_smooth_filter_brush(void);
extern int s_smooth_filter_brush_report_delay(void);
extern void s_touch_point_tracking_brush(void);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BACKEND_POSTP_H */

/* *****************************************************************************
 End of File
 */
