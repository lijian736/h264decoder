#include "h264decoder/h264_picture.h"

/**
 * @brief Get the next mb address in the same slice group
 *
 * @param slice pointer to the slice
 * @param n the current mb address
 * @return int32_t
 */
static int32_t NextMbAddress(SliceHeader* header, int32_t current_addr) {
    int32_t i = current_addr + 1;
    if (i >= header->PicSizeInMbs) {
        return ERR_INVALID_NEXT_MB_ADDR;
    }

    while (i < header->PicSizeInMbs && header->MbToSliceGroupMap[i] != header->MbToSliceGroupMap[current_addr]) {
        i++;
        if (i >= header->PicSizeInMbs) {
            return ERR_INVALID_NEXT_MB_ADDR;
        }
    }

    return i;
}

FrameOrField* create_frame_or_field() {
    FrameOrField* ff = (FrameOrField*)malloc(sizeof(FrameOrField));
    if (!ff) {
        return 0;
    }
    memset(ff, 0, sizeof(FrameOrField));

    return ff;
}

int init_frame_or_field(FrameOrField* ff, SliceHeader* slice_header) {
    if (!ff->mb_list) {
        ff->mb_list = (MacroBlock*)malloc(slice_header->PicSizeInMbs * sizeof(MacroBlock));
        if (!ff->mb_list) {
            return ERR_OOM;
        }
        memset(ff->mb_list, 0, slice_header->PicSizeInMbs * sizeof(MacroBlock));
        ff->mb_list_len = (int)slice_header->PicSizeInMbs;
        ff->current_mb = 0;
    }

    return ERR_OK;
}

void reset_frame_or_field(FrameOrField* ff) {
    /*TODO*/
    if (ff->mb_list) {
        free(ff->mb_list);
        ff->mb_list = 0;
        ff->mb_list_len = 0;
        ff->current_mb = 0;
    }
}

void free_frame_or_field(FrameOrField* ff) {
    /*TODO*/
    if (ff->mb_list) {
        free(ff->mb_list);
        ff->mb_list = 0;
        ff->mb_list_len = 0;
        ff->current_mb = 0;
    }
    free(ff);
}

Picture* create_picture() {
    Picture* pic = (Picture*)malloc(sizeof(Picture));
    if (!pic) {
        return 0;
    }
    memset(pic, 0, sizeof(Picture));

    pic->frame = create_frame_or_field();
    if (!pic->frame) {
        free_picture(pic);
        return 0;
    }

    pic->top_field = create_frame_or_field();
    if (!pic->top_field) {
        free_picture(pic);
        return 0;
    }

    pic->bottom_field = create_frame_or_field();
    if (!pic->bottom_field) {
        free_picture(pic);
        return 0;
    }

    /*TODO*/
}

void reset_picture(Picture* picture) {
    /*TODO*/
    if (picture->frame) {
        reset_frame_or_field(picture->frame);
    }

    if (picture->top_field) {
        reset_frame_or_field(picture->top_field);
    }

    if (picture->bottom_field) {
        reset_frame_or_field(picture->bottom_field);
    }

    memset(picture, 0, sizeof(Picture));
}

void free_picture(Picture* picture) {
    /*TODO*/
    if (picture->frame) {
        free_frame_or_field(picture->frame);
        picture->frame = 0;
    }

    if (picture->top_field) {
        free_frame_or_field(picture->top_field);
        picture->top_field = 0;
    }

    if (picture->bottom_field) {
        free_frame_or_field(picture->bottom_field);
        picture->bottom_field = 0;
    }

    free(picture);
}

