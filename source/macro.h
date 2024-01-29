#ifndef MACRO_H_
#define MACRO_H_

#include "gpio.h"
#include "delay.h"
#include "peripherals.h"
#include "MIMXRT1064.h"
#include "common.h"
#ifndef _nop
#define _nop()  __ASM volatile ("NOP")
#endif

#ifndef Nop
#define Nop()   _nop()
#endif

//=============================================================================
// fsys = 600 Mhz, Tsys = 1.666ns , Tsys * 12 = 19.992ns
// 1 nop -> 1.6ns , 2 nop -> 1.6ns , 3 nop -> 3.3ns , 4 nop -> 3.3ns
/* n_ns =  nop */
#if 1
#define M_NOP_Delay_10nsec()    {__asm("NOP"); __asm("NOP");__asm("NOP");\
                                  __asm("NOP");__asm("NOP");__asm("NOP");\
                                  __asm("NOP");__asm("NOP");__asm("NOP");\
                                  __asm("NOP");__asm("NOP");__asm("NOP");\
                                }
#else
#define M_NOP_Delay_10nsec()	{\
  volatile uint32_t diff = 1;\
    do { \
    }  while(diff--); \
 }
#endif




#define M_NOP_Delay_20nsec()   {    \
              M_NOP_Delay_10nsec();   \
              M_NOP_Delay_10nsec();   \
        }

#define M_NOP_Delay_40nsec()   {    \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
        }
#define M_NOP_Delay_60nsec()   {    \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
        }
#define M_NOP_Delay_80nsec()   {    \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
        }
#define M_NOP_Delay_100nsec()   {   \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
            M_NOP_Delay_20nsec();   \
        }

#define M_NOP_Delay_200nsec()   {   \
            M_NOP_Delay_100nsec();  \
            M_NOP_Delay_100nsec();  \
        }

#define M_NOP_Delay_300nsec()   {   \
            M_NOP_Delay_100nsec();  \
            M_NOP_Delay_200nsec();  \
        }
#define M_NOP_Delay_400nsec()   {   \
            M_NOP_Delay_100nsec();  \
            M_NOP_Delay_300nsec();  \
        }

#define M_NOP_Delay_500nsec()   {   \
            M_NOP_Delay_200nsec();  \
            M_NOP_Delay_300nsec();  \
        }

#define M_NOP_Delay_600nsec()   {   \
            M_NOP_Delay_300nsec();  \
            M_NOP_Delay_300nsec();  \
        }

#define M_NOP_Delay_700nsec()   {   \
            M_NOP_Delay_200nsec();  \
            M_NOP_Delay_500nsec();  \
        }

#define M_NOP_Delay_800nsec()   {   \
            M_NOP_Delay_300nsec();  \
            M_NOP_Delay_500nsec();  \
        }

#define M_NOP_Delay_1usec()   {   \
            M_NOP_Delay_500nsec();  \
            M_NOP_Delay_500nsec();  \
        }

#define M_NOP_Delay_1p3usec()   {   \
            M_NOP_Delay_600nsec();  \
            M_NOP_Delay_700nsec();  \
        }

#define M_NOP_Delay_2usec()   {   \
            M_NOP_Delay_1p3usec();  \
            M_NOP_Delay_700nsec();  \
        }
#define M_NOP_Delay_10usec()   {   \
           Ctrl_Delay(749);\
        }
#define M_NOP_Delay_20usec()   {   \
           Ctrl_Delay(1498);\
        }
#define M_NOP_Delay_200usec()   {   \
           Ctrl_Delay(14980);\
        }
//=============================================================================
// Core Timer Delay or Time-out
//=============================================================================
#define SYSTEM_CLOCK_FREQ (600000000) // 600 Mhz

#define CORE_TIMER_FREQUENCY    (SYSTEM_CLOCK_FREQ/2)

