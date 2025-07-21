#include "h264decoder/h264_macroblock.h"

#include "h264decoder/h264_picture.h"

/**
 * @brief macroblock types for I slices
 * @see Table 7-11 – Macroblock types for I slices
 *
 * {
 *   mb_type
 *   Name of mb_type
 *   transform_size_8x8_flag
 *   MbPartPredMode(mb_type, 0)
 *   Intra16x16PredMode
 *   CodedBlockPatternChroma
 *   CodedBlockPatternLuma
 * }
 */
static const MB_TYPE_FOR_I_SLICES g_mb_type_for_i_slices[27] = {
    {0, I_NxN, 0, Intra_4x4, -1, -1, -1},           {0, I_NxN, 1, Intra_8x8, -1, -1, -1},           {1, I_16x16_0_0_0, -1, Intra_16x16, 0, 0, 0},
    {2, I_16x16_1_0_0, -1, Intra_16x16, 1, 0, 0},   {3, I_16x16_2_0_0, -1, Intra_16x16, 2, 0, 0},   {4, I_16x16_3_0_0, -1, Intra_16x16, 3, 0, 0},
    {5, I_16x16_0_1_0, -1, Intra_16x16, 0, 1, 0},   {6, I_16x16_1_1_0, -1, Intra_16x16, 1, 1, 0},   {7, I_16x16_2_1_0, -1, Intra_16x16, 2, 1, 0},
    {8, I_16x16_3_1_0, -1, Intra_16x16, 3, 1, 0},   {9, I_16x16_0_2_0, -1, Intra_16x16, 0, 2, 0},   {10, I_16x16_1_2_0, -1, Intra_16x16, 1, 2, 0},
    {11, I_16x16_2_2_0, -1, Intra_16x16, 2, 2, 0},  {12, I_16x16_3_2_0, -1, Intra_16x16, 3, 2, 0},  {13, I_16x16_0_0_1, -1, Intra_16x16, 0, 0, 15},
    {14, I_16x16_1_0_1, -1, Intra_16x16, 1, 0, 15}, {15, I_16x16_2_0_1, -1, Intra_16x16, 2, 0, 15}, {16, I_16x16_3_0_1, -1, Intra_16x16, 3, 0, 15},
    {17, I_16x16_0_1_1, -1, Intra_16x16, 0, 1, 15}, {18, I_16x16_1_1_1, -1, Intra_16x16, 1, 1, 15}, {19, I_16x16_2_1_1, -1, Intra_16x16, 2, 1, 15},
    {20, I_16x16_3_1_1, -1, Intra_16x16, 3, 1, 15}, {21, I_16x16_0_2_1, -1, Intra_16x16, 0, 2, 15}, {22, I_16x16_1_2_1, -1, Intra_16x16, 1, 2, 15},
    {23, I_16x16_2_2_1, -1, Intra_16x16, 2, 2, 15}, {24, I_16x16_3_2_1, -1, Intra_16x16, 3, 2, 15}, {25, I_PCM, -1, Intra_NA, -1, -1, -1},
};

/**
 * @brief Macroblock type with value 0 for SI slices
 * @see Table 7-12 – Macroblock type with value 0 for SI slices
 *
 * {
 *   mb_type
 *   Name of mb_type
 *   MbPartPredMode(mb_type, 0)
 *   Intra16x16PredMode
 *   CodedBlockPatternChroma
 *   CodedBlockPatternLuma
 * }
 */
static const MB_TYPE_FOR_SI_SLICES g_mb_type_for_si_slices[1] = {
    {0, SI_SI, Intra_4x4, -1, -1, -1},
};

/**
 * @brief Macroblock type values 0 to 4 for P and SP slices
 * @see Table 7-13 – Macroblock type values 0 to 4 for P and SP slices
 *
 * {
 *  mb_type
 *  Name of mb_type
 *  NumMbPart( mb_type )
 *  MbPartPredMode( mb_type, 0 )
 *  MbPartPredMode( mb_type, 1 )
 *  MbPartWidth( mb_type )
 *  MbPartHeight( mb_type )
 * }
 */
