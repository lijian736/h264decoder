#include "h264decoder/h264_stream.h"

static inline uint8_t *find_start_code_interval(uint8_t *start, uint8_t *end) {
    uint8_t *align = start + 4 - ((intptr_t)start & 3);

    for (end -= 3; start < align && start < end; start++) {
        if (0 == start[0] && 0 == start[1] && 1 == start[2]) {
            return start;
        }
    }

    for (end -= 3; start < end; start += 4) {
        uint32_t x = *(uint32_t *)start;

        if ((x - 0x01010101) & (~x) & 0x80808080) {
            if (0 == start[1]) {
                if (0 == start[0] && 1 == start[2]) {
                    return start;
                }

                if (0 == start[2] && 1 == start[3]) {
                    return start + 1;
                }
            }

            if (0 == start[3]) {
                if (0 == start[2] && 1 == start[4]) {
                    return start + 2;
                }

                if (0 == start[4] && 1 == start[5]) {
                    return start + 3;
                }
            }
        }
    }

    for (end += 3; start < end; start++) {
        if (0 == start[0] && 0 == start[1] && 1 == start[2]) {
            return start;
        }
    }

    return end + 3;
}

static inline uint8_t *find_start_code(uint8_t *start, uint8_t *end) {
    uint8_t *pos = find_start_code_interval(start, end);

    if (start < pos && pos < end && !pos[-1]) {
        pos--;
    }

    return pos;
}

int read_next_nalu(H264BitStream *stream) {
    uint8_t *nalu_start = 0;
    uint8_t *nalu_end = 0;

    nalu_start = find_start_code(stream->nalu_start, stream->end);

    while (nalu_start < stream->end && !*(nalu_start++)) {
        ;
    }

    if (nalu_start == stream->end) {
        return ERR_EOS;
    }

    nalu_end = find_start_code(nalu_start, stream->end);
    stream->nalu_end = nalu_end;
    stream->nalu_start = nalu_start;

    return 0;
}