#define CORE_TIMER_CNT_10nsec   (CORE_TIMER_FREQUENCY / 50000000UL) // 9.6ns -> 1.6ns * 6
#define CORE_TIMER_CNT_20nsec   (CORE_TIMER_FREQUENCY / 25000000UL) // 19.2ns -> 1.6ns * 12
#define CORE_TIMER_CNT_30nsec   (CORE_TIMER_FREQUENCY / 16000000UL) // 30ns -> 1.6ns * 18.75
#define CORE_TIMER_CNT_40nsec   (CORE_TIMER_FREQUENCY / 12000000UL) // 40ns -> 1.6ns * 25
#define CORE_TIMER_CNT_50nsec   (CORE_TIMER_FREQUENCY / 9600000UL) // 50ns -> 1.6ns * 31.25

extern volatile uint32_t coreTimer_timeout;
extern volatile uint32_t coreTimer_start_cnt, coreTimer_diff;

#define M_Set_SysTimer_TimeOut(timeVal) { \
           /* coreTimer_timeout = (timeVal)*CORE_TIMER_CNT_20nsec;*/\
			coreTimer_timeout = (timeVal)*CORE_TIMER_CNT_10nsec;\
            coreTimer_start_cnt = DWT->CYCCNT; \
        }

#define M_Wait_SysTimer_TimeOut() { \
  volatile uint32_t diff;\
    do { \
                 coreTimer_diff= diff = DWT->CYCCNT - coreTimer_start_cnt; \
    }  while(diff < (coreTimer_timeout)); \
        }

#define CORETIMER_DelayUs(delay_us){ \
  volatile uint32_t startCount, endCount;\
		endCount = (SYSTEM_CLOCK_FREQ/1000000)*delay_us; \
        startCount =  DWT->CYCCNT; \
	while(( DWT->CYCCNT - startCount ) < endCount); \
		}

#define M_Set_LedOn_Timer(timeVal)              M_Set_SysTimer_TimeOut(timeVal)
#define M_Wait_LedOn_Timer_Expired()            M_Wait_SysTimer_TimeOut()

#define M_Set_LedOff_Timer(timeVal)             M_Set_SysTimer_TimeOut(timeVal)
#define M_Wait_LedOff_Timer_Expired()           M_Wait_SysTimer_TimeOut()




#if ENABLE_TRACE_SCAN_AXIS //nsmoon@220113
extern uint8_t gPdSftClkCnt;
extern uint8_t gLedSftClkCnt;

#define M_Pd_Shift_Clk_Cnt    (0)   gPdSftClkCnt
#define M_Pd_Shift_Clk_Cnt_Set(a)   gPdSftClkCnt=(a)
#define M_Pd_Shift_Clk_Cnt_Add(a)   gPdSftClkCnt+=(a)
#define M_Led_Shift_Clk_Cnt    (0)  gLedSftClkCnt
#define M_Led_Shift_Clk_Cnt_Set(a)  gLedSftClkCnt=(a)
#define M_Led_Shift_Clk_Cnt_Add(a)  gLedSftClkCnt+=(a)
#define M_Led_Shift_Clk_Set(a)
#else
#define M_Pd_Shift_Clk_Cnt_Set(a)
#define M_Pd_Shift_Clk_Cnt_Add(a)
#define M_Led_Shift_Clk_Cnt_Set(a)
#define M_Led_Shift_Clk_Cnt_Add(a)
#define M_Led_Shift_Clk_Set(a)
#endif



/* LED_CELL Enable/Disable */
#define M_LED_GROUP_Set_High()      (MCU_LED_GROUP_EN_Set())
#define M_LED_GROUP_Clear_Low()     (MCU_LED_GROUP_EN_Clear())
#if 0
#define M_LED_CELL_Disable_High()   (MCU_LED_CELL_EN_Set())
#define M_LED_CELL_Enable_Low()     (MCU_LED_CELL_EN_Clear())
#endif

