#include "h264decoder/h264_rbsp.h"

/* Table 9-4 – Assignment of codeNum to values of coded_block_pattern for macroblock prediction modes (a) ChromaArrayType is equal to 1 or 2 */
/* 0: codeNum, 1: coded_block_pattern value for Intra_4x4 or Intra_8x8, 2: coded_block_pattern value for Inter*/
static int32_t g_coded_block_pattern_ChromaArrayType_1_2[48][3] = {
    {0, 47, 0},   {1, 31, 16},  {2, 15, 1},   {3, 0, 2},    {4, 23, 4},   {5, 27, 8},   {6, 29, 32},  {7, 30, 3},   {8, 7, 5},    {9, 11, 10},  {10, 13, 12}, {11, 14, 15},
    {12, 39, 47}, {13, 43, 7},  {14, 45, 11}, {15, 46, 13}, {16, 16, 14}, {17, 3, 6},   {18, 5, 9},   {19, 10, 31}, {20, 12, 35}, {21, 19, 37}, {22, 21, 42}, {23, 26, 44},
    {24, 28, 33}, {25, 35, 34}, {26, 37, 36}, {27, 42, 40}, {28, 44, 39}, {29, 1, 43},  {30, 2, 45},  {31, 4, 46},  {32, 8, 17},  {33, 17, 18}, {34, 18, 20}, {35, 20, 24},
    {36, 24, 19}, {37, 6, 21},  {38, 9, 26},  {39, 22, 28}, {40, 25, 23}, {41, 32, 27}, {42, 33, 29}, {43, 34, 30}, {44, 36, 22}, {45, 40, 25}, {46, 38, 38}, {47, 41, 41},
};

/* Table 9-4 – Assignment of codeNum to values of coded_block_pattern for macroblock prediction modes (b) ChromaArrayType is equal to 0 or 3 */
/* 0: codeNum, 1: coded_block_pattern value for Intra_4x4 or Intra_8x8, 2: coded_block_pattern value for Inter*/
static int32_t g_coded_block_pattern_ChromaArrayType_0_3[16][3] = {
    {0,  15,   0},
    {1,  0,   1},
    {2,  7,   2},
    {3,  11,   4},
    {4,  13,   8},
    {5,  14,   3},
    {6,  3,   5},
    {7,  5,   10},
    {8,  10,   12},
    {9,  12,   15},
    {10,  1,   7},
    {11,  2,   11},
    {12,  4,   13},
    {13,  8,   14},
    {14,  6,   6},
    {15,  9,   9},
};

static inline uint32_t read_u_1(RBSPReader* reader) {
    uint32_t result = 0;

    --reader->bits_left;

    if (reader->current < reader->end) {
        result = (*(reader->current) >> reader->bits_left) & 0x01;
    }

    if (0 == reader->bits_left) {
        reader->current++;
        reader->bits_left = 8;
    }

    return result;
}

int extract_nalu_rbsp(const uint8_t* nalu, size_t nalu_len, uint8_t* rbsp) {
    size_t i = 0;
    size_t len = 0;

    while (i + 3 < nalu_len) {
        if (nalu[i] || nalu[i + 1]) {
            rbsp[len++] = nalu[i++];
        } else {
            if (nalu[i + 2] > 3) {
                rbsp[len++] = nalu[i++];
                rbsp[len++] = nalu[i++];
                rbsp[len++] = nalu[i++];
            } else if (nalu[i + 2] == 3) {
                /* Within the NAL unit, any four-byte sequence that starts with 0x000003 other than the following
                 * sequences shall not occur at any byte-aligned position:
                 * - 0x00000300
                 * - 0x00000301
                 * - 0x00000302
                 * - 0x00000303
                 */
                if (nalu[i + 3] < 4) {
                    rbsp[len++] = nalu[i++];
                    rbsp[len++] = nalu[i++];
                    i++; /* skip emulation_prevention_three_byte */
                    rbsp[len++] = nalu[i++];
                } else {
                    return ERR_RBSP_INVALID_4_BYTE;
                }
            } else {
                /* Within the NAL unit, the following three-byte sequences shall not occur at any byte-aligned position:
                 * - 0x000000
                 * - 0x000001
                 * - 0x000002
                 */
                return ERR_RBSP_INVALID_3_BYTE;
            }
        }
    }

    /* One or more cabac_zero_word 16-bit syntax elements equal to 0x0000 may be present in some RBSPs after the
     * rbsp_trailing_bits( ) at the end of the RBSP.*/

    /* when the last byte of the RBSP data is equal to 0x00 (which can only occur when the RBSP ends in a
     * cabac_zero_word), a final byte equal to 0x03 is appended to the end of the data */

    /* @see 7.4.2.10 RBSP slice trailing bits semantics
     * cabac_zero_word is a byte-aligned sequence of two bytes equal to 0x0000.*/

    /* the final byte (0x03) following the cabac_zero_word will be discarded, and the last two bytes of RBSP must be
     * 0x0000 */
    if ((i + 3) == nalu_len && !nalu[i] && !nalu[i + 1] && nalu[i + 2] == 3) {
        rbsp[len++] = nalu[i++];
        rbsp[len++] = nalu[i++];
    } else {
        while (i < nalu_len) {
            rbsp[len++] = nalu[i++];
        }
    }

    return (int)len;
}

