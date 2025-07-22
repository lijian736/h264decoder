#ifndef _H_H264_DEFS_H_
#define _H_H264_DEFS_H_

#include <stdint.h>

/**
 * @see E.2.2 HRD parameters semantics
 * The value of cpb_cnt_minus1 shall be in the range of 0 to 31, inclusive
 */
#define H264_MAX_CPB_CNT 32

/**
 * @see A.2.1 Baseline profile
 * @see A.2.3 Extended profile
 * Picture parameter sets shall have num_slice_groups_minus1 in the range of 0 to 7, inclusive.
 */
#define H264_MAX_SLICE_GROUPS 8

/**
 * @see 7.4.2.1.1 Sequence parameter set data semantics
 * The value of seq_parameter_set_id shall be in the range of 0 to 31, inclusive
 */
#define H264_MAX_SPS_COUNT 32

/**
 * @see 7.4.2.2 Picture parameter set RBSP semantics
 * The value of pic_parameter_set_id shall be in the range of 0 to 255, inclusive.
 */
#define H264_MAX_PPS_COUNT 256

/**
 * @see A.3.1 Level limits common to the Baseline, Constrained Baseline, Main, and Extended profiles
 * MaxDpbFrames is equal to Min( MaxDpbMbs / ( PicWidthInMbs * FrameHeightInMbs ), 16 )
 */
#define H264_MAX_DPB_FRAMES 16

/**
 * @see 7.4.2.1.1: num_ref_frames_in_pic_order_cnt_cycle shall be in the range of 0 to 255, inclusive.
 * The value of max offset reference frame count
 */
#define H264_MAX_OFFSET_REF_FRAME_COUNT 256

/**
 * @see 7.4.2.1.1 Sequence parameter set data semantics
 * The value of max_num_ref_frames shall be in the range of 0 to MaxDpbFrames (as specified in clause A.3.1 or A.3.2), inclusive
 * Each reference frame have two fields
 */
#define H264_MAX_REFS (2 * H264_MAX_DPB_FRAMES)

/**
 * @see 7.4.3.1 Reference picture list modification semantics
 * When ref_pic_list_modification_flag_lN is equal to 1, the number of times that modification_of_pic_nums_idc is not equal to 3 following
 * ref_pic_list_modification_flag_lN shall not exceed num_ref_idx_lN_active_minus1 + 1.
 */
#define H264_MAX_RPLM_COUNT (H264_MAX_REFS + 1)

/**
 * @see 7.4.3.3 Decoded reference picture marking semantics
 */
#define H264_MAX_MMCO_COUNT (H264_MAX_REFS * 2 + 3)

/**
 * @see A.3.1 Level limits common to the Baseline, Constrained Baseline, Main, and Extended profiles
 */
#define H264_MAX_MB_PIC_SIZE 139264

/**
 * @see A.3.1 Level limits common to the Baseline, Constrained Baseline, Main, and Extended profiles
 * @see A.3.2 Level limits common to the High, Progressive High, Constrained High, High 10, Progressive High 10, High 4:2:2, High 4:4:4 Predictive,
 * High 10 Intra, High 4:2:2 Intra, High 4:4:4 Intra, and CAVLC 4:4:4 Intra profiles
 *
 * PicWidthInMbs and PicHeightInMbs is less or equal than Sqrt(MaxFS* 8). the MaxFs maximum value is 139264
 */
#define H264_MAX_MB_WIDTH_NUM 1055
#define H264_MAX_MB_HEIGHT_NUM 1055
#define H264_MAX_WIDTH (H264_MAX_MB_WIDTH_NUM * 16)
#define H264_MAX_HEIGHT (H264_MAX_MB_HEIGHT_NUM * 16)

/**
 * @see 9.3.2 Binarization process
 *
 * The possible values of the context index ctxIdx are in the range 0 to 1023, inclusive.
 *
 * @note Annex F: Scalable video coding is not support now
 * @note Annex I: Multiview and depth video with enhanced non-base view coding is not support now
 */
#define H264_MAX_CONTEXT_INDEX 1024

/* the invalid PPS id indicating the PPS which is not used yet */
#define H264_INVALID_PPS_ID 0xFFFFFFFF

/* the invalid SPS id indicating the SPS which is not used yet */
#define H264_INVALID_SPS_ID 0xFFFFFFFF

/**
 * @brief the H.264 Macroblock part prediction mode
 *
 */
typedef enum H264_MB_PART_PRED_MODE {
    Intra_NA,
    Intra_4x4,
    Intra_8x8,
    Intra_16x16,
    Inter,
    Pred_NA,
    Pred_L0,
    Pred_L1,
    BiPred,
    Direct,
} H264_MB_PART_PRED_MODE;

/**
 * @see Figure 6-14 – Determination of the neighbouring macroblock, blocks, and partitions (informative)
 *
 * Figure 6-14 illustrates the relative location of the neighbouring macroblocks, blocks, or partitions A, B, C, and D to the current macroblock, partition, or block, when the
 * current macroblock, partition, or block is in frame coding mode.
 *
 *           |                 |
 *      D    |        B        |    C
 * ----------+-----------------+-----------
 *      A    |      current    |
 *           |                 |
 *
 */
typedef enum RELATIVE_LOCATION_TYPE {
    RELATIVE_LOCATION_TYPE_NOT_AVAILABLE,
    RELATIVE_LOCATION_TYPE_CurrMbAddr,
    RELATIVE_LOCATION_TYPE_mbAddrA,
    RELATIVE_LOCATION_TYPE_mbAddrB,
    RELATIVE_LOCATION_TYPE_mbAddrC,
    RELATIVE_LOCATION_TYPE_mbAddrD,
    RELATIVE_LOCATION_TYPE_mbAddrA_add_1,
    RELATIVE_LOCATION_TYPE_mbAddrB_add_1,
    RELATIVE_LOCATION_TYPE_mbAddrC_add_1,
    RELATIVE_LOCATION_TYPE_mbAddrD_add_1,
    RELATIVE_LOCATION_TYPE_CurrMbAddr_minus_1,
} RELATIVE_LOCATION_TYPE;

/**
 * @brief Table 7-6 – Name association to slice_type
 */
