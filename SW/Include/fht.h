#ifndef _FHT_H_
#define _FHT_H_		1U

#include "stm32f2xx.h"

// The FHT length is the number of input datum to the FHT (i.e. the number of samples)
// this is defined by FHT_SCALE which denotes 2^6 = 64 where FHT_SCALE = 6
// You must define both the FHT_LEN and FHT_SCALE values to match according to the
// following table:
//
// FHT_SCALE | FHT_LEN
//     5     |   32
//     6     |   64
//     7     |  128
//     8     |  256

// Define the required FHT length here (i.e. the number of input samples).
// Valid values are 32, 64, 128 or 256.
#define FHT_LEN		128
#define FHT_SCALE	7

// Define the required output range of the linear complex number
// to real dB value output scaling here.
// Valid values are 64 (output range 0-64) or 128 (output range
// 0-128)
#define N_DB 64

/*----- GLOBALS -----*/
extern u32 fht_sample_buffer[FHT_LEN];
extern s16 fht_sample_buffer_right[FHT_LEN];
extern s16 fht_sample_buffer_left[FHT_LEN];


void fht_start(void);
void fht_stop(void);
u8 fht_updated(void);
void fht_clear_update_status(void);
void fht_service(void);
void fht_DitInt(s16 *fx);
void fht_applyHammingWindow(s16 *fx);
void fht_applyHannWindow(s16 *fx);
void fht_complexToReal(s16 *fx, int16_t scale);
void fht_complexToDecibel(s16 *fx);
void fht_complexToDecibelWithGain(s16 *fx);

#endif	/* _FHT_H_ */