static const MB_TYPE_FOR_P_SP_SLICES g_mb_type_for_p_sp_slices[6] = {
    {0, P_L0_16x16, 1, Pred_L0, Pred_NA, 16, 16}, {1, P_L0_L0_16x8, 2, Pred_L0, Pred_L0, 16, 8}, {2, P_L0_L0_8x16, 2, Pred_L0, Pred_L0, 8, 16},
    {3, P_8x8, 4, Pred_NA, Pred_NA, 8, 8},        {4, P_8x8ref0, 4, Pred_NA, Pred_NA, 8, 8},     {5, P_Skip, 1, Pred_L0, Pred_NA, 16, 16},
};

/**
 * @brief Macroblock type values 0 to 22 for B slices
 * @see Table 7-14 – Macroblock type values 0 to 22 for B slices
 *
 * {
 *  mb_type
 *  Name of mb_type
 *  NumMbPart( mb_type )
 *  MbPartPredMode( mb_type, 0 )
 *  MbPartPredMode( mb_type, 1 )
 *  MbPartWidth( mb_type )
 *  MbPartHeight( mb_type )
 * }
 */
static const MB_TYPE_FOR_B_SLICES g_mb_type_for_b_slices[24] = {
    {0, B_Direct_16x16, -1, Direct, Pred_NA, 8, 8}, {1, B_L0_16x16, 1, Pred_L0, Pred_NA, 16, 16},   {2, B_L1_16x16, 1, Pred_L1, Pred_NA, 16, 16},
    {3, B_Bi_16x16, 1, BiPred, Pred_NA, 16, 16},    {4, B_L0_L0_16x8, 2, Pred_L0, Pred_L0, 16, 8},  {5, B_L0_L0_8x16, 2, Pred_L0, Pred_L0, 8, 16},
    {6, B_L1_L1_16x8, 2, Pred_L1, Pred_L1, 16, 8},  {7, B_L1_L1_8x16, 2, Pred_L1, Pred_L1, 8, 16},  {8, B_L0_L1_16x8, 2, Pred_L0, Pred_L1, 16, 8},
    {9, B_L0_L1_8x16, 2, Pred_L0, Pred_L1, 8, 16},  {10, B_L1_L0_16x8, 2, Pred_L1, Pred_L0, 16, 8}, {11, B_L1_L0_8x16, 2, Pred_L1, Pred_L0, 8, 16},
    {12, B_L0_Bi_16x8, 2, Pred_L0, BiPred, 16, 8},  {13, B_L0_Bi_8x16, 2, Pred_L0, BiPred, 8, 16},  {14, B_L1_Bi_16x8, 2, Pred_L1, BiPred, 16, 8},
    {15, B_L1_Bi_8x16, 2, Pred_L1, BiPred, 8, 16},  {16, B_Bi_L0_16x8, 2, BiPred, Pred_L0, 16, 8},  {17, B_Bi_L0_8x16, 2, BiPred, Pred_L0, 8, 16},
    {18, B_Bi_L1_16x8, 2, BiPred, Pred_L1, 16, 8},  {19, B_Bi_L1_8x16, 2, BiPred, Pred_L1, 8, 16},  {20, B_Bi_Bi_16x8, 2, BiPred, BiPred, 16, 8},
    {21, B_Bi_Bi_8x16, 2, BiPred, BiPred, 8, 16},   {22, B_8x8, 4, Pred_NA, Pred_NA, 8, 8},         {23, B_Skip, -1, Direct, Pred_NA, 8, 8},
};

/**
 * @brief Sub-macroblock types in P macroblocks
 * @see Table 7-17 – Sub-macroblock types in P macroblocks
 *
 * {
 *  sub_mb_type[ mbPartIdx ]
 *  Name of sub_mb_type[ mbPartIdx ]
 *  NumSubMbPart( sub_mb_type[ mbPartIdx ] )
 *  SubMbPredMode( sub_mb_type[ mbPartIdx ] )
 *  SubMbPartWidth( sub_mb_type[ mbPartIdx ] )
 *  SubMbPartHeight( sub_mb_type[ mbPartIdx ] )
 * }
 */
static const SUB_MB_TYPE_IN_P_MB g_sub_mb_type_p_mb[4] = {
    {0, P_L0_8x8, 1, Pred_L0, 8, 8},
    {1, P_L0_8x4, 2, Pred_L0, 8, 4},
    {2, P_L0_4x8, 2, Pred_L0, 4, 8},
    {3, P_L0_4x4, 4, Pred_L0, 4, 4},
};