typedef enum SLICE_TYPE {
    SLICE_TYPE_P = 0,
    SLICE_TYPE_B = 1,
    SLICE_TYPE_I = 2,
    SLICE_TYPE_SP = 3,
    SLICE_TYPE_SI = 4,
    SLICE_TYPE_P_PLUS = 5,
    SLICE_TYPE_B_PLUS = 6,
    SLICE_TYPE_I_PLUS = 7,
    SLICE_TYPE_SP_PLUS = 8,
    SLICE_TYPE_SI_PLUS = 9,
} SLICE_TYPE;

/**
 * @see Table 7-10 – Allowed collective macroblock types for slice_type
 * @see Table 7-11 – Macroblock types for I slices
 * @see Table 7-12 – Macroblock type with value 0 for SI slices
 * @see Table 7-13 – Macroblock type values 0 to 4 for P and SP slices
 * @see Table 7-14 – Macroblock type values 0 to 22 for B slices
 * @see Table 7-17 – Sub-macroblock types in P macroblocks
 * @see Table 7-18 – Sub-macroblock types in B macroblocks
 */
typedef enum MB_TYPE_NAME {
    /* Table 7-11 – Macroblock types for I slices */
    I_NxN,         /* 0 */
    I_16x16_0_0_0, /* 1 */
    I_16x16_1_0_0, /* 2 */
    I_16x16_2_0_0, /* 3 */
    I_16x16_3_0_0, /* 4 */
    I_16x16_0_1_0, /* 5 */
    I_16x16_1_1_0, /* 6 */
    I_16x16_2_1_0, /* 7 */
    I_16x16_3_1_0, /* 8 */
    I_16x16_0_2_0, /* 9 */
    I_16x16_1_2_0, /* 10 */
    I_16x16_2_2_0, /* 11 */
    I_16x16_3_2_0, /* 12 */
    I_16x16_0_0_1, /* 13 */
    I_16x16_1_0_1, /* 14 */
    I_16x16_2_0_1, /* 15 */
    I_16x16_3_0_1, /* 16 */
    I_16x16_0_1_1, /* 17 */
    I_16x16_1_1_1, /* 18 */
    I_16x16_2_1_1, /* 19 */
    I_16x16_3_1_1, /* 20 */
    I_16x16_0_2_1, /* 21 */
    I_16x16_1_2_1, /* 22 */
    I_16x16_2_2_1, /* 23 */
    I_16x16_3_2_1, /* 24 */
    I_PCM,         /* 25 */

    /* Table 7-12 – Macroblock type with value 0 for SI slices */
    SI_SI, /* 0 */

    /* Table 7-13 – Macroblock type values 0 to 4 for P and SP slices */
    P_L0_16x16,   /* 0 */
    P_L0_L0_16x8, /* 1 */
    P_L0_L0_8x16, /* 2 */
    P_8x8,        /* 3 */
    P_8x8ref0,    /* 4 */
    P_Skip,       /* inferred */

    /* Table 7-14 – Macroblock type values 0 to 22 for B slices */
    B_Direct_16x16, /* 0 */
    B_L0_16x16,     /* 1 */
    B_L1_16x16,     /* 2 */
    B_Bi_16x16,     /* 3 */
    B_L0_L0_16x8,   /* 4 */
    B_L0_L0_8x16,   /* 5 */
    B_L1_L1_16x8,   /* 6 */
    B_L1_L1_8x16,   /* 7 */
    B_L0_L1_16x8,   /* 8 */
    B_L0_L1_8x16,   /* 9 */
    B_L1_L0_16x8,   /* 10 */
    B_L1_L0_8x16,   /* 11 */
    B_L0_Bi_16x8,   /* 12 */
    B_L0_Bi_8x16,   /* 13 */
    B_L1_Bi_16x8,   /* 14 */
    B_L1_Bi_8x16,   /* 15 */
    B_Bi_L0_16x8,   /* 16 */
    B_Bi_L0_8x16,   /* 17 */
    B_Bi_L1_16x8,   /* 18 */
    B_Bi_L1_8x16,   /* 19 */
    B_Bi_Bi_16x8,   /* 20 */
    B_Bi_Bi_8x16,   /* 21 */
    B_8x8,          /* 22 */
    B_Skip,         /* inferred*/

    /* Table 7-17 – Sub-macroblock types in P macroblocks */
    P_L0_8x8, /* 0 */
    P_L0_8x4, /* 1 */
    P_L0_4x8, /* 2 */
    P_L0_4x4, /* 3 */

    /* Table 7-18 – Sub-macroblock types in B macroblocks */
    B_Direct_8x8, /* 0 */
    B_L0_8x8,     /* 1 */
    B_L1_8x8,     /* 2 */
    B_Bi_8x8,     /* 3 */
    B_L0_8x4,     /* 4 */
    B_L0_4x8,     /* 5 */
    B_L1_8x4,     /* 6 */
    B_L1_4x8,     /* 7 */
    B_Bi_8x4,     /* 8 */
    B_Bi_4x8,     /* 9 */
    B_L0_4x4,     /* 10 */
    B_L1_4x4,     /* 11 */
    B_Bi_4x4,     /* 12 */

} MB_TYPE_NAME;

typedef enum NALU_PRIORITY { HIGHEST = 3, HIGH = 2, LOW = 1, DISPOSABLE = 0 } NALU_PRIORITY;

/**
 * @brief the NALU type
 *
 * @see 7.4.1 NAL unit semantics
 *
 * When NAL units with nal_unit_type equal to 13 or 19 are present in a coded video sequence, decoders shall either perform the (optional) decoding
 * process specified for these NAL units or shall ignore (remove from the bitstream and discard) the contents of these NAL units.
 *
 * Decoders that conform to one or more of the profiles specified in Annex A rather than the profiles specified in Annexes F or G shall ignore (remove
 * from the bitstream and discard) the contents of all NAL units with nal_unit_type equal to 14, 15, or 20.
 *
 * NAL units that use nal_unit_type equal to 0 or in the range of 24..31, inclusive, shall not affect the decoding process.
 *
 * Decoders shall ignore (remove from the bitstream and discard) the contents of all NAL units that use reserved values of nal_unit_type.
 */
