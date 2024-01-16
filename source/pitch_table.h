/*
 * pitch_table.h
 *
 *  Created on: 2018. 11. 3.
 *      Author: shj9816
 */

#ifndef PITCH_TABLE_H_
#define PITCH_TABLE_H_


#include "TSP_Layout.h"
// Exported Global variable prototypes
extern const float sensor_zero_x__;
extern const float sensor_end_x__;
extern const float sensor_zero_y__;
extern const float sensor_end_y__;
extern const float aarea_zero_x__;
extern const float aarea_end_x__;
extern const float aarea_zero_y__;
extern const float aarea_end_y__;


extern const float sensor_XPD_Pos__[X_CELL_SIZE];
extern const float sensor_YPD_Pos__[Y_CELL_SIZE];

extern const float sensor_XLED_Pos__[X_CELL_SIZE];
extern const float sensor_YLED_Pos__[Y_CELL_SIZE];

#endif /* PITCH_TABLE_H_ */
