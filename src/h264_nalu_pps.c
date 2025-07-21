#include "h264decoder/h264_nalu_pps.h"

#include "h264decoder/h264_context.h"
#include "h264decoder/h264_math.h"
#include "h264decoder/h264_nalu_pps.h"
#include "h264decoder/h264_nalu_sps.h"

static void free_pps(void* nalu) {
    PPS* pps = (PPS*)nalu;
    if (pps->slice_group_id) {
        free(pps->slice_group_id);
        pps->slice_group_id = 0;
    }
    if (pps->mapUnitToSliceGroupMap) {
        free(pps->mapUnitToSliceGroupMap);
        pps->mapUnitToSliceGroupMap = 0;
    }
    free(pps);
}

/**
 * @brief compute the mapUnitToSliceGroupMap
 * @see 8.2.2 Decoding process for macroblock to slice group map
 *
 * @param pps the PPS struct pointer
 * @param context the context
 */
static void compute_mapUnitToSliceGroupMap(PPS* pps, H264Context* context) {
    SPS* sps = context->active_sps;

    if (pps->num_slice_groups_minus1 == 0) {
        /* the mapUnitToSliceGroupMap memory is set to 0 */
        memset(pps->mapUnitToSliceGroupMap, 0, sizeof(int32_t) * sps->PicSizeInMapUnits);
        return;
    }

    if (pps->slice_group_map_type == 0) { /* @see 8.2.2.1 Specification for interleaved slice group map type */
        int i = 0;
        do {
            for (int iGroup = 0; iGroup <= pps->num_slice_groups_minus1 && i < sps->PicSizeInMapUnits; i += pps->run_length_minus1[iGroup++] + 1) {
                for (int j = 0; j <= pps->run_length_minus1[iGroup] && i + j < sps->PicSizeInMapUnits; j++) {
                    pps->mapUnitToSliceGroupMap[i + j] = iGroup;
                }
            }
        } while (i < sps->PicSizeInMapUnits);
    } else if (pps->slice_group_map_type == 1) { /* @see 8.2.2.2 Specification for dispersed slice group map type */
        for (int i = 0; i < sps->PicSizeInMapUnits; i++) {
            pps->mapUnitToSliceGroupMap[i] = ((i % sps->PicWidthInMbs) + (((i / sps->PicWidthInMbs) * (pps->num_slice_groups_minus1 + 1)) / 2)) %
                                             (pps->num_slice_groups_minus1 + 1);
        }
    } else if (pps->slice_group_map_type == 2) { /* @see 8.2.2.3 Specification for foreground with left-over slice group map type */
        for (int i = 0; i < sps->PicSizeInMapUnits; i++) {
            pps->mapUnitToSliceGroupMap[i] = (int32_t)pps->num_slice_groups_minus1;
        }

        for (int iGroup = pps->num_slice_groups_minus1 - 1; iGroup >= 0; iGroup--) {
            uint32_t yTopLeft = pps->top_left[iGroup] / sps->PicWidthInMbs;
            uint32_t xTopLeft = pps->top_left[iGroup] % sps->PicWidthInMbs;
            uint32_t yBottomRight = pps->bottom_right[iGroup] / sps->PicWidthInMbs;
            uint32_t xBottomRight = pps->bottom_right[iGroup] % sps->PicWidthInMbs;
            for (uint32_t y = yTopLeft; y <= yBottomRight; y++) {
                for (uint32_t x = xTopLeft; x <= xBottomRight; x++) {
                    pps->mapUnitToSliceGroupMap[y * sps->PicWidthInMbs + x] = iGroup;
                }
            }
        }
    } else if (pps->slice_group_map_type == 6) { /* @see 8.2.2.7 Specification for explicit slice group map type */
        for (int i = 0; i < sps->PicSizeInMapUnits; i++) {
            pps->mapUnitToSliceGroupMap[i] = pps->slice_group_id[i];
        }
    }

    /* the cases for pps->slice_group_map_type is 3, 4, or 5 is defined in slice header parsing functions*/
}

