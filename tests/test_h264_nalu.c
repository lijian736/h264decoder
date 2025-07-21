#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264decoder/h264_context.h"
#include "h264decoder/h264_nalu.h"
#include "h264decoder/h264_nalu_sps.h"
#include "h264decoder/h264_nalu_pps.h"
#include "h264decoder/h264_rbsp.h"
#include "h264decoder/h264_stream.h"

int main(int argc, char **argv) {
    FILE *file = 0;
    long file_size = 0;
    H264BitStream *stream = 0;
    H264Context *context = 0;
    int exit_code = EXIT_FAILURE;
    int err_code = 0;

    if (argc != 2) {
        fprintf(stderr, "invalid parameters\n");
        goto exit_flag;
    }

    file = fopen(argv[1], "rb");

    if (!file) {
        fprintf(stderr, "fail to open file\n");
        goto exit_flag;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *buffer = (uint8_t *)malloc(file_size);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        goto exit_flag;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        fprintf(stderr, "Error reading file\n");
        goto exit_flag;
    }

    stream = (H264BitStream *)malloc(sizeof(H264BitStream));
    if (!stream) {
        fprintf(stderr, "Memory allocation failed\n");
        goto exit_flag;
    }
    memset(stream, 0, sizeof(H264BitStream));
    stream->start = buffer;
    stream->end = buffer + bytes_read;
    stream->nalu_start = buffer;

    context = (H264Context *)malloc(sizeof(H264Context));
    if (!context) {
        fprintf(stderr, "Memory allocation failed\n");
        goto exit_flag;
    }
    memset(context, 0, sizeof(H264Context));

    while (1) {
        err_code = read_next_nalu(stream);
        if (err_code < 0) {
            break;
        }

        void *nalu = 0;
        err_code = parse_nalu(stream->nalu_start, stream->nalu_end, context, &nalu);
        if (err_code < 0) {
            fprintf(stderr, "Parse NALU failed, error code: %d\n", err_code);
        } else {
            NALUHeader *header = (NALUHeader *)nalu;
            char *nalu_type_str = stringify_nalu_type(header->nal_unit_type);
            printf("NALU parsed: [%s]\n", nalu_type_str);

            if (header->nal_unit_type == NALU_SPS) {
                err_code = add_sps_to_context(context, (SPS *)nalu);
                if (err_code < 0) {
                    fprintf(stderr, "add SPS failed, sps id: %d\n", ((SPS *)nalu)->seq_parameter_set_id);
                } else {
                    printf("add SPS [%d] successfully\n", ((SPS *)nalu)->seq_parameter_set_id);
                }

                printf("----------------------------------SPS-----------------------------------------\n");
                printf("SPS [%d] info:\n\n", ((SPS *)nalu)->seq_parameter_set_id);
                /* fprintf_sps(stdout, (SPS *)nalu); */
                printf("----------------------------------SPS-----------------------------------------\n");

                char* chroma_format = get_chroma_format((SPS *)nalu);

                char profile_level[100];
                get_profile_level((SPS *)nalu, profile_level, 100);

                uint32_t width, height;
                get_pic_width_height((SPS *)nalu, &width, &height);

                printf("\n\n");
                printf("----------------------------------SPS---------------------------------------------\n");
                printf("chroma format: %s\n", chroma_format);
                printf("profile and level info: %s\n", profile_level);
                printf("picture width: %u, height: %u\n", width, height);
                printf("----------------------------------SPS---------------------------------------------\n");

            } else if (header->nal_unit_type == NALU_PPS) {
                err_code = add_pps_to_context(context, (PPS *)nalu);
                if (err_code < 0) {
                    fprintf(stderr, "add PPS failed, pps id: %d, ref sps id: %d\n", ((PPS *)nalu)->pic_parameter_set_id,
                            ((PPS *)nalu)->seq_parameter_set_id);
                } else {
                    printf("add PPS [%d] ref sps id: %d successfully\n", ((PPS *)nalu)->pic_parameter_set_id, ((PPS *)nalu)->seq_parameter_set_id);
                }

                printf("\n\n");
                printf("---------------------------------PPS------------------------------------------\n");
                printf("PPS [%d] info:\n\n", ((PPS *)nalu)->pic_parameter_set_id);
                /* fprintf_pps(stdout, (PPS *)nalu); */
                printf("---------------------------------PPS------------------------------------------\n");
            }
        }
    }

    exit_code = EXIT_SUCCESS;

exit_flag:

    if (context) {
        free_context(context);
    }

    if (stream) {
        free(stream);
    }

    if (buffer) {
        free(buffer);
    }

    if (file) {
        fclose(file);
    }

    return exit_code;
}