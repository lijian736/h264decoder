#ifndef _H_H264_PICTURE_H_
#define _H_H264_PICTURE_H_

#include <stdint.h>

#include "h264_defs.h"
#include "h264_error.h"
#include "h264_macroblock.h"
#include "h264_math.h"
#include "h264_nalu.h"

typedef struct {
    /* the coded type */
    PICTURE_CODED_TYPE coded_type;

    int32_t* mb_slice_ids;
    int mb_slice_ids_len;

    int32_t* mb_frame_flags;
    int mb_frame_flags_len;

    MacroBlock* mb_list;
    int mb_list_len;
    int current_mb;

} FrameOrField;

/**
 * @brief the picture
 */
typedef struct {
    /* the picture coded type */
    PICTURE_CODED_TYPE coded_type;

    FrameOrField* frame;
    FrameOrField* top_field;
    FrameOrField* bottom_field;
} Picture;

/**
 * @brief create a frame or field
 * 
 * @return FrameOrField* the picture pointer. 0 if creation fails
 */
FrameOrField* create_frame_or_field();

/**
 * @brief initialize the frame or field
 * 
 * @param ff pointer to FrameOrField 
 * @param slice_header pointer to the slice header
 * @return int 0 on success, negative value on error
 */
int init_frame_or_field(FrameOrField* ff, SliceHeader* slice_header);

/**
 * @brief reset frame of field
 * 
 * @param ff pointer to FrameOrField 
 */
void reset_frame_or_field(FrameOrField* ff);

/**
 * @brief free frame of field
 * 
 * @param ff pointer to FrameOrField 
 */
void free_frame_or_field(FrameOrField* ff);

/**
 * @brief create a picture
 *
 * @return Picture* the picture pointer. 0 if creation fails
 */
Picture* create_picture();

/**
 * @brief reset the picture
 *
 * @param picture the picture
 */
void reset_picture(Picture* picture);

/**
 * @brief free the picture
 *
 * @param picture pointer to the picture
 */
void free_picture(Picture* picture);

/**
 * @brief Detection of the first VCL NAL unit of a primary coded picture
 * @see 7.4.1.2.4 Detection of the first VCL NAL unit of a primary coded picture
 *
 * @param current the current slice header
 * @param prev the previous slice header
 * @return int 0 is not the first VCL NAL unit, otherwise is the first VCL NAL unit
 */
int detect_first_VCL_NAL_of_primary_coded_picture(SliceHeader* current, SliceHeader* prev);

/**
 * @brief decode a slice
 * @see 7.3.4 Slice data syntax
 * @see 7.4.4 Slice data semantics
 *
 * @param picture the picture
 * @param rbsp_reader the RBSP reader
 * @param header the slice header
 * @return int 0 on success, negative value on error
 */
int decode_slice(Picture* picture, RBSPReader* rbsp_reader, SliceHeader* header);

/**
 * @brief decode the slice data
 * @see 7.3.4 Slice data syntax
 * @see 7.4.4 Slice data semantics
 * 
 * @param ff pointer to FrameOrField
 * @param rbsp_reader the RBSP reader
 * @param header the slice header
 * @return int 0 on success, negative value on error
 */
int slice_data(FrameOrField* ff, RBSPReader* rbsp_reader, SliceHeader* header);

#endif