#ifndef _H_H264_CONTEXT_H_
#define _H_H264_CONTEXT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264_error.h"
#include "h264_nalu.h"
#include "h264_picture.h"

/**
 * @brief H.264 Context
 *
 */
typedef struct {
    SPS *sps[H264_MAX_SPS_COUNT]; /*the sps array*/
    PPS *pps[H264_MAX_PPS_COUNT]; /*the pps array*/

    SPS *active_sps; /*the active sps MUST be in the sps array*/
    PPS *active_pps; /*the active pps MUST be in the pps array*/

    uint8_t last_slice_nal_unit_type; /*the last slice NALU type, it's used for parsing auxiliary slices(IdrPicFlag)*/

    Picture *gop[H264_MAX_DPB_FRAMES]; /* group of pictures */
    int gop_curr_index;                /* the gop current picture index */
    Picture *current_picture;          /* the current picture, it MUST be in the gop array*/

    SliceHeader *current_slice_header; /* the current slice header */
    SliceHeader *prev_slice_header;    /* the previous slice header*/
} H264Context;

/**
 * @brief create a H264 context
 *
 * @return H264Context* the H264Context pointer, return 0 if the creation fails
 */
H264Context *create_context();

/**
 * @brief free the H264Context
 * the parameter context pointer becomes an invalid pointer after this free_context() was invoked
 *
 * @param context the H264Context pointer
 */
void free_context(H264Context *context);

/**
 * @brief get the picture from context
 *
 * @param context the H264 context pointer
 * @param is_new_picture the picture is a new picture or not
 * @param out_picture output parameter. the picture
 * @return int 0 on success, negative value on error
 */
int get_picture_from_context(H264Context *context, int is_new_picture, Picture **out_picture);

/**
 * @brief get the current slice header
 *
 * @param context the H264 context pointer
 * @param out_header output parameter. the current slice header
 */
void get_slice_header(H264Context *context, SliceHeader **out_header);

/**
 * @brief parse the NALU
 *
 * @param nalu_start the data start pointer
 * @param nalu_end the data end pointer(exclusive)
 * @param context the H264 context pointer
 * @param nalu out parameter, pointer to the NALU pointer
 * @return int 0 on success, negative value on error
 */
int parse_nalu(const uint8_t *nalu_start, const uint8_t *nalu_end, H264Context *context, void **nalu);

/**
 * @brief add sps to the context
 *
 * @param context the context pointer
 * @param sps the sps pointer
 * @return int 0 on success, negative value on error
 */
int add_sps_to_context(H264Context *context, SPS *sps);

/**
 * @brief add pps to the context
 *
 * @param context the context pointer
 * @param pps the pps pointer
 * @return int 0 on success, negative value on error
 */
int add_pps_to_context(H264Context *context, PPS *pps);

#endif