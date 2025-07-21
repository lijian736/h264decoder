#include "h264decoder/h264_coded_block_pattern.h"

/* ChromaArrayType is equal to 1 or 2*/
/* the first index is codeNum, the second index is Intra_4x4, Intra_8x8 or Inter*/
static const uint8_t cbp_map_1_2[48][2] = {
    {47, 0},  {31, 16}, {15, 1},  {0, 2},   {23, 4},  {27, 8},  {29, 32}, {30, 3},  {7, 5},   {11, 10}, {13, 12}, {14, 15}, {39, 47}, {43, 7},  {45, 11}, {46, 13},
    {16, 14}, {3, 6},   {5, 9},   {10, 31}, {12, 35}, {19, 37}, {21, 42}, {26, 44}, {28, 33}, {35, 34}, {37, 36}, {42, 40}, {44, 39}, {1, 43},  {2, 45},  {4, 46},
    {8, 17},  {17, 18}, {18, 20}, {20, 24}, {24, 19}, {6, 21},  {9, 26},  {22, 28}, {25, 23}, {32, 27}, {33, 29}, {34, 30}, {36, 22}, {40, 25}, {38, 38}, {41, 41},
};

/* ChromaArrayType is equal to 0 or 3*/
/* the first index is codeNum, the second index is Intra_4x4, Intra_8x8 or Inter*/
static const uint8_t cbp_map_0_3[16][2] = {
    {15, 0}, {0, 1}, {7, 2}, {11, 4}, {13, 8}, {14, 3}, {3, 5}, {5, 10}, {10, 12}, {12, 15}, {1, 7}, {2, 11}, {4, 13}, {8, 14}, {6, 6}, {9, 9},
};

int coded_block_pattern(uint32_t ChromaArrayType, uint32_t code_num, H264_MB_PART_PRED_MODE mode, uint8_t* out) {
    if ((ChromaArrayType == 1 || ChromaArrayType == 2) && code_num < 48) {
        if (mode == Intra_4x4 || mode == Intra_8x8) {
            *out = cbp_map_1_2[code_num][0];
            return ERR_OK;
        } else if (mode == Inter) {
            *out = cbp_map_1_2[code_num][1];
            return ERR_OK;
        }
    } else if ((ChromaArrayType == 0 || ChromaArrayType == 3) && code_num < 16) {
        if (mode == Intra_4x4 || mode == Intra_8x8) {
            *out = cbp_map_0_3[code_num][0];
            return ERR_OK;
        } else if (mode == Inter) {
            *out = cbp_map_0_3[code_num][1];
            return ERR_OK;
        }
    }

    return ERR_INVALID_PARAM;
}