/* LED_HGROUP_SHIFT_DATA */
#define M_LED_HGROUP_SHIFT_DATA_High()     (MCU_LED_HGROUP_DATA_Set())
#define M_LED_HGROUP_SHIFT_DATA_Low()      (MCU_LED_HGROUP_DATA_Clear())
#define M_LED_HGROUP_SHIFT_RESET_High()    (MCU_LED_GROUP_EN_Set())
#define M_LED_HGROUP_SHIFT_RESET_Low()     (MCU_LED_GROUP_EN_Clear())


#define M_LED_HGROUP_SHIFT_CLK_High()      { \
  MCU_LED_GROUP_CLK_Set(); \
     M_Led_Shift_Clk_Set(1); \
}
#define M_LED_HGROUP_SHIFT_CLK_Low()     { \
  MCU_LED_GROUP_CLK_Clear(); \
     M_Led_Shift_Clk_Set(0);  \
}

// M_LED_HGROUP_SHIFT_CLK_OnePulse -> 430ns
#define M_LED_HGROUP_SHIFT_CLK_OnePulse()   {       \
			M_NOP_Delay_200nsec();                  \
            M_LED_HGROUP_SHIFT_CLK_High();          \
            M_NOP_Delay_200nsec();                  \
            M_LED_HGROUP_SHIFT_CLK_Low();           \
        }


// M_LED_HGROUP_SHIFT_CLK_GenPulse ->1 num : 430 ns
#define M_LED_HGROUP_SHIFT_CLK_GenPulse(num) {      \
            register int cnt = num;                 \
            while(cnt--) {                          \
                M_LED_HGROUP_SHIFT_CLK_OnePulse();  \
            }                                       \
            M_Led_Shift_Clk_Cnt_Add(num); /*nsmoon@210906*/ \
        }

//M_Disable_All_LED_Board -> 120ns
#define  M_Disable_All_LED_Board() {                 \
            /*M_LED_CELL_Disable_High();*/              \
            M_LED_GROUP_Clear_Low();             \
            M_NOP_Delay_100nsec();                  \
            M_Led_Shift_Clk_Cnt_Set(0); /*nsmoon@210906*/ \
        }

#define M_LED_CTRL_WARMUP() { \
	{ \
	    int cnt = 3; \
	    while (cnt--) { \
	    	M_LED_HGROUP_SHIFT_DATA_High();         \
	     	M_LED_HGROUP_SHIFT_CLK_OnePulse();\
	    	M_LED_HGROUP_SHIFT_DATA_Low(); \
	    } \
	} \
}

// M_First_LED_Board_Select -> 530ns
#define M_First_LED_Board_Select() {                \
		/*M_LED_CTRL_WARMUP() */\
            M_Disable_All_LED_Board();                \
            M_LED_GROUP_Set_High();                \
            M_LED_HGROUP_SHIFT_DATA_High();         \
            M_TSPM_LED_Group_Clk_Set(1);      \
            M_LED_HGROUP_SHIFT_DATA_Low();          \
            M_Led_Shift_Clk_Cnt_Set(1); /*nsmoon@210906*/ \
        }

#define SINK_DAC_MASK_BIT     0x00FF0000   // 0000 0000 1111 1111 0000 0000 0000 0000
#define M_DAC_DATA_SET(dacval)  (GPIO2->DR ^= (GPIO2->DR ^ (dacval<<16)) & SINK_DAC_MASK_BIT);

//==========================================================LED CELL CTRL DEFINE

#define LED_CELL_CTRL_MASK_BIT     0x0E   // 0000 1110
#define M_LED_CELL_CTRL_SET(cellNo)  (GPIO2->DR ^= (GPIO2->DR ^ cellNo<<1) & LED_CELL_CTRL_MASK_BIT);    // 76 -> 126 ns

