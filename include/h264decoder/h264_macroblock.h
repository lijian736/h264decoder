#ifndef _H_H264_MACROBLOCK_H_
#define _H_H264_MACROBLOCK_H_

#include <stdint.h>

#include "h264_cabac.h"
#include "h264_defs.h"
#include "h264_error.h"

struct FrameOrField;

/**
 * @brief get the macroblock partition width
 * @see Table 7-13 – Macroblock type values 0 to 4 for P and SP slices
 * @see Table 7-14 – Macroblock type values 0 to 22 for B slices
 *
 * @param mb_type the macroblock partition type
 * @param out_width output parameter. the partition width
 * @return int 0 on success, negative value on error
 */
inline int MbPartWidth(MB_TYPE_NAME mb_type, int32_t* out_width);

/**
 * @brief get the macroblock partition height
 * @see Table 7-13 – Macroblock type values 0 to 4 for P and SP slices
 * @see Table 7-14 – Macroblock type values 0 to 22 for B slices
 *
 * @param mb_type the macroblock partition type
 * @param out_height output parameter. the partition height
 * @return int 0 on success, negative value on error
 */
inline int MbPartHeight(MB_TYPE_NAME mb_type, int32_t* out_height);

/**
 * @brief get the macroblock partition width and height
 * @see Table 7-13 – Macroblock type values 0 to 4 for P and SP slices
 * @see Table 7-14 – Macroblock type values 0 to 22 for B slices
 *
 * @param mb_type the macroblock partition type
 * @param out_width output parameter. the partition width
 * @param out_height output parameter. the partition height
 * @return int 0 on success, negative value on error
 */
inline int MbPartWidthHeight(MB_TYPE_NAME mb_type, int32_t* out_width, int32_t* out_height);

/**
 * @brief get the sub-macroblock partition width
 * @see Table 7-17 – Sub-macroblock types in P macroblocks
 * @see Table 7-18 – Sub-macroblock types in B macroblocks
 *
 * @param mb_type the sub-macroblock partition type
 * @param out_width output parameter. the partition width
 * @return int 0 on success, negative value on error
 */
inline int SubMbPartWidth(MB_TYPE_NAME mb_type, int32_t* out_width);

/**
 * @brief get the sub-macroblock partition height
 * @see Table 7-17 – Sub-macroblock types in P macroblocks
 * @see Table 7-18 – Sub-macroblock types in B macroblocks
 *
 * @param mb_type the sub-macroblock partition type
 * @param out_height output parameter. the partition height
 * @return int 0 on success, negative value on error
 */
inline int SubMbPartHeight(MB_TYPE_NAME mb_type, int32_t* out_height);

/**
 * @brief get the sub-macroblock partition width and height
 * @see Table 7-17 – Sub-macroblock types in P macroblocks
 * @see Table 7-18 – Sub-macroblock types in B macroblocks
 *
 * @param mb_type the sub-macroblock partition type
 * @param out_width output parameter. the partition width
 * @param out_height output parameter. the partition height
 * @return int 0 on success, negative value on error
 */
inline int SubMbPartWidthHeight(MB_TYPE_NAME mb_type, int32_t* out_width, int32_t* out_height);

/**
 * @brief get the macroblock or sub macroblock partition width and height
 *
 * @param mb_type the macroblock or sub macroblock type
 * @param out_width output parameter. the partition width
 * @param out_height output parameter. the partition height
 * @return int 0 on success, negative value on error
 */
int MbSubMbPartWidthHeight(MB_TYPE_NAME mb_type, int32_t* out_width, int32_t* out_height);

/**
 * @brief macroblock partition prediction mode
 *
 * @param slice_type the slice type
 * @param transform_size_8x8_flag the transform_size_8x8_flag， only valid for I slice type
 * @param mb_type macroblock type
 * @param index index of macroblock partition, it has value 0 or 1
 * @param out_mb_type_name output parameter. the macroblock type name
 * @param out_mb_pred_mode output parameter. the macroblock partition predict mode
 * @return int 0 on success, negative value on error
 */
int MbPartPredMode(int32_t slice_type, int32_t transform_size_8x8_flag, int32_t mb_type, int32_t index, MB_TYPE_NAME* out_mb_type_name, H264_MB_PART_PRED_MODE* out_mb_pred_mode);

/**
 * @brief num of macroblock partition
 *
 * @param slice_type the slice type
 * @param mb_type macroblock type
 * @param out_num output parameter. the partition number
 */
void NumMbPart(int32_t slice_type, int32_t mb_type, int32_t* out_num);

/**
 * @brief get the CodedBlockPatternLuma and CodedBlockPatternChroma
 *
 * @param slice_type the slice type
 * @param mb_type macroblock type
 * @param out_luma output parameter. the luma value
 * @param out_chroma output parameter. the chroma value
 */
void CodedBlockPatternLumaChroma(int32_t slice_type, int32_t mb_type, int32_t* out_luma, int32_t* out_chroma);

/**
 * @brief parse macroblock layer
 * @see 7.3.5 Macroblock layer syntax
 * @see 7.4.5 Macroblock layer semantics
 *
 * @param rbsp_reader the RBSPReader
 * @param picture pointer to the FrameOrField
 * @param slice_header pointer to the slice header
 * @param cabac pointer to the CABAC
 * @param CurrMbAddr the current macroblock address
 * @return int 0 on success, negative value on error
 */
int macroblock_layer(RBSPReader* rbsp_reader, FrameOrField* picture, SliceHeader* slice_header, CABAC* cabac, int32_t CurrMbAddr);

/**
 * @brief revise the slice type and macroblock type
 *
 * @param slice_type the slice type
 * @param mb_type the macroblock type
 * @param revised_slice_type output parameter. the revised slice type
 * @param revised_mb_type output parameter. the revised macroblock type
 * @return int 0 on success, negative value on error
 */
int revise_slice_type_mb_type(int32_t slice_type, int32_t mb_type, int32_t* revised_slice_type, int32_t* revised_mb_type);

/**
 * @brief macroblock prediction
 * @see 7.3.5.1 Macroblock prediction syntax
 * @see 7.4.5.1 Macroblock prediction semantics
 *
 * @param rbsp_reader the RBSPReader
 * @param picture pointer to the FrameOrField
 * @param mb the macroblock
 * @param slice_header pointer to the slice header
 * @param cabac pointer to the CABAC
 * @param CurrMbAddr the current macroblock address
 * @return int 0 on success, negative value on error
 */
int mb_pred(RBSPReader* rbsp_reader, FrameOrField* picture, MacroBlock* mb, SliceHeader* slice_header, CABAC* cabac, int32_t CurrMbAddr);

#endif