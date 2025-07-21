#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264decoder/h264_rbsp.h"
#include "h264decoder/h264_stream.h"

int main(int argc, char **argv) {
    FILE *file = 0;
    long file_size = 0;
    H264BitStream *stream = 0;
    uint8_t *rbsp_buffer = 0;
    size_t rbsp_buffer_len = 0;
    int rbsp_actual_len = 0;
    int exit_code = EXIT_FAILURE;
    int err_code = 0;
    size_t nalu_len = 0;

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

    while (1) {
        err_code = read_next_nalu(stream);
        if (err_code < 0) {
            break;
        }

        nalu_len = stream->nalu_end - stream->nalu_start;
        if (!rbsp_buffer) {
            rbsp_buffer_len = nalu_len;
            rbsp_buffer = (uint8_t *)malloc(rbsp_buffer_len);
            if (!rbsp_buffer) {
                goto exit_flag;
            }
        }

        if (nalu_len > rbsp_buffer_len) {
            rbsp_buffer_len = nalu_len;
            free(rbsp_buffer);

            rbsp_buffer = (uint8_t *)malloc(rbsp_buffer_len);
            if (!rbsp_buffer) {
                goto exit_flag;
            }
        }

        /* rbsp_actual_len = extract_nalu_rbsp(stream->nalu_start, nalu_len, rbsp_buffer); */
        rbsp_actual_len = extract_nalu_rbsp_simple(stream->nalu_start, nalu_len, rbsp_buffer);
        if (rbsp_actual_len == ERR_RBSP_INVALID_3_BYTE) {
            fprintf(stderr, "failed to extract RBSP from NALU, invalid three-byte sequences\n");
            goto exit_flag;
        } else if (rbsp_actual_len == ERR_RBSP_INVALID_4_BYTE) {
            fprintf(stderr, "failed to extract RBSP from NALU, invalid four-byte sequences\n");
            goto exit_flag;
        }

        printf("A NALU read: %d, length: %lu. RBSP length: %d \n", stream->nalu_start[0], nalu_len, rbsp_actual_len);
    }

    exit_code = EXIT_SUCCESS;

exit_flag:

    if (rbsp_buffer) {
        free(rbsp_buffer);
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