#if 0
//M_LED_CELL_CTRL_SET -> 30ns
#define M_LED_CELL_CTRL_SET(cellno) { \
          GPIO2->DR_CLEAR = 0x0E; \
          if(cellno == 1){ \
            GPIO2->DR_SET = 0x02; \
          }else if(cellno == 2){ \
            GPIO2->DR_SET = 0x04; \
          }else if(cellno == 3){  \
            GPIO2->DR_SET = 0x06; \
          }else if(cellno == 4){  \
            GPIO2->DR_SET = 0x08; \
          }else if(cellno == 5){ \
            GPIO2->DR_SET = 0x0A; \
          }else{ \
            GPIO2->DR_CLEAR = 0x0E; \
          }\
}
#endif

//==========================================================ADC MUX DEFINE



#if 1
#if 1
#define M_ADC_MUX_DATA_SET(channel){  \
    GPIO1->DR_CLEAR = 0x00380000;\
    if(channel==1){ \
      GPIO1->DR_SET = 0x00080000;\
      }else if(channel == 2){ \
        GPIO1->DR_SET = 0x00100000;\
      }else if(channel == 3){ \
        GPIO1->DR_SET = 0x00180000;\
      }else if(channel == 4){ \
        GPIO1->DR_SET = 0x00200000; \
      }else if(channel == 5){  \
        GPIO1->DR_SET = 0x00280000;\
      }else{  \
        GPIO1->DR_CLEAR = 0x00380000;\
      } \
}
#else
	#define ADC_CHANNEL_MUX_MASK_BIT     0x00380000   // 0000 0000 0011 1000 0000 0000 0000 0000
	#define M_ADC_MUX_DATA_SET(channel)  (GPIO1->DR ^= (GPIO1->DR ^ (channel<<19)) & ADC_CHANNEL_MUX_MASK_BIT); // 100ns
#endif
#else

//#define ADC_CHANNEL_MUX_MASK_BIT     0x01300000   // 0000 0001 0011 0000 0000 0000 0000 0000
//#define M_ADC_MUX_DATA_SET(channel)  (GPIO1->DR ^= (GPIO1->DR ^ (channel<<19)) & ADC_CHANNEL_MUX_MASK_BIT); // 100ns

#define M_ADC_MUX_DATA_SET(channel){  \
    GPIO1->DR_CLEAR = 0x01300000;\
    if(channel==1){ \
      GPIO1->DR_SET = 0x01000000;\
      }else if(channel == 2){ \
        GPIO1->DR_SET = 0x00100000;\
      }else if(channel == 3){ \
        GPIO1->DR_SET = 0x01100000;\
      }else if(channel == 4){ \
        GPIO1->DR_SET = 0x00200000; \
      }else if(channel == 7){  \
        GPIO1->DR_SET = 0x01300000;\
      }else{  \
        GPIO1->DR_CLEAR = 0x01300000;\
      } \
}
#endif


//==========================================================PD GAIN CTRL DEFINE

//#define PD_GAIN_CTRL_MASK_BIT     0xC0000000  // 1100 0000 0000 0000 0000 0000 0000 0000
//#define M_PD_GAIN_CTRL_SET(cellNo)   (GPIO1->DR ^= (GPIO1->DR ^ (cellNo<<30)) & PD_GAIN_CTRL_MASK_BIT);

#define M_PD_GAIN_CTRL_SET(cellno) { \
          GPIO1->DR_CLEAR = 0xC0000000; \
          if(!cellno){ \
            GPIO1->DR_CLEAR = 0xC0000000; \
          }else if(cellno == 1){ \
            GPIO1->DR_SET = 0x40000000; \
          }else if(cellno == 2){ \
            GPIO1->DR_SET = 0x80000000; \
          }else{  \
            GPIO1->DR_SET = 0xC0000000; \
          } \
}

