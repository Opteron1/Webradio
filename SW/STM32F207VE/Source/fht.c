#include "fht.h"

u32 fht_sample_buffer[FHT_LEN];
s16 fht_sample_buffer_left[FHT_LEN];
s16 fht_sample_buffer_right[FHT_LEN];
u8 fht_update_status = 0;
u8 fht_new_sample_status = 0;

/* ADCs samples transfer complete */
__irq void DMA2_Stream0_IRQHandler(void)
{
	u16 i;
	/* Test on DMA Stream Transfer Complete interrupt */
  if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
		// Copy into buffers
		for(i = 0;i < FHT_LEN; i++)
		{
			fht_sample_buffer_right[i] = fht_sample_buffer[i] & 0xFFFF;
			fht_sample_buffer_left[i] = fht_sample_buffer[i] >> 16U;
		}
		fht_new_sample_status = 1;	//New samples available
  }
}

void fht_start(void)
{
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA2_Stream0, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	
}

void fht_stop(void)
{
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);
	DMA_Cmd(DMA2_Stream0, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
}

u8 fht_updated(void)
{
	return fht_update_status;
}

void fht_clear_update_status(void)
{
	fht_update_status = 0;
}

void fht_service(void)
{
	if(fht_new_sample_status)	//New samples available?
	{
		fht_applyHannWindow(fht_sample_buffer_right);
		fht_applyHannWindow(fht_sample_buffer_left);
		fht_DitInt(fht_sample_buffer_right);	//Compute FHT for right chanel
		fht_DitInt(fht_sample_buffer_left);		//Compute FHT for left chanel
		fht_update_status = 1;	//New data for draw available
		fht_new_sample_status = 0;	//Zero flag
	}
}

// Sine and Cosine tables for 32 to 256 elements (stored in RAM):
#if (FHT_LEN == 32)
#define SC_SCALE  10
// FHT sine and cosine tables for FHT with 32 elements:
const s16 cosTable[FHT_SCALE] = {-1024,     0,   724,   946,  1004};
const s16 sinTable[FHT_SCALE] = {    0,  1024,   724,   391,   199};
#elif (FHT_LEN == 64)
#define SC_SCALE  10
// FHT sine and cosine tables for FHT with 64 elements:
const s16 cosTable[FHT_SCALE] = {-1024,     0,   724,   946,  1004,  1019};
const s16 sinTable[FHT_SCALE] = {    0,  1024,   724,   391,   199,   100};
#elif (FHT_LEN == 128)
#define SC_SCALE  10
// FHT sine and cosine tables for FHT with 128 elements:
const s16 cosTable[FHT_SCALE] = {-1024,     0,   724,   946,  1004,  1019,  1022};
const s16 sinTable[FHT_SCALE] = {    0,  1024,   724,   391,   199,   100,    50};
#elif (FHT_LEN == 256)
#define SC_SCALE  10
// FHT sine and cosine tables for FHT with 256 elements:
const s16 cosTable[FHT_SCALE] = {-1024,     0,   724,   946,  1004,  1019,  1022,  1023};
const s16 sinTable[FHT_SCALE] = {    0,  1024,   724,   391,   199,   100,    50,    25};
#endif

