#ifndef _H_H264_STREAM_H_
#define _H_H264_STREAM_H_

#include <stdint.h>

#include "h264_error.h"

/**
 * @brief the H.264 bit stream
 */
typedef struct {
    uint8_t* start;      /* the stream start position(inclusive) */
    uint8_t* end;        /* the stream end position(exclusive)   */
    uint8_t* nalu_start; /* the current NALU start position(inclusive) */
    uint8_t* nalu_end;   /* the current NALU end position(exclusive) */
} H264BitStream;

/**
 * @brief Read the next NALU
 *
 * @param stream the H264BitStream pointer
 * @return int: 0 is ok, negative value on error
 */
int read_next_nalu(H264BitStream* stream);

#endif