/**
 * @brief Sub-macroblock types in B macroblocks
 * @see Table 7-18 – Sub-macroblock types in B macroblocks
 *
 * {
 *  sub_mb_type[ mbPartIdx ]
 *  Name of sub_mb_type[ mbPartIdx ]
 *  NumSubMbPart ( sub_mb_type[ mbPartIdx ] )
 *  SubMbPredMode( sub_mb_type[ mbPartIdx ] )
 *  SubMbPartWidth( sub_mb_type[ mbPartIdx ] )
 *  SubMbPartHeight( sub_mb_type[ mbPartIdx ] )
 * }
 */
static const SUB_MB_TYPE_IN_B_MB g_sub_mb_type_b_mb[13] = {
    {0, B_Direct_8x8, 4, Direct, 4, 4}, {1, B_L0_8x8, 1, Pred_L0, 8, 8},  {2, B_L1_8x8, 1, Pred_L1, 8, 8}, {3, B_Bi_8x8, 1, BiPred, 8, 8}, {4, B_L0_8x4, 2, Pred_L0, 8, 4},
    {5, B_L0_4x8, 2, Pred_L0, 4, 8},    {6, B_L1_8x4, 2, Pred_L1, 8, 4},  {7, B_L1_4x8, 2, Pred_L1, 4, 8}, {8, B_Bi_8x4, 2, BiPred, 8, 4}, {9, B_Bi_4x8, 2, BiPred, 4, 8},
    {10, B_L0_4x4, 4, Pred_L0, 4, 4},   {11, B_L1_4x4, 4, Pred_L1, 4, 4}, {12, B_Bi_4x4, 4, BiPred, 4, 4},
};

int MbPartWidth(MB_TYPE_NAME mb_type, int32_t* out_width) {
    if (mb_type >= P_L0_16x16 && mb_type <= P_Skip) {
        *out_width = g_mb_type_for_p_sp_slices[mb_type - P_L0_16x16].MbPartWidth;
    } else if (mb_type >= B_Direct_16x16 && mb_type <= B_Skip) {
        *out_width = g_mb_type_for_b_slices[mb_type - B_Direct_16x16].MbPartWidth;
    } else {
        return ERR_UNRECOGNIZED_MB;
    }

    return ERR_OK;
}

int MbPartHeight(MB_TYPE_NAME mb_type, int32_t* out_height) {
    if (mb_type >= P_L0_16x16 && mb_type <= P_Skip) {
        *out_height = g_mb_type_for_p_sp_slices[mb_type - P_L0_16x16].MbPartHeight;
    } else if (mb_type >= B_Direct_16x16 && mb_type <= B_Skip) {
        *out_height = g_mb_type_for_b_slices[mb_type - B_Direct_16x16].MbPartHeight;
    } else {
        return ERR_UNRECOGNIZED_MB;
    }

    return ERR_OK;
}

int MbPartWidthHeight(MB_TYPE_NAME mb_type, int32_t* out_width, int32_t* out_height) {
    if (mb_type >= P_L0_16x16 && mb_type <= P_Skip) {
        *out_width = g_mb_type_for_p_sp_slices[mb_type - P_L0_16x16].MbPartWidth;
        *out_height = g_mb_type_for_p_sp_slices[mb_type - P_L0_16x16].MbPartHeight;
    } else if (mb_type >= B_Direct_16x16 && mb_type <= B_Skip) {
        *out_width = g_mb_type_for_b_slices[mb_type - B_Direct_16x16].MbPartWidth;
        *out_height = g_mb_type_for_b_slices[mb_type - B_Direct_16x16].MbPartHeight;
    } else {
        return ERR_UNRECOGNIZED_MB;
    }

    return ERR_OK;
}

int SubMbPartWidth(MB_TYPE_NAME mb_type, int32_t* out_width) {
    if (mb_type >= P_L0_8x8 && mb_type <= P_L0_4x4) {
        *out_width = g_sub_mb_type_p_mb[mb_type - P_L0_8x8].SubMbPartWidth;
    } else if (mb_type >= B_Direct_8x8 && mb_type <= B_Bi_4x4) {
        *out_width = g_sub_mb_type_b_mb[mb_type - B_Direct_8x8].SubMbPartWidth;
    } else {
        return ERR_UNRECOGNIZED_MB;
    }

    return ERR_OK;
}

