#ifndef _H_H264_NALU_SPS_H_
#define _H_H264_NALU_SPS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264_nalu.h"
#include "h264_rbsp.h"

/**
 * @brief parse SPS
 *
 * @see 7.3.2.1 Sequence parameter set RBSP syntax
 * @see 7.4.2.1 Sequence parameter set RBSP semantics
 *
 * @param rbsp_reader the RBSPReader
 * @param sps the SPS struct pointer
 * @return int 0 on success, negative value on error
 */
int seq_parameter_set_rbsp(RBSPReader* rbsp_reader, SPS* sps);

/**
 * @brief post-process for sps. this function should invoked immediately after seq_parameter_set_rbsp()
 *
 * @param sps the SPS struct pointer
 * @return int 0 on success, negative value on error
 */
int post_process_sps(SPS* sps);

/**
 * @brief get the chroma format string
 * 
 * @param sps the SPS struct pointer
 * @return char* the chroma format string
 */
char* get_chroma_format(SPS* sps);

/**
 * @brief get the profile level info string
 * 
 * @param sps the SPS struct pointer
 * @param info output parameter. the received string buffer for profile and level info
 * @param info_size the info buffer size
 */
void get_profile_level(SPS* sps, char* info, size_t info_size);

/**
 * @brief get the picture's width and height
 * 
 * @param sps the SPS struct pointer
 * @param width output parameter. the width of the picture
 * @param height output parameter. the height of the picture
 */
void get_pic_width_height(SPS* sps, uint32_t* width, uint32_t* height);

/**
 * @brief print SPS info to the stream
 *
 * @param stream pointer to a FILE object that identifies an output stream
 * @param sps pointer to a SPS struct
 */
void fprintf_sps(FILE* stream, SPS* sps);

#endif