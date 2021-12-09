#include "fp_sin.h"

/* define common macros used for integer sine function implementation */
#define TRIG_TABLE_STEPS			(32)
#define TRIG_TABLE_STEP_SIZE		(HALF_PI/TRIG_TABLE_STEPS)


/* 33-entry lookup table for lookup and interpolation */
static const int32_t sin_lookup_table[TRIG_TABLE_STEPS+1] =
{
	0, 100, 200, 299, 398, 495, 592, 687,
	780, 872, 961, 1048, 1132, 1214, 1293, 1369,
	1441, 1510, 1575, 1637, 1694, 1748, 1797, 1842,
	1883, 1919, 1950, 1977, 1998, 2015, 2028, 2035, 2037
};


/**
 * @brief perform either a look-up from the lookup table
 * or the linear interpolation between adjacent points
 * of the input point.
 * @param num : the input frequency
 *  @return sine result of int32_t type.
 */
static int32_t fp_sin_lookup(int32_t num)
{
	int index = (int)(num / TRIG_TABLE_STEP_SIZE);

	/* perform linear interpolation if no integer index match */
	if((int32_t)(index * TRIG_TABLE_STEP_SIZE) != num) {

        int32_t x0,y0,y1;

        y0 = sin_lookup_table[index];
        y1 = sin_lookup_table[index + 1];
        x0 = index * TRIG_TABLE_STEP_SIZE;

        return y0 + (int32_t)((num - x0) * (y1 - y0) / TRIG_TABLE_STEP_SIZE);
	}
	else{
        if(index == TRIG_TABLE_STEPS)
            index -= 1;
		return sin_lookup_table[index];
	}
}

/**
 * @brief Calculate the sin function of the given
 * frequency using integers only.
 * @param num: input frequency
 * @return sine result of int16_t type.
 */
int16_t fp_sin(int32_t num)
{
	if(num < 0) {
        /* keep adding 2 * PI until num is positive */
        while(num < 0)
            num +=  TWO_PI;
    }

    if(num > TWO_PI) {
        /* keep adding 2 * PI until num is less than 2 PI */
        while(num > TWO_PI)
            num -=  TWO_PI;
    }

    if(num <= HALF_PI)
        return fp_sin_lookup(num);
    else if(num > HALF_PI && num <= PI){
        num = PI - num;
        return fp_sin_lookup(num);
    }
    else if(num > PI && num <= (PI + HALF_PI)){
        num -= PI;
        return (-1) * fp_sin_lookup(num);
    }
    else{
        num = TWO_PI - num;
        return (-1) * fp_sin_lookup(num);
    }
}
