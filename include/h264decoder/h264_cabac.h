#ifndef _H_H264_CABAC_H_
#define _H_H264_CABAC_H_

#include <stddef.h>
#include <stdint.h>

#include "h264_defs.h"
#include "h264_error.h"
#include "h264_picture.h"
#include "h264_rbsp.h"

/**
 * Context-based Adaptive Binary Arithmetic Coding, CABAC
 *
 * @see 9.3 CABAC parsing process for slice data
 * @see 7.3.4 Slice data syntax
 * @see 7.3.5 Macroblock layer syntax
 *
 * This process is invoked when parsing syntax elements with descriptor ae(v) in clauses 7.3.4 and 7.3.5 when entropy_coding_mode_flag is equal to 1.
 *
 * Inputs to this process are a request for a value of a syntax element and values of prior parsed syntax elements.
 * Output of this process is the value of the syntax element.
 *
 * When starting the parsing of the slice data of a slice in clause 7.3.4, the initialization process of the CABAC parsing process is invoked as
 * specified in clause 9.3.1.
 */

typedef struct {
    /* probability state index */
    int32_t pStateIdx[H264_MAX_CONTEXT_INDEX];

    /* the value of the most probable symbol*/
    int32_t valMPS[H264_MAX_CONTEXT_INDEX];

    int32_t codIRange;
    int32_t codIOffset;
} CABAC;

/**
 * @brief get the singleton cabac object
 *
 * @return CABAC* the singleton cabac
 */
CABAC* get_singleton_cabac();

/**
 * @brief initialize context variable when starting the parsing of the slice data of a slice in clause 7.3.4
 *
 * @see 7.3.4 Slice data syntax
 * @see 9.3.1.1 Initialization process for context variables
 *
 * @param cabac pointer to the CABAC
 * @param slice_type the slice type
 * @param cabac_init_idc the cabac init index
 * @param SliceQPY the qpy
 * @return int 0 on success, negative value on error
 */
int cabac_init_context_variables(CABAC* cabac, uint32_t slice_type, uint32_t cabac_init_idc, int32_t SliceQPY);

/**
 * @brief retrieve the m and n for CABAC
 *
 * @param ctxIdx the context index
 * @param slice_type the slice type
 * @param cabac_init_idc the cabac init index
 * @param m_out output parameter. the output m
 * @param n_out output parameter. the output n
 * @return int 0 on success, negative value on error
 */
int cabac_retrieve_m_n(int32_t ctxIdx, uint32_t slice_type, uint32_t cabac_init_idc, int8_t* m_out, int8_t* n_out);

/**
 * @brief initialize the arithmetic decoding engine
 * @see 9.3.1.2 Initialization process for the arithmetic decoding engine
 *
 * This process is invoked before decoding the first macroblock of a slice or after the decoding of any pcm_alignment_zero_bit and all pcm_sample_luma and pcm_sample_chroma data
 * for a macroblock of type I_PCM
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac pointer to the CABAC
 * @return int 0 on success, negative value on error
 */
int cabac_init_arithmetic_decoding_engine(RBSPReader* rbsp_reader, CABAC* cabac);

/**
 * @brief decode the unary binarization
 * @see 9.3.2.1 Unary (U) binarization process
 *
 * @param rbsp_reader the RBSPReader
 * @param out_synElVal output parameter. the syntax element value
 */
void cabac_decode_unary_binarization(RBSPReader* rbsp_reader, uint32_t* out_synElVal);

/**
 * @brief decode the truncated unary binarization
 * @see 9.3.2.2 Truncated unary (TU) binarization process
 *
 * @param rbsp_reader the RBSPReader
 * @param cMax the max value of the syntax element
 * @param out_synElVal output parameter. the syntax element value
 */
void cabac_decode_truncated_unary_binarization(RBSPReader* rbsp_reader, int32_t cMax, uint32_t* out_synElVal);