void fht_DitInt(s16 *fx)
{
	s32 dcos, dsin, fcos, fsin;
	s16 istep, k;
	u16 n;
	s16 temp16bit1, butdis, butloc;
	s32 temp32bit1, temp32bit2;
	u8 tablePointer;

	// Permute - bit reversal
	butdis = 0;
	for (k = 0; k < FHT_LEN; k++)
	{
		if (k <= butdis)
		{
			temp16bit1 = fx[butdis];
			fx[butdis] = fx[k];
			fx[k] = temp16bit1;
		}
		butloc = FHT_LEN >> 1;

		while(butdis >= butloc && butloc > 0)
		{
			butdis -= butloc;
			butloc = butloc >> 1;
		}
		butdis += butloc;
	}
	// End permute

	// Start of FHT function
	n = 1;
	tablePointer = 0;

	while (n < FHT_LEN)
	{
		istep = n << 1;
		// Get the sine and cosine data from the look-up tables
		dcos = (s32)cosTable[tablePointer];
		dsin = (s32)sinTable[tablePointer];
		fcos = dcos;
		fsin = dsin;
		tablePointer++;
		// Zero Frequency loop
		for (k = 0; k < FHT_LEN; k += istep)
		{
			temp16bit1 = fx[k + n];
			fx[k + n] = (fx[k] - temp16bit1) >> 1;
			fx[k] = (fx[k] + temp16bit1) >> 1;
		}
		// End zero frequency loop
		if (n > 2)
		{
			// Double butterfly
			butdis = n - 2;
			for (butloc = 1; butloc < (n >> 1); butloc++)
			{
				if (n > 2)
				{
					// Double butterfly
					butdis = n - 2;
					for (butloc = 1; butloc < (n >> 1); butloc++)
					{
						for (k = butloc; k <= FHT_LEN; k += istep)
						{
							// Using 32 bit integers for the main multiplication to prevent overflow errors
							// and we add 1 to SC_SCALE to divide the result by 2 or the addition below can overflow
							temp32bit1 = ((fcos * (s32)fx[k + n]) + (fsin * (s32)fx[k + n + butdis])) >> (SC_SCALE + 1);
							temp32bit2 = ((fsin * (s32)fx[k + n]) - (fcos * (s32)fx[k + n + butdis])) >> (SC_SCALE + 1);

							fx[k + n] = (fx[k] >> 1) - (s16)temp32bit1;
							fx[k + n + butdis] = (fx[k + butdis] >> 1) - (s16)temp32bit2;
							fx[k] = (fx[k] >> 1) + (s16)temp32bit1;
							fx[k + butdis] = (fx[k + butdis] >> 1) + (s16)temp32bit2;
						}
						temp32bit1 = (fcos * dcos - fsin * dsin) >> SC_SCALE;
						fsin = (fsin * dcos + fcos * dsin) >> SC_SCALE;
						fcos = temp32bit1;
						butdis -= 2;
					}
					// End double butterfly
				}
				temp32bit1 = (fcos * dcos - fsin * dsin) >> SC_SCALE;
				fsin = (fsin * dcos + fcos * dsin) >> SC_SCALE;
				fcos = temp32bit1;
				butdis -= 2;
			}
			// End double butterfly
		}
		if (n > 1)
		{
			// Nyquist Frequency Loop
			for (k = (n >> 1); k < FHT_LEN; k += istep)
			{
				temp16bit1 = fx[k + n];
				fx[k + n] = (fx[k] - temp16bit1) >> 1;
				fx[k] = (fx[k] + temp16bit1) >> 1;
			}
			// End Nyquist frequency loop
		}
		n = istep;
	}
	// End FHT
}