typedef enum NALU_TYPE {
    NALU_UNSPECIFIED = 0,                  /* Unspecified */
    NALU_CODED_SLICE_NON_IDR = 1,          /* Coded slice of a non-IDR picture */
    NALU_CODED_SLICE_DATA_PARTITION_A = 2, /* Coded slice data partition A */
    NALU_CODED_SLICE_DATA_PARTITION_B = 3, /* Coded slice data partition B */
    NALU_CODED_SLICE_DATA_PARTITION_C = 4, /* Coded slice data partition C */
    NALU_CODED_SLICE_IDR = 5,              /* Coded slice of an IDR picture */
    NALU_SEI = 6,                          /* Supplemental enhancement information (SEI) */
    NALU_SPS = 7,                          /* Sequence parameter set (SPS) */
    NALU_PPS = 8,                          /* Picture parameter set (PPS) */
    NALU_ACCESS_UNIT_DELIMITER = 9,        /* Access unit delimiter */
    NALU_END_OF_SEQUENCE = 10,             /* End of sequence */
    NALU_END_OF_STREAM = 11,               /* End of stream */
    NALU_FILLER = 12,                      /* Filler data */
    NALU_SPS_EXTENSION = 13,               /* Sequence parameter set extension */
    NALU_PREFIX_NALU = 14,                 /* Prefix NAL unit */
    NALU_SUBSET_SPS = 15,                  /* Subset Sequence parameter set */
    NALU_DPS = 16,                         /* Depth Parameter Set */
    NALU_RESERVED_17 = 17,                 /* Reserved */
    NALU_RESERVED_18 = 18,                 /* Reserved */
    NALU_CODED_SLICE_AUXILIARY = 19,       /* Coded slice of an auxiliary coded picture without partitioning */
    NALU_CODED_SLICE_EXTENSION = 20,       /* Coded slice extension */
    NALU_CODED_SLICE_EXTENSION_DV = 21,    /* Coded slice extension for a depth view component or a 3D-AVC texture view component */
    NALU_RESERVED_22 = 22,                 /* Reserved */
    NALU_RESERVED_23 = 23                  /* Reserved */
} NALU_TYPE;

/**
 * @brief Sample aspect ratio
 * @see E.2 VUI semantics
 * @see E.2.1 VUI parameters semantics
 * @see Appendix E. Table E-1  Meaning of sample aspect ratio indicator
 *
 * When aspect_ratio_idc indicates Extended_SAR, the sample aspect ratio is represented by sar_width : sar_height.
 * When the aspect_ratio_idc syntax element is not present, aspect_ratio_idc value shall be inferred to be equal to 0.
 */
typedef enum SAR_TYPE {
    SAR_Unspecified = 0, /* Unspecified */
    SAR_1_1 = 1,         /* 1:1 */
    SAR_12_11 = 2,       /* 12:11 */
    SAR_10_11 = 3,       /* 10:11 */
    SAR_16_11 = 4,       /* 16:11 */
    SAR_40_33 = 5,       /* 40:33 */
    SAR_24_11 = 6,       /* 24:11 */
    SAR_20_11 = 7,       /* 20:11 */
    SAR_32_11 = 8,       /* 32:11 */
    SAR_80_33 = 9,       /* 80:33 */
    SAR_18_11 = 10,      /* 18:11 */
    SAR_15_11 = 11,      /* 15:11 */
    SAR_64_33 = 12,      /* 64:33 */
    SAR_160_99 = 13,     /* 160:99 */
    SAR_4_3 = 14,        /* 4:3 */
    SAR_3_2 = 15,        /* 3:2 */
    SAR_2_1 = 16,        /* 2:1 */
    SAR_Extended = 255   /* Extended_SAR */
} SAR_TYPE;

/**
 * @brief Picture coded type
 *
 */
typedef enum PICTURE_CODED_TYPE {
    PICTURE_CODED_INVALID = 0,                 /* invalid*/
    PICTURE_CODED_FRAME = 1,                   /* frame */
    PICTURE_CODED_FIELD = 2,                   /* field */
    PICTURE_CODED_TOP_FIELD = 3,               /* top field */
    PICTURE_CODED_BOTTOM_FIELD = 4,            /* bottom field */
    PICTURE_CODED_COMPLEMENTARY_FIELD_PAIR = 5 /* complementary field pair*/
} PICTURE_CODED_TYPE;

/**
 * @brief macroblock types for I slices
 * @see Table 7-11 – Macroblock types for I slices
 */
typedef struct MB_TYPE_FOR_I_SLICES {
    int32_t mb_type;
    MB_TYPE_NAME mb_type_name;
    int32_t transform_size_8x8_flag;
    H264_MB_PART_PRED_MODE MbPartPredMode;
    int32_t Intra16x16PredMode;
    int32_t CodedBlockPatternChroma;
    int32_t CodedBlockPatternLuma;
} MB_TYPE_FOR_I_SLICES;

/**
 * @brief Macroblock type with value 0 for SI slices
 * @see Table 7-12 – Macroblock type with value 0 for SI slices
 */
typedef struct MB_TYPE_FOR_SI_SLICES {
    int32_t mb_type;
    MB_TYPE_NAME mb_type_name;
    H264_MB_PART_PRED_MODE MbPartPredMode;
    int32_t Intra16x16PredMode;
    int32_t CodedBlockPatternChroma;
    int32_t CodedBlockPatternLuma;
} MB_TYPE_FOR_SI_SLICES;

/**
 * @brief Macroblock type values 0 to 4 for P and SP slices
 * @see Table 7-13 – Macroblock type values 0 to 4 for P and SP slices
 */
typedef struct MB_TYPE_FOR_P_SP_SLICES {
    int32_t mb_type;
    MB_TYPE_NAME mb_type_name;
    int32_t NumMbPart;
    H264_MB_PART_PRED_MODE MbPartPredMode0;
    H264_MB_PART_PRED_MODE MbPartPredMode1;
    int32_t MbPartWidth;
    int32_t MbPartHeight;
} MB_TYPE_FOR_P_SP_SLICES;

/**
 * @brief Macroblock type values 0 to 22 for B slices
 * @see Table 7-14 – Macroblock type values 0 to 22 for B slices
 */