/**
 * @brief decode mb_skip_flag
 * @see 7.3.4 Slice data syntax
 * @see Table 9-11 – Association of ctxIdx and syntax elements for each slice type in the initialization process
 * @see Table 9-13 – Values of variables m and n for ctxIdx from 11 to 23
 * @see Table 9-14 – Values of variables m and n for ctxIdx from 24 to 39
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 * @see Clause 9.3.3.1.1.1 specifies the derivation process of ctxIdxInc for the syntax element mb_skip_flag.
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_mb_skip_flag(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);

/**
 * @brief decode mb_field_decoding_flag
 * @see 7.3.4 Slice data syntax
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 * @see 9.3.3.1.1.2 Derivation process of ctxIdxInc for the syntax element mb_field_decoding_flag
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_mb_field_decoding_flag(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);

/**
 * @brief decode
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_end_of_slice_flag(RBSPReader* rbsp_reader, CABAC* cabac, int32_t* out_syntax_element);

/**
 * @brief decode mb_type
 * @see 7.3.5 Macroblock layer syntax
 * @see 7.4.5 Macroblock layer semantics
 * @see 9.3.2.5 Binarization process for macroblock type and sub-macroblock type
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_mb_type(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);
int cabac_mb_type_for_SI_slices(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t maxBinIdxCtx,
                                int32_t ctxIdxOffset, int32_t* out_syntax_element);
int cabac_mb_type_for_I_slices(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t maxBinIdxCtx,
                               int32_t ctxIdxOffset, int32_t* out_syntax_element);
int cabac_mb_type_for_B_slices(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, int32_t CurrMbAddr, int32_t maxBinIdxCtx, int32_t ctxIdxOffset,
                               int32_t* out_syntax_element);
int cabac_mb_type_for_P_SP_slices(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, int32_t CurrMbAddr, int32_t maxBinIdxCtx, int32_t ctxIdxOffset,
                                  int32_t* out_syntax_element);

/**
 * @brief decode sub_mb_type
 *
 * @see 7.3.5.2 Sub-macroblock prediction syntax
 * @see 7.4.5.2 Sub-macroblock prediction semantics
 * @see 9.3.2.5 Binarization process for macroblock type and sub-macroblock type
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_sub_mb_type(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);
int cabac_sub_mb_type_for_B_slices(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);
int cabac_sub_mb_type_for_P_SP_slices(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);

/**
 * @brief decode transform_size_8x8_flag
 *
 * @see 9.3.3.1.1.10 Derivation process of ctxIdxInc for the syntax element transform_size_8x8_flag
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_transform_size_8x8_flag(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);

/**
 * @brief decode coded_block_pattern
 *
 * @see 9.3.3.1.1.4 Derivation process of ctxIdxInc for the syntax element coded_block_pattern
 * @see 9.3.2.6 Binarization process for coded block pattern
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param ChromaArrayType the chroma array type
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_coded_block_pattern(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t ChromaArrayType,
                              int32_t* out_syntax_element);

/**
 * @brief decode mb_qp_delta
 *
 * @see 9.3.2.7 Binarization process for mb_qp_delta
 * @see 9.3.3.1.1.5 Derivation process of ctxIdxInc for the syntax element mb_qp_delta
 * @see Table 9-3 – Assignment of syntax element to codeNum for signed Exp-Golomb coded syntax elements se(v)
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_mb_qp_delta(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);

/**
 * @brief decode prev_intra4x4_pred_mode_flag or prev_intra8x8_pred_mode_flag
 *
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_prev_intra4x4_or_intra8x8_pred_mode_flag(RBSPReader* rbsp_reader, CABAC* cabac, int32_t* out_syntax_element);

/**
 * @brief decode rem_intra4x4_pred_mode or rem_intra8x8_pred_mode
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_rem_intra4x4_or_intra8x8_pred_mode(RBSPReader* rbsp_reader, CABAC* cabac, int32_t* out_syntax_element);

/**
 * @brief decode intra_chroma_pred_mode
 *
 * @see Table 9-34 – Syntax elements and associated types of binarization, maxBinIdxCtx, and ctxIdxOffset
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error
 */
int cabac_intra_chroma_pred_mode(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_syntax_element);

/**
 * @brief decode coded_block_flag
 * 
 * @param rbsp_reader the RBSPReader
 * @param cabac the cabac
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param ctxBlockCat the context block categories
 * @param xBlkIdx the x block index
 * @param iCbCr the index of Cb or Cr
 * @param out_syntax_element output parameter. the syntax element value
 * @return int 0 on success, negative value on error 
 */
int cabac_coded_block_flag(RBSPReader* rbsp_reader, CABAC* cabac, FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t ctxBlockCat, int32_t xBlkIdx,
                           int32_t iCbCr, int32_t* out_syntax_element);

/**
 * @brief decode binary value
 * @see 9.3.3.2 Arithmetic decoding process
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac pointer to the CABAC
 * @param bypassFlag bypass flag
 * @param ctxIdx the context index
 * @param bin_val output parameter. the binary value
 * @return int 0 on success, negative value on error
 */
int DecodeBin(RBSPReader* rbsp_reader, CABAC* cabac, int32_t bypassFlag, int32_t ctxIdx, int32_t* bin_val);

/**
 * @brief bypass decoding process for binary decisions
 * @see 9.3.3.2.3 Bypass decoding process for binary decisions
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac pointer to the CABAC
 * @param bin_val output parameter. the binary value
 * @return int 0 on success, negative value on error
 */
int DecodeBypass(RBSPReader* rbsp_reader, CABAC* cabac, int32_t* bin_val);

/**
 * @brief Decoding process for binary decisions before termination
 * @see 9.3.3.2.4 Decoding process for binary decisions before termination
 *
 * @param rbsp_reader  the RBSPReader
 * @param cabac pointer to the CABAC
 * @param bin_val output parameter. the binary value
 * @return int 0 on success, negative value on error
 */
int DecodeTerminate(RBSPReader* rbsp_reader, CABAC* cabac, int32_t* bin_val);

/**
 * @brief Renormalization process in the arithmetic decoding engine
 * @see 9.3.3.2.2 Renormalization process in the arithmetic decoding engine
 *
 * @param rbsp_reader  the RBSPReader
 * @param cabac pointer to the CABAC
 * @return int 0 on success, negative value on error
 */
int RenormD(RBSPReader* rbsp_reader, CABAC* cabac);