// Hamming window tables for 32 to 256 elements (stored in PROGMEM):
#if (FHT_LEN == 32)
// Hamming window table for samples with 32 elements (scaled x16384):
// Since the hamming window is symmetrical we only need half of the window
const s16 hammingTable[16] = {
	1310,  1464,  1921,  2661,  3654,  4860,  6229,  7706,  9229, 10736, 12166, 13460, 14565, 15436, 16038, 16345
};
#elif (FHT_LEN == 64)
// Hamming window table for samples with 64 elements (scaled x16384):
// Since the hamming window is symmetrical we only need half of the window
const s16 hammingTable[32] = {
	1310,  1348,  1460,  1645,  1902,  2228,  2620,  3073,  3585,  4148,  4758,  5409,  6093,  6806,  7538,  8284,
	9035,  9784, 10524, 11247, 11947, 12615, 13246, 13834, 14372, 14855, 15278, 15637, 15929, 16150, 16299, 16374
};
#elif (FHT_LEN == 128)
// Hamming window table for samples with 128 elements (scaled x16384):
// Since the hamming window is symmetrical we only need half of the window
const s16 hammingTable[64] = {
	1310,  1319,  1347,  1393,  1457,  1540,  1640,  1758,  1893,  2045,  2214,  2399,  2600,  2816,  3047,  3292,
	3551,  3822,  4106,  4402,  4708,  5025,  5351,  5685,  6027,  6377,  6732,  7092,  7457,  7825,  8195,  8567,
	8940,  9313,  9684, 10053, 10420, 10782, 11140, 11493, 11838, 12177, 12507, 12828, 13140, 13441, 13731, 14009,
	14274, 14526, 14764, 14988, 15196, 15389, 15566, 15727, 15871, 15997, 16106, 16197, 16271, 16326, 16363, 16381
};
#elif (FHT_LEN == 256)
// Hamming window table for samples with 256 elements (scaled x16384):
// Since the hamming window is symmetrical we only need half of the window
const s16 hammingTable[128] = {
	1310,  1313,  1319,  1331,  1347,  1367,  1392,  1422,  1456,  1495,  1538,  1585,  1637,  1694,  1754,  1819,
	1888,  1962,  2039,  2121,  2207,  2297,  2391,  2488,  2590,  2695,  2805,  2917,  3034,  3154,  3277,  3404,
	3534,  3667,  3804,  3943,  4086,  4231,  4379,  4530,  4684,  4840,  4998,  5159,  5322,  5487,  5655,  5824,
	5995,  6168,  6342,  6518,  6695,  6874,  7054,  7235,  7416,  7599,  7783,  7967,  8151,  8337,  8522,  8708,
	8893,  9079,  9264,  9450,  9635,  9819, 10003, 10186, 10368, 10550, 10730, 10909, 11087, 11264, 11439, 11613,
	11785, 11955, 12123, 12289, 12453, 12615, 12775, 12932, 13087, 13239, 13389, 13536, 13679, 13820, 13958, 14093,
	14225, 14354, 14479, 14600, 14719, 14833, 14944, 15051, 15155, 15255, 15350, 15442, 15530, 15614, 15694, 15769,
	15840, 15908, 15970, 16029, 16083, 16133, 16178, 16219, 16255, 16287, 16314, 16337, 16355, 16369, 16378, 16383
};
#endif

// Hann window tables for 32 to 256 elements (stored in PROGMEM):
#if (FHT_LEN == 32)
// Hann window table for samples with 32 elements (scaled x16384):
// Since the hann window is symmetrical we only need half of the window
const s16 hannTable[16] = {
	0,   167,   663,  1468,  2547,  3858,  5346,  6951,  8606, 10245, 11799, 13206, 14407, 15354, 16008, 16341
};
#elif (FHT_LEN == 64)
// Hann window table for samples with 64 elements (scaled x16384):
// Since the hann window is symmetrical we only need half of the window
const s16 hannTable[32] = {
	0,    40,   162,   363,   643,   997,  1423,  1916,  2472,  3084,  3747,  4454,  5199,  5973,  6769,  7579,
	8396,  9210, 10014, 10801, 11561, 12288, 12974, 13612, 14197, 14722, 15182, 15572, 15889, 16130, 16292, 16373
};
#elif (FHT_LEN == 128)
// Hann window table for samples with 128 elements (scaled x16384):
// Since the hann window is symmetrical we only need half of the window
const s16 hannTable[64] = {
	0,    10,    40,    90,   159,   249,   358,   486,   633,   798,   982,  1183,  1401,  1636,  1887,  2154,
	2435,  2730,  3039,  3360,  3693,  4037,  4391,  4755,  5127,  5506,  5892,  6284,  6680,  7080,  7483,  7888,
	8293,  8698,  9102,  9503,  9901, 10295, 10684, 11067, 11443, 11811, 12170, 12519, 12858, 13185, 13500, 13802,
	14091, 14364, 14623, 14866, 15093, 15303, 15495, 15670, 15826, 15964, 16082, 16181, 16261, 16321, 16361, 16381
};
#elif (FHT_LEN == 256)
// Hann window table for samples with 256 elements (scaled x16384):
// Since the hann window is symmetrical we only need half of the window
const s16 hannTable[128] = {
	0,     2,     9,    22,    39,    62,    89,   121,   158,   200,   247,   299,   355,   416,   482,   553,
	628,   708,   792,   881,   974,  1072,  1174,  1280,  1391,  1505,  1624,  1746,  1873,  2003,  2138,  2275,
	2417,  2562,  2710,  2862,  3017,  3175,  3336,  3500,  3667,  3836,  4008,  4183,  4360,  4540,  4722,  4906,
	5092,  5279,  5469,  5660,  5853,  6047,  6242,  6439,  6637,  6835,  7035,  7235,  7436,  7637,  7838,  8040,
	8242,  8444,  8645,  8847,  9048,  9248,  9448,  9647,  9845, 10042, 10239, 10433, 10627, 10819, 11009, 11198,
	11385, 11570, 11752, 11933, 12111, 12288, 12461, 12632, 12800, 12966, 13128, 13288, 13444, 13598, 13748, 13894,
	14037, 14177, 14313, 14445, 14574, 14698, 14819, 14936, 15048, 15156, 15261, 15360, 15456, 15547, 15634, 15716,
	15793, 15866, 15934, 15998, 16057, 16111, 16160, 16204, 16244, 16279, 16308, 16333, 16353, 16368, 16378, 16383
};
#endif

