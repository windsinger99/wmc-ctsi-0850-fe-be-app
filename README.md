[240201] border 주석 처리, 입구 0.14 -> 0.15f 변경, pen/Marker size 변경     
WMB-CTSI-0850-VE-FE_BE-APP -> WMB-CTSO-0850-VE-FE_BE-APP 위치 변경     

[240126] add LED on time xy control       
- 별도로 튜닝 가능한 on off time 구현       
- config tool에 영향 X        
- 기능 on/off option 처리가 안되어 있음       
  
[240125] (feature/240125) 75VE 손바닥 터치 중 Pen/Marker size 튜닝 진행 중     
    
[240124] (feature/240124) 75VE size 튜닝     
- 하드웨어 변경 및 4pen pen/marker, 2pen 빠르게 드로잉 시 끊김현상으로 인해 수정    
 
backend.h       
#define D2MIN_LIMIT_INITIAL	2000->5000      
#define D2MIN_LIMIT_PEN		5000 -> 50000        

#define MAX_SIZE_PEN_2_EDGE     	10.0f ->11.0f     
#define MAX_SIZE_MARKER_1_EDGE  	11.0f ->11.5f      
#define MAX_SIZE_PEN_2_SIDE     	9.0f  -> 11.0f      
#define MAX_SIZE_MARKER_1_SIDE  	10.0f -> 11.5f     

[240122] (feature/240122) size 튜닝 변경점      
TSP_Layout.h    
#define Y_CORNER_CELL_SIZE      8 -> 12   
#define X_CORNER_CELL_SIZE      8 -> 12   

common.h    
partial = On    

backend_postp.h
#define D2MIN_LIMIT_INITIAL      8000 -> 2000      
#define D2MIN_LIMIT_PEN          80000 -> 5000     
#define MAX_SIZE_PEN_2_EDGE  	13.0f -> 10.0f     
#define MAX_SIZE_MARKER_1_EDGE 	14.0f -> 11.0f    
#define MAX_SIZE_PEN_2_SIDE     	11.0f -> 9.0f     
#define MAX_SIZE_MARKER_1_SIDE  	11.5f ->10.0f    

backend_postp.c    
#define THR10_50_RATIO_DOWN	0.17f -> 0.13f     
#define UP_CON_SIZE		1.6f -> 1.3f    
#define UP_CON_SIZE_EDGE		1.5f -> 1.4f     
#define UP_CON_SIZE_SIDE		1.5f -> 0.9f
#define UP_CON_SIZE_ERASE		1.8f -> 1.5f

on/off time 변경     
on Time(us) = 1.5    
off Time(us) = 3.0     

[240116] (feature/240117) bugfix pinmux     

add tr group mex file update    
.project rename    



[240116] (master) WMB_85VE feature/240106 start      

partial enable     
add MCU_TR_GROUP_SELECT_0 Control      