typedef struct MB_TYPE_FOR_B_SLICES {
    int32_t mb_type;
    MB_TYPE_NAME mb_type_name;
    int32_t NumMbPart;
    H264_MB_PART_PRED_MODE MbPartPredMode0;
    H264_MB_PART_PRED_MODE MbPartPredMode1;
    int32_t MbPartWidth;
    int32_t MbPartHeight;
} MB_TYPE_FOR_B_SLICES;

/**
 * @brief Sub-macroblock types in P macroblocks
 * @see Table 7-17 – Sub-macroblock types in P macroblocks
 */
typedef struct SUB_MB_TYPE_IN_P_MB {
    int32_t sub_mb_type;
    MB_TYPE_NAME sub_mb_type_name;
    int32_t NumSubMbPart;
    H264_MB_PART_PRED_MODE SubMbPredMode;
    int32_t SubMbPartWidth;
    int32_t SubMbPartHeight;
} SUB_MB_TYPE_IN_P_MB;

/**
 * @brief Sub-macroblock types in B macroblocks
 * @see Table 7-18 – Sub-macroblock types in B macroblocks
 */
typedef struct SUB_MB_TYPE_IN_B_MB {
    int32_t sub_mb_type;
    MB_TYPE_NAME sub_mb_type_name;
    int32_t NumSubMbPart;
    H264_MB_PART_PRED_MODE SubMbPredMode;
    int32_t SubMbPartWidth;
    int32_t SubMbPartHeight;
} SUB_MB_TYPE_IN_B_MB;

/**
 * @brief the NALU free function pointer
 */
typedef void (*free_nalu_p)(void* nalu);

/**
 * @brief Network Abstraction Layer Unit (NALU) Header
 * @see ITU-T H.264 7.3.1 NAL unit syntax
 */
typedef struct {
    uint8_t forbidden_zero_bit; /* shall be equal to 0 */
    uint8_t nal_ref_idc;        /* enum NALU_PRIORITY */
    uint8_t nal_unit_type;      /* enum NALU_TYPE */

    uint8_t svc_extension_flag;
    uint8_t avc_3d_extension_flag;

    /********************** the following data members are not in the H.264 bit stream ************************/
    int32_t IdrPicFlag;

    free_nalu_p free; /* the NALU free function pointer*/
} NALUHeader;

/**
 * @brief Hypothetical Reference Decoder
 * @see E.1.2 HRD parameters syntax
 */
typedef struct {
    uint32_t cpb_cnt_minus1;
    uint8_t bit_rate_scale;
    uint8_t cpb_size_scale;
    uint32_t bit_rate_value_minus1[H264_MAX_CPB_CNT];
    uint32_t cpb_size_value_minus1[H264_MAX_CPB_CNT];
    uint8_t cbr_flag[H264_MAX_CPB_CNT];
    uint8_t initial_cpb_removal_delay_length_minus1;
    uint8_t cpb_removal_delay_length_minus1;
    uint8_t dpb_output_delay_length_minus1;
    uint8_t time_offset_length;
} HRD;

/**
 * @brief Video usability information
 * @see Annex E
 *
 * VUI parameters are not required for constructing the luma or chroma samples by the decoding process.
 */
typedef struct {
    uint8_t aspect_ratio_info_present_flag;
    /* begin for aspect_ratio_info_present_flag is 1 */
    uint8_t aspect_ratio_idc;
    /* begin for aspect_ratio_idc is Extended_SAR */
    uint16_t sar_width;
    uint16_t sar_height;
    /* end for aspect_ratio_idc is Extended_SAR*/
    /* end for aspect_ratio_info_present_flag is 1 */

    uint8_t overscan_info_present_flag;
    /* begin for overscan_info_present_flag is 1*/
    uint8_t overscan_appropriate_flag;
    /* end for overscan_info_present_flag is 1*/

    uint8_t video_signal_type_present_flag;
    /* begin for video_signal_type_present_flag is 1 */
    uint8_t video_format;
    uint8_t video_full_range_flag;
    uint8_t colour_description_present_flag;
    /* begin for colour_description_present_flag is 1*/
    uint8_t colour_primaries;
    uint8_t transfer_characteristics;
    uint8_t matrix_coefficients;
    /* end for colour_description_present_flag is 1*/
    /* end for video_signal_type_present_flag is 1 */

    uint8_t chroma_loc_info_present_flag;
    /* begin for chroma_loc_info_present_flag is 1 */
    uint32_t chroma_sample_loc_type_top_field;
    uint32_t chroma_sample_loc_type_bottom_field;
    /* end for chroma_loc_info_present_flag is 1 */

    uint8_t timing_info_present_flag;
    /* begin for timing_info_present_flag is 1 */
    uint32_t num_units_in_tick;
    uint32_t time_scale;
    uint8_t fixed_frame_rate_flag;
    /* end for timing_info_present_flag is 1*/

    uint8_t nal_hrd_parameters_present_flag;
    /* begin for nal_hrd_parameters_present_flag is 1 */
    HRD hrd_nal;
    /* end for nal_hrd_parameters_present_flag is 1 */

    uint8_t vcl_hrd_parameters_present_flag;
    /* begin for vcl_hrd_parameters_present_flag is 1 */
    HRD hrd_vcl;
    /* end for vcl_hrd_parameters_present_flag is 1 */

    /* begin for nal_hrd_parameters_present_flag is 1 or vcl_hrd_parameters_present_flag is 1*/
    uint8_t low_delay_hrd_flag;
    /* end for nal_hrd_parameters_present_flag is 1 or vcl_hrd_parameters_present_flag is 1*/

    uint8_t pic_struct_present_flag;

    uint8_t bitstream_restriction_flag;
    /* begin for bitstream_restriction_flag is 1 */
    uint8_t motion_vectors_over_pic_boundaries_flag;
    uint32_t max_bytes_per_pic_denom;
    uint32_t max_bits_per_mb_denom;
    uint32_t log2_max_mv_length_horizontal;
    uint32_t log2_max_mv_length_vertical;
    uint32_t max_num_reorder_frames;
    uint32_t max_dec_frame_buffering;
    /* end for bitstream_restriction_flag is 1 */
} VUI;