//==============================================================================
#define M_PD_SHIFT_DATA_High()      (MCU_PD_SHIFT_DATA_Set())
#define M_PD_SHIFT_DATA_Low()       (MCU_PD_SHIFT_DATA_Clear())
#define M_PD_SHIFT_CLK_High()       (MCU_PD_SHIFT_CLK_Set())
#define M_PD_SHIFT_CLK_Low()        (MCU_PD_SHIFT_CLK_Clear())    // Initial state
#define M_PD_SHIFT_DELAY_IO		Ctrl_Delay(15);		//10 = 200ns
//#define M_PD_SHIFT_DELAY_IO   M_NOP_Delay_200nsec();

// M_PD_SHIFT_CLK_OnePulse -> 610ns
#define M_PD_SHIFT_CLK_OnePulse() {  \
    M_PD_SHIFT_CLK_Low(); \
    M_PD_SHIFT_DELAY_IO \
    M_PD_SHIFT_CLK_High(); \
    M_PD_SHIFT_DELAY_IO\
    }

// M_PD_SHIFT_CLK_OnePulse_wodelay -> 310ns
#define M_PD_SHIFT_CLK_OnePulse_wodelay() {  \
    M_PD_SHIFT_CLK_Low(); \
    M_PD_SHIFT_DELAY_IO \
    M_PD_SHIFT_CLK_High(); \
    }


#define M_PD_SHIFT_CLK_GenPulse(num) {      \
            register int cnt = num;         \
            while(cnt-- > 1) {                  \
                M_PD_SHIFT_CLK_OnePulse();  \
            }                               \
            M_PD_SHIFT_CLK_OnePulse_wodelay(); \
            M_Pd_Shift_Clk_Cnt_Add(num); /*nsmoon@210906*/ \
        }

#define M_Disable_All_Pd(pulseCnt) { \
            M_PD_SHIFT_DATA_Low();          \
            while ((pulseCnt)--) {            \
                M_PD_SHIFT_CLK_OnePulse();  \
            }                               \
            M_Pd_Shift_Clk_Cnt_Set(0);      \
}

// M_First_PD_Gr_Select -> 628ns
#define M_First_PD_Gr_Select() {                    \
            /*M_Disable_All_Pd(); */                    \
            M_PD_SHIFT_DATA_High();                 \
            M_PD_SHIFT_CLK_GenPulse(ENABLED_CS_NUM);\
            M_NOP_Delay_300nsec(); \
            M_PD_SHIFT_DATA_Low();                  \
        }

#define Coupling_Sig_TSPM()         BOARD_TMR1_PERIPHERAL->CHANNEL[BOARD_TMR1_CHANNEL_0_CHANNEL].CTRL = 0x30E4;
#define MCU_LED_CELL_EN_TSPM()      BOARD_TMR2_PERIPHERAL->CHANNEL[BOARD_TMR2_CHANNEL_3_CHANNEL].CTRL = 0x30E4;
#define MCU_AN_SW_EN1_TSPM()        BOARD_TMR3_PERIPHERAL->CHANNEL[BOARD_TMR3_CHANNEL_0_CHANNEL].CTRL = 0x30E4;
#define MCU_LED_GROUP_CLK_TSPM()    BOARD_TMR4_PERIPHERAL->CHANNEL[BOARD_TMR4_CHANNEL_3_CHANNEL].CTRL = 0x30E4;

#ifdef TIMER2_CH3_LAST
#define M_TSPM_Triger_Set(){\
    MCU_AN_SW_EN1_TSPM();\
    Coupling_Sig_TSPM();\
    MCU_LED_CELL_EN_TSPM();\
}
#else
#define M_TSPM_Triger_Set(){\
    MCU_AN_SW_EN1_TSPM();\
    MCU_LED_CELL_EN_TSPM();\
    Coupling_Sig_TSPM();\
}