/* 7.4.1.2.4 Detection of the first VCL NAL unit of a primary coded picture */
int detect_first_VCL_NAL_of_primary_coded_picture(SliceHeader* current, SliceHeader* prev) {
    /* 7.4.3 Slice header semantics When present, the value of the slice header syntax elements pic_parameter_set_id, frame_num, field_pic_flag, bottom_field_flag, idr_pic_id,
     * pic_order_cnt_lsb, delta_pic_order_cnt_bottom, delta_pic_order_cnt[ 0 ], delta_pic_order_cnt[ 1 ], sp_for_switch_flag, and slice_group_change_cycle shall be the same in all
     * slice headers of a coded picture. */

#define check_result_and_return(result) \
    if (result) {                       \
        return 1;                       \
    }

    int result = 0;

    /* check if the previous slice exists*/
    if (!prev) {
        return 1;
    }

    /* check if the current slice's PPS id is same with the previous slice's PPS id*/
    result = current->pic_parameter_set_id != prev->pic_parameter_set_id;
    check_result_and_return(result);

    /* The value of frame_num used to test this condition is the value of frame_num that appears in the syntax of the slice header */
    result = current->frame_num != prev->frame_num;
    check_result_and_return(result);

    /* field_pic_flag equal to 1 specifies that the slice is a slice of a coded field. field_pic_flag equal to 0 specifies that the slice is a slice of a coded frame. */
    result = current->field_pic_flag != prev->field_pic_flag;
    check_result_and_return(result);

    if (current->field_pic_flag && prev->field_pic_flag) {
        /* bottom_field_flag is present in both and differs in value. */
        result = current->bottom_field_flag != prev->bottom_field_flag;
        check_result_and_return(result);
    }

    /* nal_ref_idc differs in value with one of the nal_ref_idc values being equal to 0. */
    result = (current->nalu_header.nal_ref_idc != prev->nalu_header.nal_ref_idc) && ((current->nalu_header.nal_ref_idc == 0) || (prev->nalu_header.nal_ref_idc == 0));
    check_result_and_return(result);

    /* pic_order_cnt_type is equal to 0 for both and either pic_order_cnt_lsb differs in value, or delta_pic_order_cnt_bottom differs in value. */
    if (current->sps->pic_order_cnt_type == 0) {
        result = current->pic_order_cnt_lsb != prev->pic_order_cnt_lsb;
        check_result_and_return(result);

        if (current->pps->bottom_field_pic_order_in_frame_present_flag && !current->field_pic_flag) {
            result = current->delta_pic_order_cnt_bottom != prev->delta_pic_order_cnt_bottom;
            check_result_and_return(result);
        }
    }

    /* pic_order_cnt_type is equal to 1 for both and either delta_pic_order_cnt[0] differs in value, or delta_pic_order_cnt[1] differs in value. */
    if (current->sps->pic_order_cnt_type == 1) {
        if (!current->sps->delta_pic_order_always_zero_flag) {
            result = current->delta_pic_order_cnt[0] != prev->delta_pic_order_cnt[0];
            check_result_and_return(result);

            if (current->pps->bottom_field_pic_order_in_frame_present_flag && !current->field_pic_flag) {
                result = current->delta_pic_order_cnt[1] != prev->delta_pic_order_cnt[1];
                check_result_and_return(result);
            }
        }
    }

    /* IdrPicFlag differs in value. */
    result = current->nalu_header.IdrPicFlag != prev->nalu_header.IdrPicFlag;
    check_result_and_return(result);

    /* IdrPicFlag is equal to 1 for both and idr_pic_id differs in value. */
    if (current->nalu_header.IdrPicFlag && prev->nalu_header.IdrPicFlag) {
        result = current->idr_pic_id != prev->idr_pic_id;
        check_result_and_return(result);
    }

#undef check_result_and_return

    return 0;
}

