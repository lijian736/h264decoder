#include "h264decoder/h264_nalu_sps.h"

static void free_sps(void* nalu) {
    SPS* sps = (SPS*)nalu;
    free(sps);
}

void fprintf_sps(FILE* stream, SPS* sps) {
    fprintf(stream, "forbidden_zero_bit: %d\n", sps->nalu_header.forbidden_zero_bit);
    fprintf(stream, "nal_ref_idc: %d\n", sps->nalu_header.nal_ref_idc);
    fprintf(stream, "nal_unit_type: %d\n", sps->nalu_header.nal_unit_type);
    fprintf(stream, "svc_extension_flag: %d\n", sps->nalu_header.svc_extension_flag);
    fprintf(stream, "avc_3d_extension_flag: %d\n", sps->nalu_header.avc_3d_extension_flag);

    fprintf(stream, "\n");

    fprintf(stream, "profile_idc: %d\n", sps->profile_idc);
    fprintf(stream, "constraint_set0_flag: %d\n", sps->constraint_set0_flag);
    fprintf(stream, "constraint_set1_flag: %d\n", sps->constraint_set1_flag);
    fprintf(stream, "constraint_set2_flag: %d\n", sps->constraint_set2_flag);
    fprintf(stream, "constraint_set3_flag: %d\n", sps->constraint_set3_flag);
    fprintf(stream, "constraint_set4_flag: %d\n", sps->constraint_set4_flag);
    fprintf(stream, "constraint_set5_flag: %d\n", sps->constraint_set5_flag);
    fprintf(stream, "reserved_zero_2bits: %d\n", sps->reserved_zero_2bits);

    fprintf(stream, "\n");

    fprintf(stream, "level_idc: %d\n", sps->level_idc);
    fprintf(stream, "seq_parameter_set_id: %d\n", sps->seq_parameter_set_id);

    fprintf(stream, "\n");

    fprintf(stream, "if profile_idc is 100, 110, 122, 244, 44, 83, 86, 118, 128, 138, 139, 134 or 135\n");
    fprintf(stream, "    chroma_format_idc: %d\n", sps->chroma_format_idc);
    fprintf(stream, "    if chroma_format_idc is 3\n");
    fprintf(stream, "        separate_colour_plane_flag: %d\n", sps->separate_colour_plane_flag);
    fprintf(stream, "    endif\n");
    fprintf(stream, "    bit_depth_luma_minus8: %d\n", sps->bit_depth_luma_minus8);
    fprintf(stream, "    bit_depth_chroma_minus8: %d\n", sps->bit_depth_chroma_minus8);
    fprintf(stream, "    qpprime_y_zero_transform_bypass_flag: %d\n", sps->qpprime_y_zero_transform_bypass_flag);
    fprintf(stream, "    \n");
    fprintf(stream, "    seq_scaling_matrix_present_flag: %d\n", sps->seq_scaling_matrix_present_flag);
    fprintf(stream, "    if seq_scaling_matrix_present_flag is 1\n");
    fprintf(stream, "        seq_scaling_list_present_flag[12]: %d %d %d %d %d %d %d %d %d %d %d %d\n", sps->seq_scaling_list_present_flag[0],
            sps->seq_scaling_list_present_flag[1], sps->seq_scaling_list_present_flag[2], sps->seq_scaling_list_present_flag[3],
            sps->seq_scaling_list_present_flag[4], sps->seq_scaling_list_present_flag[5], sps->seq_scaling_list_present_flag[6],
            sps->seq_scaling_list_present_flag[7], sps->seq_scaling_list_present_flag[8], sps->seq_scaling_list_present_flag[9],
            sps->seq_scaling_list_present_flag[10], sps->seq_scaling_list_present_flag[11]);
    fprintf(stream, "        ScalingList4x4[6][16]:\n");
    for (int i = 0; i < 6; ++i) {
        fprintf(stream, "            ");
        for (int j = 0; j < 16; ++j) {
            fprintf(stream, " %d", sps->ScalingList4x4[i][j]);
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "        UseDefaultScalingMatrix4x4Flag[6]: %d %d %d %d %d %d\n", sps->UseDefaultScalingMatrix4x4Flag[0],
            sps->UseDefaultScalingMatrix4x4Flag[1], sps->UseDefaultScalingMatrix4x4Flag[2], sps->UseDefaultScalingMatrix4x4Flag[3],
            sps->UseDefaultScalingMatrix4x4Flag[4], sps->UseDefaultScalingMatrix4x4Flag[5]);
    fprintf(stream, "        ScalingList8x8[6][64]: \n");
    for (int i = 0; i < 6; ++i) {
        fprintf(stream, "           ");
        for (int j = 0; j < 64; ++j) {
            fprintf(stream, " %d", sps->ScalingList8x8[i][j]);
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "        UseDefaultScalingMatrix8x8Flag[6]: %d %d %d %d %d %d\n", sps->UseDefaultScalingMatrix8x8Flag[0],
            sps->UseDefaultScalingMatrix8x8Flag[1], sps->UseDefaultScalingMatrix8x8Flag[2], sps->UseDefaultScalingMatrix8x8Flag[3],
            sps->UseDefaultScalingMatrix8x8Flag[4], sps->UseDefaultScalingMatrix8x8Flag[5]);

    fprintf(stream, "    endif\n");
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "log2_max_frame_num_minus4: %d\n", sps->log2_max_frame_num_minus4);

    fprintf(stream, "\n");

    fprintf(stream, "pic_order_cnt_type: %d\n", sps->pic_order_cnt_type);
    fprintf(stream, "if pic_order_cnt_type is 0\n");
    fprintf(stream, "       log2_max_pic_order_cnt_lsb_minus4: %d\n", sps->log2_max_pic_order_cnt_lsb_minus4);
    fprintf(stream, "else if pic_order_cnt_type is 1\n");
    fprintf(stream, "    delta_pic_order_always_zero_flag: %d\n", sps->delta_pic_order_always_zero_flag);
    fprintf(stream, "    offset_for_non_ref_pic: %d\n", sps->offset_for_non_ref_pic);
    fprintf(stream, "    offset_for_top_to_bottom_field: %d\n", sps->offset_for_top_to_bottom_field);
    fprintf(stream, "    num_ref_frames_in_pic_order_cnt_cycle: %d\n", sps->num_ref_frames_in_pic_order_cnt_cycle);
    fprintf(stream, "    offset_for_ref_frame[256]:");
    for (int i = 0; i < H264_MAX_OFFSET_REF_FRAME_COUNT; ++i) {
        fprintf(stream, " %d", sps->offset_for_ref_frame[i]);
    }
    fprintf(stream, "\n");
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "max_num_ref_frames: %d\n", sps->max_num_ref_frames);
    fprintf(stream, "gaps_in_frame_num_value_allowed_flag: %d\n", sps->gaps_in_frame_num_value_allowed_flag);
    fprintf(stream, "pic_width_in_mbs_minus1: %d\n", sps->pic_width_in_mbs_minus1);
    fprintf(stream, "pic_height_in_map_units_minus1: %d\n", sps->pic_height_in_map_units_minus1);

    fprintf(stream, "\n");

    fprintf(stream, "frame_mbs_only_flag: %d\n", sps->frame_mbs_only_flag);
    fprintf(stream, "if frame_mbs_only_flag is 0\n");
    fprintf(stream, "    mb_adaptive_frame_field_flag: %d\n", sps->mb_adaptive_frame_field_flag);
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "direct_8x8_inference_flag: %d\n", sps->direct_8x8_inference_flag);

    fprintf(stream, "\n");

    fprintf(stream, "frame_cropping_flag: %d\n", sps->frame_cropping_flag);
    fprintf(stream, "if frame_cropping_flag is 1\n");
    fprintf(stream, "    frame_crop_left_offset: %d\n", sps->frame_crop_left_offset);
    fprintf(stream, "    frame_crop_right_offset: %d\n", sps->frame_crop_right_offset);
    fprintf(stream, "    frame_crop_top_offset: %d\n", sps->frame_crop_top_offset);
    fprintf(stream, "    frame_crop_bottom_offset: %d\n", sps->frame_crop_bottom_offset);
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "vui_parameters_present_flag: %d\n", sps->vui_parameters_present_flag);

    fprintf(stream, "---------------------VUI-----------------------\n");
    fprintf_vui(stream, &sps->vui);
    fprintf(stream, "---------------------VUI-----------------------\n");
}

