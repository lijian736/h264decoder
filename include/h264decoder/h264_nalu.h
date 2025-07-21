#ifndef _H_H264_NALU_H_
#define _H_H264_NALU_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264_defs.h"
#include "h264_error.h"
#include "h264_rbsp.h"

/**
 * @brief get the NALU type string
 *
 * @param nalu_type the NALU type
 * @return char*
 */
char* stringify_nalu_type(uint8_t nalu_type);

/**
 * @brief free the NALU
 * the parameter nalu pointer becomes an invalid pointer after this free_nalu() was invoked
 *
 * @param nalu the NALU pointer
 */
void free_nalu(void* nalu);

/**
 * @brief check if there is more rbsp data
 * @see 7.2 Specification of syntax functions, categories, and descriptors
 *
 * @param rbsp_reader the RBSPReader
 * @return int 1: there is more rbsp data, 0: there is no more data
 */
int more_rbsp_data(RBSPReader* rbsp_reader);

/**
 * @brief rbsp trailing bits
 * @see 7.3.2.11 RBSP trailing bits syntax
 *
 * @param rbsp_reader the RBSPReader
 */
void rbsp_trailing_bits(RBSPReader* rbsp_reader);

/**
 * @brief more rbsp trailing data
 * @see 7.2 Specification of syntax functions, categories, and descriptors
 *
 * @param rbsp_reader the RBSPReader
 * @return int 1: there is more rbsp trailing data, 0: there is no more rbsp trailing data
 */
int more_rbsp_trailing_data(RBSPReader* rbsp_reader);

/**
 * @brief rbsp slice trailing bits
 * @see 7.3.2.10 RBSP slice trailing bits syntax
 *
 * @param rbsp_reader the RBSPReader
 * @param entropy_coding_mode_flag entropy coding model flag
 */
void rbsp_slice_trailing_bits(RBSPReader* rbsp_reader, int entropy_coding_mode_flag);

/**
 * @brief scaling list
 *
 * @see 7.3.2.1.1.1 Scaling list syntax
 * @param rbsp_reader the RBSPReader
 * @param scaling_list output parameter, the scaling list
 * @param scaling_list_size the scaling list size
 * @param use_default_scaling_matrix_flag output parameter, use default scaling matrix flag
 * @return int
 */
int scaling_list(RBSPReader* rbsp_reader, int32_t* scaling_list, size_t scaling_list_size, uint8_t* use_default_scaling_matrix_flag);

/**
 * @brief parse the VUI parameters
 * @see E.2.1 VUI parameters semantics
 * @see E.1.1 VUI parameters syntax
 *
 * @param rbsp_reader the RBSPReader
 * @param vui the VUI struct pointer
 * @param sps the SPS struct pointer
 * @return int 0 on success, negative value on error
 */
int vui_parameters(RBSPReader* rbsp_reader, VUI* vui, SPS* sps);
void vui_parameters_default(VUI* vui, SPS* sps);

/**
 * @brief parse the HRD parameters
 * @see E.2.2 HRD parameters semantics
 * @see E.1.2 HRD parameters syntax
 *
 * @param rbsp_reader the RBSPReader
 * @param hrd the HRD struct pointer
 * @return int 0 on success, negative value on error
 */
int hrd_parameters(RBSPReader* rbsp_reader, HRD* hrd);
void hrd_parameters_default(HRD* hrd);

/**
 * @brief print HRD info to the stream
 *
 * @param stream pointer to a FILE object that identifies an output stream
 * @param hrd pointer to a HRD struct
 */
void fprintf_hrd(FILE* stream, HRD* hrd);

/**
 * @brief print vui info to the stream
 *
 * @param stream pointer to a FILE object that identifies an output stream
 * @param vui pointer to a VUI struct
 */
void fprintf_vui(FILE* stream, VUI* vui);

#endif