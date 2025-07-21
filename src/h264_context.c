#include "h264decoder/h264_context.h"

#include "h264decoder/h264_nalu_pps.h"
#include "h264decoder/h264_nalu_slice_header.h"
#include "h264decoder/h264_nalu_sps.h"
#include "h264decoder/h264_rbsp.h"

int get_picture_from_context(H264Context* context, int is_new_picture, Picture** out_picture) {
    int err_code = ERR_OK;

    /* FIXME */
    if (is_new_picture || !context->current_picture) {
        context->gop_curr_index = (context->gop_curr_index + 1) % H264_MAX_DPB_FRAMES;
        context->current_picture = context->gop[context->gop_curr_index];

        reset_picture(context->current_picture);
    }

    *out_picture = context->current_picture;

    return err_code;
}

void get_slice_header(H264Context* context, SliceHeader** out_header) {
    SliceHeader* swap_header = context->current_slice_header;
    context->current_slice_header = context->prev_slice_header;
    context->prev_slice_header = swap_header;

    reset_slice_header(context->current_slice_header);

    *out_header = context->current_slice_header;
}

int parse_nalu(const uint8_t* nalu_start, const uint8_t* nalu_end, H264Context* context, void** nalu) {
    int err_code = ERR_OK;
    int rbsp_len = 0;
    uint8_t* rbsp_buffer = 0;
    RBSPReader* rbsp_reader = 0;

    int nal_unit_header_bytes = 1;
    uint8_t svc_extension_flag = 0;
    uint8_t avc_3d_extension_flag = 0;

    uint8_t forbidden_zero_bit = (nalu_start[0] >> 7) & 0x01;
    uint8_t nal_ref_idc = (nalu_start[0] >> 5) & 0x03;
    uint8_t nal_unit_type = nalu_start[0] & 0x1F;

    if (nal_unit_type == NALU_PREFIX_NALU                 /* 14 */
        || nal_unit_type == NALU_CODED_SLICE_EXTENSION    /* 20 */
        || nal_unit_type == NALU_CODED_SLICE_EXTENSION_DV /* 21 */
    ) {
        if (nalu_end - nalu_start <= 1) {
            err_code = ERR_EOS;
            goto exit_flag;
        }

        if (nal_unit_type != NALU_CODED_SLICE_EXTENSION_DV) {
            svc_extension_flag = (uint8_t)(nalu_start[1] >> 7) & 0x01;
        } else {
            avc_3d_extension_flag = (uint8_t)(nalu_start[1] >> 7) & 0x01;
        }

        if (svc_extension_flag) {
            /* TODO: */
            /* specified in Annex F */
            nal_unit_header_bytes += 3;
        } else if (avc_3d_extension_flag) {
            /* TODO: */
            /* specified in Annex I */
            nal_unit_header_bytes += 2;
        } else {
            /* TODO: */
            /* specified in Annex G */
            nal_unit_header_bytes += 3;
        }
    }

    rbsp_buffer = (uint8_t*)malloc(nalu_end - nalu_start - nal_unit_header_bytes);
    if (!rbsp_buffer) {
        err_code = ERR_OOM;
    }
    memset(rbsp_buffer, 0, nalu_end - nalu_start - nal_unit_header_bytes);

    /* rbsp_len = extract_nalu_rbsp(nalu_start + nal_unit_header_bytes, nalu_end - nalu_start - nal_unit_header_bytes, rbsp_buffer); */
    rbsp_len = extract_nalu_rbsp_simple(nalu_start + nal_unit_header_bytes, nalu_end - nalu_start - nal_unit_header_bytes, rbsp_buffer);
    if (rbsp_len < 0) {
        err_code = ERR_INVALID_RBSP;
        goto exit_flag;
    }

    rbsp_reader = (RBSPReader*)malloc(sizeof(RBSPReader));
    if (!rbsp_reader) {
        err_code = ERR_OOM;
        goto exit_flag;
    }
    memset(rbsp_reader, 0, sizeof(RBSPReader));
    rbsp_reader->start = rbsp_buffer;
    rbsp_reader->end = rbsp_buffer + rbsp_len;
    rbsp_reader->current = rbsp_buffer;
    rbsp_reader->bits_left = 8;

    switch (nal_unit_type) {
        case NALU_CODED_SLICE_NON_IDR:
        case NALU_CODED_SLICE_IDR:
        case NALU_CODED_SLICE_AUXILIARY: {
            /* @see 7.3.2.8 Slice layer without partitioning RBSP syntax */
            SliceHeader* slice_header = 0;
            get_slice_header(context, &slice_header);
            slice_header->nalu_header.forbidden_zero_bit = forbidden_zero_bit;
            slice_header->nalu_header.nal_ref_idc = nal_ref_idc;
            slice_header->nalu_header.nal_unit_type = nal_unit_type;
            slice_header->nalu_header.svc_extension_flag = svc_extension_flag;
            slice_header->nalu_header.avc_3d_extension_flag = avc_3d_extension_flag;

            /* parse the slice header */
            err_code = slice_layer_without_partitioning_rbsp(rbsp_reader, slice_header, context);
            if (err_code < 0) {
                err_code = ERR_INVALID_SLICE;
                goto exit_flag;
            }

            /* check if the slice is the first VCL NAL of a primary coded picture */
            int is_first_VCL_NAL = detect_first_VCL_NAL_of_primary_coded_picture(slice_header, context->prev_slice_header);

            Picture* picture;

            /* get a picture from the H264 context to decode the slice data*/
            err_code = get_picture_from_context(context, is_first_VCL_NAL, &picture);
            if (err_code < 0) {
                err_code = ERR_INVALID_SLICE;
                goto exit_flag;
            }

            err_code = decode_slice(picture, rbsp_reader, slice_header);
            if (err_code < 0) {
                err_code = ERR_INVALID_SLICE;
                goto exit_flag;
            }

            rbsp_slice_trailing_bits(rbsp_reader, context->active_pps->entropy_coding_mode_flag);

            break;
        }

        case NALU_SPS: {
            /* @see 7.3.2.1 Sequence parameter set RBSP syntax */
            SPS* sps = (SPS*)malloc(sizeof(SPS));
            if (!sps) {
                err_code = ERR_OOM;
                goto exit_flag;
            }
            memset(sps, 0, sizeof(SPS));
            sps->nalu_header.forbidden_zero_bit = forbidden_zero_bit;
            sps->nalu_header.nal_ref_idc = nal_ref_idc;
            sps->nalu_header.nal_unit_type = nal_unit_type;
            sps->nalu_header.svc_extension_flag = svc_extension_flag;
            sps->nalu_header.avc_3d_extension_flag = avc_3d_extension_flag;

            err_code = seq_parameter_set_rbsp(rbsp_reader, sps);
            if (err_code < 0) {
                free_nalu(sps);
                err_code = ERR_INVALID_SPS;
                goto exit_flag;
            }
            err_code = post_process_sps(sps);
            if (err_code < 0) {
                free_nalu(sps);
                err_code = ERR_INVALID_SPS;
                goto exit_flag;
            }
            *nalu = sps;

            if (!is_end_valid(rbsp_reader)) {
                err_code = ERR_INVALID_RBSP;
            } else {
                err_code = ERR_OK;
            }
            break;
        }

        case NALU_PPS: {
            /* @see 7.3.2.2 Picture parameter set RBSP syntax */
            PPS* pps = (PPS*)malloc(sizeof(PPS));
            if (!pps) {
                err_code = ERR_OOM;
                goto exit_flag;
            }
            memset(pps, 0, sizeof(PPS));
            pps->nalu_header.forbidden_zero_bit = forbidden_zero_bit;
            pps->nalu_header.nal_ref_idc = nal_ref_idc;
            pps->nalu_header.nal_unit_type = nal_unit_type;
            pps->nalu_header.svc_extension_flag = svc_extension_flag;
            pps->nalu_header.avc_3d_extension_flag = avc_3d_extension_flag;

            err_code = pic_parameter_set_rbsp(rbsp_reader, pps, context);
            if (err_code < 0) {
                free_nalu(pps);
                goto exit_flag;
            }
            err_code = post_process_pps(pps, context);
            if (err_code < 0) {
                free_nalu(pps);
                goto exit_flag;
            }

            *nalu = pps;

            if (!is_end_valid(rbsp_reader)) {
                err_code = ERR_INVALID_RBSP;
            } else {
                err_code = ERR_OK;
            }
            break;
        }

        default: {
            NALUHeader* header = (NALUHeader*)malloc(sizeof(NALUHeader));
            if (!header) {
                free_nalu(header);
                err_code = ERR_OOM;
                goto exit_flag;
            }
            memset(header, 0, sizeof(NALUHeader));
            header->forbidden_zero_bit = forbidden_zero_bit;
            header->nal_ref_idc = nal_ref_idc;
            header->nal_unit_type = nal_unit_type;
            header->svc_extension_flag = svc_extension_flag;
            header->avc_3d_extension_flag = avc_3d_extension_flag;

            *nalu = header;

            break;
        }
    }

exit_flag:
    if (rbsp_reader) {
        free(rbsp_reader);
    }

    if (rbsp_buffer) {
        free(rbsp_buffer);
    }

    return err_code;
}