// Apply a Hamming window to the input sample data
void fht_applyHammingWindow(s16 *fx)
{
	s16 k, i = 0;
	s32 calc;

	// First half of the window
	for (k = 0; k < FHT_LEN/2; k++)
	{
		calc = ((s32)fx[i] * (s32)hammingTable[k]) >> 14;
		fx[i] = (s16)calc;
		i++;
	}

	// Second half of the window
	for (k = (FHT_LEN/2)-1; k >= 0; k--)
	{
		calc = ((s32)fx[i] * (s32)hammingTable[k]) >> 14;
		fx[i] = (s16)calc;
		i++;
	}
}

// Apply a Hann window to the input sample data
void fht_applyHannWindow(s16 *fx)
{
	s16 k, i = 0;
	s32 calc;

	// First half of the window
	for (k = 0; k < FHT_LEN/2; k++)
	{
		calc = ((s32)fx[i] * (s32)hannTable[k]) >> 14;
		fx[i] = (s16)calc;
		i++;
	}

	// Second half of the window
	for (k = (FHT_LEN/2)-1; k >= 0; k--)
	{
		calc = ((s32)fx[i] * (s32)hannTable[k]) >> 14;
		fx[i] = (s16)calc;
		i++;
	}
}

// This function calculates the absolute value of the
// complex numbers returned by the FHT and outputs real
// number results.
//
// This calculates the linear amplitude (magnitude)
// spectrum of the FHT results using the following formula:
// fx(k) = SQRT( (fx(k)^2) + (fx(-k)^2) )
//
// The function calculates the square-roots using a
// fast (but compiler/processor dependent) integer
// approximation adapted from code at:
// http://www.codecodex.com/wiki/Calculate_an_integer_square_root
//
// Finding the SQRT of a 32-bit number using look-up tables
// requires a very big table - so this is probably the better
// option for low RAM footprint unless you want to reduce the
// output resolution
//
// Note: The output from this function is 0-8191 if scale = 0
// Setting scale to non-zero causes the output to be scaled down
// by output >> scale, i.e. a scale of 8 will output 0-63.
void fht_complexToReal(s16 *fx, s16 scale)
{
	s32 place, root, calc;
	s16 k;

	for (k = 0; k < (FHT_LEN / 2); k++)
	{
		calc = ((s32)fx[k] * (s32)fx[k] +
		(s32)fx[FHT_LEN - k] * (s32)fx[FHT_LEN - k]);
		// Find the square root of the 32bit number
		place = 0x40000000;
		root = 0;
		// Ensure we don't have a negative number
		if (calc >= 0)
		{
			while (place > calc) place = place >> 2;
			while (place)
			{
				if (calc >= root + place)
				{
					calc -= root + place;
					root += place << 1;
				}
				root = root >> 1;
				place = place >> 2;
			}
		}
		// Now scale back up to 16 bits

		// The FHT's maximum input range is -16383 to +16383
		// and the FHT output is -16383 to +16383 (for both
		// the real and imaginary number parts)
		//
		// Therefore the maximum real linear amplitude of a
		// frequency bin is:
		// = 8191 * 8191 + -8192 * -8192
		// = 134201345
		// = SQRT(134201345)
		// = 11585
		//
		// To avoid a floating-point multiplication by *(16383/23169) =
		// 0.707108637 we can approximate the percentage by using a fraction
		// which has a power-of-two denominator (in other words we
		// scale up the result (since we have a 32 bit variable) and
		// then divide it by 2^15.
		//
		// root = root * 11584 / 16384 = root * 11584 >> 14
		// and we also add in the additional scaling division:
		root = root * 11584 >> (14 + scale); // Outputs 0-8191 >> scale from 0-11584
		
		// Scale the result and store it
		fx[k] = (s16)root;
	}
}

