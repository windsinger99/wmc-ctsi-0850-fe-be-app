#include "stdint.h"
#include "speed_test_float.h"
#include "backend.h"

#ifdef SPEED_TEST_FLOAT
#define SP_TEST
#ifdef SP_TEST
#define REAL float
#define TEST_CONST1  3.123f
#define TEST_CONST2  4.456f
#endif
#ifdef DP_TEST
#define REAL double
#define TEST_CONST1  3.123
#define TEST_CONST2  4.456
#endif
#ifdef IT_TEST
#define REAL uint32_t
#define TEST_CONST1  3123
#define TEST_CONST2  4456
#endif

static void vectorNop(REAL *a, REAL *b, REAL *c, int nsize) {
    volatile int j;
    for (j = nsize; j >0; j--) {
        a++; b++; c++;
    }
}

static void vectorSum(REAL *a, REAL *b, REAL *c, int nsize) {
    volatile int j;
    for (j = nsize; j >0; j--)
        *(c++) = *(a++) + *(b++);
}

static void vectorMultiply(REAL *a, REAL *b, REAL *c, int nsize) {
    volatile int j;
    for (j = nsize; j >0; j--)
        *(c++) = *(a++) * *(b++);
}

static void vectorDivide(REAL *a, REAL *b, REAL *c, int nsize) {
    volatile int j;
    for (j = nsize; j >0; j--)
        *(c++) = *(a++) / *(b++);
}

#define VECTOR_NOP 0
#define VECTOR_ADD 1
#define VECTOR_MULTIPLY 2
#define VECTOR_DIVIDE 3

static void vectorOps(REAL *a, REAL *b, REAL *c, int nsize, int niters, int nmode)
{
	volatile int j;

    switch (nmode) {
    case VECTOR_NOP:
        for (j = niters; j > 0; j--)
            vectorNop(a, b, c, nsize);
        break;

    case VECTOR_ADD:
        for (j = niters; j > 0; j--)
            vectorSum(a, b, c, nsize);
        break;

    case VECTOR_MULTIPLY:
        for (j = niters; j > 0; j--)
            vectorMultiply(a, b, c, nsize);
        break;

    case VECTOR_DIVIDE:
        for (j = niters; j > 0; j--)
            vectorDivide(a, b, c, nsize);
        break;
    }
}

extern uint32_t s_get_time_diff(void);

#if 0 //core clock test
#define LOOP_NOP_CNT    1000000
__attribute__((optimize(0)))
void executeSpeedTest(void)
{
    int i;
    DEBUG_LED_ON; //inverted!! CHECK!! 8.48ms
    s_get_time_diff();
    i = LOOP_NOP_CNT;
    while (i-- > 0) {
        __asm(" nop");
    }
    DEBUG_LED_OFF;
    uint32_t ru2 = s_get_time_diff();
    i = LOOP_NOP_CNT/2;
    while (i-- > 0) {
        __asm(" nop");
    }
    uint32_t ru3 = s_get_time_diff();
    TRACE_ERROR("(((%d-%d", ru2, ru3);
}
#else
#if defined(_WIN32) || defined(WIN32)
#define TEST_ARRAY_SIZE 1000
#define TIME_DIFF_USEC	1 //ms
#define TICKDEF     unsigned int //float
#define DEBUG_LED_ON
#define DEBUG_LED_OFF
#define TRACE_STF(...) TRACE_ERROR(__VA_ARGS__)
#else
#define TEST_ARRAY_SIZE 100
#define TIME_DIFF_USEC	100 //100us
#define TICKDEF     uint32_t
#define TRACE_STF(...) TRACE_ERROR(__VA_ARGS__)
#endif

#define USE_STATIC_TEST_ARRAY
#ifdef USE_STATIC_TEST_ARRAY //static
static REAL a_test_arry[TEST_ARRAY_SIZE];
static REAL b_test_arry[TEST_ARRAY_SIZE];
static REAL c_test_arry[TEST_ARRAY_SIZE];
#endif

void executeSpeedTest_init(void)
{
    REAL *a = &a_test_arry[0];
    REAL *b = &b_test_arry[0];
    int nsize = TEST_ARRAY_SIZE;
    int j;
    for (j = 0; j < nsize; j++) {
        a[j] = (j*TEST_CONST1) / (j + TEST_CONST2);
        b[j] = (j*TEST_CONST2) / (j + TEST_CONST1);
    }
}

void executeSpeedTest(void)
{
#ifdef USE_STATIC_TEST_ARRAY //static
    REAL *a = &a_test_arry[0];
    REAL *b = &b_test_arry[0];
    REAL *c = &c_test_arry[0];
#else
    REAL a[TEST_ARRAY_SIZE], b[TEST_ARRAY_SIZE], c[TEST_ARRAY_SIZE];
#endif

    int nsize = TEST_ARRAY_SIZE;
#if defined(_WIN32) || defined(WIN32)
    uint32_t niters = 1000000;
#else
    uint32_t niters = 1000;
#endif

    int j, ticks;
    uint32_t ru2, ru3;

    //for (j = VECTOR_ADD; j < VECTOR_DIVIDE + 1; j++)
    j = VECTOR_ADD;
    {
            s_get_time_diff();
        DEBUG_LED_ON;
            vectorOps(a, b, c, nsize, niters, j);
        DEBUG_LED_OFF;
            ru2 = s_get_time_diff();
        TRACE_STF("ru2= %d", ru2);

            s_get_time_diff();
        DEBUG_LED_ON;
            vectorOps(a, b, c, nsize, niters, VECTOR_NOP);
        DEBUG_LED_OFF;
            ru3 = s_get_time_diff();

            ticks = (ru2 - ru3) * TIME_DIFF_USEC;
        TRACE_STF("ru3= %d (%d)", ru3, ticks);
#if 0 //for test
        TICKDEF dops = nsize * niters;
        uint32_t dticks = (ticks < 1) ? 1 : ticks;
        TICKDEF mflops = dops / dticks;
#if defined(_WIN32) || defined(WIN32)
        TRACE_STF("(%d) %d msec to perform %d, %d mflops", j, dticks, dops, (mflops / 1000));
#else
        TRACE_STF("(%d) %d usec to perform %d, %d kflops", j, dticks, dops, mflops); //1.0e6
#endif
#endif //0
    }
}
#endif //1
#endif