/* @see 7.3.2.1 Sequence parameter set RBSP syntax */
/* @see 7.4.2.1 Sequence parameter set RBSP semantics */
typedef struct {
    NALUHeader nalu_header;

    uint8_t profile_idc;
    uint8_t constraint_set0_flag;
    uint8_t constraint_set1_flag;
    uint8_t constraint_set2_flag;
    uint8_t constraint_set3_flag;
    uint8_t constraint_set4_flag;
    uint8_t constraint_set5_flag;
    uint8_t reserved_zero_2bits; /* equal to 0 */

    uint8_t level_idc;
    uint32_t seq_parameter_set_id;

    /* begin for profile_idc 100, 110, 122, 244, 44, 83, 86, 118, 128, 138, 139, 134 and 135 */
    uint32_t chroma_format_idc;
    uint8_t separate_colour_plane_flag; /* for chroma_format_idc is 3 */
    uint32_t bit_depth_luma_minus8;
    uint32_t bit_depth_chroma_minus8;
    uint8_t qpprime_y_zero_transform_bypass_flag;

    uint8_t seq_scaling_matrix_present_flag;
    /* begin for seq_scaling_matrix_present_flag is 1 */
    uint8_t seq_scaling_list_present_flag[12];
    int32_t ScalingList4x4[6][16];
    uint8_t UseDefaultScalingMatrix4x4Flag[6];
    int32_t ScalingList8x8[6][64];
    uint8_t UseDefaultScalingMatrix8x8Flag[6];
    /* end for seq_scaling_matrix_present_flag is 1 */
    /* end for profile_idc 100, 110, 122, 244, 44, 83, 86, 118, 128, 138, 139, 134 and 135 */

    uint32_t log2_max_frame_num_minus4;

    uint32_t pic_order_cnt_type;
    /* begin for pic_order_cnt_type is 0 */
    uint32_t log2_max_pic_order_cnt_lsb_minus4;
    /* end for pic_order_cnt_type is 0 */

    /* begin for pic_order_cnt_type is 1 */
    uint8_t delta_pic_order_always_zero_flag;
    int32_t offset_for_non_ref_pic;
    int32_t offset_for_top_to_bottom_field;
    uint32_t num_ref_frames_in_pic_order_cnt_cycle;
    int32_t offset_for_ref_frame[H264_MAX_OFFSET_REF_FRAME_COUNT];
    /* end for pic_order_cnt_type is 1 */

    uint32_t max_num_ref_frames;
    uint8_t gaps_in_frame_num_value_allowed_flag;
    uint32_t pic_width_in_mbs_minus1;
    uint32_t pic_height_in_map_units_minus1;

    uint8_t frame_mbs_only_flag;
    /* begin for frame_mbs_only_flag is 0 */
    uint8_t mb_adaptive_frame_field_flag;
    /* end for frame_mbs_only_flag is 0 */

    uint8_t direct_8x8_inference_flag;

    uint8_t frame_cropping_flag;
    /* begin for frame_cropping_flag is 1 */
    uint32_t frame_crop_left_offset;
    uint32_t frame_crop_right_offset;
    uint32_t frame_crop_top_offset;
    uint32_t frame_crop_bottom_offset;
    /* end for frame_cropping_flag is 1*/

    uint8_t vui_parameters_present_flag;

    /* begin for vui_parameters_present_flag is 1*/
    VUI vui;
    /* end for vui_parameters_present_flag is 1*/

    /********************** the following data members are not in the H.264 bit stream ************************/
    /* the variable for the picture width in units of macroblocks, PicWidthInMbs = pic_width_in_mbs_minus1 + 1 */
    uint32_t PicWidthInMbs;

    /* the variable for picture width for the luma component, PicWidthInSamplesL = PicWidthInMbs * 16 */
    uint32_t PicWidthInSamplesL;

    /* The variable for picture width for the chroma components, PicWidthInSamplesC = PicWidthInMbs * MbWidthC */
    uint32_t PicWidthInSamplesC;

    /* FrameHeightInMbs = (2 - frame_mbs_only_flag) * PicHeightInMapUnits*/
    uint32_t FrameHeightInMbs;

    /* PicHeightInMapUnits = pic_height_in_map_units_minus1 + 1 */
    uint32_t PicHeightInMapUnits;

    /* PicSizeInMapUnits = PicWidthInMbs * PicHeightInMapUnits */
    uint32_t PicSizeInMapUnits;

    /* the bit depth of the samples of the luma array, BitDepthY = 8 + bit_depth_luma_minus8 */
    uint32_t BitDepthY;

    /* the value of the luma quantization parameter range offset QpBdOffsetY = 6 * bit_depth_luma_minus8*/
    uint32_t QpBdOffsetY;

    /* the bit depth of the samples of the chroma arrays, BitDepthC = 8 + bit_depth_chroma_minus8 */
    uint32_t BitDepthC;

    /*  the value of the chroma quantization parameter range offset QpBdOffsetC= 6 * bit_depth_chroma_minus8 */
    uint32_t QpBdOffsetC;

    /* SubWidthC is defined in Table 6-1 – SubWidthC, and SubHeightC values derived from chroma_format_idc and separate_colour_plane_flag */
    int32_t SubWidthC;

    /* SubHeightC is defined in Table 6-1 – SubWidthC, and SubHeightC values derived from chroma_format_idc and separate_colour_plane_flag */
    int32_t SubHeightC;

    /**
     * the width of the chroma arrays for each macroblock
     * if chroma_format_idc is equal to 0 (monochrome) or separate_colour_plane_flag is equal to 1, MbWidthC is equal to 0.
     * otherwise, MbWidthC is derived as MbWidthC = 16 / SubWidthC
     */
    uint32_t MbWidthC;

    /**
     * the height of the chroma arrays for each macroblock
     * if chroma_format_idc is equal to 0 (monochrome) or separate_colour_plane_flag is equal to 1, MbHeightC is equal to 0.
     * otherwise, MbHeightC is derived as MbHeightC = 16 / SubHeightC
     */
    uint32_t MbHeightC;

    /* RawMbBits = 256 * BitDepthY + 2 * MbWidthC * MbHeightC * BitDepthC */
    uint32_t RawMbBits;

    /* MaxFrameNum = 2 ^ (log2_max_frame_num_minus4 + 4) */
    uint32_t MaxFrameNum;

    /* MaxPicOrderCntLsb = 2 ^ (log2_max_pic_order_cnt_lsb_minus4 + 4) */
    uint32_t MaxPicOrderCntLsb;

    /**
     * When pic_order_cnt_type is equal to 1, the variable ExpectedDeltaPerPicOrderCntCycle is derived by
     * ExpectedDeltaPerPicOrderCntCycle = 0
     * for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
     *     ExpectedDeltaPerPicOrderCntCycle += offset_for_ref_frame[ i ]
     */
    uint32_t ExpectedDeltaPerPicOrderCntCycle;

    /**
     * if separate_colour_plane_flag is equal to 0, ChromaArrayType is set equal to chroma_format_idc.
     * otherwise (separate_colour_plane_flag is equal to 1), ChromaArrayType is set equal to 0.
     */
    uint32_t ChromaArrayType;
} SPS;

