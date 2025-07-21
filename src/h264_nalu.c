#include "h264decoder/h264_nalu.h"

#include "h264decoder/h264_math.h"
#include "h264decoder/h264_nalu_pps.h"
#include "h264decoder/h264_nalu_slice_header.h"
#include "h264decoder/h264_nalu_sps.h"
#include "h264decoder/h264_rbsp.h"

/* @see 7.2 Specification of syntax functions, categories, and descriptors */
int more_rbsp_data(RBSPReader* rbsp_reader) {
    /* clone the rbsp reader */
    RBSPReader reader = *rbsp_reader;

    /* the reader has reached its end, no more data */
    if (is_end_of_reader(&reader)) {
        return 0;
    }

    uint8_t rbsp_stop_1_bit = (uint8_t)read_u(&reader, 1);
    /* no rbsp_stop_one_bit */
    if (rbsp_stop_1_bit == 0) {
        return 1;
    }

    /* the next bit is rbsp_stop_one_bit, check if the rest of bits are 0 */
    while (!is_end_of_reader(&reader)) {
        if (read_u(&reader, 1) == 1) {
            return 1;
        }
    }

    return 0;
}

void rbsp_trailing_bits(RBSPReader* rbsp_reader) {
    /* @see 7.3.2.11 RBSP trailing bits syntax */
    /* rbsp_stop_one_bit, equal to 1 */
    skip_n_bits(rbsp_reader, 1);
    while (!is_byte_aligned(rbsp_reader)) {
        /* rbsp_alignment_zero_bit, equal to 0 */
        skip_n_bits(rbsp_reader, 1);
    }
}

int more_rbsp_trailing_data(RBSPReader* rbsp_reader) { return !is_end_of_reader(rbsp_reader); }

void rbsp_slice_trailing_bits(RBSPReader* rbsp_reader, int entropy_coding_mode_flag) {
    rbsp_trailing_bits(rbsp_reader);
    if (entropy_coding_mode_flag) {
        if (more_rbsp_trailing_data(rbsp_reader)) {
            /* cabac_zero_word, equal to 0x0000 */
            skip_n_bits(rbsp_reader, 16);
        }
    }
}

void free_nalu(void* nalu) {
    if (nalu) {
        NALUHeader* nalu_header = (NALUHeader*)nalu;
        if (nalu_header->free) {
            nalu_header->free(nalu);
        } else {
            free(nalu);
        }
    }
}