int seq_parameter_set_rbsp(RBSPReader* rbsp_reader, SPS* sps) {
    /* @see 7.3.2.1 Sequence parameter set RBSP syntax */
    /* @see 7.4.2.1.1 Sequence parameter set data semantics */

#define check_range(name, low, high)           \
    if (sps->name < low || sps->name > high) { \
        goto error_flag;                       \
    }

    int err_code = ERR_INVALID_SPS_PARAM;
    int i = 0;

    sps->nalu_header.free = free_sps;

    sps->profile_idc = (uint8_t)read_u8(rbsp_reader);

    sps->constraint_set0_flag = (uint8_t)read_u(rbsp_reader, 1);
    sps->constraint_set1_flag = (uint8_t)read_u(rbsp_reader, 1);
    sps->constraint_set2_flag = (uint8_t)read_u(rbsp_reader, 1);
    sps->constraint_set3_flag = (uint8_t)read_u(rbsp_reader, 1);
    sps->constraint_set4_flag = (uint8_t)read_u(rbsp_reader, 1);
    sps->constraint_set5_flag = (uint8_t)read_u(rbsp_reader, 1);

    skip_n_bits(rbsp_reader, 2);

    sps->level_idc = (uint8_t)read_u8(rbsp_reader);
    sps->seq_parameter_set_id = read_ue(rbsp_reader);
    check_range(seq_parameter_set_id, 0, 31);

    if (sps->profile_idc == 100 || sps->profile_idc == 110 || sps->profile_idc == 122 || sps->profile_idc == 244 || sps->profile_idc == 44 ||
        sps->profile_idc == 83 || sps->profile_idc == 86 || sps->profile_idc == 118 || sps->profile_idc == 128 || sps->profile_idc == 138 ||
        sps->profile_idc == 139 || sps->profile_idc == 134 || sps->profile_idc == 135) {
        sps->chroma_format_idc = read_ue(rbsp_reader);
        check_range(chroma_format_idc, 0, 3);

        if (sps->chroma_format_idc == 3) {
            sps->separate_colour_plane_flag = (uint8_t)read_u(rbsp_reader, 1);
        } else {
            sps->separate_colour_plane_flag = 0;
        }

        sps->bit_depth_luma_minus8 = read_ue(rbsp_reader);
        check_range(bit_depth_luma_minus8, 0, 6);

        sps->bit_depth_chroma_minus8 = read_ue(rbsp_reader);
        check_range(bit_depth_chroma_minus8, 0, 6);

        sps->qpprime_y_zero_transform_bypass_flag = (uint8_t)read_u(rbsp_reader, 1);
        sps->seq_scaling_matrix_present_flag = (uint8_t)read_u(rbsp_reader, 1);
        if (sps->seq_scaling_matrix_present_flag) {
            for (i = 0; i < ((sps->chroma_format_idc != 3) ? 8 : 12); i++) {
                sps->seq_scaling_list_present_flag[i] = (uint8_t)read_u(rbsp_reader, 1);
                if (sps->seq_scaling_list_present_flag[i]) {
                    if (i < 6) {
                        err_code = scaling_list(rbsp_reader, sps->ScalingList4x4[i], 16, &(sps->UseDefaultScalingMatrix4x4Flag[i]));
                        if (err_code < 0) {
                            goto error_flag;
                        }
                    } else {
                        err_code = scaling_list(rbsp_reader, sps->ScalingList8x8[i - 6], 64, &(sps->UseDefaultScalingMatrix8x8Flag[i - 6]));
                        if (err_code < 0) {
                            goto error_flag;
                        }
                    }
                }
            }
        }
    } else {
        sps->chroma_format_idc = 1;
        sps->separate_colour_plane_flag = 0;
        sps->bit_depth_luma_minus8 = 0;
        sps->bit_depth_chroma_minus8 = 0;
        sps->qpprime_y_zero_transform_bypass_flag = 0;
        sps->seq_scaling_matrix_present_flag = 0;
    }

    sps->log2_max_frame_num_minus4 = read_ue(rbsp_reader);
    check_range(log2_max_frame_num_minus4, 0, 12);

    sps->pic_order_cnt_type = read_ue(rbsp_reader);
    check_range(pic_order_cnt_type, 0, 2);
    if (sps->pic_order_cnt_type == 0) {
        sps->log2_max_pic_order_cnt_lsb_minus4 = read_ue(rbsp_reader);
        check_range(log2_max_pic_order_cnt_lsb_minus4, 0, 12);
    } else if (sps->pic_order_cnt_type == 1) {
        sps->delta_pic_order_always_zero_flag = (uint8_t)read_u(rbsp_reader, 1);
        sps->offset_for_non_ref_pic = read_se(rbsp_reader);
        check_range(offset_for_non_ref_pic, INT32_MIN + 1, INT32_MAX);

        sps->offset_for_top_to_bottom_field = read_se(rbsp_reader);
        check_range(offset_for_top_to_bottom_field, INT32_MIN + 1, INT32_MAX);

        sps->num_ref_frames_in_pic_order_cnt_cycle = read_ue(rbsp_reader);
        check_range(num_ref_frames_in_pic_order_cnt_cycle, 0, 255);

        for (i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++) {
            sps->offset_for_ref_frame[i] = read_se(rbsp_reader);
            check_range(offset_for_ref_frame[i], INT32_MIN + 1, INT32_MAX);
        }
    }

    sps->max_num_ref_frames = read_ue(rbsp_reader);
    check_range(max_num_ref_frames, 0, H264_MAX_DPB_FRAMES);

    sps->gaps_in_frame_num_value_allowed_flag = (uint8_t)read_u(rbsp_reader, 1);
    sps->pic_width_in_mbs_minus1 = read_ue(rbsp_reader);
    check_range(pic_width_in_mbs_minus1, 0, H264_MAX_MB_WIDTH_NUM);

    sps->pic_height_in_map_units_minus1 = read_ue(rbsp_reader);
    check_range(pic_height_in_map_units_minus1, 0, H264_MAX_MB_HEIGHT_NUM);

    sps->frame_mbs_only_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (!sps->frame_mbs_only_flag) {
        sps->mb_adaptive_frame_field_flag = (uint8_t)read_u(rbsp_reader, 1);
    } else {
        sps->mb_adaptive_frame_field_flag = 0;
    }

    sps->direct_8x8_inference_flag = (uint8_t)read_u(rbsp_reader, 1);

    sps->frame_cropping_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (sps->frame_cropping_flag) {
        sps->frame_crop_left_offset = read_ue(rbsp_reader);
        check_range(frame_crop_left_offset, 0, H264_MAX_WIDTH);

        sps->frame_crop_right_offset = read_ue(rbsp_reader);
        check_range(frame_crop_right_offset, 0, H264_MAX_WIDTH);

        sps->frame_crop_top_offset = read_ue(rbsp_reader);
        check_range(frame_crop_top_offset, 0, H264_MAX_HEIGHT);

        sps->frame_crop_bottom_offset = read_ue(rbsp_reader);
        check_range(frame_crop_bottom_offset, 0, H264_MAX_HEIGHT);
    } else {
        sps->frame_crop_left_offset = 0;
        sps->frame_crop_right_offset = 0;
        sps->frame_crop_top_offset = 0;
        sps->frame_crop_bottom_offset = 0;
    }

    sps->vui_parameters_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    if (sps->vui_parameters_present_flag) {
        err_code = vui_parameters(rbsp_reader, &(sps->vui), sps);
        if (err_code < 0) {
            goto error_flag;
        }
    } else {
        vui_parameters_default(&(sps->vui), sps);
    }

#undef check_range

    rbsp_trailing_bits(rbsp_reader);
    return ERR_OK;

error_flag:
    return err_code;
}

