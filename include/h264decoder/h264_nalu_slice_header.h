#ifndef _H_H264_NALU_SLICE_HEADER_H_
#define _H_H264_NALU_SLICE_HEADER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264_context.h"
#include "h264_nalu.h"
#include "h264_rbsp.h"

/**
 * @brief reset the slice header
 * 
 * @param header the slice header 
 */
void reset_slice_header(SliceHeader* header);

/**
 * @brief parse slice layer without partitioning RBSP
 * @see 7.3.2.8 Slice layer without partitioning RBSP syntax
 *
 * @param rbsp_reader the RBSPReader
 * @param slice_header the slice header
 * @param context the H264 context
 * @return int 0 on success, negative value on error
 */
int slice_layer_without_partitioning_rbsp(RBSPReader* rbsp_reader, SliceHeader* slice_header, H264Context* context);

#endif