int hrd_parameters(RBSPReader* rbsp_reader, HRD* hrd) {
    /* @see E.2.2 HRD parameters semantics*/
    uint32_t i = 0;
    int err_code = ERR_INVALID_HRD_PARAM;

#define check_range(name, low, high)           \
    if (hrd->name < low || hrd->name > high) { \
        goto error_flag;                       \
    }

    hrd->cpb_cnt_minus1 = read_ue(rbsp_reader);
    check_range(cpb_cnt_minus1, 0, 31);

    hrd->bit_rate_scale = (uint8_t)read_u(rbsp_reader, 4);
    hrd->cpb_size_scale = (uint8_t)read_u(rbsp_reader, 4);
    for (i = 0; i <= hrd->cpb_cnt_minus1; i++) {
        hrd->bit_rate_value_minus1[i] = read_ue(rbsp_reader);
        check_range(bit_rate_value_minus1[i], 0, UINT32_MAX - 1);

        hrd->cpb_size_value_minus1[i] = read_ue(rbsp_reader);
        check_range(cpb_size_value_minus1[i], 0, UINT32_MAX - 1);

        hrd->cbr_flag[i] = (uint8_t)read_u(rbsp_reader, 1);
    }

    hrd->initial_cpb_removal_delay_length_minus1 = (uint8_t)read_u(rbsp_reader, 5);
    hrd->cpb_removal_delay_length_minus1 = (uint8_t)read_u(rbsp_reader, 5);
    hrd->dpb_output_delay_length_minus1 = (uint8_t)read_u(rbsp_reader, 5);
    hrd->time_offset_length = (uint8_t)read_u(rbsp_reader, 5);

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

void hrd_parameters_default(HRD* hrd) {
    hrd->cpb_cnt_minus1 = 0;
    hrd->initial_cpb_removal_delay_length_minus1 = 23;
    hrd->cpb_removal_delay_length_minus1 = 23;
    hrd->dpb_output_delay_length_minus1 = 23;
    hrd->time_offset_length = 24;
}

int vui_parameters(RBSPReader* rbsp_reader, VUI* vui, SPS* sps) {
    /* @see E.1.1 VUI parameter syntax */

#define check_range(name, low, high)           \
    if (vui->name < low || vui->name > high) { \
        goto error_flag;                       \
    }

    int err_code = ERR_INVALID_VUI_PARAM;

    vui->aspect_ratio_info_present_flag = (uint8_t)read_u(rbsp_reader, 1);

    if (vui->aspect_ratio_info_present_flag) {
        vui->aspect_ratio_idc = (uint8_t)read_u8(rbsp_reader);
        if (vui->aspect_ratio_idc == SAR_Extended) {
            vui->sar_width = (uint16_t)read_u16(rbsp_reader);
            vui->sar_height = (uint16_t)read_u16(rbsp_reader);
        }
    } else {
        vui->aspect_ratio_idc = 0;
    }

    vui->overscan_info_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (vui->overscan_info_present_flag) {
        vui->overscan_appropriate_flag = (uint8_t)read_u(rbsp_reader, 1);
    }

    vui->video_signal_type_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (vui->video_signal_type_present_flag) {
        vui->video_format = (uint8_t)read_u(rbsp_reader, 3);
        vui->video_full_range_flag = (uint8_t)read_u(rbsp_reader, 1);
        vui->colour_description_present_flag = (uint8_t)read_u(rbsp_reader, 1);
        if (vui->colour_description_present_flag) {
            vui->colour_primaries = (uint8_t)read_u8(rbsp_reader);
            vui->transfer_characteristics = (uint8_t)read_u8(rbsp_reader);
            vui->matrix_coefficients = (uint8_t)read_u8(rbsp_reader);
        } else {
            vui->colour_primaries = 2;
            vui->transfer_characteristics = 2;
            vui->matrix_coefficients = 2;
        }
    } else {
        vui->video_format = 5;
        vui->video_full_range_flag = 0;
        vui->colour_primaries = 2;
        vui->transfer_characteristics = 2;
        vui->matrix_coefficients = 2;
    }

    vui->chroma_loc_info_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (vui->chroma_loc_info_present_flag) {
        vui->chroma_sample_loc_type_top_field = read_ue(rbsp_reader);
        check_range(chroma_sample_loc_type_top_field, 0, 5);

        vui->chroma_sample_loc_type_bottom_field = read_ue(rbsp_reader);
        check_range(chroma_sample_loc_type_bottom_field, 0, 5);
    } else {
        vui->chroma_sample_loc_type_top_field = 0;
        vui->chroma_sample_loc_type_bottom_field = 0;
    }

    vui->timing_info_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (vui->timing_info_present_flag) {
        vui->num_units_in_tick = read_u32(rbsp_reader);
        vui->time_scale = read_u32(rbsp_reader);
        vui->fixed_frame_rate_flag = (uint8_t)read_u(rbsp_reader, 1);
    } else {
        vui->fixed_frame_rate_flag = 0;
    }

    vui->nal_hrd_parameters_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (vui->nal_hrd_parameters_present_flag) {
        err_code = hrd_parameters(rbsp_reader, &(vui->hrd_nal));
        if (err_code < 0) {
            err_code = ERR_INVALID_HRD;
            goto error_flag;
        }
    } else {
        hrd_parameters_default(&(vui->hrd_nal));
    }

    vui->vcl_hrd_parameters_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (vui->vcl_hrd_parameters_present_flag) {
        err_code = hrd_parameters(rbsp_reader, &(vui->hrd_vcl));
        if (err_code < 0) {
            err_code = ERR_INVALID_HRD;
            goto error_flag;
        }
    } else {
        hrd_parameters_default(&(vui->hrd_vcl));
    }

    if (vui->nal_hrd_parameters_present_flag || vui->vcl_hrd_parameters_present_flag) {
        vui->low_delay_hrd_flag = (uint8_t)read_u(rbsp_reader, 1);
    } else {
        vui->low_delay_hrd_flag = 1 - vui->fixed_frame_rate_flag;
    }

    vui->pic_struct_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    vui->bitstream_restriction_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (vui->bitstream_restriction_flag) {
        vui->motion_vectors_over_pic_boundaries_flag = (uint8_t)read_u(rbsp_reader, 1);
        vui->max_bytes_per_pic_denom = read_ue(rbsp_reader);
        check_range(max_bytes_per_pic_denom, 0, 16);

        vui->max_bits_per_mb_denom = read_ue(rbsp_reader);
        check_range(max_bits_per_mb_denom, 0, 16);

        vui->log2_max_mv_length_horizontal = read_ue(rbsp_reader);
        check_range(log2_max_mv_length_horizontal, 0, 15);

        vui->log2_max_mv_length_vertical = read_ue(rbsp_reader);
        check_range(log2_max_mv_length_vertical, 0, 15);

        vui->max_num_reorder_frames = read_ue(rbsp_reader);
        check_range(max_num_reorder_frames, 0, H264_MAX_DPB_FRAMES);

        vui->max_dec_frame_buffering = read_ue(rbsp_reader);
        check_range(max_dec_frame_buffering, 0, H264_MAX_DPB_FRAMES);
    } else {
        vui->motion_vectors_over_pic_boundaries_flag = 1;
        vui->max_bytes_per_pic_denom = 2;
        vui->max_bits_per_mb_denom = 1;
        vui->log2_max_mv_length_horizontal = 15;
        vui->log2_max_mv_length_vertical = 15;

        if ((sps->profile_idc == 44 || sps->profile_idc == 86 || sps->profile_idc == 100 || sps->profile_idc == 110 || sps->profile_idc == 122 ||
             sps->profile_idc == 244) &&
            sps->constraint_set3_flag) {
            vui->max_num_reorder_frames = 0;
            vui->max_dec_frame_buffering = 0;
        } else {
            vui->max_num_reorder_frames = H264_MAX_DPB_FRAMES;
            vui->max_dec_frame_buffering = H264_MAX_DPB_FRAMES;
        }
    }

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

void vui_parameters_default(VUI* vui, SPS* sps) {
    vui->aspect_ratio_idc = 0;
    vui->video_format = 5;
    vui->video_full_range_flag = 0;
    vui->colour_primaries = 2;
    vui->transfer_characteristics = 2;
    vui->matrix_coefficients = 2;

    vui->chroma_sample_loc_type_top_field = 0;
    vui->chroma_sample_loc_type_bottom_field = 0;

    vui->fixed_frame_rate_flag = 0;
    vui->low_delay_hrd_flag = 1 - vui->fixed_frame_rate_flag;

    vui->pic_struct_present_flag = 0;

    vui->motion_vectors_over_pic_boundaries_flag = 1;
    vui->max_bytes_per_pic_denom = 2;
    vui->max_bits_per_mb_denom = 1;
    vui->log2_max_mv_length_horizontal = 15;
    vui->log2_max_mv_length_vertical = 15;

    if ((sps->profile_idc == 44 || sps->profile_idc == 86 || sps->profile_idc == 100 || sps->profile_idc == 110 || sps->profile_idc == 122 ||
         sps->profile_idc == 244) &&
        sps->constraint_set3_flag) {
        vui->max_num_reorder_frames = 0;
        vui->max_dec_frame_buffering = 0;
    } else {
        vui->max_num_reorder_frames = H264_MAX_DPB_FRAMES;
        vui->max_dec_frame_buffering = H264_MAX_DPB_FRAMES;
    }

    hrd_parameters_default(&vui->hrd_nal);
    hrd_parameters_default(&vui->hrd_vcl);
}

int scaling_list(RBSPReader* rbsp_reader, int32_t* scaling_list, size_t scaling_list_size, uint8_t* use_default_scaling_matrix_flag) {
    /* @see 7.3.2.1.1.1 Scaling list syntax */

    int err_code = ERR_INVALID_SCALING;
    int32_t last_scale = 8;
    int32_t next_scale = 8;
    int32_t delta_scale = 0;
    size_t i = 0;

#define check_range(name, low, high) \
    if (name < low || name > high) { \
        goto error_flag;             \
    }

    for (i = 0; i < scaling_list_size; i++) {
        if (next_scale != 0) {
            delta_scale = read_se(rbsp_reader);
            check_range(delta_scale, -128, 127);

            next_scale = (last_scale + delta_scale + 256) % 256;
            *use_default_scaling_matrix_flag = (i == 0 && next_scale == 0);
        }

        scaling_list[i] = (next_scale == 0) ? last_scale : next_scale;
        last_scale = scaling_list[i];
    }

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

char* stringify_nalu_type(uint8_t nalu_type) {
    switch (nalu_type) {
        case NALU_UNSPECIFIED:
            return "Unspecified";

        case NALU_CODED_SLICE_NON_IDR:
            return "Coded slice of a non-IDR picture";

        case NALU_CODED_SLICE_DATA_PARTITION_A:
            return "Coded slice data partition A";

        case NALU_CODED_SLICE_DATA_PARTITION_B:
            return "Coded slice data partition B";

        case NALU_CODED_SLICE_DATA_PARTITION_C:
            return "Coded slice data partition C";

        case NALU_CODED_SLICE_IDR:
            return "Coded slice of an IDR picture";

        case NALU_SEI:
            return "SEI";

        case NALU_SPS:
            return "SPS";

        case NALU_PPS:
            return "PPS";

        case NALU_ACCESS_UNIT_DELIMITER:
            return "Access unit delimiter";

        case NALU_END_OF_SEQUENCE:
            return "End of sequence";

        case NALU_END_OF_STREAM:
            return "End of stream";

        case NALU_FILLER:
            return "Filler data";

        case NALU_SPS_EXTENSION:
            return "SPS extension";

        case NALU_PREFIX_NALU:
            return "Prefix NAL unit";

        case NALU_SUBSET_SPS:
            return "Subset SPS";

        case NALU_DPS:
            return "Depth Parameter Set";

        case NALU_RESERVED_17:
        case NALU_RESERVED_18:
            return "Reserved";

        case NALU_CODED_SLICE_AUXILIARY:
            return "Coded slice of an auxiliary coded picture without partitioning";

        case NALU_CODED_SLICE_EXTENSION:
            return "Coded slice extension";

        case NALU_CODED_SLICE_EXTENSION_DV:
            return "Coded slice extension for a depth view component or a 3D-AVC texture view component";

        case NALU_RESERVED_22:
        case NALU_RESERVED_23:
            return "Reserved";

        default:
            return "Unknown NALU";
    }
}

void fprintf_hrd(FILE* stream, HRD* hrd) {
    fprintf(stream, "cpb_cnt_minus1: %d\n", hrd->cpb_cnt_minus1);
    fprintf(stream, "bit_rate_scale: %d\n", hrd->bit_rate_scale);
    fprintf(stream, "cpb_size_scale: %d\n", hrd->cpb_size_scale);

    fprintf(stream, "bit_rate_value_minus1: ");
    for (int i = 0; i < H264_MAX_CPB_CNT; ++i) {
        fprintf(stream, " %d", hrd->bit_rate_value_minus1[i]);
    }
    fprintf(stream, "\n");

    fprintf(stream, "cpb_size_value_minus1: ");
    for (int i = 0; i < H264_MAX_CPB_CNT; ++i) {
        fprintf(stream, " %d", hrd->cpb_size_value_minus1[i]);
    }
    fprintf(stream, "\n");

    fprintf(stream, "cbr_flag: ");
    for (int i = 0; i < H264_MAX_CPB_CNT; ++i) {
        fprintf(stream, " %d", hrd->cbr_flag[i]);
    }
    fprintf(stream, "\n");

    fprintf(stream, "initial_cpb_removal_delay_length_minus1: %d\n", hrd->initial_cpb_removal_delay_length_minus1);
    fprintf(stream, "cpb_removal_delay_length_minus1: %d\n", hrd->cpb_removal_delay_length_minus1);
    fprintf(stream, "dpb_output_delay_length_minus1: %d\n", hrd->dpb_output_delay_length_minus1);
    fprintf(stream, "time_offset_length: %d\n", hrd->time_offset_length);
}

void fprintf_vui(FILE* stream, VUI* vui) {
    fprintf(stream, "aspect_ratio_info_present_flag: %d\n", vui->aspect_ratio_info_present_flag);
    fprintf(stream, "if aspect_ratio_info_present_flag is 1\n");
    fprintf(stream, "    aspect_ratio_idc:%d\n", vui->aspect_ratio_idc);
    fprintf(stream, "    if aspect_ratio_idc == Extended_SAR(255)\n");
    fprintf(stream, "        sar_width:%d\n", vui->sar_width);
    fprintf(stream, "        sar_height:%d\n", vui->sar_height);
    fprintf(stream, "    endif\n");
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "overscan_info_present_flag: %d\n", vui->overscan_info_present_flag);
    fprintf(stream, "if overscan_info_present_flag is 1\n");
    fprintf(stream, "    overscan_appropriate_flag:%d\n", vui->overscan_appropriate_flag);
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "video_signal_type_present_flag: %d\n", vui->video_signal_type_present_flag);
    fprintf(stream, "if video_signal_type_present_flag is 1\n");
    fprintf(stream, "    video_format:%d\n", vui->video_format);
    fprintf(stream, "    video_full_range_flag:%d\n", vui->video_full_range_flag);
    fprintf(stream, "    colour_description_present_flag:%d\n", vui->colour_description_present_flag);
    fprintf(stream, "    if colour_description_present_flag == 1\n");
    fprintf(stream, "        colour_primaries:%d\n", vui->colour_primaries);
    fprintf(stream, "        transfer_characteristics:%d\n", vui->transfer_characteristics);
    fprintf(stream, "        matrix_coefficients:%d\n", vui->matrix_coefficients);
    fprintf(stream, "    endif\n");
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "chroma_loc_info_present_flag: %d\n", vui->chroma_loc_info_present_flag);
    fprintf(stream, "if chroma_loc_info_present_flag is 1\n");
    fprintf(stream, "    chroma_sample_loc_type_top_field:%d\n", vui->chroma_sample_loc_type_top_field);
    fprintf(stream, "    chroma_sample_loc_type_bottom_field:%d\n", vui->chroma_sample_loc_type_bottom_field);
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "timing_info_present_flag: %d\n", vui->timing_info_present_flag);
    fprintf(stream, "if timing_info_present_flag is 1\n");
    fprintf(stream, "    num_units_in_tick:%d\n", vui->num_units_in_tick);
    fprintf(stream, "    time_scale:%d\n", vui->time_scale);
    fprintf(stream, "    fixed_frame_rate_flag:%d\n", vui->fixed_frame_rate_flag);
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "nal_hrd_parameters_present_flag: %d\n", vui->nal_hrd_parameters_present_flag);
    fprintf(stream, "if nal_hrd_parameters_present_flag is 1\n");
    fprintf(stream, "\n");
    fprintf(stream, "----------------HRD----------------\n");
    fprintf_hrd(stream, &vui->hrd_nal);
    fprintf(stream, "----------------HRD----------------\n");
    fprintf(stream, "\n");
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "vcl_hrd_parameters_present_flag: %d\n", vui->vcl_hrd_parameters_present_flag);
    fprintf(stream, "if vcl_hrd_parameters_present_flag is 1\n");
    fprintf(stream, "\n");
    fprintf(stream, "----------------HRD----------------\n");
    fprintf_hrd(stream, &vui->hrd_vcl);
    fprintf(stream, "----------------HRD----------------\n");
    fprintf(stream, "\n");
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "if nal_hrd_parameters_present_flag is 1 or vcl_hrd_parameters_present_flag is 1\n");
    fprintf(stream, "    low_delay_hrd_flag:%d\n", vui->low_delay_hrd_flag);
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "pic_struct_present_flag:%d\n", vui->pic_struct_present_flag);

    fprintf(stream, "\n");

    fprintf(stream, "bitstream_restriction_flag: %d\n", vui->bitstream_restriction_flag);
    fprintf(stream, "if bitstream_restriction_flag is 1\n");
    fprintf(stream, "    motion_vectors_over_pic_boundaries_flag:%d\n", vui->motion_vectors_over_pic_boundaries_flag);
    fprintf(stream, "    max_bytes_per_pic_denom:%d\n", vui->max_bytes_per_pic_denom);
    fprintf(stream, "    max_bits_per_mb_denom:%d\n", vui->max_bits_per_mb_denom);
    fprintf(stream, "    log2_max_mv_length_horizontal:%d\n", vui->log2_max_mv_length_horizontal);
    fprintf(stream, "    log2_max_mv_length_vertical:%d\n", vui->log2_max_mv_length_vertical);
    fprintf(stream, "    max_num_reorder_frames:%d\n", vui->max_num_reorder_frames);
    fprintf(stream, "    max_dec_frame_buffering:%d\n", vui->max_dec_frame_buffering);
    fprintf(stream, "endif\n");
}