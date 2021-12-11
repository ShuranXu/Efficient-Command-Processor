#include "tsi.h"
#include "MKL25Z4.h"
#include "board.h"
#include "common.h"

/* Macros for TSI slider position calculation */
#define TSI_LEFT_SIDE 			(10)
#define TSI_CENTRE_SIDE 		(30)
#define TOTAL_TSI_ELECTRODES    (2)
#define TSI9         			(9)
#define TSI10        			(10)
#define TSICHANNEL9   			(TSI9)
#define TSICHANNEL10  			(TSI10)
#define HIGH_THRESHOLD0   		(100)
#define HIGH_THRESHOLD1   		(100)
#define LOW_THRESHOLD0   		(10)
#define LOW_THRESHOLD1   		(10)

/* Structure used to hold the low_thrshd
 * and high_thrshd. values lower than
 * low_thrshd is deemed to be noise;
 * values lower than high_thrshd means
 * that the value is too small to
 * lead to state changes.
 */
typedef struct {
	uint16_t low_thrshd;
	uint16_t high_thrshd;
}tsi_thrshd;

/* Declare global variables */
static uint8_t tsiChannels[]={TSICHANNEL9,TSICHANNEL10};
static uint16_t tsiBaseCnt[TOTAL_TSI_ELECTRODES];
const static tsi_thrshd tsiThreshold[] = { {HIGH_THRESHOLD0, LOW_THRESHOLD0},
		{HIGH_THRESHOLD1, LOW_THRESHOLD1}
};
static uint16_t tsiCntDelta[] = {0,0};
static uint8_t curr_channel = 0;

/**
 * @brief restart TSI slider.
 */
static void inline restart_tsi()
{
	/* update the TSI scan using the updated TSI channel */
	MODIFY_FIELD(TSI0->DATA, TSI_DATA_TSICH, tsiChannels[curr_channel]);
	/* SW trigger Start */
	MODIFY_FIELD(TSI0->DATA,TSI_DATA_SWTS, 1);
}


/**
 * @brief read the position of the finger on the slider. The idea
 * to determine the slider is to compute the relative length by using
 * the counter from channel 9 (i.e.tsiCntDelta[0]) and channel 10
 * (i.e.tsiCntDelta[1]). Based on the schematic diagram of the KL25Z4 board
 * we can see that the channel 9 is connected to the top(i.e.right)
 * side of the slider and the channel 10 is connected to the bottom (i.e.left)
 * side of the slider. So in order to compute the position of the touch
 * with the left side as the bottom side, we need to compute each channel's
 * reading relative to the slider length. To do so, we need to compute
 * their percentages for this round of reading, and then calculate their
 * respective length. Finally, we need to subtract the length for channel 9
 * and add the length for channel 10:
 *
 * slider_length - length_for_chnn9 + length_for_chnn10
 *
 * The above equation has to be divided by 10 to handle the case where
 * the user touches the middle of the slider. Without the division of 2 the
 * computed length might exceed the slider length, which is 40 mm. So
 * the final equation we will use is as follows:
 *
 * (slider_length - length_for_chnn9 + length_for_chnn10) / 2
 *
 * Note: If we choose to set the right side of the slider as the bottom
 * then we just need to swap the length of slider channels:
 *
 *  (slider_length - length_for_chnn10 + length_for_chnn9) / 2
 *
 * uint8_t: the current position on the slider
 */
uint8_t read_slider_distance()
{
	if((tsiCntDelta[0] > tsiThreshold[0].high_thrshd &&
			tsiCntDelta[1] > tsiThreshold[1].low_thrshd)||
			(tsiCntDelta[1] > tsiThreshold[1].high_thrshd &&
					tsiCntDelta[0] > tsiThreshold[0].low_thrshd))
	{
		uint8_t positions[2];
		uint32_t delta_sum = tsiCntDelta[0]+tsiCntDelta[1];
		uint8_t slider_dist;

		positions[0] = (tsiCntDelta[0]* SLIDER_LENGTH)/delta_sum;
		positions[1] = (tsiCntDelta[1]* SLIDER_LENGTH)/delta_sum;
		slider_dist = ((SLIDER_LENGTH - positions[0]) + positions[1])/2;
//#ifdef DEBUG
//		PRINTF("\r\n SLIDER VALUE %u", slider_dist);
//#endif
		return slider_dist;
	}
	return 0;
}