int extract_nalu_rbsp_simple(const uint8_t* nalu, size_t nalu_len, uint8_t* rbsp) {
    size_t i = 0;
    size_t len = 0;

    while (i + 3 < nalu_len) {
        if (nalu[i] || nalu[i + 1]) {
            rbsp[len++] = nalu[i++];
        } else {
            rbsp[len++] = nalu[i++];
            rbsp[len++] = nalu[i++];

            if (nalu[i] == 3 && nalu[i + 1] < 4) {
                i++; /* skip emulation_prevention_three_byte */
            }

            rbsp[len++] = nalu[i++];
        }
    }

    if ((i + 3) == nalu_len && !nalu[i] && !nalu[i + 1] && nalu[i + 2] == 3) {
        rbsp[len++] = nalu[i++];
        rbsp[len++] = nalu[i++];
    } else {
        while (i < nalu_len) {
            rbsp[len++] = nalu[i++];
        }
    }

    return (int)len;
}

inline int is_end_of_reader(RBSPReader* reader) { return reader->current >= reader->end; }

inline int is_n_bits_available(RBSPReader* reader, int n) { return (reader->bits_left + (reader->end - reader->current) * 8 - 8) >= n; }

inline int is_byte_aligned(RBSPReader* reader) { return reader->bits_left == 8; }

inline int is_end_valid(RBSPReader* reader) { return reader->current <= reader->end; }

inline void read_byte_aligned_pointer(RBSPReader* reader, uint8_t** ptr, int* length) {
    uint8_t* aligned_ptr = is_byte_aligned(reader) ? reader->current : (reader->current + 1);
    *ptr = aligned_ptr;
    *length = reader->end - aligned_ptr;

    reader->current = reader->end;
    reader->bits_left = 8;
}

inline void skip_n_bits(RBSPReader* reader, int n) {
    int i = 0;
    for (i = 0; i < n; ++i) {
        if (0 == (--reader->bits_left)) {
            reader->current++;
            reader->bits_left = 8;
        }
    }
}

inline uint32_t read_f(RBSPReader* reader, int n) { return read_u(reader, n); }

inline uint32_t read_u(RBSPReader* reader, int n) {
    int i = 0;
    uint32_t result = 0;

    for (i = 0; i < n; ++i) {
        result |= (read_u_1(reader) << (n - i - 1));
    }

    return result;
}

inline uint32_t read_u8(RBSPReader* reader) {
    uint32_t result = 0;

    if (reader->bits_left == 8 && reader->current < reader->end) {
        result = reader->current[0];
        reader->current++;
        return result;
    }

    return read_u(reader, 8);
}

inline uint32_t read_u16(RBSPReader* reader) {
    uint32_t result = 0;

    if (reader->bits_left == 8 && reader->current + 1 < reader->end) {
        result = reader->current[0];
        result = (result << 8) | reader->current[1];
        reader->current += 2;
        return result;
    }

    return read_u(reader, 16);
}

inline uint32_t read_u32(RBSPReader* reader) {
    uint32_t result = 0;

    if (reader->bits_left == 8 && reader->current + 3 < reader->end) {
        result = reader->current[0];
        result = (result << 8) | reader->current[1];
        result = (result << 8) | reader->current[2];
        result = (result << 8) | reader->current[3];

        reader->current += 4;
        return result;
    }

    return read_u(reader, 32);
}

inline uint32_t read_ue(RBSPReader* reader) {
    int leading_zero_bits = 0;
    uint32_t result = 0;

    while (!read_u_1(reader) && (leading_zero_bits < 32) && (reader->current < reader->end)) {
        leading_zero_bits++;
    }

    result = read_u(reader, leading_zero_bits);
    result += (1 << leading_zero_bits) - 1;

    return result;
}

inline int32_t read_se(RBSPReader* reader) {
    int32_t result = 0;

    result = (int32_t)read_ue(reader);

    if (result & 0x01) {
        result = (result + 1) / 2;
    } else {
        result = -(result / 2);
    }

    return result;
}

int32_t read_me(RBSPReader* reader, int32_t ChromaArrayType, H264_MB_PART_PRED_MODE pred_mode) {
    int32_t coded_block_pattern = -1;

    uint32_t codeNum = read_ue(reader);
    if(ChromaArrayType == 1 || ChromaArrayType == 2){
        if(codeNum <= 47){
            if(pred_mode == Intra_4x4 || pred_mode == Intra_8x8){
                coded_block_pattern = g_coded_block_pattern_ChromaArrayType_1_2[codeNum][0];
            }else if(pred_mode == Inter){
                coded_block_pattern = g_coded_block_pattern_ChromaArrayType_1_2[codeNum][1];
            }
        }
    }else if(ChromaArrayType == 0 || ChromaArrayType == 3){
        if(codeNum <= 15){
            if(pred_mode == Intra_4x4 || pred_mode == Intra_8x8){
                coded_block_pattern = g_coded_block_pattern_ChromaArrayType_0_3[codeNum][0];
            }else if(pred_mode == Inter){
                coded_block_pattern = g_coded_block_pattern_ChromaArrayType_0_3[codeNum][1];
            }
        }
    }

    return coded_block_pattern;
}

uint32_t read_te(RBSPReader* reader, int32_t range) {
    if (range > 1) {
        return read_ue(reader);
    } else if (range == 1) {
        return !read_u(reader, 1);
    } else {
        return 0;
    }
}

inline uint8_t peek_u1(RBSPReader* reader) {
    uint8_t result = 0;

    if (reader->current < reader->end) {
        result = (*(reader->current) >> (reader->bits_left - 1)) & 0x01;
    }

    return result;
}