int SubMbPartHeight(MB_TYPE_NAME mb_type, int32_t* out_height) {
    if (mb_type >= P_L0_8x8 && mb_type <= P_L0_4x4) {
        *out_height = g_sub_mb_type_p_mb[mb_type - P_L0_8x8].SubMbPartHeight;
    } else if (mb_type >= B_Direct_8x8 && mb_type <= B_Bi_4x4) {
        *out_height = g_sub_mb_type_b_mb[mb_type - B_Direct_8x8].SubMbPartHeight;
    } else {
        return ERR_UNRECOGNIZED_MB;
    }

    return ERR_OK;
}

int SubMbPartWidthHeight(MB_TYPE_NAME mb_type, int32_t* out_width, int32_t* out_height) {
    if (mb_type >= P_L0_8x8 && mb_type <= P_L0_4x4) {
        *out_width = g_sub_mb_type_p_mb[mb_type - P_L0_8x8].SubMbPartWidth;
        *out_height = g_sub_mb_type_p_mb[mb_type - P_L0_8x8].SubMbPartHeight;
    } else if (mb_type >= B_Direct_8x8 && mb_type <= B_Bi_4x4) {
        *out_width = g_sub_mb_type_b_mb[mb_type - B_Direct_8x8].SubMbPartWidth;
        *out_height = g_sub_mb_type_b_mb[mb_type - B_Direct_8x8].SubMbPartHeight;
    } else {
        return ERR_UNRECOGNIZED_MB;
    }

    return ERR_OK;
}

int MbSubMbPartWidthHeight(MB_TYPE_NAME mb_type, int32_t* out_width, int32_t* out_height) {
    if (mb_type >= P_L0_16x16 && mb_type <= P_Skip) {
        *out_width = g_mb_type_for_p_sp_slices[mb_type - P_L0_16x16].MbPartWidth;
        *out_height = g_mb_type_for_p_sp_slices[mb_type - P_L0_16x16].MbPartHeight;
    } else if (mb_type >= B_Direct_16x16 && mb_type <= B_Skip) {
        *out_width = g_mb_type_for_b_slices[mb_type - B_Direct_16x16].MbPartWidth;
        *out_height = g_mb_type_for_b_slices[mb_type - B_Direct_16x16].MbPartHeight;
    } else if (mb_type >= P_L0_8x8 && mb_type <= P_L0_4x4) {
        *out_width = g_sub_mb_type_p_mb[mb_type - P_L0_8x8].SubMbPartWidth;
        *out_height = g_sub_mb_type_p_mb[mb_type - P_L0_8x8].SubMbPartHeight;
    } else if (mb_type >= B_Direct_8x8 && mb_type <= B_Bi_4x4) {
        *out_width = g_sub_mb_type_b_mb[mb_type - B_Direct_8x8].SubMbPartWidth;
        *out_height = g_sub_mb_type_b_mb[mb_type - B_Direct_8x8].SubMbPartHeight;
    } else {
        return ERR_UNRECOGNIZED_MB;
    }

    return ERR_OK;
}

