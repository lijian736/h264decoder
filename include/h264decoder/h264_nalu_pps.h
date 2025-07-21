#ifndef _H_H264_NALU_PPS_H_
#define _H_H264_NALU_PPS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264_context.h"
#include "h264_nalu.h"

/**
 * @brief parses PPS
 * @see 7.3.2.2 Picture parameter set RBSP syntax
 * @see 7.4.2.2 Picture parameter set RBSP semantics
 *
 * @param rbsp_reader the RBSPReader
 * @param pps the PPS struct pointer
 * @param context the H264 context
 * @return int 0 on success, negative value on error
 */
int pic_parameter_set_rbsp(RBSPReader* rbsp_reader, PPS* pps, H264Context* context);

/**
 * @brief post-process for pps. this function should invoked immediately after pic_parameter_set_rbsp()
 *
 * @param pps the PPS struct pointer
 * @param context the H264 context
 * @return int 0 on success, negative value on error
 */
int post_process_pps(PPS* pps, H264Context* context);

/**
 * @brief print PPS info to the stream
 *
 * @param stream pointer to a FILE object that identifies an output stream
 * @param pps pointer to a PPS struct
 */
void fprintf_pps(FILE* stream, PPS* pps);

#endif