/**
 * @brief Arithmetic decoding process for a binary decision
 * @see 9.3.3.2.1 Arithmetic decoding process for a binary decision
 *
 * @param rbsp_reader the RBSPReader
 * @param cabac pointer to the CABAC
 * @param ctxIdx the context index
 * @param bin_val output parameter. the binary value
 * @return int 0 on success, negative value on error
 */
int DecodeDecision(RBSPReader* rbsp_reader, CABAC* cabac, int32_t ctxIdx, int32_t* bin_val);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element mb_skip_flag
 * @see 9.3.3.1.1.1 Derivation process of ctxIdxInc for the syntax element mb_skip_flag
 *
 * Output of this process is ctxIdxInc.
 *
 * @param picture the picture data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_mb_skip_flag(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_ctxIdxInc);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element mb_field_decoding_flag
 * @see 9.3.3.1.1.2 Derivation process of ctxIdxInc for the syntax element mb_field_decoding_flag
 *
 * Output of this process is ctxIdxInc.
 *
 * @param picture the picture data
 * @param CurrMbAddr the current macroblock address
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_mb_field_decoding_flag(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_ctxIdxInc);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element mb_type
 * @see 9.3.3.1.1.3 Derivation process of ctxIdxInc for the syntax element mb_type
 *
 * Input to this process is ctxIdxOffset.
 * Output of this process is ctxIdxInc.
 *
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param ctxIdxOffset the ctxIdxOffset
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_mb_type(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t ctxIdxOffset, int32_t* out_ctxIdxInc);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element coded_block_pattern
 * @see 9.3.3.1.1.4 Derivation process of ctxIdxInc for the syntax element coded_block_pattern
 * @see 9.3.2.6 Binarization process for coded block pattern
 * @see Table 9-39 – Assignment of ctxIdxInc to binIdx for all ctxIdxOffset values except those related to the syntax elements coded_block_flag, significant_coeff_flag,
 * last_significant_coeff_flag, and coeff_abs_level_minus1
 *
 * Inputs to this process are ctxIdxOffset and binIdx
 * Output of this process is ctxIdxInc.
 *
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param ctxIdxOffset the ctxIdxOffset
 * @param binIdx the bin index
 * @param binValues the decoded bin string
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_coded_block_pattern(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t ctxIdxOffset, int32_t binIdx, int32_t binValues,
                                                 int32_t* out_ctxIdxInc);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element mb_qp_delta
 * @see 9.3.3.1.1.5 Derivation process of ctxIdxInc for the syntax element mb_qp_delta
 *
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_mb_qp_delta(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_ctxIdxInc);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element intra_chroma_pred_mode
 * @see 9.3.3.1.1.8 Derivation process of ctxIdxInc for the syntax element intra_chroma_pred_mode
 *
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_intra_chroma_pred_mode(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t* out_ctxIdxInc);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element coded_block_flag
 * @see 9.3.3.1.1.9 Derivation process of ctxIdxInc for the syntax element coded_block_flag
 *
 * Input to this process is ctxBlockCat and additional input is specified as follows:
 *  – If ctxBlockCat is equal to 0, 6, or 10, no additional input.
 *  – Otherwise, if ctxBlockCat is equal to 1 or 2, luma4x4BlkIdx.
 *  – Otherwise, if ctxBlockCat is equal to 3, the chroma component index iCbCr.
 *  – Otherwise, if ctxBlockCat is equal to 4, chroma4x4BlkIdx and the chroma component index iCbCr.
 *  – Otherwise, if ctxBlockCat is equal to 5, luma8x8BlkIdx.
 *  – Otherwise, if ctxBlockCat is equal to 7 or 8, cb4x4BlkIdx.
 *  – Otherwise, if ctxBlockCat is equal to 9, cb8x8BlkIdx.
 *  – Otherwise, if ctxBlockCat is equal to 11 or 12, cr4x4BlkIdx.
 *  – Otherwise (ctxBlockCat is equal to 13), cr8x8BlkIdx.
 *
 * Output of this process is ctxIdxInc( ctxBlockCat ).
 *
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param ctxBlockCat the context block categories
 * @param xBlkIdx the x block index
 * @param iCbCr the index of Cb or Cr
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_coded_block_flag(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t ctxBlockCat, int32_t xBlkIdx, int32_t iCbCr,
                                              int32_t* out_ctxIdxInc);

/**
 * @brief Derivation process of ctxIdxInc for the syntax element transform_size_8x8_flag
 * @see 9.3.3.1.1.10 Derivation process of ctxIdxInc for the syntax element transform_size_8x8_flag
 *
 * @param picture the FrameOrField data
 * @param slice_header the slice header
 * @param CurrMbAddr the current macroblock address
 * @param ctxIdxOffset the ctxIdxOffset
 * @param out_ctxIdxInc output parameter. the ctxIdxInc
 * @return int 0 on success, negative value on error
 */
int derivation_for_ctxIdxInc_transform_size_8x8_flag(FrameOrField* picture, SliceHeader* slice_header, int32_t CurrMbAddr, int32_t ctxIdxOffset, int32_t* out_ctxIdxInc);

#endif