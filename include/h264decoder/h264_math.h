#ifndef _H_H264_MATH_H_
#define _H_H264_MATH_H_

#include <stdint.h>

#define codec_max(a, b) (((a) > (b)) ? (a) : (b))
#define codec_min(a, b) (((a) < (b)) ? (a) : (b))
#define clip3(lower, upper, val) (((val) < (lower)) ? (lower) : (((val) > (upper)) ? (upper) : (val)))

int int_log2(uint32_t v);
int int_ceil_log2(uint32_t v);

/**
 * @brief convert the macroblock address or block index to the upper-left location (x or y) of the picture
 *
 * @param index macroblock address or block index
 * @param sub_width the sub width
 * @param sub_height the sub height
 * @param width picture width or region width
 * @param is_y 0 for x, 1 for y
 * @return int32_t the position, x on is_y==0, y on is_y==1
 */
inline int32_t InverseRasterScan(int32_t index, int32_t sub_width, int32_t sub_height, int32_t width, int32_t is_y);

#endif