int MbPartPredMode(int32_t slice_type, int32_t transform_size_8x8_flag, int32_t mb_type, int32_t index, MB_TYPE_NAME* out_mb_type_name, H264_MB_PART_PRED_MODE* out_mb_pred_mode) {
    slice_type %= 5;

    if (slice_type == SLICE_TYPE_I) {
        if (mb_type == 0) {
            if (transform_size_8x8_flag == 0) {
                *out_mb_type_name = g_mb_type_for_i_slices[0].mb_type_name;
                *out_mb_pred_mode = g_mb_type_for_i_slices[0].MbPartPredMode;
            } else {
                *out_mb_type_name = g_mb_type_for_i_slices[1].mb_type_name;
                *out_mb_pred_mode = g_mb_type_for_i_slices[1].MbPartPredMode;
            }
        } else if (mb_type >= 1 && mb_type <= 25) {
            *out_mb_type_name = g_mb_type_for_i_slices[mb_type + 1].mb_type_name;
            *out_mb_pred_mode = g_mb_type_for_i_slices[mb_type + 1].MbPartPredMode;
        } else {
            return ERR_UNKNOWN_MB_PART_PRED_MODE;
        }
    } else if (slice_type == SLICE_TYPE_SI) {
        if (mb_type == 0) {
            *out_mb_type_name = g_mb_type_for_si_slices[0].mb_type_name;
            *out_mb_pred_mode = g_mb_type_for_si_slices[0].MbPartPredMode;
        } else {
            return ERR_UNKNOWN_MB_PART_PRED_MODE;
        }
    } else if (slice_type == SLICE_TYPE_P || slice_type == SLICE_TYPE_SP) {
        if (mb_type >= 0 && mb_type <= 5) {
            *out_mb_type_name = g_mb_type_for_p_sp_slices[mb_type].mb_type_name;
            if (index == 0) {
                *out_mb_pred_mode = g_mb_type_for_p_sp_slices[mb_type].MbPartPredMode0;
            } else {
                *out_mb_pred_mode = g_mb_type_for_p_sp_slices[mb_type].MbPartPredMode1;
            }
        } else {
            return ERR_UNKNOWN_MB_PART_PRED_MODE;
        }
    } else if (slice_type == SLICE_TYPE_B) {
        if (mb_type >= 0 && mb_type <= 23) {
            *out_mb_type_name = g_mb_type_for_b_slices[mb_type].mb_type_name;
            if (index == 0) {
                *out_mb_pred_mode = g_mb_type_for_b_slices[mb_type].MbPartPredMode0;
            } else {
                *out_mb_pred_mode = g_mb_type_for_b_slices[mb_type].MbPartPredMode1;
            }
        } else {
            return ERR_UNKNOWN_MB_PART_PRED_MODE;
        }
    }

    return ERR_OK;
}

void NumMbPart(int32_t slice_type, int32_t mb_type, int32_t* out_num) {
    slice_type %= 5;

    *out_num = 0;

    if (slice_type == SLICE_TYPE_P || slice_type == SLICE_TYPE_SP) {
        if (mb_type >= 0 && mb_type <= 5) {
            *out_num = g_mb_type_for_p_sp_slices[mb_type].NumMbPart;
        }
    } else if (slice_type == SLICE_TYPE_B) {
        if (mb_type >= 0 && mb_type <= 23) {
            *out_num = g_mb_type_for_b_slices[mb_type].NumMbPart;
        }
    }
}

void CodedBlockPatternLumaChroma(int32_t slice_type, int32_t mb_type, int32_t* out_luma, int32_t* out_chroma) {
    slice_type %= 5;

    *out_luma = 0;
    *out_chroma = 0;

    if (slice_type == SLICE_TYPE_I) {
        if (mb_type >= 1 && mb_type <= 24) {
            *out_chroma = g_mb_type_for_i_slices[mb_type + 1].CodedBlockPatternChroma;
            *out_luma = g_mb_type_for_i_slices[mb_type + 1].CodedBlockPatternLuma;
        }
    }
}