int post_process_sps(SPS* sps) {
    sps->PicWidthInMbs = sps->pic_width_in_mbs_minus1 + 1;
    sps->PicWidthInSamplesL = sps->PicWidthInMbs * 16;

    if (sps->chroma_format_idc == 0 || sps->separate_colour_plane_flag == 1) { /*monochrome*/
        sps->SubWidthC = -1;
        sps->SubHeightC = -1;

        sps->MbWidthC = 0;
        sps->MbHeightC = 0;
    } else if (sps->chroma_format_idc == 1) { /* 4:2:0 */
        sps->SubWidthC = 2;
        sps->SubHeightC = 2;

        sps->MbWidthC = 8;                    /* 16/2 */
        sps->MbHeightC = 8;                   /* 16/2 */
    } else if (sps->chroma_format_idc == 2) { /* 4:2:2 */
        sps->SubWidthC = 2;
        sps->SubHeightC = 1;

        sps->MbWidthC = 8;                    /* 16/2 */
        sps->MbHeightC = 16;                  /* 16/1 */
    } else if (sps->chroma_format_idc == 3) { /* 4:4:4 */
        sps->SubWidthC = 1;
        sps->SubHeightC = 1;

        sps->MbWidthC = 16;  /* 16/1 */
        sps->MbHeightC = 16; /* 16/1 */
    }

    sps->PicWidthInSamplesC = sps->PicWidthInMbs * sps->MbWidthC;

    sps->PicHeightInMapUnits = sps->pic_height_in_map_units_minus1 + 1;
    sps->PicSizeInMapUnits = sps->PicWidthInMbs * sps->PicHeightInMapUnits;

    sps->FrameHeightInMbs = (2 - sps->frame_mbs_only_flag) * sps->PicHeightInMapUnits;
    sps->BitDepthY = 8 + sps->bit_depth_luma_minus8;
    sps->QpBdOffsetY = 6 * sps->bit_depth_luma_minus8;

    sps->BitDepthC = 8 + sps->bit_depth_chroma_minus8;
    sps->QpBdOffsetC = 6 * sps->bit_depth_chroma_minus8;

    sps->RawMbBits = 256 * sps->BitDepthY + 2 * sps->MbWidthC * sps->MbHeightC * sps->BitDepthC;

    sps->MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4);

    if (sps->pic_order_cnt_type == 0) {
        sps->MaxPicOrderCntLsb = 1 << (sps->log2_max_pic_order_cnt_lsb_minus4 + 4);
    } else {
        sps->MaxPicOrderCntLsb = 0;
    }

    if (sps->pic_order_cnt_type == 1) {
        for (uint32_t i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++) {
            sps->ExpectedDeltaPerPicOrderCntCycle += sps->offset_for_ref_frame[i];
        }
    }

    if (sps->separate_colour_plane_flag) {
        sps->ChromaArrayType = 0;
    } else {
        sps->ChromaArrayType = sps->chroma_format_idc;
    }

    int32_t scaling_list_size = (sps->chroma_format_idc != 3) ? 8 : 12;
    if (!sps->seq_scaling_matrix_present_flag) {
        for (int32_t i = 0; i < scaling_list_size; i++) {
            if (i < 6) {
                memcpy(sps->ScalingList4x4[i], Flat_4x4_16, sizeof(int32_t) * 16);
            } else {
                memcpy(sps->ScalingList8x8[i - 6], Flat_8x8_16, sizeof(int32_t) * 64);
            }
        }
    } else {
        for (int32_t i = 0; i < scaling_list_size; i++) {
            if (i < 6) {
                if (!sps->seq_scaling_list_present_flag[i]) { /* the scaling list fall-back rule set A specified in Table 7-2 */
                    if (i == 0) {
                        memcpy(sps->ScalingList4x4[i], Default_4x4_Intra, sizeof(int32_t) * 16);
                    } else if (i == 3) {
                        memcpy(sps->ScalingList4x4[i], Default_4x4_Inter, sizeof(int32_t) * 16);
                    } else {
                        memcpy(sps->ScalingList4x4[i], sps->ScalingList4x4[i - 1], sizeof(int32_t) * 16);
                    }
                } else {
                    if (sps->UseDefaultScalingMatrix4x4Flag[i]) {
                        if (i < 3) {
                            memcpy(sps->ScalingList4x4[i], Default_4x4_Intra, sizeof(int32_t) * 16);
                        } else {
                            memcpy(sps->ScalingList4x4[i], Default_4x4_Inter, sizeof(int32_t) * 16);
                        }
                    }
                }
            } else {
                if (!sps->seq_scaling_list_present_flag[i]) { /* the scaling list fall-back rule set A specified in Table 7-2 */
                    if (i == 6) {
                        memcpy(sps->ScalingList8x8[i - 6], Default_8x8_Intra, sizeof(int32_t) * 64);
                    } else if (i == 7) {
                        memcpy(sps->ScalingList8x8[i - 6], Default_8x8_Inter, sizeof(int32_t) * 64);
                    } else {
                        memcpy(sps->ScalingList8x8[i - 6], sps->ScalingList8x8[i - 8], sizeof(int32_t) * 64);
                    }
                } else {
                    if (sps->UseDefaultScalingMatrix8x8Flag[i - 6]) {
                        if (i == 6 || i == 8 || i == 10) {
                            memcpy(sps->ScalingList8x8[i - 6], Default_8x8_Intra, sizeof(int32_t) * 64);
                        } else {
                            memcpy(sps->ScalingList8x8[i - 6], Default_8x8_Inter, sizeof(int32_t) * 64);
                        }
                    }
                }
            }
        }
    }

    return ERR_OK;
}