/* @see 7.3.2.2 Picture parameter set RBSP syntax */
/* @see 7.4.2.2 Picture parameter set RBSP semantics */
typedef struct {
    NALUHeader nalu_header;
    uint32_t pic_parameter_set_id;
    uint32_t seq_parameter_set_id;
    uint8_t entropy_coding_mode_flag;
    uint8_t bottom_field_pic_order_in_frame_present_flag;

    uint32_t num_slice_groups_minus1;
    /* begin for num_slice_groups_minus1 > 0 */
    uint32_t slice_group_map_type;

    /* begin for slice_group_map_type is 0 */
    uint32_t run_length_minus1[H264_MAX_SLICE_GROUPS];
    /* end for slice_group_map_type is 0 */

    /* begin for slice_group_map_type is 2 */
    uint32_t top_left[H264_MAX_SLICE_GROUPS];
    uint32_t bottom_right[H264_MAX_SLICE_GROUPS];
    /* end for slice_group_map_type is 2 */

    /* begin for slice_group_map_type is 3, 4, 5 */
    uint8_t slice_group_change_direction_flag;
    uint32_t slice_group_change_rate_minus1;
    /* end for slice_group_map_type is 3, 4, 5 */

    /* begin for slice_group_map_type is 6 */
    uint32_t pic_size_in_map_units_minus1;
    uint32_t* slice_group_id;
    /* end for slice_group_map_type is 6 */
    /* end of num_slice_groups_minus1 > 0 */

    uint32_t num_ref_idx_l0_active_minus1;
    uint32_t num_ref_idx_l1_active_minus1;
    uint8_t weighted_pred_flag;
    uint8_t weighted_bipred_idc;
    int32_t pic_init_qp_minus26;
    int32_t pic_init_qs_minus26;
    int32_t chroma_qp_index_offset;
    uint8_t deblocking_filter_control_present_flag;
    uint8_t constrained_intra_pred_flag;
    uint8_t redundant_pic_cnt_present_flag;

    uint8_t more_rbsp_data; /*this field is not defined in the PPS NALU*/
    /* begin for more_rbsp_data()*/
    uint8_t transform_8x8_mode_flag;

    uint8_t pic_scaling_matrix_present_flag;
    /* begin for pic_scaling_matrix_present_flag is 1 */
    uint8_t pic_scaling_list_present_flag[12];
    int32_t ScalingList4x4[6][16];
    uint8_t UseDefaultScalingMatrix4x4Flag[6];
    int32_t ScalingList8x8[6][64];
    uint8_t UseDefaultScalingMatrix8x8Flag[6];
    /* end for pic_scaling_matrix_present_flag is 1 */

    int32_t second_chroma_qp_index_offset;
    /* end for more_rbsp_data()*/

    /********************** the following data members are not in the PPS H.264 bit stream ************************/
    int32_t* mapUnitToSliceGroupMap;

} PPS;

/**
 * @brief Reference picture list MVC modification
 * @see 7.3.3.1 Reference picture list modification syntax
 * @see G.7.3.3.1.1 Reference picture list MVC modification syntax
 */
typedef struct {
    /* begin for slice_type %5 != 2 && slice_type %5 != 4 */
    uint8_t ref_pic_list_modification_flag_l0;
    /* end for slice_type %5 != 2 && slice_type %5 != 4 */

    /* begin for ref_pic_list_modification_flag_l0 is 1 */
    struct {
        uint32_t modification_of_pic_nums_idc;

        /* begin for modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1*/
        uint32_t abs_diff_pic_num_minus1;
        /* end for modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1*/

        /* begin for modification_of_pic_nums_idc == 2 */
        uint32_t long_term_pic_num;
        /* end for modification_of_pic_nums_idc == 2 */

        /* begin for modification_of_pic_nums_idc == 4 || modification_of_pic_nums_idc == 5*/
        uint32_t abs_diff_view_idx_minus;
        /* end for modification_of_pic_nums_idc == 4 || modification_of_pic_nums_idc == 5*/
    } rplm_l0[H264_MAX_RPLM_COUNT];
    /* end for ref_pic_list_modification_flag_l0 is 1 */

    /* the rplm_l0 length, it is not defined in Reference picture list MVC modification */
    size_t rplm_l0_len;

    /* begin for slice_type %5 == 1*/
    uint8_t ref_pic_list_modification_flag_l1;
    /* end for slice_type %5 == 1*/

    /* begin for ref_pic_list_modification_flag_l1 is 1 */
    struct {
        uint32_t modification_of_pic_nums_idc;

        /* begin for modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1*/
        uint32_t abs_diff_pic_num_minus1;
        /* end for modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1*/

        /* begin for modification_of_pic_nums_idc == 2 */
        uint32_t long_term_pic_num;
        /* end for modification_of_pic_nums_idc == 2 */

        /* begin for modification_of_pic_nums_idc == 4 || modification_of_pic_nums_idc == 5*/
        uint32_t abs_diff_view_idx_minus;
        /* end for modification_of_pic_nums_idc == 4 || modification_of_pic_nums_idc == 5*/
    } rplm_l1[H264_MAX_RPLM_COUNT];
    /* end for ref_pic_list_modification_flag_l1 is 1 */

    /* the rplm_l1 length, it is not defined in Reference picture list MVC modification */
    size_t rplm_l1_len;

} RPLM;

/**
 * @brief Prediction weight table
 * @see 7.3.3.2 Prediction weight table syntax
 */
