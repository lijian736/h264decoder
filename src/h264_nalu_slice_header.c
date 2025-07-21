#include "h264decoder/h264_nalu_slice_header.h"

#include "h264decoder/h264_context.h"
#include "h264decoder/h264_math.h"

static void compute_mapUnitToSliceGroupMap(SliceHeader* header, H264Context* context) {
    PPS* pps = context->active_pps;
    SPS* sps = context->active_sps;

    /* the cases for pps->slice_group_map_type is 0, 1, 2 or 6 is defined h264_nalu_pps.c*/

    if (pps->slice_group_map_type == 3) { /* @see 8.2.2.4 Specification for box-out slice group map types */
        for (int i = 0; i < sps->PicSizeInMapUnits; i++) {
            header->mapUnitToSliceGroupMap[i] = 1;
        }
        int32_t x = ((int32_t)sps->PicWidthInMbs - (int32_t)pps->slice_group_change_direction_flag) / 2;
        int32_t y = ((int32_t)sps->PicHeightInMapUnits - (int32_t)pps->slice_group_change_direction_flag) / 2;

        int32_t leftBound = x;
        int32_t topBound = y;
        int32_t rightBound = x;
        int32_t bottomBound = y;
        int32_t xDir = (int32_t)pps->slice_group_change_direction_flag - 1;
        int32_t yDir = pps->slice_group_change_direction_flag;
        int32_t mapUnitVacant = 0;

        for (int32_t k = 0; k < header->MapUnitsInSliceGroup0; k += mapUnitVacant) {
            mapUnitVacant = (header->mapUnitToSliceGroupMap[y * sps->PicWidthInMbs + x] == 1);
            if (mapUnitVacant) {
                header->mapUnitToSliceGroupMap[y * sps->PicWidthInMbs + x] = 0;
            }
            if (xDir == -1 && x == leftBound) {
                leftBound = codec_max(leftBound - 1, 0);
                x = leftBound;
                xDir = 0;
                yDir = 2 * (int32_t)pps->slice_group_change_direction_flag - 1;
            } else if (xDir == 1 && x == rightBound) {
                rightBound = codec_min(rightBound + 1, sps->PicWidthInMbs - 1);
                x = rightBound;
                xDir = 0;
                yDir = 1 - 2 * (int32_t)pps->slice_group_change_direction_flag;
            } else if (yDir == -1 && y == topBound) {
                topBound = codec_max(topBound - 1, 0);
                y = topBound;
                xDir = 1 - 2 * (int32_t)pps->slice_group_change_direction_flag;
                yDir = 0;
            } else if (yDir == 1 && y == bottomBound) {
                bottomBound = codec_min(bottomBound + 1, sps->PicHeightInMapUnits - 1);
                y = bottomBound;
                xDir = 2 * (int32_t)pps->slice_group_change_direction_flag - 1;
                yDir = 0;
            } else {
                x += xDir;
                y += yDir;
            }
        }
    } else if (pps->slice_group_map_type == 4) { /* @see 8.2.2.5 Specification for raster scan slice group map types */
        int32_t sizeOfUpperLeftGroup = 0;
        if (pps->num_slice_groups_minus1 == 1) {
            sizeOfUpperLeftGroup =
                (pps->slice_group_change_direction_flag ? ((int32_t)sps->PicSizeInMapUnits - (int32_t)header->MapUnitsInSliceGroup0) : (int32_t)header->MapUnitsInSliceGroup0);
        }

        for (uint32_t i = 0; i < sps->PicSizeInMapUnits; i++) {
            if (i < sizeOfUpperLeftGroup) {
                header->mapUnitToSliceGroupMap[i] = pps->slice_group_change_direction_flag;
            } else {
                header->mapUnitToSliceGroupMap[i] = 1 - (int32_t)pps->slice_group_change_direction_flag;
            }
        }
    } else if (pps->slice_group_map_type == 5) { /* @see 8.2.2.6 Specification for wipe slice group map types */
        int32_t sizeOfUpperLeftGroup = 0;
        if (pps->num_slice_groups_minus1 == 1) {
            sizeOfUpperLeftGroup =
                (pps->slice_group_change_direction_flag ? ((int32_t)sps->PicSizeInMapUnits - (int32_t)header->MapUnitsInSliceGroup0) : (int32_t)header->MapUnitsInSliceGroup0);
        }

        int32_t k = 0;
        for (uint32_t j = 0; j < sps->PicWidthInMbs; j++) {
            for (uint32_t i = 0; i < sps->PicHeightInMapUnits; i++) {
                if (k++ < sizeOfUpperLeftGroup) {
                    header->mapUnitToSliceGroupMap[i * sps->PicWidthInMbs + j] = pps->slice_group_change_direction_flag;
                } else {
                    header->mapUnitToSliceGroupMap[i * sps->PicWidthInMbs + j] = 1 - (int32_t)pps->slice_group_change_direction_flag;
                }
            }
        }
    }
}