#define M_TSPM_Triger_Set_first(){\
    MCU_AN_SW_EN1_TSPM();\
    Coupling_Sig_TSPM();\
}
#endif
#if 0
#define M_TSPM_LED_Group_Clk_Set(val){\
    register int cnt = val;         \
    if(val > 0) {					\
    while(cnt--) {                          \
      MCU_LED_GROUP_CLK_TSPM();  		\
      M_NOP_Delay_500nsec(); }}			\
	else { MCU_LED_GROUP_CLK_TSPM();} 	\
                                     	 \
}
#else
#if 1
#define M_TSPM_LED_Group_Clk_Set(val){\
    register int cnt = val;         \
    while(cnt--) {  \
      MCU_LED_GROUP_CLK_TSPM();     \
      M_TSPM_LED_Group_Clk_Wait();   \
       } \
}
#else
#define M_TSPM_LED_Group_Clk_Set(val){\
    register int cnt = val;         \
    while(cnt--) { 	\
      MCU_LED_GROUP_CLK_TSPM();  		\
      M_NOP_Delay_200nsec();   \
       } \
}
#endif
#endif
#define M_TSPM_Triger_Wait(){\
       while((BOARD_TMR3_PERIPHERAL->CHANNEL[BOARD_TMR3_CHANNEL_0_CHANNEL].CTRL & 0xe000) >0){\
       }\
}

#define M_TSPM_LED_Group_Clk_Wait(){\
       while((BOARD_TMR4_PERIPHERAL->CHANNEL[BOARD_TMR4_CHANNEL_3_CHANNEL].CTRL & 0xe000) >0){\
       }\
}


#if 0 //Not Used

#if (MODEL_TYPE == CTSI_0650_NXP)||(MODEL_TYPE == WMC_0650_NXP)||(MODEL_TYPE == WMC_0650_VE_NXP)||(MODEL_TYPE == SLIM_0320_NXP)
//=============================================================================
// QTMR COM1, COM2 Value -> Used : QTMR_Init
// Operating at 150Mhz
// ex) clocks/sec = 150,000,000 / 1,000,000 = 150 => 1us
// 100ns -> 15
//=============================================================================

#define TMR_100NS                    12-1   //  15-1
#define TMR_200NS                    28-1   //  30-1
#define TMR_400NS					 60-1
#define TMR_600NS                    80-1
#define TMR_800NS                   110-1
#define TMR_900NS					125-1
#define TMR_1US                     150-1
#define TMR_1_2US                   180-1
#define TMR_1_3US                   195-1
#define TMR_1_4US                   220-1
#define TMR_1_5US                   335-1
#define TMR_1_6US                   250-1
#define TMR_1_8US                   280-1
#define TMR_2US                     300-1
#define TMR_2_1US                   315-1
#define TMR_2_2US                   330-1
#define TMR_2_3US                   345-1

#if 1
#define MCU_AN_SW_EN1_COMP1_VALUE       TMR_1_8US   //SW Low Time
#define MCU_AN_SW_EN1_COMP2_VALUE       TMR_200NS   //SW HIgh Time

#define MCU_LED_GROUP_CLK_COMP1_VALUE   TMR_100NS
#define MCU_LED_GROUP_CLK_COMP2_VALUE   TMR_100NS

#define MCU_COUPLING_SIG1_COMP1_VALUE   TMR_1US     //COUPLING High Time
#define MCU_COUPLING_SIG1_COMP2_VALUE   TMR_1US     //COUPLING LOW Time

#define MCU_LED_CELL_EN_COMP1_VALUE     TMR_1US     //LED OFF Time
#define MCU_LED_CELL_EN_COMP2_VALUE     TMR_1US     //LED ON Time
#else
#define MCU_AN_SW_EN1_COMP1_VALUE       TMR_1_4US   //SW Low Time
#define MCU_AN_SW_EN1_COMP2_VALUE       TMR_200NS   //SW HIgh Time

#define MCU_LED_GROUP_CLK_COMP1_VALUE   TMR_100NS
#define MCU_LED_GROUP_CLK_COMP2_VALUE   TMR_100NS

#define MCU_COUPLING_SIG1_COMP1_VALUE   TMR_1US     //COUPLING High Time
#define MCU_COUPLING_SIG1_COMP2_VALUE   TMR_800NS     //COUPLING LOW Time