typedef struct {
    uint32_t luma_log2_weight_denom;

    /* begin for ChromaArrayType != 0*/
    uint32_t chroma_log2_weight_denom;
    /* end for ChromaArrayType != 0*/

    uint8_t luma_weight_l0_flag[H264_MAX_REFS];
    /* begin for luma_weight_l0_flag is 1*/
    int32_t luma_weight_l0[H264_MAX_REFS];
    int32_t luma_offset_l0[H264_MAX_REFS];
    /* end for luma_weight_l0_flag is 1*/

    /* begin for ChromaArrayType != 0*/
    uint8_t chroma_weight_l0_flag[H264_MAX_REFS];
    /* begin for chroma_weight_l0_flag is 1*/
    int32_t chroma_weight_l0[H264_MAX_REFS][2];
    int32_t chroma_offset_l0[H264_MAX_REFS][2];
    /* end for chroma_weight_l0_flag is 1*/
    /* end for ChromaArrayType != 0*/

    /* begin for slice_type %5 == 1*/
    uint8_t luma_weight_l1_flag[H264_MAX_REFS];
    /* begin for luma_weight_l1_flag is 1*/
    int32_t luma_weight_l1[H264_MAX_REFS];
    int32_t luma_offset_l1[H264_MAX_REFS];
    /* end for luma_weight_l1_flag is 1*/

    /* begin for ChromaArrayType != 0*/
    uint8_t chroma_weight_l1_flag[H264_MAX_REFS];
    /* begin for chroma_weight_l1_flag is 1*/
    int32_t chroma_weight_l1[H264_MAX_REFS][2];
    int32_t chroma_offset_l1[H264_MAX_REFS][2];
    /* end for chroma_weight_l1_flag is 1*/
    /* end for ChromaArrayType != 0*/
    /* end for slice_type %5 == 1*/
} PredWeightTable;

/**
 * @brief Decoded Reference picture marking
 * @see 7.3.3.3 Decoded reference picture marking syntax
 */
typedef struct {
    /* begin for IdrPicFlag is 1*/
    uint8_t no_output_of_prior_pics_flag;
    uint8_t long_term_reference_flag;
    /* end for IdrPicFlag is 1*/

    /* begin for IdrPicFlag is 0*/
    uint8_t adaptive_ref_pic_marking_mode_flag;
    /* begin for adaptive_ref_pic_marking_mode_flag is 1*/
    struct {
        uint32_t memory_management_control_operation;
        /* begin of memory_management_control_operation is 1 or 3*/
        uint32_t difference_of_pic_nums_minus1;
        /* end of memory_management_control_operation is 1 or 3*/

        /* begin of memory_management_control_operation is 2*/
        uint32_t long_term_pic_num;
        /* end of memory_management_control_operation is 2*/

        /* begin of memory_management_control_operation is 3 or 6*/
        uint32_t long_term_frame_idx;
        /* end of memory_management_control_operation is 3 or 6*/

        /* begin of memory_management_control_operation is 4*/
        uint32_t max_long_term_frame_idx_plus1;
        /* end of memory_management_control_operation is 4*/
    } mmco[H264_MAX_MMCO_COUNT];
    /* end for adaptive_ref_pic_marking_mode_flag is 1*/

    /* the mmco length, this field is not defined in Decoded reference picture marking syntax */
    size_t mmco_len;
    /* end for IdrPicFlag is 0*/
} DecRefPicMark;

/**
 * @brief the slice header
 * @see 7.4.1 NAL unit semantics
 * @see Table 7-1 – NAL unit type codes, syntax element categories, and NAL unit type classes
 * @see 7.3.2.8 Slice layer without partitioning RBSP syntax
 * @see 7.3.3 Slice header syntax
 */