/* 8.2.2.8 Specification for conversion of map unit to slice group map to macroblock to slice group map */
static int compute_MbToSliceGroupMap(SliceHeader* header, H264Context* context) {
    PPS* pps = context->active_pps;
    SPS* sps = context->active_sps;

    uint32_t i = 0;

    int32_t* mapUnitToSliceGroupMapPtr;

    if (pps->slice_group_map_type == 3 || pps->slice_group_map_type == 4 || pps->slice_group_map_type == 5) {
        mapUnitToSliceGroupMapPtr = header->mapUnitToSliceGroupMap;
    } else {
        mapUnitToSliceGroupMapPtr = pps->mapUnitToSliceGroupMap;
    }

    if (sps->frame_mbs_only_flag == 1 || header->field_pic_flag == 1) {
        for (i = 0; i < header->PicSizeInMbs; i++) {
            header->MbToSliceGroupMap[i] = mapUnitToSliceGroupMapPtr[i];
        }
    } else if (header->MbaffFrameFlag == 1) {
        for (i = 0; i < header->PicSizeInMbs; i++) {
            header->MbToSliceGroupMap[i] = mapUnitToSliceGroupMapPtr[i / 2];
        }
    } else { /* frame_mbs_only_flag is equal to 0 and mb_adaptive_frame_field_flag is equal to 0 and field_pic_flag is equal to 0 */
        for (i = 0; i < header->PicSizeInMbs; i++) {
            header->MbToSliceGroupMap[i] = mapUnitToSliceGroupMapPtr[(i / (2 * sps->PicWidthInMbs)) * sps->PicWidthInMbs + (i % sps->PicWidthInMbs)];
        }
    }

    return ERR_OK;
}

/**
 * @brief post-process of slice layer header without partitioning RBSP. this function should invoked immediately after slice_header()
 *
 * @param header the Slice header
 * @param context the H264 context
 *
 * @return int 0 on success, negative value on error
 */
static int post_process_slice_header_without_partitioning(SliceHeader* header, H264Context* context) {
    PPS* pps = context->active_pps;
    SPS* sps = context->active_sps;

    if (sps->frame_mbs_only_flag) {
        header->pic_coded_type = PICTURE_CODED_FRAME;
    } else if (header->bottom_field_flag) {
        header->pic_coded_type = PICTURE_CODED_BOTTOM_FIELD;
    } else {
        header->pic_coded_type = PICTURE_CODED_TOP_FIELD;
    }

    header->SliceQPY = 26 + pps->pic_init_qp_minus26 + header->slice_qp_delta;
    header->MbaffFrameFlag = (sps->mb_adaptive_frame_field_flag && !header->field_pic_flag);
    header->PicHeightInMbs = sps->FrameHeightInMbs / (1 + header->field_pic_flag);
    header->PicHeightInSamplesL = header->PicHeightInMbs * 16;
    header->PicHeightInSamplesC = header->PicHeightInMbs * sps->MbHeightC;
    header->PicSizeInMbs = sps->PicWidthInMbs * header->PicHeightInMbs;
    header->MaxPicNum = (header->field_pic_flag == 0) ? sps->MaxFrameNum : (2 * sps->MaxFrameNum);
    header->CurrPicNum = (header->field_pic_flag == 0) ? header->frame_num : (2 * header->frame_num + 1);

    uint32_t tmp = header->slice_group_change_cycle * (pps->slice_group_change_rate_minus1 + 1);
    header->MapUnitsInSliceGroup0 = codec_min(tmp, sps->PicSizeInMapUnits);
    header->QSY = 26 + pps->pic_init_qs_minus26 + header->slice_qs_delta;

    if (!header->mapUnitToSliceGroupMap) {
        header->mapUnitToSliceGroupMap = (int32_t*)malloc(sizeof(int32_t) * sps->PicSizeInMapUnits);
        if (!header->mapUnitToSliceGroupMap) {
            return ERR_OOM;
        }
        memset(header->mapUnitToSliceGroupMap, 0, sizeof(int32_t) * sps->PicSizeInMapUnits);
    }

    compute_mapUnitToSliceGroupMap(header, context);

    if (!header->MbToSliceGroupMap) {
        header->MbToSliceGroupMap = (int32_t*)malloc(sizeof(int32_t) * header->PicSizeInMbs);
        if (!header->MbToSliceGroupMap) {
            return ERR_OOM;
        }
        memset(header->MbToSliceGroupMap, 0, sizeof(int32_t) * header->PicSizeInMbs);
    }

    compute_MbToSliceGroupMap(header, context);

    header->pps = context->active_pps;
    header->sps = context->active_sps;

    return ERR_OK;
}