int slice_data(FrameOrField* ff, RBSPReader* rbsp_reader, SliceHeader* header) {
    /* @see 7.3.4 Slice data syntax */
    /* @see 7.4.4 Slice data semantics */

    int err_code = ERR_OK;

    SPS* sps = header->sps;
    PPS* pps = header->pps;

    int32_t is_slice_type_i = (header->slice_type % 5 == SLICE_TYPE_I);
    int32_t is_slice_type_si = (header->slice_type % 5 == SLICE_TYPE_SI);

    /* get the global singleton CABAC object */
    CABAC* cabac = get_singleton_cabac();

    uint8_t entropy_coding_mode_flag = pps->entropy_coding_mode_flag;
    if (entropy_coding_mode_flag) {
        while (!is_byte_aligned(rbsp_reader)) {
            /* uint32_t cabac_alignment_one_bit; */
            (void)read_u(rbsp_reader, 1);
        }

        err_code = cabac_init_context_variables(cabac, header->slice_type, header->cabac_init_idc, header->SliceQPY);
        if (err_code < 0) {
            goto error_flag;
        }

        err_code = cabac_init_arithmetic_decoding_engine(rbsp_reader, cabac);
        if (err_code < 0) {
            goto error_flag;
        }
    }

    int32_t CurrMbAddr = header->first_mb_in_slice * (1 + header->MbaffFrameFlag);
    int32_t moreDataFlag = 1;
    int32_t prevMbSkipped = 0;

    uint32_t mb_skip_run = 0;
    int32_t mb_skip_flag = 0;
    int32_t end_of_slice_flag = 0;

    do {
        if (!is_slice_type_i && !is_slice_type_si) {
            if (!pps->entropy_coding_mode_flag) {
                mb_skip_run = read_ue(rbsp_reader);
                prevMbSkipped = (mb_skip_run > 0);
                for (uint32_t i = 0; i < mb_skip_run; i++) {
                    CurrMbAddr = NextMbAddress(header, CurrMbAddr);
                }
                if (mb_skip_run > 0) {
                    moreDataFlag = more_rbsp_data(rbsp_reader);
                }
            } else {
                err_code = cabac_mb_skip_flag(rbsp_reader, cabac, ff, header, CurrMbAddr, &mb_skip_flag);
                if (err_code < 0) {
                    return err_code;
                }

                moreDataFlag = !mb_skip_flag;
            }
        }

        if (moreDataFlag) {
            if (header->MbaffFrameFlag && (CurrMbAddr % 2 == 0 || (CurrMbAddr % 2 == 1 && prevMbSkipped))) {
                uint32_t mb_field_decoding_flag;
                if (entropy_coding_mode_flag) {
                    err_code = cabac_mb_field_decoding_flag(rbsp_reader, cabac, ff, header, CurrMbAddr, &mb_field_decoding_flag);
                    if (err_code < 0) {
                        return err_code;
                    }
                } else {
                    mb_field_decoding_flag = read_u(rbsp_reader, 1);
                }
            }

            err_code = macroblock_layer(rbsp_reader, ff, header, cabac, CurrMbAddr);
            if (err_code < 0) {
                return err_code;
            }
        }

        if (!entropy_coding_mode_flag) {
            moreDataFlag = more_rbsp_data(rbsp_reader);
        } else {
            if (!is_slice_type_i && !is_slice_type_si) {
                prevMbSkipped = mb_skip_flag;
            }

            if (header->MbaffFrameFlag && (CurrMbAddr % 2 == 0)) {
                moreDataFlag = 1;
            } else {
                err_code = cabac_end_of_slice_flag(rbsp_reader, cabac, &end_of_slice_flag);
                if (err_code < 0) {
                    return err_code;
                }
                moreDataFlag = !end_of_slice_flag;
            }
        }

        CurrMbAddr = NextMbAddress(header, CurrMbAddr);
    } while (moreDataFlag);

error_flag:
    if (cabac) {
        free(cabac);
    }

    return err_code;
}

int decode_slice(Picture* picture, RBSPReader* rbsp_reader, SliceHeader* header) {
    /* @see 7.3.4 Slice data syntax */
    /* @see 7.4.4 Slice data semantics */
    int err_code = ERR_OK;

    if (!header->field_pic_flag) { /* frame */
        picture->coded_type = PICTURE_CODED_FRAME;

        err_code = init_frame_or_field(picture->frame, header);
        if (err_code < 0) {
            return err_code;
        }
        err_code = slice_data(picture->frame, rbsp_reader, header);
    } else {
        picture->coded_type = PICTURE_CODED_COMPLEMENTARY_FIELD_PAIR;

        if (header->bottom_field_flag) { /* bottom field*/
            picture->coded_type = PICTURE_CODED_BOTTOM_FIELD;
            err_code = slice_data(picture->bottom_field, rbsp_reader, header);
        } else { /* top field */
            picture->coded_type = PICTURE_CODED_TOP_FIELD;
            err_code = slice_data(picture->bottom_field, rbsp_reader, header);
        }
    }

    return err_code;
}