// Linear to dB output tables for 64 to 128 scaled output (stored in PROGMEM):
#if (N_DB == 64)
// Linear complex number to real number dB table for output range of 63-0:
// Range -78 dB to 0 dB
const s16 dbMap[64] = {
	1,     1,     1,     1,     2,     2,     2,     3,
	3,     4,     4,     5,     6,     7,     8,     9,
	11,    12,    14,    17,    19,    22,    25,    29,
	34,    39,    45,    52,    60,    69,    79,    91,
	105,   121,   140,   161,   185,   213,   245,   282,
	324,   373,   430,   494,   569,   655,   754,   867,
	998,  1148,  1321,  1520,  1749,  2013,  2316,  2665,
	3067,  3529,  4061,  4672,  5376,  6186,  7118,  8191
};
#elif (N_DB == 128)
// Linear complex number to real number dB table for output range of 127-0:
// Range -78 dB to 0 dB
const s16 dbMap[128] = {
	1,     1,     1,     1,     1,     1,     1,     1,
	1,     2,     2,     2,     2,     2,     2,     3,
	3,     3,     3,     4,     4,     4,     5,     5,
	5,     6,     6,     7,     7,     8,     9,     9,
	10,    11,    12,    12,    13,    14,    15,    17,
	18,    19,    21,    22,    24,    25,    27,    29,
	32,    34,    36,    39,    42,    45,    48,    52,
	56,    60,    64,    69,    74,    79,    85,    91,
	98,   105,   113,   121,   130,   140,   150,   161,
	172,   185,   198,   213,   228,   245,   263,   282,
	302,   324,   348,   373,   401,   430,   461,   494,
	530,   569,   610,   655,   702,   754,   808,   867,
	930,   998,  1070,  1148,  1232,  1321,  1417,  1520,
	1631,  1749,  1877,  2013,  2159,  2316,  2485,  2665,
	2859,  3067,  3290,  3529,  3786,  4061,  4356,  4672,
	5012,  5376,  5767,  6186,  6636,  7118,  7636,  8191
};
#endif

