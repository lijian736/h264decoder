#ifndef _H_H264_CODED_BLOCK_PATTERN_H_
#define _H_H264_CODED_BLOCK_PATTERN_H_

#include <stdint.h>
#include "h264_defs.h"
#include "h264_error.h"

/**
 * @brief Mapping process for coded block pattern
 * 
 * @see 9.1.2 Mapping process for coded block pattern
 * 
 * @param ChromaArrayType ChromaArrayType
 * @param code_num the codeNum for ue(v)
 * @param mode the MbPartPredMode
 * @param out output parameter, the coded block pattern
 * @return int 0 on success, negative value on error
 */
int coded_block_pattern(uint32_t ChromaArrayType, uint32_t code_num, H264_MB_PART_PRED_MODE mode, uint8_t* out);

#endif