int add_sps_to_context(H264Context* context, SPS* sps) {
    if (sps->seq_parameter_set_id >= H264_MAX_SPS_COUNT) {
        return ERR_INVALID_PARAM;
    }

    if (context->sps[sps->seq_parameter_set_id]) {
        free_nalu(context->sps[sps->seq_parameter_set_id]);
        context->sps[sps->seq_parameter_set_id] = 0;
    }

    context->sps[sps->seq_parameter_set_id] = sps;
    return ERR_OK;
}

int add_pps_to_context(H264Context* context, PPS* pps) {
    if (pps->pic_parameter_set_id >= H264_MAX_PPS_COUNT) {
        return ERR_INVALID_PARAM;
    }

    if (context->pps[pps->pic_parameter_set_id]) {
        free_nalu(context->pps[pps->pic_parameter_set_id]);
        context->pps[pps->pic_parameter_set_id] = 0;
    }

    context->pps[pps->pic_parameter_set_id] = pps;
    return ERR_OK;
}

H264Context* create_context() {
    H264Context* ctx = (H264Context*)malloc(sizeof(H264Context));
    if (!ctx) {
        return 0;
    }
    memset(ctx, 0, sizeof(H264Context));

    for (int i = 0; i < H264_MAX_SPS_COUNT; ++i) {
        SPS* sps = (SPS*)malloc(sizeof(SPS));
        if (!sps) {
            free_context(ctx);
            return 0;
        }
        memset(sps, 0, sizeof(SPS));
        sps->seq_parameter_set_id = H264_INVALID_SPS_ID;

        ctx->sps[i] = sps;
    }

    for (int i = 0; i < H264_MAX_PPS_COUNT; ++i) {
        PPS* pps = (PPS*)malloc(sizeof(PPS));
        if (!pps) {
            free_context(ctx);
            return 0;
        }
        memset(pps, 0, sizeof(PPS));
        pps->pic_parameter_set_id = H264_INVALID_PPS_ID;

        ctx->pps[i] = pps;
    }

    ctx->current_slice_header = (SliceHeader*)malloc(sizeof(SliceHeader));
    if (!ctx->current_slice_header) {
        free_context(ctx);
        return 0;
    }
    memset(ctx->current_slice_header, 0, sizeof(SliceHeader));

    ctx->prev_slice_header = (SliceHeader*)malloc(sizeof(SliceHeader));
    if (!ctx->prev_slice_header) {
        free_context(ctx);
        return 0;
    }
    memset(ctx->prev_slice_header, 0, sizeof(SliceHeader));

    for (int i = 0; i < H264_MAX_DPB_FRAMES; ++i) {
        Picture* pic = (Picture*)malloc(sizeof(Picture));
        if (!pic) {
            free_context(ctx);
            return 0;
        }
        memset(pic, 0, sizeof(Picture));
        ctx->gop[i] = pic;
    }

    ctx->gop_curr_index = -1;

    return ctx;
}

void free_context(H264Context* context) {
    for (int i = 0; i < H264_MAX_SPS_COUNT; ++i) {
        if (context->sps[i]) {
            free_nalu(context->sps[i]);
            context->sps[i] = 0;
        }
    }

    for (int i = 0; i < H264_MAX_PPS_COUNT; ++i) {
        if (context->pps[i]) {
            free_nalu(context->pps[i]);
            context->pps[i] = 0;
        }
    }

    context->active_sps = 0;
    context->active_pps = 0;

    if (context->current_slice_header) {
        free_nalu(context->current_slice_header);
        context->current_slice_header = 0;
    }

    if (context->prev_slice_header) {
        free_nalu(context->prev_slice_header);
        context->prev_slice_header = 0;
    }

    for (int i = 0; i < H264_MAX_DPB_FRAMES; ++i) {
        if (context->gop[i]) {
            free_picture(context->gop[i]);
            context->gop[i] = 0;
        }
    }

    free(context);
}