// Linear to dB output tables with gain for 64 to 128 scaled output (stored in PROGMEM):
#if (N_DB == 64)
// Linear complex number to real number dB table for output range of 63-0:
// Range -78 dB to -30 dB
const s16 dbMapPlusGain[64] = {
	0,     0,     0,     0,     0,     0,     0,     0,
	0,     0,     0,     0,     0,     0,     0,     0,
	0,     0,     0,     0,     0,     0,     0,     0,
	1,     1,     1,     1,     1,     2,     2,     2,
	3,     3,     4,     5,     5,     6,     7,     8,
	10,    11,    13,    15,    18,    20,    23,    27,
	31,    36,    41,    48,    55,    63,    73,    84,
	97,   111,   128,   147,   170,   195,   225,   259
};
#elif (N_DB == 128)
// Linear complex number to real number dB table for output range of 127-0:
// Range -78 dB to -30 dB
const s16 dbMapPlusGain[128] = {
	0,     0,     0,     0,     0,     0,     0,     0,
	0,     0,     0,     0,     0,     0,     0,     0,
	0,     0,     0,     0,     0,     0,     0,     0,
	0,     0,     0,     0,     0,     0,     0,     0,
	0,     0,     0,     0,     0,     0,     0,     0,
	0,     0,     0,     0,     0,     0,     0,     0,
	1,     1,     1,     1,     1,     1,     1,     1,
	1,     1,     2,     2,     2,     2,     2,     2,
	3,     3,     3,     3,     4,     4,     4,     5,
	5,     5,     6,     6,     7,     7,     8,     8,
	9,    10,    11,    11,    12,    13,    14,    15,
	16,    18,    19,    20,    22,    23,    25,    27,
	29,    31,    33,    36,    38,    41,    44,    48,
	51,    55,    59,    63,    68,    73,    78,    84,
	90,    97,   104,   111,   119,   128,   137,   147,
	158,   170,   182,   195,   209,   225,   241,   259
};
#endif

// This function converts the complex number output from the FHT
// into real number logarithmic decibel values.
//
// The stages are:
// 1. convert to magnitude values: magnitude = sqrt(re * re + im * im)
// 2. convert to dB (log) scale dB = 20 * log10(magnitude)
//
// However we can combine these two steps to save processing
// since db = 10 * log10(re * re + im * im) is mathematically
// equivalent to the previous two steps and saves us from having
// to perform the sqrt().
//
void fht_complexToDecibel(s16 *fx)
{
	// Process for each complex number in fx[FHT_LEN/2]
	s16 k, i;
	s32 calc;

	for (k = 0; k < FHT_LEN / 2; k++)
	{
		// calc = (fx(k)^2) + (fx(-k)^2)
		calc = ((s32)fx[k] * (s32)fx[k] +
		(s32)fx[FHT_LEN - k] * (s32)fx[FHT_LEN - k]);
		// The table expects a maximum result of 16383
		// Since the maximum output from the FHT is as follows:
		// = 16383 * 16383 + -16383 * -16383
		// The maximum result of the above calculation is:
		// = 536805378
		//
		// 536805378 / 16834 = 16384 = 1 << 14
		//
		// Therefore we scale like this:
		calc = calc >> 14;
		// Now we seek the position in the decibel table which contains
		// a lower value than calc.  The position in the table gives
		// us the approximate dB level
		for (i = 0; i < N_DB; i++)
		{
			if (calc <= dbMap[i]) break;
		}
		// Check for overflow in the case that we didn't break
		// from the for loop
		if (i == N_DB) i = N_DB - 1;
		// Now we write the decibel value back into fx[k]
		fx[k] = i;
	}
}

// This function is identical to the previous function
// but uses a table with built in gain for a higher 
// output signal
void fht_complexToDecibelWithGain(s16 *fx)
{
	// Process for each complex number in fx[FHT_LEN/2]
	s16 k, i;
	s32 calc;
	for (k = 0; k < FHT_LEN / 2; k++)
	{
		// calc = (fx(k)^2) + (fx(-k)^2)
		calc = ((s32)fx[k] * (s32)fx[k] +
		(s32)fx[FHT_LEN - k] * (s32)fx[FHT_LEN - k]);
		// The table expects a maximum result of 16383
		// Since the maximum output from the FHT is as follows:
		// = 16383 * 16383 + -16383 * -16383
		// The maximum result of the above calculation is:
		// = 536805378
		//
		// 536805378 / 16834 = 16384 = 1 << 14
		//
		// Therefore we scale like this:
		calc = calc >> 14;
		// Now we seek the position in the decibel table which contains
		// a lower value than calc.  The position in the table gives
		// us the approximate dB level
		for (i = 0; i < N_DB; i++)
		{
			if (calc <= dbMapPlusGain[i]) break;
		}
		// Check for overflow in the case that we didn't break
		// from the for loop
		if (i == N_DB) i = N_DB - 1;
		// Now we write the decibel value back into fx[k]
		fx[k] = i;
	}
}