/* 7.3.5 Macroblock layer syntax */
/* 7.4.5 Macroblock layer semantics */
int macroblock_layer(RBSPReader* rbsp_reader, FrameOrField* picture, SliceHeader* slice_header, CABAC* cabac, int32_t CurrMbAddr) {
    int err_code = ERR_OK;

    PPS* pps = slice_header->pps;
    SPS* sps = slice_header->sps;
    int32_t slice_type = (int32_t)slice_header->slice_type;
    int32_t mb_type;

    int32_t CodedBlockPatternLuma = 0;
    int32_t CodedBlockPatternChroma = 0;

    uint8_t is_entropy_coding = pps->entropy_coding_mode_flag;

    if (is_entropy_coding) { /*entropy coding*/
        err_code = cabac_mb_type(rbsp_reader, cabac, picture, slice_header, CurrMbAddr, &mb_type);
        if (err_code < 0) {
            return err_code;
        }
    } else { /* unsigned Exp-Golomb */
        mb_type = (int32_t)read_ue(rbsp_reader);
    }

    err_code = revise_slice_type_mb_type((int32_t)slice_type, mb_type, &slice_type, &mb_type);
    if (err_code < 0) {
        return err_code;
    }

    MacroBlock* mb = &picture->mb_list[picture->current_mb++];

    if (mb_type == 25) { /* I_PCM */
        while (!is_byte_aligned(rbsp_reader)) {
            /* uint32_t pcm_alignment_zero_bit; */
            (void)read_u(rbsp_reader, 1);
        }

        for (int i = 0; i < 256; ++i) {
            mb->pcm_sample_luma[i] = read_u(rbsp_reader, sps->BitDepthY);
        }

        for (int i = 0; i < 2 * sps->MbHeightC * sps->MbWidthC; ++i) {
            mb->pcm_sample_chroma[i] = read_u(rbsp_reader, sps->BitDepthC);
        }
    } else {
        int noSubMbPartSizeLessThan8x8Flag = 1;

        MB_TYPE_NAME mb_type_name;
        H264_MB_PART_PRED_MODE mb_part_pred_mode;
        err_code = MbPartPredMode(slice_type, 0, mb_type, 0, &mb_type_name, &mb_part_pred_mode);
        if (err_code < 0) {
            return err_code;
        }

        CodedBlockPatternLumaChroma(slice_type, mb_type, &CodedBlockPatternLuma, &CodedBlockPatternChroma);

        int32_t num_mb_part = 0;
        NumMbPart(slice_type, mb_type, &num_mb_part);

        if (mb_type != 0 /*I_NxN*/ && mb_part_pred_mode != Intra_16x16 && num_mb_part == 4) {
            return ERR_NOT_IMPL;
        } else {
            if (pps->transform_8x8_mode_flag && mb_type == 0 /*I_NxN*/) {
                int32_t transform_size_8x8_flag = 0;
                if (is_entropy_coding) {
                    err_code = cabac_transform_size_8x8_flag(rbsp_reader, cabac, picture, slice_header, CurrMbAddr, &transform_size_8x8_flag);
                    if (err_code < 0) {
                        return err_code;
                    }
                } else {
                    transform_size_8x8_flag = (int32_t)read_u(rbsp_reader, 1);
                }

                mb->transform_size_8x8_flag = transform_size_8x8_flag;

                err_code = MbPartPredMode(slice_type, transform_size_8x8_flag, mb_type, 0, &mb_type_name, &mb_part_pred_mode);
                if (err_code < 0) {
                    return err_code;
                }
            }

            /* TODO mb_pred(); */
        }

        if (mb_part_pred_mode != Intra_16x16) {
            int32_t coded_block_pattern = 0;

            if (is_entropy_coding) {
                err_code = cabac_coded_block_pattern(rbsp_reader, cabac, picture, slice_header, CurrMbAddr, sps->ChromaArrayType, &coded_block_pattern);
                if (err_code < 0) {
                    return err_code;
                }
            } else {
                coded_block_pattern = read_me(rbsp_reader, sps->ChromaArrayType, mb_part_pred_mode);
                if (coded_block_pattern != -1) {
                    mb->coded_block_pattern = coded_block_pattern;
                } else {
                    return ERR_INVALID_CODED_BLOCK_PATTERN;
                }
            }

            CodedBlockPatternLuma = coded_block_pattern % 16;
            CodedBlockPatternChroma = coded_block_pattern / 16;

            if (CodedBlockPatternLuma > 0 && pps->transform_8x8_mode_flag && mb_type != 0 /*I_NxN*/ && noSubMbPartSizeLessThan8x8Flag &&
                (mb_type_name != B_Direct_16x16 || sps->direct_8x8_inference_flag)) {
                int32_t transform_size_8x8_flag = 0;

                if (is_entropy_coding) {
                    err_code = cabac_transform_size_8x8_flag(rbsp_reader, cabac, picture, slice_header, CurrMbAddr, &transform_size_8x8_flag);
                    if (err_code < 0) {
                        return err_code;
                    }
                } else {
                    transform_size_8x8_flag = (int32_t)read_u(rbsp_reader, 1);
                }

                err_code = MbPartPredMode(slice_type, transform_size_8x8_flag, mb_type, 0, &mb_type_name, &mb_part_pred_mode);
                if (err_code < 0) {
                    return err_code;
                }
            }
        }

        if (CodedBlockPatternLuma > 0 || CodedBlockPatternChroma > 0 || mb_part_pred_mode == Intra_16x16) {
            int32_t mb_qp_delta;
            if (is_entropy_coding) {
                err_code = cabac_mb_qp_delta(rbsp_reader, cabac, picture, slice_header, CurrMbAddr, &mb_qp_delta);
                if (err_code < 0) {
                    return err_code;
                }
            } else {
                mb_qp_delta = (int32_t)read_se(rbsp_reader);
            }

            mb->mb_qp_delta = mb_qp_delta;

            /* residual( 0, 15 ); */
        }

        return ERR_OK;
    }
}