typedef struct {
    NALUHeader nalu_header;

    uint32_t first_mb_in_slice;
    uint32_t slice_type;
    uint32_t pic_parameter_set_id;

    /* begin for separate_colour_plane_flag is 1 */
    uint8_t colour_plane_id;
    /* end for separate_colour_plane_flag is 1 */

    uint32_t frame_num;

    /* begin for frame_mbs_only_flag is 0 */
    uint8_t field_pic_flag;
    /* begin for field_pic_flag is 1 */
    uint8_t bottom_field_flag;
    /* end for field_pic_flag is 1 */
    /* end for frame_mbs_only_flag is 0 */

    /* begin for IdrPicFlag is 1 */
    uint32_t idr_pic_id;
    /* end for IdrPicFlag is 1 */

    /* begin for pic_order_cnt_type is 0 */
    uint32_t pic_order_cnt_lsb;
    /* begin for bottom_field_pic_order_in_frame_present_flag && !field_pic_flag */
    int32_t delta_pic_order_cnt_bottom;
    /* end for bottom_field_pic_order_in_frame_present_flag && !field_pic_flag */
    /* end for pic_order_cnt_type is 0 */

    /* begin for pic_order_cnt_type == 1 && !delta_pic_order_always_zero_flag */
    /* 0 index item read */
    /* begin index 1 item read for bottom_field_pic_order_in_frame_present_flag && !field_pic_flag */
    int32_t delta_pic_order_cnt[2];
    /* end index 1 item read for bottom_field_pic_order_in_frame_present_flag && !field_pic_flag */
    /* end for pic_order_cnt_type == 1 && !delta_pic_order_always_zero_flag*/

    /* begin for redundant_pic_cnt_present_flag is 1*/
    uint32_t redundant_pic_cnt;
    /* end for redundant_pic_cnt_present_flag is 1*/

    /* begin for slice_type == B */
    uint8_t direct_spatial_mv_pred_flag;
    /* end for slice_type == B */

    /* begin for slice_type == P || slice_type == SP || slice_type == B */
    uint8_t num_ref_idx_active_override_flag;
    /* begin for num_ref_idx_active_override_flag is 1 */
    uint32_t num_ref_idx_l0_active_minus1;
    /* end for num_ref_idx_active_override_flag is 1 */
    /* begin for slice_type == B */
    uint32_t num_ref_idx_l1_active_minus1;
    /* end for slice_type == B */
    /* end for slice_type == P || slice_type == SP || slice_type == B */

    uint8_t rplm_mvc; /*this filed is not defined in the Slice Header*/
    /* begin ref_pic_list_mvc_modification( ) for nal_unit_type == 20 || nal_unit_type == 21 else ref_pic_list_modification( ) */
    RPLM rplm;
    /* end ref_pic_list_mvc_modification( ) for nal_unit_type == 20 || nal_unit_type == 21 else ref_pic_list_modification( ) */

    /* begin for weighted_pred_flag && (slice_type == P || slice_type == SP)) || (weighted_bipred_idc == 1 && slice_type == B) */
    PredWeightTable pred_weight_table;
    /* end for weighted_pred_flag && (slice_type == P || slice_type == SP)) || (weighted_bipred_idc == 1 && slice_type == B) */

    /* begin for nal_ref_idc != 0*/
    DecRefPicMark dec_ref_pic_mark;
    /* end for nal_ref_idc != 0*/

    /* begin for entropy_coding_mode_flag && slice_type != I && slice_type != SI */
    uint32_t cabac_init_idc;
    /* end for entropy_coding_mode_flag && slice_type != I && slice_type != SI*/

    int32_t slice_qp_delta;

    /* begin for slice_type == SP || slice_type == SI */
    /* begin for slice_type == SP */
    uint8_t sp_for_switch_flag;
    /* end for slice_type == SP */
    int32_t slice_qs_delta;
    /* end for slice_type == SP || slice_type == SI */

    /* begin for deblocking_filter_control_present_flag is 1 */
    uint32_t disable_deblocking_filter_idc;
    /* begin disable_deblocking_filter_idc != 1 */
    int32_t slice_alpha_c0_offset_div2;
    int32_t slice_beta_offset_div2;
    /* end disable_deblocking_filter_idc != 1 */
    /* end for deblocking_filter_control_present_flag is 1 */

    /* begin for num_slice_groups_minus1 > 0 && slice_group_map_type >=3 && slice_group_map_type <= 5*/
    uint32_t slice_group_change_cycle;
    /* end for num_slice_groups_minus1 > 0 && slice_group_map_type >=3 && slice_group_map_type <= 5*/

    /********************** the following data members are not in the H.264 bit stream ************************/
    /* enum PICTURE_CODED_TYPE */
    int32_t pic_coded_type;

    /* SliceQPY = 26 + pic_init_qp_minus26 + slice_qp_delta */
    int32_t SliceQPY;

    /* MbaffFrameFlag = ( mb_adaptive_frame_field_flag && !field_pic_flag ) */
    int32_t MbaffFrameFlag;

    /* PicHeightInMbs = FrameHeightInMbs / ( 1 + field_pic_flag ) */
    uint32_t PicHeightInMbs;

    /* PicHeightInSamplesL = PicHeightInMbs * 16 */
    uint32_t PicHeightInSamplesL;

    /* PicHeightInSamplesC = PicHeightInMbs * MbHeightC */
    uint32_t PicHeightInSamplesC;

    /* PicSizeInMbs = PicWidthInMbs * PicHeightInMbs */
    uint32_t PicSizeInMbs;

    /**
     * the variable MaxPicNum is derived as follows：
     * if field_pic_flag is equal to 0, MaxPicNum is set equal to MaxFrameNum.
     * otherwise (field_pic_flag is equal to 1), MaxPicNum is set equal to 2*MaxFrameNum.
     */
    uint32_t MaxPicNum;

    /**
     * the variable CurrPicNum is derived as follows:
     * if field_pic_flag is equal to 0, CurrPicNum is set equal to frame_num.
     * otherwise (field_pic_flag is equal to 1), CurrPicNum is set equal to 2 * frame_num + 1.
     */
    uint32_t CurrPicNum;

    /**
     * when slice_group_map_type is equal to 3, 4, or 5:
     * MapUnitsInSliceGroup0 = Min( slice_group_change_cycle * SliceGroupChangeRate, PicSizeInMapUnits )
     */
    uint32_t MapUnitsInSliceGroup0;

    /* QSY = 26 + pic_init_qs_minus26 + slice_qs_delta */
    int32_t QSY;

    /* map unit to slice group map */
    int32_t* mapUnitToSliceGroupMap;
    /* macroblock to slice group map*/
    int32_t* MbToSliceGroupMap;

    /* the sps this slice refers to */
    SPS* sps;
    /* the pps this slice refers to*/
    PPS* pps;

} SliceHeader;

typedef struct {
    int32_t mb_type;
    int32_t slice_id;
    int32_t mb_field_decoding_flag;
    int32_t mb_skip_flag;
    int32_t transform_size_8x8_flag;
    int32_t coded_block_pattern;
    int32_t mb_qp_delta;
    int32_t intra_chroma_pred_mode;

    H264_MB_PART_PRED_MODE mb_pred_type;

    MB_TYPE_NAME mb_type_name;

    int32_t CodedBlockPatternLuma;
    int32_t CodedBlockPatternChroma;

    uint32_t pcm_sample_luma[256];
    uint32_t pcm_sample_chroma[512];

    int32_t     prev_intra4x4_pred_mode_flag[16];
    int32_t     rem_intra4x4_pred_mode[16];

    int32_t     prev_intra8x8_pred_mode_flag[4];
    int32_t     rem_intra8x8_pred_mode[4];

    int32_t     i16x16DClevel[16];
    int32_t     i16x16AClevel[16][16];
    int32_t     level4x4[16][16];
    int32_t     level8x8[4][64];

    MB_TYPE_NAME sub_mb_type_name[4];
} MacroBlock;

/**
 * @brief the default value for scaling lists
 * @see 7.4.2.1.1 Sequence parameter set data semantics
 */
extern const int32_t Flat_4x4_16[16];
extern const int32_t Flat_8x8_16[64];

/**
 * @brief the default value for scaling lists
 * @see Table 7-3 – Specification of default scaling lists Default_4x4_Intra and Default_4x4_Inter
 */
extern const int32_t Default_4x4_Intra[16];
extern const int32_t Default_4x4_Inter[16];

/**
 * @brief the default value for scaling lists
 * @see Table 7-4 – Specification of default scaling lists Default_8x8_Intra and Default_8x8_Inter
 */
extern const int32_t Default_8x8_Intra[64];
extern const int32_t Default_8x8_Inter[64];

#endif