char* get_chroma_format(SPS* sps) {
    switch (sps->chroma_format_idc) {
        case 0:
            return "monochrome";
        case 1:
            return "4:2:0";
        case 2:
            return "4:2:2";
        case 3:
            return "4:4:4";
        default:
            return "Unknown";
    }
}

void get_profile_level(SPS* sps, char* info, size_t info_size) {
    switch (sps->profile_idc) {
        case 44: {
            (void)snprintf(info, info_size, "CAVLC 4:4:4 Intra (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            break;
        }

        case 66: {
            (void)snprintf(info, info_size, "Baseline (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            break;
        }

        case 77: {
            (void)snprintf(info, info_size, "Main (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            break;
        }

        case 88: {
            (void)snprintf(info, info_size, "Extended (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            break;
        }

        case 100: {
            if (sps->constraint_set4_flag) {
                if (sps->constraint_set5_flag) {
                    (void)snprintf(info, info_size, "Constrained High (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
                } else {
                    (void)snprintf(info, info_size, "Progressive High (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
                }
            } else {
                (void)snprintf(info, info_size, "High (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            }
            break;
        }

        case 110: {
            if (sps->constraint_set4_flag) {
                (void)snprintf(info, info_size, "Progressive High 10 (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            } else if (sps->constraint_set3_flag) {
                (void)snprintf(info, info_size, "High 10 Intra (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            } else {
                (void)snprintf(info, info_size, "High 10 (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            }
            break;
        }

        case 122: {
            if (sps->constraint_set3_flag) {
                (void)snprintf(info, info_size, "High 4:2:2 Intra (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            } else {
                (void)snprintf(info, info_size, "High 4:2:2 (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            }
            break;
        }

        case 244: {
            if (sps->constraint_set3_flag) {
                (void)snprintf(info, info_size, "High 4:4:4 Intra (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            } else {
                (void)snprintf(info, info_size, "High 4:4:4 Predictive (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            }
            break;
        }

        default: {
            (void)snprintf(info, info_size, "Unknown (profile=%d, level=%.1f)", sps->profile_idc, sps->level_idc / 10.0f);
            break;
        }
    }
}

void get_pic_width_height(SPS* sps, uint32_t* width, uint32_t* height) {
    *width = sps->PicWidthInSamplesL;
    *height = sps->FrameHeightInMbs * 16;
}

/**
 * Table 6-1 – SubWidthC, and SubHeightC values derived from chroma_format_idc and separate_colour_plane_flag
 *
 * +-----------------------------------------------------------------------------------------+
 * | chroma_format_idc | separate_colour_plane_flag | Chroma Format | SubWidthC | SubHeightC |
 * +-----------------------------------------------------------------------------------------+
 * |  0                |  0                         |  monochrome   |  -        |  -         |
 * +-----------------------------------------------------------------------------------------+
 * |  1                |  0                         |  4:2:0        |  2        |  2         |
 * +-----------------------------------------------------------------------------------------+
 * |  2                |  0                         |  4:2:2        |  2        |  1         |
 * +-----------------------------------------------------------------------------------------+
 * |  3                |  0                         |  4:4:4        |  1        |  1         |
 * +-----------------------------------------------------------------------------------------+
 * |  3                |  1                         |  4:4:4        |  -        |  -         |
 * +-----------------------------------------------------------------------------------------+
 */