/**
 * @brief poll TSI slider ISR flag and clear it.
 * Besides, the TSI channel is updated to use the
 * another one.
 */

void poll_and_handle_tsi()
{
	for(int i=0; i < TOTAL_TSI_ELECTRODES; i++) {

		while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK));
		/* reset TSI_GENCS_EOSF bit */
		MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_EOSF, 1);
		/* read the current counter and update the delta for the
		 * current channel.
		 */
		uint16_t curr_cnt = GET_FIELD(TSI0->DATA, TSI_DATA_TSICNT);
		int16_t cnt_delta = curr_cnt - tsiBaseCnt[curr_channel];
		tsiCntDelta[curr_channel] = cnt_delta > 0 ? cnt_delta : 0;
		/* change channel to scan */
		curr_channel = (TOTAL_TSI_ELECTRODES - 1) - curr_channel;
		restart_tsi();
	}

//#ifdef DEBUG
//		PRINTF("\r\n SLIDER DELTA[0] = %u", tsiCntDelta[0]);
//		PRINTF("\r\n SLIDER DELTA[1] = %u", tsiCntDelta[1]);
//#endif

	return;
}


/**
 * @brief calibrate TSI sensor
 */
void TSI_calibrate()
{
	/* Clear End of Scan Flag */
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_EOSF, 1);
	/* Disable TSI module */
	CLEAR_FIELD(TSI0->GENCS, TSI_GENCS_TSIEN);
	/* Use SW trigger */
	CLEAR_FIELD(TSI0->GENCS, TSI_GENCS_STM);
	/* Enable TSI module */
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_TSIEN, 1);
	/* set baseline count for channel 9 and 10 not pressed */
	for(int cnt=0; cnt < TOTAL_TSI_ELECTRODES; cnt++)
	{
		TSI0->DATA = tsiChannels[cnt] << TSI_DATA_TSICH_SHIFT;
		MODIFY_FIELD(TSI0->DATA, TSI_DATA_SWTS, 1);
		/* wait for scan to complete */
		while(!GET_FIELD(TSI0->GENCS, TSI_GENCS_EOSF));
		/* read TSI0->DATA before resetting TSI_GENCS_EOSF bit */
		tsiBaseCnt[cnt] = GET_FIELD(TSI0->DATA, TSI_DATA_TSICNT);
		/* reset TSI_GENCS_EOSF bit */
		MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_EOSF, 1);
	}

	/* Disable TSI module */
	CLEAR_FIELD(TSI0->GENCS, TSI_GENCS_TSIEN);
	/* software trigger */
	CLEAR_FIELD(TSI0->GENCS, TSI_GENCS_STM);
	/* Enable TSI module */
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_TSIEN, 1);
	/* reset to use channel 9 */
	TSI0->DATA = tsiChannels[0] << TSI_DATA_TSICH_SHIFT;
	MODIFY_FIELD(TSI0->DATA, TSI_DATA_SWTS, 1);
}

/**
 * @brief initialize TSI sensor
 */
void TSI_init()
{
	MODIFY_FIELD(SIM->SCGC5, SIM_SCGC5_PORTB, 1);
	MODIFY_FIELD(SIM->SCGC5, SIM_SCGC5_TSI, 1);
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_MODE, 0);
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_REFCHRG, 4);
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_DVOLT, 0);
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_EXTCHRG, 7);
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_PS, 4);
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_NSCN, 11);
	/* disable Touch Sensing Input Interrupt */
	CLEAR_FIELD(TSI0->GENCS, TSI_GENCS_TSIIEN);
	/* disable low-power mode */
	CLEAR_FIELD(TSI0->GENCS, TSI_GENCS_STPE);
	MODIFY_FIELD(TSI0->GENCS, TSI_GENCS_TSIEN, 1);
}