int revise_slice_type_mb_type(int32_t slice_type, int32_t mb_type, int32_t* revised_slice_type, int32_t* revised_mb_type) {
    slice_type = slice_type % 5;

    *revised_slice_type = slice_type;
    *revised_mb_type = mb_type;

    if (slice_type == SLICE_TYPE_I) {
        /* do nothing*/
    } else if (slice_type == SLICE_TYPE_SI) {
        /* A macroblock type that may be referred to as the SI macroblock type is specified in Table 7-12. */

        /* The macroblock types for SI slices are specified in Tables 7-12 and 7-11. The mb_type value 0 is specified in Table 7-12 and the mb_type values 1 to 26 are specified
         * in Table 7-11, indexed by subtracting 1 from the value of mb_type. */

        if (mb_type == 0) {
            /* do nothing*/
        } else if (mb_type >= 1 && mb_type <= 26) {
            *revised_slice_type = SLICE_TYPE_I;
            *revised_mb_type = mb_type - 1;
        } else {
            return ERR_UNRECOGNIZED_MB;
        }
    } else if (slice_type == SLICE_TYPE_P || slice_type == SLICE_TYPE_SP) {
        /* Macroblock types that may be collectively referred to as P macroblock types are specified in Table 7-13.*/

        /* The macroblock types for P and SP slices are specified in Tables 7-13 and 7-11. mb_type values 0 to 4 are specified in Table 7-13 and mb_type values 5 to 30 are
         * specified in Table 7-11, indexed by subtracting 5 from the value of mb_type.*/

        if (mb_type >= 0 && mb_type <= 4) {
            /*do nothing*/
        } else if (mb_type >= 5 && mb_type <= 30) {
            *revised_slice_type = SLICE_TYPE_I;
            *revised_mb_type = mb_type - 5;
        } else {
            return ERR_UNRECOGNIZED_MB;
        }
    } else if (slice_type == SLICE_TYPE_B) {
        /* Macroblock types that may be collectively referred to as B macroblock types are specified in Table 7-14. */

        /* The macroblock types for B slices are specified in Tables 7-14 and 7-11. The mb_type values 0 to 22 are specified in Table 7-14 and the mb_type values 23 to 48 are
         * specified in Table 7-11, indexed by subtracting 23 from the value of mb_type.*/

        if (mb_type >= 0 && mb_type <= 22) {
            /*do nothing*/
        } else if (mb_type >= 23 && mb_type <= 48) {
            *revised_slice_type = SLICE_TYPE_I;
            *revised_mb_type = mb_type - 23;
        } else {
            return ERR_UNRECOGNIZED_MB;
        }
    }

    return ERR_OK;
}

int mb_pred(RBSPReader* rbsp_reader, FrameOrField* picture, MacroBlock* mb, SliceHeader* slice_header, CABAC* cabac, int32_t CurrMbAddr){
    int err_code = ERR_OK;

    PPS* pps = slice_header->pps;
    SPS* sps = slice_header->sps;
    int32_t slice_type = (int32_t)slice_header->slice_type;

    H264_MB_PART_PRED_MODE pred_type = mb->mb_pred_type;

    if(pred_type == Intra_4x4 || pred_type == Intra_8x8 || pred_type == Intra_16x16){
        if(pred_type == Intra_4x4){

        }

        if(pred_type == Intra_8x8){

        }

        if(sps->ChromaArrayType == 1 || sps->ChromaArrayType == 2){
            
        }
    }

    return ERR_OK;
}