/**
 * @brief parse reference picture list MVC modification
 * @see G.7.3.3.1.1 Reference picture list MVC modification syntax
 *
 * @param rbsp_reader the RBSPReader
 * @param rplm the RPLM struct pointer
 * @param header the Slice Header
 * @param sps the SPS
 * @param slice_type slice type
 * @return int 0 on success, negative value on error
 */
int ref_pic_list_mvc_modification(RBSPReader* rbsp_reader, RPLM* rplm, SliceHeader* header, SPS* sps, uint32_t slice_type) {
    int err_code = ERR_INVALID_REF_PIC_LIST_MVC_PARAM;
    size_t i = 0;
    uint32_t mopn = 0;

#define check_range(name, low, high)             \
    if (rplm->name < low || rplm->name > high) { \
        goto error_flag;                         \
    }

    if (slice_type % 5 != 2 && slice_type % 5 != 4) {
        rplm->ref_pic_list_modification_flag_l0 = (uint8_t)read_u(rbsp_reader, 1);
        if (rplm->ref_pic_list_modification_flag_l0) {
            for (i = 0; i < H264_MAX_RPLM_COUNT; i++) {
                rplm->rplm_l0[i].modification_of_pic_nums_idc = read_ue(rbsp_reader);
                check_range(rplm_l0[i].modification_of_pic_nums_idc, 0, 5);

                mopn = rplm->rplm_l0[i].modification_of_pic_nums_idc;

                if (mopn == 3) {
                    break;
                }

                if (mopn == 0 || mopn == 1) {
                    rplm->rplm_l0[i].abs_diff_pic_num_minus1 = read_ue(rbsp_reader);
                    check_range(rplm_l0[i].abs_diff_pic_num_minus1, 0, (1 + header->field_pic_flag) * (1 << (sps->log2_max_frame_num_minus4 + 4)) - 1);
                } else if (mopn == 2) {
                    rplm->rplm_l0[i].long_term_pic_num = read_ue(rbsp_reader);
                } else if (mopn == 4 || mopn == 5) {
                    rplm->rplm_l0[i].abs_diff_view_idx_minus = read_ue(rbsp_reader);
                }
            }

            if (i == H264_MAX_RPLM_COUNT) {
                rplm->rplm_l0_len = H264_MAX_RPLM_COUNT;
            } else {
                rplm->rplm_l0_len = i + 1;
            }
        }
    }

    if (slice_type % 5 == 1) {
        rplm->ref_pic_list_modification_flag_l1 = (uint8_t)read_u(rbsp_reader, 1);
        if (rplm->ref_pic_list_modification_flag_l1) {
            for (i = 0; i < H264_MAX_RPLM_COUNT; i++) {
                rplm->rplm_l1[i].modification_of_pic_nums_idc = read_ue(rbsp_reader);
                check_range(rplm_l1[i].modification_of_pic_nums_idc, 0, 5);

                mopn = rplm->rplm_l1[i].modification_of_pic_nums_idc;

                if (mopn == 3) {
                    break;
                }

                if (mopn == 0 || mopn == 1) {
                    rplm->rplm_l1[i].abs_diff_pic_num_minus1 = read_ue(rbsp_reader);
                    check_range(rplm_l1[i].abs_diff_pic_num_minus1, 0, (1 + header->field_pic_flag) * (1 << (sps->log2_max_frame_num_minus4 + 4)) - 1);
                } else if (mopn == 2) {
                    rplm->rplm_l1[i].long_term_pic_num = read_ue(rbsp_reader);
                } else if (mopn == 4 || mopn == 5) {
                    rplm->rplm_l1[i].abs_diff_view_idx_minus = read_ue(rbsp_reader);
                }
            }

            if (i == H264_MAX_RPLM_COUNT) {
                rplm->rplm_l1_len = H264_MAX_RPLM_COUNT;
            } else {
                rplm->rplm_l1_len = i + 1;
            }
        }
    }

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

/**
 * @brief parse reference picture list modification
 * @see 7.3.3.1 Reference picture list modification syntax
 *
 * @param rbsp_reader the RBSPReader
 * @param rplm the RPLM struct pointer
 * @param header the Slice Header
 * @param sps the SPS
 * @param slice_type slice type
 * @return int 0 on success, negative value on error
 */
int ref_pic_list_modification(RBSPReader* rbsp_reader, RPLM* rplm, SliceHeader* header, SPS* sps, uint32_t slice_type) {
    int err_code = ERR_INVALID_REF_PIC_LIST_PARAM;
    size_t i = 0;
    uint32_t mopn = 0;

#define check_range(name, low, high)             \
    if (rplm->name < low || rplm->name > high) { \
        goto error_flag;                         \
    }

    if (slice_type % 5 != 2 && slice_type % 5 != 4) {
        rplm->ref_pic_list_modification_flag_l0 = (uint8_t)read_u(rbsp_reader, 1);
        if (rplm->ref_pic_list_modification_flag_l0) {
            for (i = 0; i < H264_MAX_RPLM_COUNT; i++) {
                rplm->rplm_l0[i].modification_of_pic_nums_idc = read_ue(rbsp_reader);
                check_range(rplm_l0[i].modification_of_pic_nums_idc, 0, 3);

                mopn = rplm->rplm_l0[i].modification_of_pic_nums_idc;

                if (mopn == 3) {
                    break;
                }

                if (mopn == 0 || mopn == 1) {
                    rplm->rplm_l0[i].abs_diff_pic_num_minus1 = read_ue(rbsp_reader);
                    check_range(rplm_l0[i].abs_diff_pic_num_minus1, 0, (1 + header->field_pic_flag) * (1 << (sps->log2_max_frame_num_minus4 + 4)) - 1);
                } else if (mopn == 2) {
                    rplm->rplm_l0[i].long_term_pic_num = read_ue(rbsp_reader);
                }
            }

            if (i == H264_MAX_RPLM_COUNT) {
                rplm->rplm_l0_len = H264_MAX_RPLM_COUNT;
            } else {
                rplm->rplm_l0_len = i + 1;
            }
        }
    }

    if (slice_type % 5 == 1) {
        rplm->ref_pic_list_modification_flag_l1 = (uint8_t)read_u(rbsp_reader, 1);
        if (rplm->ref_pic_list_modification_flag_l1) {
            for (i = 0; i < H264_MAX_RPLM_COUNT; i++) {
                rplm->rplm_l1[i].modification_of_pic_nums_idc = read_ue(rbsp_reader);
                check_range(rplm_l1[i].modification_of_pic_nums_idc, 0, 3);

                mopn = rplm->rplm_l1[i].modification_of_pic_nums_idc;

                if (mopn == 3) {
                    break;
                }

                if (mopn == 0 || mopn == 1) {
                    rplm->rplm_l1[i].abs_diff_pic_num_minus1 = read_ue(rbsp_reader);
                    check_range(rplm_l1[i].abs_diff_pic_num_minus1, 0, (1 + header->field_pic_flag) * (1 << (sps->log2_max_frame_num_minus4 + 4)) - 1);
                } else if (mopn == 2) {
                    rplm->rplm_l1[i].long_term_pic_num = read_ue(rbsp_reader);
                }
            }

            if (i == H264_MAX_RPLM_COUNT) {
                rplm->rplm_l1_len = H264_MAX_RPLM_COUNT;
            } else {
                rplm->rplm_l1_len = i + 1;
            }
        }
    }

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

/**
 * @brief parse decoded reference picture marking
 * @see 7.3.3.3 Decoded reference picture marking syntax
 * @see 7.4.3.3 Decoded reference picture marking semantics
 *
 * @param rbsp_reader the RBSPReader
 * @param mark the DecRefPicMark struct pointer
 * @param idr_pic_flag
 * @return int 0 on success, negative value on error
 */
int dec_ref_pic_marking(RBSPReader* rbsp_reader, DecRefPicMark* mark, SPS* sps, int idr_pic_flag) {
    int err_code = ERR_INVALID_DEC_REF_PIC_MARKING;
    size_t i = 0;
    uint32_t mmco = 0;

#define check_range(name, low, high)             \
    if (mark->name < low || mark->name > high) { \
        goto error_flag;                         \
    }

    if (idr_pic_flag) {
        mark->no_output_of_prior_pics_flag = (uint8_t)read_u(rbsp_reader, 1);
        mark->long_term_reference_flag = (uint8_t)read_u(rbsp_reader, 1);
    } else {
        mark->adaptive_ref_pic_marking_mode_flag = (uint8_t)read_u(rbsp_reader, 1);
        if (mark->adaptive_ref_pic_marking_mode_flag) {
            for (i = 0; i < H264_MAX_MMCO_COUNT; ++i) {
                mark->mmco[i].memory_management_control_operation = read_ue(rbsp_reader);
                check_range(mmco[i].memory_management_control_operation, 0, 6);

                mmco = mark->mmco[i].memory_management_control_operation;
                if (mmco == 0) {
                    break;
                }

                if (mmco == 1 || mmco == 3) {
                    mark->mmco[i].difference_of_pic_nums_minus1 = read_ue(rbsp_reader);
                }

                if (mmco == 2) {
                    mark->mmco[i].long_term_pic_num = read_ue(rbsp_reader);
                    check_range(mmco[i].long_term_pic_num, 0, (sps->max_num_ref_frames - 1));
                }

                if (mmco == 3 || mmco == 6) {
                    mark->mmco[i].long_term_frame_idx = read_ue(rbsp_reader);
                    check_range(mmco[i].long_term_frame_idx, 0, (sps->max_num_ref_frames - 1));
                }

                if (mmco == 4) {
                    mark->mmco[i].max_long_term_frame_idx_plus1 = read_ue(rbsp_reader);
                    check_range(mmco[i].max_long_term_frame_idx_plus1, 0, sps->max_num_ref_frames);
                }
            }

            if (i == H264_MAX_MMCO_COUNT) {
                goto error_flag;
            }

            mark->mmco_len = i + 1;
        }
    }

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

static int pred_weight_table(RBSPReader* rbsp_reader, SliceHeader* header, SPS* sps, PPS* pps, H264Context* context) {
    /* @see 7.3.3.2 Prediction weight table syntax */
    /* @see 7.4.3.2 Prediction weight table semantics*/
    int err_code = ERR_INVALID_PRE_WEIGHT_TABLE;
    int i = 0;
    int j = 0;
    PredWeightTable* pwt = &header->pred_weight_table;

#define check_range(name, low, high)           \
    if (pwt->name < low || pwt->name > high) { \
        goto error_flag;                       \
    }

    pwt->luma_log2_weight_denom = read_ue(rbsp_reader);
    check_range(luma_log2_weight_denom, 0, 7);

    if (sps->ChromaArrayType) {
        pwt->chroma_log2_weight_denom = read_ue(rbsp_reader);
        check_range(chroma_log2_weight_denom, 0, 7);
    }

    for (i = 0; i <= header->num_ref_idx_l0_active_minus1; ++i) {
        pwt->luma_weight_l0_flag[i] = (uint8_t)read_u(rbsp_reader, 1);
        if (pwt->luma_weight_l0_flag[i]) {
            pwt->luma_weight_l0[i] = read_se(rbsp_reader);
            check_range(luma_weight_l0[i], -128, 127);

            pwt->luma_offset_l0[i] = read_se(rbsp_reader);
            check_range(luma_offset_l0[i], -128, 127);
        } else {
            pwt->luma_weight_l0[i] = 1 << pwt->luma_log2_weight_denom;
            pwt->luma_offset_l0[i] = 0;
        }

        if (sps->ChromaArrayType) {
            pwt->chroma_weight_l0_flag[i] = (uint8_t)read_u(rbsp_reader, 1);
            if (pwt->chroma_weight_l0_flag[i]) {
                for (j = 0; j < 2; ++j) {
                    pwt->chroma_weight_l0[i][j] = read_se(rbsp_reader);
                    check_range(chroma_weight_l0[i][j], -128, 127);

                    pwt->chroma_offset_l0[i][j] = read_se(rbsp_reader);
                    check_range(chroma_offset_l0[i][j], -128, 127);
                }
            } else {
                pwt->chroma_weight_l0[i][0] = 1 << pwt->chroma_log2_weight_denom;
                pwt->chroma_weight_l0[i][1] = 1 << pwt->chroma_log2_weight_denom;
                pwt->chroma_offset_l0[i][0] = 0;
                pwt->chroma_offset_l0[i][1] = 0;
            }
        }
    }

    if (header->slice_type % 5 == 1) {
        for (i = 0; i <= header->num_ref_idx_l1_active_minus1; ++i) {
            pwt->luma_weight_l1_flag[i] = (uint8_t)read_u(rbsp_reader, 1);
            if (pwt->luma_weight_l1_flag[i]) {
                pwt->luma_weight_l1[i] = read_se(rbsp_reader);
                check_range(luma_weight_l1[i], -128, 127);

                pwt->luma_offset_l1[i] = read_se(rbsp_reader);
                check_range(luma_offset_l1[i], -128, 127);
            } else {
                pwt->luma_weight_l1[i] = 1 << pwt->luma_log2_weight_denom;
                pwt->luma_offset_l1[i] = 0;
            }

            if (sps->ChromaArrayType) {
                pwt->chroma_weight_l1_flag[i] = (uint8_t)read_u(rbsp_reader, 1);
                if (pwt->chroma_weight_l1_flag[i]) {
                    for (j = 0; j < 2; ++j) {
                        pwt->chroma_weight_l1[i][j] = read_se(rbsp_reader);
                        check_range(chroma_weight_l1[i][j], -128, 127);

                        pwt->chroma_offset_l1[i][j] = read_se(rbsp_reader);
                        check_range(chroma_offset_l1[i][j], -128, 127);
                    }
                } else {
                    pwt->chroma_weight_l1[i][0] = 1 << pwt->chroma_log2_weight_denom;
                    pwt->chroma_weight_l1[i][1] = 1 << pwt->chroma_log2_weight_denom;
                    pwt->chroma_offset_l1[i][0] = 0;
                    pwt->chroma_offset_l1[i][1] = 0;
                }
            }
        }
    }

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

static int slice_header(RBSPReader* rbsp_reader, SliceHeader* header, H264Context* context) {
    /* @see 7.3.3 Slice header syntax*/
    /* @see 7.4.3 Slice header semantics*/
    int err_code = ERR_INVALID_SLICE_PARAM;
    SPS* sps = 0;
    PPS* pps = 0;
    int idr_pic_flag = 0;
    int is_slice_type_i = 0;
    int is_slice_type_p = 0;
    int is_slice_type_b = 0;
    int is_slice_type_si = 0;
    int is_slice_type_sp = 0;

#define check_range(name, low, high)                 \
    if (header->name < low || header->name > high) { \
        goto error_flag;                             \
    }

    if (header->nalu_header.nal_unit_type == NALU_CODED_SLICE_AUXILIARY) {
        if (!context->last_slice_nal_unit_type) {
            err_code = ERR_NO_MAIN_PIC_4_AUXILIARY;
            goto error_flag;
        }
        idr_pic_flag = context->last_slice_nal_unit_type == NALU_CODED_SLICE_IDR;
    } else {
        idr_pic_flag = header->nalu_header.nal_unit_type == NALU_CODED_SLICE_IDR;
    }

    header->nalu_header.IdrPicFlag = (int32_t)idr_pic_flag;

    header->first_mb_in_slice = read_ue(rbsp_reader);
    check_range(first_mb_in_slice, 0, H264_MAX_MB_PIC_SIZE - 1);

    header->slice_type = read_ue(rbsp_reader);
    check_range(slice_type, 0, 9);

    /* @see Table 7-6 – Name association to slice_type */
    is_slice_type_i = header->slice_type % 5 == 2;
    is_slice_type_p = header->slice_type % 5 == 0;
    is_slice_type_b = header->slice_type % 5 == 1;
    is_slice_type_si = header->slice_type % 5 == 4;
    is_slice_type_sp = header->slice_type % 5 == 3;

    if (idr_pic_flag && !(is_slice_type_i || is_slice_type_si)) {
        err_code = ERR_INVALID_SLICE_4_IDR;
        goto error_flag;
    }

    header->pic_parameter_set_id = read_ue(rbsp_reader);
    check_range(pic_parameter_set_id, 0, (H264_MAX_PPS_COUNT - 1));

    pps = context->pps[header->pic_parameter_set_id];
    if (!pps) {
        fprintf(stderr, "invalid ref pps id: %d when parsing slice header\n", header->pic_parameter_set_id);
        err_code = ERR_NO_PPS_PARSED;
        goto error_flag;
    }
    context->active_pps = pps;

    if (pps->seq_parameter_set_id >= H264_MAX_SPS_COUNT) {
        err_code = ERR_INVALID_SPS_ID;
        goto error_flag;
    }
    sps = context->sps[pps->seq_parameter_set_id];
    if (!sps) {
        fprintf(stderr, "invalid ref sps id: %d when parsing slice header\n", pps->seq_parameter_set_id);
        err_code = ERR_NO_SPS_PARSED;
        goto error_flag;
    }
    context->active_sps = sps;

    if (sps->separate_colour_plane_flag == 1) {
        /* colour_plane_id equal to 0, 1, and 2 correspond to the Y, Cb, and Cr planes, respectively. */
        header->colour_plane_id = (uint8_t)read_u(rbsp_reader, 2);
        check_range(colour_plane_id, 0, 2);
    }

    header->frame_num = read_u(rbsp_reader, sps->log2_max_frame_num_minus4 + 4);
    if (!sps->frame_mbs_only_flag) {
        header->field_pic_flag = (uint8_t)read_u(rbsp_reader, 1);
        if (header->field_pic_flag) {
            header->bottom_field_flag = (uint8_t)read_u(rbsp_reader, 1);
        } else {
            header->bottom_field_flag = 0;
        }
    } else {
        header->field_pic_flag = 0;
        header->bottom_field_flag = 0;
    }

    if (idr_pic_flag) {
        header->idr_pic_id = read_ue(rbsp_reader);
        check_range(idr_pic_id, 0, 65535);
    }

    if (sps->pic_order_cnt_type == 0) {
        header->pic_order_cnt_lsb = read_u(rbsp_reader, sps->log2_max_pic_order_cnt_lsb_minus4 + 4);
        /* skip checking range of pic_order_cnt_lsb */
        if (pps->bottom_field_pic_order_in_frame_present_flag && !header->field_pic_flag) {
            header->delta_pic_order_cnt_bottom = read_se(rbsp_reader);
            check_range(delta_pic_order_cnt_bottom, INT32_MIN + 1, INT32_MAX);
        } else {
            header->delta_pic_order_cnt_bottom = 0;
        }
    } else {
        header->delta_pic_order_cnt_bottom = 0;
    }

    if (sps->pic_order_cnt_type == 1 && !sps->delta_pic_order_always_zero_flag) {
        header->delta_pic_order_cnt[0] = read_se(rbsp_reader);
        check_range(delta_pic_order_cnt[0], INT32_MIN + 1, INT32_MAX);

        if (pps->bottom_field_pic_order_in_frame_present_flag && !header->field_pic_flag) {
            header->delta_pic_order_cnt[1] = read_se(rbsp_reader);
            check_range(delta_pic_order_cnt[1], INT32_MIN + 1, INT32_MAX);
        } else {
            header->delta_pic_order_cnt[1] = 0;
        }
    } else {
        header->delta_pic_order_cnt[0] = 0;
        header->delta_pic_order_cnt[1] = 0;
    }

    if (pps->redundant_pic_cnt_present_flag) {
        header->redundant_pic_cnt = read_ue(rbsp_reader);
        check_range(redundant_pic_cnt, 0, 127);
    } else {
        header->redundant_pic_cnt = 0;
    }

    if (header->nalu_header.nal_unit_type != NALU_CODED_SLICE_AUXILIARY && !header->redundant_pic_cnt) {
        context->last_slice_nal_unit_type = header->nalu_header.nal_unit_type;
    }

    if (is_slice_type_b) {
        header->direct_spatial_mv_pred_flag = (uint8_t)read_u(rbsp_reader, 1);
    }

    if (is_slice_type_p || is_slice_type_sp || is_slice_type_b) {
        header->num_ref_idx_active_override_flag = (uint8_t)read_u(rbsp_reader, 1);
        if (header->num_ref_idx_active_override_flag) {
            header->num_ref_idx_l0_active_minus1 = read_ue(rbsp_reader);
            check_range(num_ref_idx_l0_active_minus1, 0, 31);
            if (is_slice_type_b) {
                header->num_ref_idx_l1_active_minus1 = read_ue(rbsp_reader);
                check_range(num_ref_idx_l1_active_minus1, 0, 31);
            } else {
                header->num_ref_idx_l1_active_minus1 = pps->num_ref_idx_l1_active_minus1;
            }
        } else {
            header->num_ref_idx_l0_active_minus1 = pps->num_ref_idx_l0_active_minus1;
            header->num_ref_idx_l1_active_minus1 = pps->num_ref_idx_l1_active_minus1;
        }
    } else {
        header->num_ref_idx_l0_active_minus1 = pps->num_ref_idx_l0_active_minus1;
        header->num_ref_idx_l1_active_minus1 = pps->num_ref_idx_l1_active_minus1;
    }

    if (header->nalu_header.nal_unit_type == NALU_CODED_SLICE_EXTENSION || header->nalu_header.nal_unit_type == NALU_CODED_SLICE_EXTENSION_DV) {
        header->rplm_mvc = 1;
        err_code = ref_pic_list_mvc_modification(rbsp_reader, &header->rplm, header, sps, header->slice_type);
        goto error_flag;
    } else {
        header->rplm_mvc = 0;
        err_code = ref_pic_list_modification(rbsp_reader, &header->rplm, header, sps, header->slice_type);
        goto error_flag;
    }

    if ((pps->weighted_pred_flag && (is_slice_type_p || is_slice_type_sp)) || (pps->weighted_bipred_idc == 1 && is_slice_type_b)) {
        err_code = pred_weight_table(rbsp_reader, header, sps, pps, context);
        goto error_flag;
    }

    if (header->nalu_header.nal_ref_idc != 0) {
        err_code = dec_ref_pic_marking(rbsp_reader, &header->dec_ref_pic_mark, sps, idr_pic_flag);
        goto error_flag;
    }

    if (pps->entropy_coding_mode_flag && !is_slice_type_i && !is_slice_type_si) {
        header->cabac_init_idc = read_ue(rbsp_reader);
        check_range(cabac_init_idc, 0, 2);
    }

    header->slice_qp_delta = read_se(rbsp_reader);
    check_range(slice_qp_delta, (-6 * (int32_t)sps->bit_depth_luma_minus8 - 26 - pps->pic_init_qp_minus26), (51 - 26 - pps->pic_init_qp_minus26));

    if (is_slice_type_sp || is_slice_type_si) {
        if (is_slice_type_sp) {
            header->sp_for_switch_flag = (uint8_t)read_u(rbsp_reader, 1);
        }
        header->slice_qs_delta = read_se(rbsp_reader);
        check_range(slice_qp_delta, (-26 - pps->pic_init_qs_minus26), (51 - 26 - pps->pic_init_qs_minus26));
    }

    if (pps->deblocking_filter_control_present_flag) {
        header->disable_deblocking_filter_idc = read_ue(rbsp_reader);
        check_range(disable_deblocking_filter_idc, 0, 2);
        if (header->disable_deblocking_filter_idc != 1) {
            header->slice_alpha_c0_offset_div2 = read_se(rbsp_reader);
            check_range(slice_alpha_c0_offset_div2, -6, 6);

            header->slice_beta_offset_div2 = read_se(rbsp_reader);
            check_range(slice_beta_offset_div2, -6, 6);
        } else {
            header->slice_alpha_c0_offset_div2 = 0;
            header->slice_beta_offset_div2 = 0;
        }
    } else {
        header->disable_deblocking_filter_idc = 0;
        header->slice_alpha_c0_offset_div2 = 0;
        header->slice_beta_offset_div2 = 0;
    }

    if (pps->num_slice_groups_minus1 > 0 && pps->slice_group_map_type >= 3 && pps->slice_group_map_type <= 5) {
        uint32_t max_v = (sps->PicSizeInMapUnits + pps->slice_group_change_rate_minus1) / (pps->slice_group_change_rate_minus1 + 1);

        header->slice_group_change_cycle = read_u(rbsp_reader, int_ceil_log2(max_v + 1));
        check_range(slice_group_change_cycle, 0, max_v);
    }

#undef check_range

    return ERR_OK;

error_flag:
    return err_code;
}

static void free_slice_header(void* nalu) {
    SliceHeader* header = (SliceHeader*)nalu;

    if (header->mapUnitToSliceGroupMap) {
        free(header->mapUnitToSliceGroupMap);
        header->mapUnitToSliceGroupMap = 0;
    }
    if (header->MbToSliceGroupMap) {
        free(header->MbToSliceGroupMap);
        header->MbToSliceGroupMap = 0;
    }

    free(header);
}

void reset_slice_header(SliceHeader* header) {
    if (header->mapUnitToSliceGroupMap) {
        free(header->mapUnitToSliceGroupMap);
        header->mapUnitToSliceGroupMap = 0;
    }
    if (header->MbToSliceGroupMap) {
        free(header->MbToSliceGroupMap);
        header->MbToSliceGroupMap = 0;
    }

    memset(header, 0, sizeof(SliceHeader));
}

int slice_layer_without_partitioning_rbsp(RBSPReader* rbsp_reader, SliceHeader* header, H264Context* context) {
    /*@see 7.3.2.8 Slice layer without partitioning RBSP syntax */

    int err_code = ERR_OK;

    header->nalu_header.free = free_slice_header;

    err_code = slice_header(rbsp_reader, header, context);
    if (err_code < 0) {
        goto error_flag;
    }

    if (!context->active_pps) {
        err_code = ERR_NO_ACTIVE_PPS;
        goto error_flag;
    }

    if (!context->active_sps) {
        err_code = ERR_NO_ACTIVE_SPS;
        goto error_flag;
    }

    err_code = post_process_slice_header_without_partitioning(header, context);
    if (err_code < 0) {
        goto error_flag;
    }

    /* rbsp_slice_trailing_bits(rbsp_reader, context->active_pps->entropy_coding_mode_flag); */

    return ERR_OK;

error_flag:
    return err_code;
}