int pic_parameter_set_rbsp(RBSPReader* rbsp_reader, PPS* pps, H264Context* context) {
    int err_code = ERR_INVALID_PPS_PARAM;
    uint32_t i_group = 0;
    uint32_t i = 0;
    int v = 0;
    SPS* sps = 0;

#define check_range(name, low, high)           \
    if (pps->name < low || pps->name > high) { \
        goto error_flag;                       \
    }

#define check_range_right_exclusive(name, low, high) \
    if (pps->name < low || pps->name >= high) {      \
        goto error_flag;                             \
    }

    pps->nalu_header.free = free_pps;

    pps->pic_parameter_set_id = read_ue(rbsp_reader);
    check_range(pic_parameter_set_id, 0, 255);

    pps->seq_parameter_set_id = read_ue(rbsp_reader);
    check_range(seq_parameter_set_id, 0, 31);

    if (pps->seq_parameter_set_id >= H264_MAX_SPS_COUNT) {
        err_code = ERR_INVALID_SPS_ID;
        goto error_flag;
    }

    sps = context->sps[pps->seq_parameter_set_id];
    if (!sps) {
        err_code = ERR_NO_SPS_PARSED;
        goto error_flag;
    }
    context->active_sps = sps;

    pps->entropy_coding_mode_flag = (uint8_t)read_u(rbsp_reader, 1);
    pps->bottom_field_pic_order_in_frame_present_flag = (uint8_t)read_u(rbsp_reader, 1);

    pps->num_slice_groups_minus1 = read_ue(rbsp_reader);
    check_range(num_slice_groups_minus1, 0, 7);

    if (pps->num_slice_groups_minus1 > 0) {
        pps->slice_group_map_type = read_ue(rbsp_reader);
        check_range(slice_group_map_type, 0, 6);

        uint32_t pic_size = sps->PicSizeInMapUnits; /* (sps->pic_width_in_mbs_minus1 + 1) * (sps->pic_height_in_map_units_minus1 + 1) */

        if (pps->slice_group_map_type == 0) {
            for (i_group = 0; i_group <= pps->num_slice_groups_minus1; i_group++) {
                pps->run_length_minus1[i_group] = read_ue(rbsp_reader);
                check_range(run_length_minus1[i_group], 0, pic_size - 1);
            }
        } else if (pps->slice_group_map_type == 2) {
            for (i_group = 0; i_group < pps->num_slice_groups_minus1; i_group++) {
                pps->top_left[i_group] = read_ue(rbsp_reader);
                /* check_range(top_left[i_group], 0, pic_size - 1) */

                pps->bottom_right[i_group] = read_ue(rbsp_reader);
                check_range_right_exclusive(bottom_right[i_group], pps->top_left[i_group], pic_size - 1);
                if ((pps->top_left[i_group] % sps->PicWidthInMbs) > (pps->bottom_right[i_group] % sps->PicWidthInMbs)) {
                    goto error_flag;
                }
            }
        } else if (pps->slice_group_map_type == 3 || pps->slice_group_map_type == 4 || pps->slice_group_map_type == 5) {
            pps->slice_group_change_direction_flag = (uint8_t)read_u(rbsp_reader, 1);
            pps->slice_group_change_rate_minus1 = read_ue(rbsp_reader);
            check_range(slice_group_change_rate_minus1, 0, pic_size - 1);

        } else if (pps->slice_group_map_type == 6) {
            pps->pic_size_in_map_units_minus1 = read_ue(rbsp_reader);
            if (pps->pic_size_in_map_units_minus1 != pic_size - 1) {
                goto error_flag;
            }

            pps->slice_group_id = (uint32_t*)malloc((pps->pic_size_in_map_units_minus1 + 1) * sizeof(uint32_t));
            if (!pps->slice_group_id) {
                err_code = ERR_OOM;
                goto error_flag;
            }
            memset(pps->slice_group_id, 0, (pps->pic_size_in_map_units_minus1 + 1) * sizeof(uint32_t));

            for (i = 0; i <= pps->pic_size_in_map_units_minus1; i++) {
                v = int_log2(2 * pps->num_slice_groups_minus1 + 1);
                pps->slice_group_id[i] = read_u(rbsp_reader, v);
                check_range(slice_group_id[i], 0, pps->num_slice_groups_minus1);
            }
        }
    }

    pps->num_ref_idx_l0_active_minus1 = read_ue(rbsp_reader);
    check_range(num_ref_idx_l0_active_minus1, 0, 31);

    pps->num_ref_idx_l1_active_minus1 = read_ue(rbsp_reader);
    check_range(num_ref_idx_l1_active_minus1, 0, 31);

    pps->weighted_pred_flag = (uint8_t)read_u(rbsp_reader, 1);
    pps->weighted_bipred_idc = (uint8_t)read_u(rbsp_reader, 2);
    check_range(weighted_bipred_idc, 0, 2);

    pps->pic_init_qp_minus26 = read_se(rbsp_reader);
    check_range(pic_init_qp_minus26, (-26 - 6 * (int32_t)sps->bit_depth_luma_minus8), 25);

    pps->pic_init_qs_minus26 = read_se(rbsp_reader);
    check_range(pic_init_qs_minus26, -26, 25);

    pps->chroma_qp_index_offset = read_se(rbsp_reader);
    check_range(chroma_qp_index_offset, -12, 12);

    pps->deblocking_filter_control_present_flag = (uint8_t)read_u(rbsp_reader, 1);
    pps->constrained_intra_pred_flag = (uint8_t)read_u(rbsp_reader, 1);
    pps->redundant_pic_cnt_present_flag = (uint8_t)read_u(rbsp_reader, 1);

    if (more_rbsp_data(rbsp_reader)) {
        pps->more_rbsp_data = 1;
        pps->transform_8x8_mode_flag = (uint8_t)read_u(rbsp_reader, 1);
        pps->pic_scaling_matrix_present_flag = (uint8_t)read_u(rbsp_reader, 1);
        if (pps->pic_scaling_matrix_present_flag) {
            for (i = 0; i < 6 + ((sps->chroma_format_idc != 3) ? 2 : 6) * pps->transform_8x8_mode_flag; i++) {
                pps->pic_scaling_list_present_flag[i] = (uint8_t)read_u(rbsp_reader, 1);
                if (pps->pic_scaling_list_present_flag[i]) {
                    if (i < 6) {
                        err_code = scaling_list(rbsp_reader, pps->ScalingList4x4[i], 16, &(pps->UseDefaultScalingMatrix4x4Flag[i]));
                        if (err_code < 0) {
                            goto error_flag;
                        }

                    } else {
                        err_code = scaling_list(rbsp_reader, pps->ScalingList8x8[i - 6], 64, &(pps->UseDefaultScalingMatrix8x8Flag[i - 6]));
                        if (err_code < 0) {
                            goto error_flag;
                        }
                    }
                }
            }
        }

        pps->second_chroma_qp_index_offset = read_se(rbsp_reader);
        check_range(second_chroma_qp_index_offset, -12, 12);
    } else {
        pps->more_rbsp_data = 0;
        pps->transform_8x8_mode_flag = 0;
        pps->pic_scaling_matrix_present_flag = 0;
        pps->second_chroma_qp_index_offset = pps->chroma_qp_index_offset;
    }

    rbsp_trailing_bits(rbsp_reader);

#undef check_range_right_exclusive
#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

int post_process_pps(PPS* pps, H264Context* context) {
    SPS* sps = context->active_sps;

    if (!pps->mapUnitToSliceGroupMap) {
        pps->mapUnitToSliceGroupMap = (int32_t*)malloc(sizeof(int32_t) * sps->PicSizeInMapUnits);
        if (!pps->mapUnitToSliceGroupMap) {
            return ERR_OOM;
        }

        memset(pps->mapUnitToSliceGroupMap, 0, sizeof(int32_t) * sps->PicSizeInMapUnits);

        compute_mapUnitToSliceGroupMap(pps, context);
    }

    if (!pps->pic_scaling_matrix_present_flag) {
        /* the scaling lists used for the picture shall be inferred to be equal to those specified by the sequence parameter set. */
        memcpy(pps->ScalingList4x4, sps->ScalingList4x4, sizeof(int32_t) * 6 * 16);
        memcpy(pps->ScalingList8x8, sps->ScalingList8x8, sizeof(int32_t) * 6 * 64);
    } else {
        int32_t scaling_list_size = 6 + ((sps->chroma_format_idc != 3) ? 2 : 6) * pps->transform_8x8_mode_flag;
        for (int32_t i = 0; i < scaling_list_size; i++) {
            if (i < 6) {
                if (!pps->pic_scaling_list_present_flag[i]) {
                    if (!sps->seq_scaling_matrix_present_flag) {
                        if (i == 0) {
                            memcpy(pps->ScalingList4x4[i], Default_4x4_Intra, sizeof(int32_t) * 16);
                        } else if (i == 3) {
                            memcpy(pps->ScalingList4x4[i], Default_4x4_Inter, sizeof(int32_t) * 16);
                        } else {
                            memcpy(pps->ScalingList4x4[i], pps->ScalingList4x4[i - 1], sizeof(int32_t) * 16);
                        }

                    } else {
                        if (i == 0 || i == 3) {
                            memcpy(pps->ScalingList4x4[i], sps->ScalingList4x4[i], sizeof(int32_t) * 16);
                        } else {
                            memcpy(pps->ScalingList4x4[i], pps->ScalingList4x4[i - 1], sizeof(int32_t) * 16);
                        }
                    }
                } else {
                    if (pps->UseDefaultScalingMatrix4x4Flag[i]) {
                        if (i < 3) {
                            memcpy(pps->ScalingList4x4[i], Default_4x4_Intra, sizeof(int32_t) * 16);
                        } else {
                            memcpy(pps->ScalingList4x4[i], Default_4x4_Inter, sizeof(int32_t) * 16);
                        }
                    }
                }
            } else {
                if (!pps->pic_scaling_list_present_flag[i]) {
                    if (!sps->seq_scaling_matrix_present_flag) {
                        if (i == 6) {
                            memcpy(pps->ScalingList8x8[i - 6], Default_8x8_Intra, sizeof(int32_t) * 64);
                        } else if (i == 7) {
                            memcpy(pps->ScalingList8x8[i - 6], Default_8x8_Inter, sizeof(int32_t) * 64);
                        } else {
                            memcpy(pps->ScalingList8x8[i - 6], pps->ScalingList8x8[i - 8], sizeof(int32_t) * 64);
                        }
                    } else {
                        if (i == 6 || i == 7) {
                            memcpy(pps->ScalingList8x8[i - 6], sps->ScalingList8x8[i - 6], sizeof(int32_t) * 64);
                        } else {
                            memcpy(pps->ScalingList8x8[i - 6], pps->ScalingList8x8[i - 8], sizeof(int32_t) * 64);
                        }
                    }
                } else {
                    if (pps->UseDefaultScalingMatrix8x8Flag[i - 6]) {
                        if (i == 6 || i == 8 || i == 10) {
                            memcpy(pps->ScalingList8x8[i - 6], Default_8x8_Intra, sizeof(int32_t) * 64);
                        } else {
                            memcpy(pps->ScalingList8x8[i - 6], Default_8x8_Inter, sizeof(int32_t) * 64);
                        }
                    }
                }
            }
        }
    }

    return ERR_OK;
}

void fprintf_pps(FILE* stream, PPS* pps) {
    fprintf(stream, "forbidden_zero_bit: %d\n", pps->nalu_header.forbidden_zero_bit);
    fprintf(stream, "nal_ref_idc: %d\n", pps->nalu_header.nal_ref_idc);
    fprintf(stream, "nal_unit_type: %d\n", pps->nalu_header.nal_unit_type);
    fprintf(stream, "svc_extension_flag: %d\n", pps->nalu_header.svc_extension_flag);
    fprintf(stream, "avc_3d_extension_flag: %d\n", pps->nalu_header.avc_3d_extension_flag);

    fprintf(stream, "\n");

    fprintf(stream, "num_slice_groups_minus1: %d\n", pps->num_slice_groups_minus1);
    fprintf(stream, "if num_slice_groups_minus1 > 0\n");
    fprintf(stream, "    slice_group_map_type: %d\n", pps->slice_group_map_type);
    fprintf(stream, "    if slice_group_map_type == 0\n");
    fprintf(stream, "        run_length_minus1[8]:");
    for (int i = 0; i < 8; ++i) {
        fprintf(stream, " %d", pps->run_length_minus1[i]);
    }
    fprintf(stream, "\n");
    fprintf(stream, "    else if slice_group_map_type == 2\n");
    fprintf(stream, "        top_left[8]:");
    for (int i = 0; i < 8; ++i) {
        fprintf(stream, " %d", pps->top_left[i]);
    }
    fprintf(stream, "\n");
    fprintf(stream, "        bottom_right[8]:");
    for (int i = 0; i < 8; ++i) {
        fprintf(stream, " %d", pps->bottom_right[i]);
    }
    fprintf(stream, "\n");

    fprintf(stream, "    else if slice_group_map_type == 3 or 4 or 5\n");
    fprintf(stream, "        slice_group_change_direction_flag: %d\n", pps->slice_group_change_direction_flag);
    fprintf(stream, "        slice_group_change_rate_minus1: %d\n", pps->slice_group_change_rate_minus1);
    fprintf(stream, "    else if slice_group_map_type == 6\n");
    fprintf(stream, "        pic_size_in_map_units_minus1: %d\n", pps->pic_size_in_map_units_minus1);
    fprintf(stream, "        slice_group_id[%d]: ", (pps->pic_size_in_map_units_minus1 + 1));
    for (int i = 0; i <= pps->pic_size_in_map_units_minus1 && pps->slice_group_id; ++i) {
        fprintf(stream, " %d", pps->slice_group_id[i]);
    }
    fprintf(stream, "\n");
    fprintf(stream, "endif\n");

    fprintf(stream, "\n");

    fprintf(stream, "num_ref_idx_l0_active_minus1: %d\n", pps->num_ref_idx_l0_active_minus1);
    fprintf(stream, "num_ref_idx_l1_active_minus1: %d\n", pps->num_ref_idx_l1_active_minus1);
    fprintf(stream, "weighted_pred_flag: %d\n", pps->weighted_pred_flag);
    fprintf(stream, "weighted_bipred_idc: %d\n", pps->weighted_bipred_idc);
    fprintf(stream, "pic_init_qp_minus26: %d\n", pps->pic_init_qp_minus26);
    fprintf(stream, "pic_init_qs_minus26: %d\n", pps->pic_init_qs_minus26);
    fprintf(stream, "chroma_qp_index_offset: %d\n", pps->chroma_qp_index_offset);
    fprintf(stream, "deblocking_filter_control_present_flag: %d\n", pps->deblocking_filter_control_present_flag);
    fprintf(stream, "constrained_intra_pred_flag: %d\n", pps->constrained_intra_pred_flag);
    fprintf(stream, "redundant_pic_cnt_present_flag: %d\n", pps->redundant_pic_cnt_present_flag);

    fprintf(stream, "more_rbsp_data: %d\n", pps->more_rbsp_data);
    fprintf(stream, "if more_rbsp_data == 1\n");
    fprintf(stream, "    transform_8x8_mode_flag: %d\n", pps->transform_8x8_mode_flag);
    fprintf(stream, "    pic_scaling_matrix_present_flag: %d\n", pps->pic_scaling_matrix_present_flag);
    fprintf(stream, "    if pic_scaling_matrix_present_flag == 1\n");
    fprintf(stream, "        pic_scaling_list_present_flag[12]: %d %d %d %d %d %d %d %d %d %d %d %d\n", pps->pic_scaling_list_present_flag[0],
            pps->pic_scaling_list_present_flag[1], pps->pic_scaling_list_present_flag[2], pps->pic_scaling_list_present_flag[3],
            pps->pic_scaling_list_present_flag[4], pps->pic_scaling_list_present_flag[5], pps->pic_scaling_list_present_flag[6],
            pps->pic_scaling_list_present_flag[7], pps->pic_scaling_list_present_flag[8], pps->pic_scaling_list_present_flag[9],
            pps->pic_scaling_list_present_flag[10], pps->pic_scaling_list_present_flag[11]);

    fprintf(stream, "        ScalingList4x4[6][16]:\n");
    for (int i = 0; i < 6; ++i) {
        fprintf(stream, "            ");
        for (int j = 0; j < 16; ++j) {
            fprintf(stream, " %d", pps->ScalingList4x4[i][j]);
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "        UseDefaultScalingMatrix4x4Flag[6]: %d %d %d %d %d %d\n", pps->UseDefaultScalingMatrix4x4Flag[0],
            pps->UseDefaultScalingMatrix4x4Flag[1], pps->UseDefaultScalingMatrix4x4Flag[2], pps->UseDefaultScalingMatrix4x4Flag[3],
            pps->UseDefaultScalingMatrix4x4Flag[4], pps->UseDefaultScalingMatrix4x4Flag[5]);
    fprintf(stream, "        ScalingList8x8[6][64]: \n");
    for (int i = 0; i < 6; ++i) {
        fprintf(stream, "           ");
        for (int j = 0; j < 64; ++j) {
            fprintf(stream, " %d", pps->ScalingList8x8[i][j]);
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "        UseDefaultScalingMatrix8x8Flag[6]: %d %d %d %d %d %d\n", pps->UseDefaultScalingMatrix8x8Flag[0],
            pps->UseDefaultScalingMatrix8x8Flag[1], pps->UseDefaultScalingMatrix8x8Flag[2], pps->UseDefaultScalingMatrix8x8Flag[3],
            pps->UseDefaultScalingMatrix8x8Flag[4], pps->UseDefaultScalingMatrix8x8Flag[5]);

    fprintf(stream, "    endif\n");
    fprintf(stream, "    second_chroma_qp_index_offset: %d\n", pps->second_chroma_qp_index_offset);
    fprintf(stream, "endif\n");
}