#define MCU_LED_CELL_EN_COMP1_VALUE     TMR_1US     //LED OFF Time
#define MCU_LED_CELL_EN_COMP2_VALUE     TMR_800NS     //LED ON Time
#endif
#elif (MODEL_TYPE == WMC_0750_NXP)||(MODEL_TYPE == WMC_0850_NXP)||(MODEL_TYPE ==  WMC_0850_NXP_VE)
//=============================================================================
// QTMR COM1, COM2 Value -> Used : QTMR_Init
// Operating at 150Mhz
// ex) clocks/sec = 150,000,000 / 1,000,000 = 150 => 1us
// 100ns -> 15
//=============================================================================

#define TMR_100NS                    12-1   //  15-1
#define TMR_200NS                    28-1   //  30-1
#define TMR_300NS                    45-1
#define TMR_400NS					 60-1
#define TMR_600NS                    80-1
#define TMR_800NS                   110-1
#define TMR_900NS					125-1
#define TMR_1US                     150-1
#define TMR_1_1US                   165-1
#define TMR_1_2US                   180-1
#define TMR_1_3US                   195-1
#define TMR_1_4US                   220-1
#define TMR_1_5US                   335-1
#define TMR_1_6US                   250-1
#define TMR_1_7US                   265-1
#define TMR_1_8US                   280-1
#define TMR_1_9US                   295-1
#define TMR_2US                     310-1
#define TMR_2_1US                   315-1
#define TMR_2_2US                   330-1
#define TMR_2_3US                   345-1

#define TMR_3_9US                   635-1

#if 0
#define MCU_AN_SW_EN1_COMP1_VALUE       TMR_2_3US//TMR_1_8US   //SW Low Time
#define MCU_AN_SW_EN1_COMP2_VALUE       TMR_100NS   //SW HIgh Time

#define MCU_LED_GROUP_CLK_COMP1_VALUE   TMR_100NS
#define MCU_LED_GROUP_CLK_COMP2_VALUE   TMR_100NS

#define MCU_COUPLING_SIG1_COMP1_VALUE   TMR_1_3US     //COUPLING High Time
#define MCU_COUPLING_SIG1_COMP2_VALUE   TMR_1US     //COUPLING LOW Time

#define MCU_LED_CELL_EN_COMP1_VALUE     TMR_1_3US     //LED OFF Time
#define MCU_LED_CELL_EN_COMP2_VALUE     TMR_1US     //LED ON Time

#else
#define MCU_AN_SW_EN1_COMP1_VALUE       TMR_1_8US//TMR_1_8US   //SW Low Time
#define MCU_AN_SW_EN1_COMP2_VALUE       TMR_200NS   //SW HIgh Time

#define MCU_LED_GROUP_CLK_COMP1_VALUE   TMR_100NS
#define MCU_LED_GROUP_CLK_COMP2_VALUE   TMR_100NS

#define MCU_COUPLING_SIG1_COMP1_VALUE   TMR_1US     //COUPLING High Time
#define MCU_COUPLING_SIG1_COMP2_VALUE   TMR_1US     //COUPLING LOW Time

#define MCU_LED_CELL_EN_COMP1_VALUE     TMR_1US     //LED OFF Time
#define MCU_LED_CELL_EN_COMP2_VALUE     TMR_1US     //LED ON Time
#endif
#endif


//=============================================================================
// PD Signal OP Amp control
#define M_1stOpAmp_Coupling_SIG1_High()     (MCU_COUPLING_SIG1_Set())
#define M_1stOpAmp_Coupling_SIG1_Low()      (MCU_COUPLING_SIG1_Clear())

#define M_2ndOpAmp_SW_EN1_Off()             (MCU_AN_SW_EN1_Clear())
#define M_2ndOpAmp_SW_EN1_On()              (MCU_AN_SW_EN1_Set())
#endif
#endif /* MACRO_H_ */
