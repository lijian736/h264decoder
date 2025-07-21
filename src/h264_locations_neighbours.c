#include "h264decoder/h264_locations_neighbours.h"

#include "h264decoder/h264_macroblock.h"

/* 6.4.1 Inverse macroblock scanning process */
void inverse_macroblock_scanning_process(int32_t mbAddr, int32_t PicWidthInSamplesL, int32_t MbaffFrameFlag, int32_t is_frame_macroblock, int32_t* out_x, int32_t* out_y) {
    if (!MbaffFrameFlag) {
        *out_x = InverseRasterScan(mbAddr, 16, 16, PicWidthInSamplesL, 0);
        *out_y = InverseRasterScan(mbAddr, 16, 16, PicWidthInSamplesL, 1);
    } else {
        int32_t xO = InverseRasterScan(mbAddr / 2, 16, 32, PicWidthInSamplesL, 0);
        int32_t yO = InverseRasterScan(mbAddr / 2, 16, 32, PicWidthInSamplesL, 1);

        if (is_frame_macroblock) {
            *out_x = xO;
            *out_y = yO + (mbAddr % 2) * 16;
        } else {
            *out_x = xO;
            *out_y = yO + (mbAddr % 2);
        }
    }
}

/* 6.4.2.1 Inverse macroblock partition scanning process */
/* Table 7-13 – Macroblock type values 0 to 4 for P and SP slices */
/* Table 7-14 – Macroblock type values 0 to 22 for B slices */
int inverse_macroblock_partition_scanning_process(int32_t mbPartIdx, MB_TYPE_NAME mb_type, int32_t* out_x, int32_t* out_y) {
    int err_code = ERR_OK;
    int32_t width = 0;
    int32_t height = 0;

    err_code = MbPartWidthHeight(mb_type, &width, &height);
    if (err_code < 0) {
        return err_code;
    }

    *out_x = InverseRasterScan(mbPartIdx, width, height, 16, 0);
    *out_y = InverseRasterScan(mbPartIdx, width, height, 16, 1);

    return ERR_OK;
}

/* 6.4.2.2 Inverse sub-macroblock partition scanning process */
int inverse_sub_macroblock_partition_scanning_process(MB_TYPE_NAME mb_type, MB_TYPE_NAME sub_mb_type_of_mbPartIdx, int32_t subMbPartIdx, int32_t* out_x, int32_t* out_y) {
    if (mb_type == P_8x8 || mb_type == P_8x8ref0 || mb_type == B_8x8) {
        int32_t sub_mb_part_width = 0;
        int32_t sub_mb_part_height = 0;
        int err_code = ERR_OK;

        err_code = SubMbPartWidthHeight(sub_mb_type_of_mbPartIdx, &sub_mb_part_width, &sub_mb_part_height);
        if (err_code < 0) {
            return err_code;
        }

        *out_x = InverseRasterScan(subMbPartIdx, sub_mb_part_width, sub_mb_part_height, 8, 0);
        *out_y = InverseRasterScan(subMbPartIdx, sub_mb_part_width, sub_mb_part_height, 8, 1);
    } else {
        *out_x = InverseRasterScan(subMbPartIdx, 4, 4, 8, 0);
        *out_y = InverseRasterScan(subMbPartIdx, 4, 4, 8, 1);
    }

    return ERR_OK;
}

/* 6.4.3 Inverse 4x4 luma block scanning process */
void inverse_4x4_luma_block_scanning_process(int32_t luma4x4BlkIdx, int32_t* out_x, int32_t* out_y) {
    *out_x = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 0);
    *out_y = InverseRasterScan(luma4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(luma4x4BlkIdx % 4, 4, 4, 8, 1);
}

/* 6.4.4 Inverse 4x4 Cb or Cr block scanning process for ChromaArrayType equal to 3 */
void inverse_4x4_Cb_Cr_block_scanning_process(int32_t cbcr4x4BlkIdx, int32_t* out_x, int32_t* out_y) {
    *out_x = InverseRasterScan(cbcr4x4BlkIdx / 4, 8, 8, 16, 0) + InverseRasterScan(cbcr4x4BlkIdx % 4, 4, 4, 8, 0);
    *out_y = InverseRasterScan(cbcr4x4BlkIdx / 4, 8, 8, 16, 1) + InverseRasterScan(cbcr4x4BlkIdx % 4, 4, 4, 8, 1);
}

/* 6.4.5 Inverse 8x8 luma block scanning process */
void inverse_8x8_luma_block_scanning_process(int32_t luma8x8BlkIdx, int32_t* out_x, int32_t* out_y) {
    *out_x = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 0);
    *out_y = InverseRasterScan(luma8x8BlkIdx, 8, 8, 16, 1);
}

/* 6.4.6 Inverse 8x8 Cb or Cr block scanning process for ChromaArrayType equal to 3 */
void inverse_8x8_Cb_Cr_block_scanning_process(int32_t cbcr8x8BlkIdx, int32_t* out_x, int32_t* out_y) {
    *out_x = InverseRasterScan(cbcr8x8BlkIdx, 8, 8, 16, 0);
    *out_y = InverseRasterScan(cbcr8x8BlkIdx, 8, 8, 16, 1);
}

/* 6.4.7 Inverse 4x4 chroma block scanning process */
void inverse_4x4_chroma_block_scanning_process(int32_t chroma4x4BlkIdx, int32_t* out_x, int32_t* out_y) {
    *out_x = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 0);
    *out_y = InverseRasterScan(chroma4x4BlkIdx, 4, 4, 8, 1);
}

/* 6.4.8 Derivation process of the availability for macroblock addresses */
int is_available_of_macroblock(int32_t mbAddr, int32_t mbAddr_slice_id, int32_t CurrMbAddr, int32_t CurrMbAddr_slice_id) {
    if (mbAddr < 0 || mbAddr > CurrMbAddr || mbAddr_slice_id != CurrMbAddr_slice_id) {
        return 0;
    }

    return 1;
}

/* 6.4.9 Derivation process for neighbouring macroblock addresses and their availability */
void neighbouring_mb_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA, int32_t* out_mbAddrB, int32_t* out_mbAddrC,
                                          int32_t* out_mbAddrD) {
    *out_mbAddrA = CurrMbAddr - 1;
    *out_mbAddrB = CurrMbAddr - PicWidthInMbs;
    *out_mbAddrC = CurrMbAddr - PicWidthInMbs + 1;
    *out_mbAddrD = CurrMbAddr - PicWidthInMbs - 1;

    if (*out_mbAddrA < 0 /* || *out_mbAddrA > CurrMbAddr (Note: redundant)*/ || CurrMbAddr % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrA]) {
        *out_mbAddrA = -1;
    }

    if (*out_mbAddrB < 0 /*|| *out_mbAddrB > CurrMbAddr (Note: redundant)*/ || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrB]) {
        *out_mbAddrB = -1;
    }

    if (*out_mbAddrC < 0 || *out_mbAddrC > CurrMbAddr || (CurrMbAddr + 1) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrC]) {
        *out_mbAddrC = -1;
    }

    if (*out_mbAddrD < 0 /* || *out_mbAddrD > CurrMbAddr (Note: redundant)*/ || CurrMbAddr % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrD]) {
        *out_mbAddrD = -1;
    }
}

/* 6.4.9 Derivation process for neighbouring macroblock addresses and their availability */
void neighbouring_mb_A_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA) {
    *out_mbAddrA = CurrMbAddr - 1;

    if (*out_mbAddrA < 0 /* || *out_mbAddrA > CurrMbAddr (Note: redundant)*/ || CurrMbAddr % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrA]) {
        *out_mbAddrA = -1;
    }
}

/* 6.4.9 Derivation process for neighbouring macroblock addresses and their availability */
void neighbouring_mb_B_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrB) {
    *out_mbAddrB = CurrMbAddr - PicWidthInMbs;

    if (*out_mbAddrB < 0 /*|| *out_mbAddrB > CurrMbAddr (Note: redundant)*/ || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrB]) {
        *out_mbAddrB = -1;
    }
}

/* 6.4.9 Derivation process for neighbouring macroblock addresses and their availability */
void neighbouring_mb_C_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrC) {
    *out_mbAddrC = CurrMbAddr - PicWidthInMbs + 1;

    if (*out_mbAddrC < 0 || *out_mbAddrC > CurrMbAddr || (CurrMbAddr + 1) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrC]) {
        *out_mbAddrC = -1;
    }
}

/* 6.4.9 Derivation process for neighbouring macroblock addresses and their availability */
void neighbouring_mb_D_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrD) {
    *out_mbAddrD = CurrMbAddr - PicWidthInMbs - 1;

    if (*out_mbAddrD < 0 /* || *out_mbAddrD > CurrMbAddr (Note: redundant)*/ || CurrMbAddr % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrD]) {
        *out_mbAddrD = -1;
    }
}

/* 6.4.10 Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames */
void neighbouring_mb_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA, int32_t* out_mbAddrB,
                                                         int32_t* out_mbAddrC, int32_t* out_mbAddrD) {
    *out_mbAddrA = 2 * (CurrMbAddr / 2 - 1);
    *out_mbAddrB = 2 * (CurrMbAddr / 2 - PicWidthInMbs);
    *out_mbAddrC = 2 * (CurrMbAddr / 2 - PicWidthInMbs + 1);
    *out_mbAddrD = 2 * (CurrMbAddr / 2 - PicWidthInMbs - 1);

    if (*out_mbAddrA < 0 /*|| *out_mbAddrA > CurrMbAddr (Note: redundant) */ || (CurrMbAddr / 2) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrA]) {
        *out_mbAddrA = -2;
    }

    if (*out_mbAddrB < 0 /* || *out_mbAddrB > CurrMbAddr (Note: redundant) */ || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrB]) {
        *out_mbAddrB = -2;
    }

    if (*out_mbAddrC < 0 || *out_mbAddrC > CurrMbAddr || (CurrMbAddr / 2 + 1) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrC]) {
        *out_mbAddrC = -2;
    }

    if (*out_mbAddrD < 0 /* || *out_mbAddrD > CurrMbAddr (Note: redundant)*/ || (CurrMbAddr / 2) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrD]) {
        *out_mbAddrD = -2;
    }
}

/* 6.4.10 Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames */
void neighbouring_mb_A_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA) {
    *out_mbAddrA = 2 * (CurrMbAddr / 2 - 1);
    if (*out_mbAddrA < 0 /*|| *out_mbAddrA > CurrMbAddr (Note: redundant) */ || (CurrMbAddr / 2) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrA]) {
        *out_mbAddrA = -1;
    }
}

/* 6.4.10 Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames */
void neighbouring_mb_B_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrB) {
    *out_mbAddrB = 2 * (CurrMbAddr / 2 - PicWidthInMbs);
    if (*out_mbAddrB < 0 /* || *out_mbAddrB > CurrMbAddr (Note: redundant) */ || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrB]) {
        *out_mbAddrB = -1;
    }
}

/* 6.4.10 Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames */
void neighbouring_mb_C_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrC) {
    *out_mbAddrC = 2 * (CurrMbAddr / 2 - PicWidthInMbs + 1);
    if (*out_mbAddrC < 0 || *out_mbAddrC > CurrMbAddr || (CurrMbAddr / 2 + 1) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrC]) {
        *out_mbAddrC = -1;
    }
}

/* 6.4.10 Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames */
void neighbouring_mb_D_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrD) {
    *out_mbAddrD = 2 * (CurrMbAddr / 2 - PicWidthInMbs - 1);

    if (*out_mbAddrD < 0 /* || *out_mbAddrD > CurrMbAddr (Note: redundant)*/ || (CurrMbAddr / 2) % PicWidthInMbs == 0 || mb_slice_ids[CurrMbAddr] != mb_slice_ids[*out_mbAddrD]) {
        *out_mbAddrD = -1;
    }
}

/* 6.4.11.1 Derivation process for neighbouring macroblocks*/
void neighbouring_macroblocks(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                              int32_t is_chroma, int32_t MbWidthC, int32_t MbHeightC, int32_t* out_mbAddrA, int32_t* out_mbAddrB) {
    int32_t out_xW = 0;
    int32_t out_yW = 0;

    /* mbAddrA */
    RELATIVE_LOCATION_TYPE out_mbAddrA_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    int32_t xA = -1;
    int32_t yA = 0;

    /* mbAddrB */
    RELATIVE_LOCATION_TYPE out_mbAddrB_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    int32_t xB = 0;
    int32_t yB = -1;

    int32_t maxW = 0;
    int32_t maxH = 0;

    if (is_chroma) {
        maxW = MbWidthC;
        maxH = MbHeightC;

    } else {
        maxW = 16;
        maxH = 16;
    }

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xA, yA, maxW, maxH, &out_mbAddrA_type, out_mbAddrA, &out_xW,
                           &out_yW);

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xB, yB, maxW, maxH, &out_mbAddrB_type, out_mbAddrB, &out_xW,
                           &out_yW);
}

/* 6.4.11.2 Derivation process for neighbouring 8x8 luma block */
void neighbouring_8x8_luma_block(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                                 int32_t luma8x8BlkIdx, int32_t* out_mbAddrA, int32_t* out_luma8x8BlkIdxA, int32_t* out_mbAddrB, int32_t* out_luma8x8BlkIdxB) {
    int32_t xW = 0;
    int32_t yW = 0;

    int32_t xA = (luma8x8BlkIdx % 2) * 8 - 1;
    int32_t yA = (luma8x8BlkIdx / 2) * 8;
    RELATIVE_LOCATION_TYPE mbAddrA_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xA, yA, 16, 16, &mbAddrA_type, out_mbAddrA, &xW, &yW);
    if (*out_mbAddrA < 0) {
        *out_luma8x8BlkIdxA = -1;
    } else {
        derivate_8x8_luma_block_indices(xW, yW, out_luma8x8BlkIdxA);
    }

    int32_t xB = (luma8x8BlkIdx % 2) * 8;
    int32_t yB = (luma8x8BlkIdx / 2) * 8 - 1;
    RELATIVE_LOCATION_TYPE mbAddrB_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xB, yB, 16, 16, &mbAddrB_type, out_mbAddrB, &xW, &yW);
    if (*out_mbAddrB < 0) {
        *out_luma8x8BlkIdxB = -1;
    } else {
        derivate_8x8_luma_block_indices(xW, yW, out_luma8x8BlkIdxB);
    }
}

/* 6.4.11.3 Derivation process for neighbouring 8x8 chroma blocks for ChromaArrayType equal to 3 */
void neighbouring_8x8_chroma_block(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t MbWidthC, int32_t MbHeightC,
                                   int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t cbcr8x8BlkIdx, int32_t* out_mbAddrA, int32_t* out_cbcr8x8BlkIdxA, int32_t* out_mbAddrB,
                                   int32_t* out_cbcr8x8BlkIdxB) {
    int32_t xW = 0;
    int32_t yW = 0;

    int32_t xA = (cbcr8x8BlkIdx % 2) * 8 - 1;
    int32_t yA = (cbcr8x8BlkIdx / 2) * 8;
    RELATIVE_LOCATION_TYPE mbAddrA_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xA, yA, MbWidthC, MbHeightC, &mbAddrA_type, out_mbAddrA, &xW,
                           &yW);
    if (*out_mbAddrA < 0) {
        *out_cbcr8x8BlkIdxA = -1;
    } else {
        derivate_8x8_luma_block_indices(xW, yW, out_cbcr8x8BlkIdxA);
    }

    int32_t xB = (cbcr8x8BlkIdx % 2) * 8;
    int32_t yB = (cbcr8x8BlkIdx / 2) * 8 - 1;
    RELATIVE_LOCATION_TYPE mbAddrB_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xB, yB, MbWidthC, MbHeightC, &mbAddrB_type, out_mbAddrB, &xW,
                           &yW);
    if (*out_mbAddrB < 0) {
        *out_cbcr8x8BlkIdxB = -1;
    } else {
        derivate_8x8_luma_block_indices(xW, yW, out_cbcr8x8BlkIdxB);
    }
}

/* 6.4.11.4 Derivation process for neighbouring 4x4 luma blocks */
void neighbouring_4x4_luma_block(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                                 int32_t luma4x4BlkIdx, int32_t* out_mbAddrA, int32_t* out_luma4x4BlkIdxA, int32_t* out_mbAddrB, int32_t* out_luma4x4BlkIdxB) {
    int32_t xW = 0;
    int32_t yW = 0;

    int32_t x = 0;
    int32_t y = 0;

    /* 6.4.3 Inverse 4x4 luma block scanning process */
    inverse_4x4_luma_block_scanning_process(luma4x4BlkIdx, &x, &y);

    /* A */
    int32_t xA = x - 1;
    int32_t yA = y;
    RELATIVE_LOCATION_TYPE mbAddrA_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xA, yA, 16, 16, &mbAddrA_type, out_mbAddrA, &xW, &yW);
    if (*out_mbAddrA < 0) {
        *out_luma4x4BlkIdxA = -1;
    } else {
        derivate_4x4_luma_block_indices(xW, yW, out_luma4x4BlkIdxA);
    }

    /* B */
    int32_t xB = x;
    int32_t yB = y - 1;
    RELATIVE_LOCATION_TYPE mbAddrB_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xB, yB, 16, 16, &mbAddrB_type, out_mbAddrB, &xW, &yW);
    if (*out_mbAddrB < 0) {
        *out_luma4x4BlkIdxB = -1;
    } else {
        derivate_4x4_luma_block_indices(xW, yW, out_luma4x4BlkIdxB);
    }
}

/* 6.4.11.5 Derivation process for neighbouring 4x4 chroma blocks */
void neighbouring_4x4_chroma_block_ChromaArrayType_12(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t MbWidthC,
                                                      int32_t MbHeightC, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t chroma4x4BlkIdx, int32_t* out_mbAddrA,
                                                      int32_t* out_chroma4x4BlkIdxA, int32_t* out_mbAddrB, int32_t* out_chroma4x4BlkIdxB) {
    int32_t xW = 0;
    int32_t yW = 0;

    int32_t x = 0;
    int32_t y = 0;

    /* 6.4.7 Inverse 4x4 chroma block scanning process */
    inverse_4x4_chroma_block_scanning_process(chroma4x4BlkIdx, &x, &y);

    /* A */
    int32_t xA = x - 1;
    int32_t yA = y;
    RELATIVE_LOCATION_TYPE mbAddrA_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xA, yA, MbWidthC, MbHeightC, &mbAddrA_type, out_mbAddrA, &xW,
                           &yW);
    if (*out_mbAddrA < 0) {
        *out_chroma4x4BlkIdxA = -1;
    } else {
        derivate_4x4_chroma_block_indices(xW, yW, out_chroma4x4BlkIdxA);
    }

    /* B */
    int32_t xB = x;
    int32_t yB = y - 1;
    RELATIVE_LOCATION_TYPE mbAddrB_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xB, yB, MbWidthC, MbHeightC, &mbAddrB_type, out_mbAddrB, &xW,
                           &yW);
    if (*out_mbAddrB < 0) {
        *out_chroma4x4BlkIdxB = -1;
    } else {
        derivate_4x4_chroma_block_indices(xW, yW, out_chroma4x4BlkIdxB);
    }
}

/* 6.4.11.6 Derivation process for neighbouring 4x4 chroma blocks for ChromaArrayType equal to 3 */
void neighbouring_4x4_chroma_block_ChromaArrayType_3(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t MbWidthC,
                                                     int32_t MbHeightC, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t chroma4x4BlkIdx, int32_t* out_mbAddrA,
                                                     int32_t* out_chroma4x4BlkIdxA, int32_t* out_mbAddrB, int32_t* out_chroma4x4BlkIdxB) {
    int32_t xW = 0;
    int32_t yW = 0;

    int32_t x = 0;
    int32_t y = 0;

    /* 6.4.3 Inverse 4x4 luma block scanning process */
    inverse_4x4_luma_block_scanning_process(chroma4x4BlkIdx, &x, &y);

    /* A */
    int32_t xA = x - 1;
    int32_t yA = y;
    RELATIVE_LOCATION_TYPE mbAddrA_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xA, yA, MbWidthC, MbHeightC, &mbAddrA_type, out_mbAddrA, &xW,
                           &yW);
    if (*out_mbAddrA < 0) {
        *out_chroma4x4BlkIdxA = -1;
    } else {
        derivate_4x4_luma_block_indices(xW, yW, out_chroma4x4BlkIdxA);
    }

    /* B */
    int32_t xB = x;
    int32_t yB = y - 1;
    RELATIVE_LOCATION_TYPE mbAddrB_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xB, yB, MbWidthC, MbHeightC, &mbAddrB_type, out_mbAddrB, &xW,
                           &yW);
    if (*out_mbAddrB < 0) {
        *out_chroma4x4BlkIdxB = -1;
    } else {
        derivate_4x4_luma_block_indices(xW, yW, out_chroma4x4BlkIdxB);
    }
}

/* 6.4.11.7 Derivation process for neighbouring partitions */
int neighbouring_partitions(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                            int32_t mbPartIdx, MB_TYPE_NAME currSubMbType, int32_t subMbPartIdx, MB_TYPE_NAME subMbPartType, int32_t* out_mbAddrA, int32_t* out_mbPartIdxA,
                            int32_t* out_subMbPartIdxA, int32_t* out_mbAddrB, int32_t* out_mbPartIdxB, int32_t* out_subMbPartIdxB, int32_t* out_mbAddrC, int32_t* out_mbPartIdxC,
                            int32_t* out_subMbPartIdxC, int32_t* out_mbAddrD, int32_t* out_mbPartIdxD, int32_t* out_subMbPartIdxD) {
    int err_code = ERR_OK;

    int32_t x = 0;
    int32_t y = 0;

    /* 6.4.2.1 Inverse macroblock partition scanning process */
    err_code = inverse_macroblock_partition_scanning_process(mbPartIdx, currSubMbType, &x, &y);
    if (err_code < 0) {
        return err_code;
    }

    int32_t xS = 0;
    int32_t yS = 0;

    if (currSubMbType == P_8x8 || currSubMbType == P_8x8ref0 || currSubMbType == B_8x8) {
        /* 6.4.2.2 Inverse sub-macroblock partition scanning process */
        err_code = inverse_sub_macroblock_partition_scanning_process(currSubMbType, subMbPartType, subMbPartIdx, &xS, &yS);
        if (err_code < 0) {
            return err_code;
        }
    }

    int32_t predPartWidth = 0;
    /* If mb_type is equal to P_Skip, B_Skip, or B_Direct_16x16, predPartWidth = 16. */
    if (currSubMbType == P_Skip || currSubMbType == B_Skip || currSubMbType == B_Direct_16x16) {
        predPartWidth = 16;
    } else if (currSubMbType == B_8x8) { /* Otherwise, if mb_type is equal to B_8x8, the following applies: */

    } else if (currSubMbType == P_8x8 || currSubMbType == P_8x8ref0) { /* Otherwise, if mb_type is equal to P_8x8 or P_8x8ref0, */
        err_code = SubMbPartWidth(subMbPartType, &predPartWidth);
        if (err_code) {
            return err_code;
        }
    } else { /* Otherwise, predPartWidth = MbPartWidth( mb_type ). */
        predPartWidth = MbPartWidth(currSubMbType, &predPartWidth);
    }

    int32_t xD_A = -1;
    int32_t yD_A = 0;
    int32_t xD_B = 0;
    int32_t yD_B = -1;
    int32_t xD_C = predPartWidth;
    int32_t yD_C = -1;
    int32_t xD_D = -1;
    int32_t yD_D = -1;

    int32_t xN_A = x + xS + xD_A;
    int32_t yN_A = y + yS + yD_A;
    int32_t xN_B = x + xS + xD_B;
    int32_t yN_B = y + yS + yD_B;
    int32_t xN_C = x + xS + xD_C;
    int32_t yN_C = y + yS + yD_C;
    int32_t xN_D = x + xS + xD_D;
    int32_t yN_D = y + yS + yD_D;

    RELATIVE_LOCATION_TYPE mbAddrA_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    RELATIVE_LOCATION_TYPE mbAddrB_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    RELATIVE_LOCATION_TYPE mbAddrC_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    RELATIVE_LOCATION_TYPE mbAddrD_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    int32_t xW_A = 0;
    int32_t yW_A = 0;
    int32_t xW_B = 0;
    int32_t yW_B = 0;
    int32_t xW_C = 0;
    int32_t yW_C = 0;
    int32_t xW_D = 0;
    int32_t yW_D = 0;

    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xN_A, yN_A, 16, 16, &mbAddrA_type, out_mbAddrA, &xW_A, &yW_A);
    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xN_B, yN_B, 16, 16, &mbAddrB_type, out_mbAddrB, &xW_B, &yW_B);
    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xN_C, yN_C, 16, 16, &mbAddrC_type, out_mbAddrC, &xW_C, &yW_C);
    /* 6.4.12 Derivation process for neighbouring locations */
    neighbouring_locations(MbaffFrameFlag, CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xN_D, yN_D, 16, 16, &mbAddrD_type, out_mbAddrD, &xW_D, &yW_D);

    if (*out_mbAddrA < 0) {
        *out_mbPartIdxA = -1;
        *out_subMbPartIdxA = -1;
    }

    if (*out_mbAddrB < 0) {
        *out_mbPartIdxB = -1;
        *out_subMbPartIdxB = -1;
    }

    if (*out_mbAddrC < 0) {
        *out_mbPartIdxC = -1;
        *out_subMbPartIdxC = -1;
    }

    if (*out_mbAddrD < 0) {
        *out_mbPartIdxD = -1;
        *out_subMbPartIdxD = -1;
    }

    return ERR_NOT_IMPL;
}

/* 6.4.12 Derivation process for neighbouring locations */
void neighbouring_locations(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t xN,
                            int32_t yN, int32_t maxW, int32_t maxH, RELATIVE_LOCATION_TYPE* out_mbAddrN_type, int32_t* out_mbAddrN, int32_t* out_xW, int32_t* out_yW) {
    if (!MbaffFrameFlag) { /* 6.4.12.1 Specification for neighbouring locations in fields and non-MBAFF frames */
        neighbouring_locations_in_frame_non_MBAFF_frame(CurrMbAddr, PicWidthInMbs, mb_slice_ids, xN, yN, maxW, maxH, out_mbAddrN_type, out_mbAddrN, out_xW, out_yW);
    } else { /* 6.4.12.2 Specification for neighbouring locations in MBAFF frames */
        neighbouring_locations_in_MBAFF_frame(CurrMbAddr, currMbFrameFlag, PicWidthInMbs, mb_slice_ids, mb_frame_flags, xN, yN, maxW, maxH, out_mbAddrN_type, out_mbAddrN, out_xW,
                                              out_yW);
    }
}

/* 6.4.12.1 Specification for neighbouring locations in fields and non-MBAFF frames */
void neighbouring_locations_in_frame_non_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t xN, int32_t yN, int32_t maxW, int32_t maxH,
                                                     RELATIVE_LOCATION_TYPE* out_mbAddrN_type, int32_t* out_mbAddrN, int32_t* out_xW, int32_t* out_yW) {
    *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    *out_mbAddrN = -1;

    /* 6.4.9 Derivation process for neighbouring macroblock addresses and their availability */
    /* Table 6-3 – Specification of mbAddrN */
    if (xN < 0 && yN < 0) { /* mbAddrD */
        neighbouring_mb_D_address_availability(CurrMbAddr, PicWidthInMbs, mb_slice_ids, out_mbAddrN);

        if (*out_mbAddrN < 0) {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
        } else {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrD;
        }
    } else if (xN < 0 && (yN >= 0 && yN <= maxH - 1)) { /* mAddrA */
        neighbouring_mb_A_address_availability(CurrMbAddr, PicWidthInMbs, mb_slice_ids, out_mbAddrN);

        if (*out_mbAddrN < 0) {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
        } else {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
        }

    } else if ((xN >= 0 && xN <= maxW - 1) && yN < 0) { /* mbAddrB */
        neighbouring_mb_B_address_availability(CurrMbAddr, PicWidthInMbs, mb_slice_ids, out_mbAddrN);

        if (*out_mbAddrN < 0) {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
        } else {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrB;
        }
    } else if ((xN >= 0 && xN <= maxW - 1) && (yN >= 0 && yN <= maxH - 1)) { /* CurrMbAddr */
        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_CurrMbAddr;
        *out_mbAddrN = CurrMbAddr;
    } else if (xN > maxW - 1 && yN < 0) { /* mbAddrC */
        neighbouring_mb_C_address_availability(CurrMbAddr, PicWidthInMbs, mb_slice_ids, out_mbAddrN);

        if (*out_mbAddrN < 0) {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
        } else {
            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrC;
        }
    } else {
        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
        *out_mbAddrN = -1;
    }

    if (*out_mbAddrN_type != RELATIVE_LOCATION_TYPE_NOT_AVAILABLE) {
        *out_xW = (xN + maxW) % maxW;
        *out_yW = (yN + maxH) % maxH;
    }
}

/* 6.4.12.2 Specification for neighbouring locations in MBAFF frames */
void neighbouring_locations_in_MBAFF_frame(int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t xN,
                                           int32_t yN, int32_t maxW, int32_t maxH, RELATIVE_LOCATION_TYPE* out_mbAddrN_type, int32_t* out_mbAddrN, int32_t* out_xW,
                                           int32_t* out_yW) {
    int32_t mbIsTopMbFlag = 0;
    int32_t mbAddrXFrameFlag = 0;
    int32_t yM = 0;
    int32_t mbAddrX = -1;
    RELATIVE_LOCATION_TYPE mbAddrX_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;

    *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    *out_mbAddrN = -1;

    /* 6.4.10 Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames */
    int32_t mbAddrA = 0;
    int32_t mbAddrB = 0;
    int32_t mbAddrC = 0;
    int32_t mbAddrD = 0;

    neighbouring_mb_address_availability_in_MBAFF_frame(CurrMbAddr, PicWidthInMbs, mb_slice_ids, &mbAddrA, mbAddrB, mbAddrC, mbAddrD);

    /* check if the current macroblock is a top macroblock */
    if (CurrMbAddr % 2 == 0) {
        mbIsTopMbFlag = 1;
    } else {
        mbIsTopMbFlag = 0;
    }

    if (xN < 0 && yN < 0) {
        if (currMbFrameFlag) {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrD;
                mbAddrX = mbAddrD;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrD_add_1;
                *out_mbAddrN = mbAddrD + 1;
                yM = yN;
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                mbAddrX = mbAddrA;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                        *out_mbAddrN = mbAddrA;
                        yM = yN;
                    } else {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA_add_1;
                        *out_mbAddrN = mbAddrA + 1;
                        yM = (yN + maxH) >> 1;
                    }
                }
            }
        } else {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrD;
                mbAddrX = mbAddrD;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrD_add_1;
                        *out_mbAddrN = mbAddrD + 1;
                        yM = 2 * yN;
                    } else {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrD;
                        *out_mbAddrN = mbAddrD;
                        yM = yN;
                    }
                }
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrD;
                mbAddrX = mbAddrD;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrD_add_1;
                *out_mbAddrN = mbAddrD + 1;
                yM = yN;
            }
        }
    } else if (xN < 0 && (yN >= 0 && yN <= maxH - 1)) {
        if (currMbFrameFlag) {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                mbAddrX = mbAddrA;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                        *out_mbAddrN = mbAddrA;
                        yM = yN;
                    } else {
                        if (yN % 2 == 0) {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                            *out_mbAddrN = mbAddrA;
                            yM = yN >> 1;
                        } else {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA_add_1;
                            *out_mbAddrN = mbAddrA + 1;
                            yM = yN >> 1;
                        }
                    }
                }
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                mbAddrX = mbAddrA;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA_add_1;
                        *out_mbAddrN = mbAddrA + 1;
                        yM = yN;
                    } else {
                        if (yN % 2 == 0) {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                            *out_mbAddrN = mbAddrA;
                            yM = (yN + maxH) >> 1;
                        } else {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA_add_1;
                            *out_mbAddrN = mbAddrA + 1;
                            yM = (yN + maxH) >> 1;
                        }
                    }
                }
            }
        } else {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                mbAddrX = mbAddrA;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        if (yN < (maxH / 2)) {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                            *out_mbAddrN = mbAddrA;
                            yM = yN << 1;
                        } else {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA_add_1;
                            *out_mbAddrN = mbAddrA + 1;
                            yM = (yN << 1) - maxH;
                        }
                    } else {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                        *out_mbAddrN = mbAddrA;
                        yM = yN;
                    }
                }
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                mbAddrX = mbAddrA;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        if (yN < (maxH / 2)) {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA;
                            *out_mbAddrN = mbAddrA;
                            yM = (yN << 1) + 1;
                        } else {
                            *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA_add_1;
                            *out_mbAddrN = mbAddrA + 1;
                            yM = (yN << 1) + 1 - maxH;
                        }
                    } else {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrA_add_1;
                        *out_mbAddrN = mbAddrA + 1;
                        yM = yN;
                    }
                }
            }
        }
    } else if ((xN >= 0 && xN <= maxW - 1) && yN < 0) {
        if (currMbFrameFlag) {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrB;
                mbAddrX = mbAddrB;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrB_add_1;
                *out_mbAddrN = mbAddrB + 1;
                yM = yN;
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_CurrMbAddr;
                mbAddrX = CurrMbAddr;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_CurrMbAddr_minus_1;
                *out_mbAddrN = CurrMbAddr - 1;
                yM = yN;
            }
        } else {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrB;
                mbAddrX = mbAddrB;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrB_add_1;
                        *out_mbAddrN = mbAddrB + 1;
                        yM = 2 * yN;
                    } else {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrB;
                        *out_mbAddrN = mbAddrB;
                        yM = yN;
                    }
                }
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrB;
                mbAddrX = mbAddrB;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrB_add_1;
                *out_mbAddrN = mbAddrB + 1;
                yM = yN;
            }
        }
    } else if ((xN >= 0 && xN <= maxW - 1) && (yN >= 0 && yN <= maxH - 1)) {
        mbAddrX_type = RELATIVE_LOCATION_TYPE_CurrMbAddr;
        mbAddrX = CurrMbAddr;
        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_CurrMbAddr;
        *out_mbAddrN = CurrMbAddr;
        yM = yN;
    } else if (xN > maxW - 1 && yN < 0) {
        if (currMbFrameFlag) {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrC;
                mbAddrX = mbAddrC;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrC_add_1;
                *out_mbAddrN = mbAddrC + 1;
                yM = yN;
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
                mbAddrX = -2;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
                *out_mbAddrN = -2;
                yM = -1;
            }
        } else {
            if (mbIsTopMbFlag) {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrC;
                mbAddrX = mbAddrC;
                if (mbAddrX >= 0) {
                    mbAddrXFrameFlag = mb_frame_flags[mbAddrX];
                    if (mbAddrXFrameFlag) {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrC_add_1;
                        *out_mbAddrN = mbAddrC + 1;
                        yM = 2 * yN;
                    } else {
                        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrC;
                        *out_mbAddrN = mbAddrC;
                        yM = yN;
                    }
                }
            } else {
                mbAddrX_type = RELATIVE_LOCATION_TYPE_mbAddrC;
                mbAddrX = mbAddrC;
                *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_mbAddrC_add_1;
                *out_mbAddrN = mbAddrC + 1;
                yM = yN;
            }
        }
    } else {
        *out_mbAddrN = -1;
    }

    if (*out_mbAddrN < 0) {
        *out_mbAddrN_type = RELATIVE_LOCATION_TYPE_NOT_AVAILABLE;
    }

    *out_xW = (xN + maxW) % maxW;
    *out_yW = (yM + maxH) % maxH;
}

/* 6.4.13.1 Derivation process for 4x4 luma block indices */
void derivate_4x4_luma_block_indices(int32_t xP, int32_t yP, int32_t* out_luma4x4BlkIdx) { *out_luma4x4BlkIdx = 8 * (yP / 8) + 4 * (xP / 8) + 2 * ((yP % 8) / 4) + ((xP % 8) / 4); }

/* 6.4.13.2 Derivation process for 4x4 chroma block indices */
void derivate_4x4_chroma_block_indices(int32_t xP, int32_t yP, int32_t* out_chroma4x4BlkIdx) { *out_chroma4x4BlkIdx = 2 * (yP / 4) + (xP / 4); }

/* 6.4.13.3 Derivation process for 8x8 luma block indices */
void derivate_8x8_luma_block_indices(int32_t xP, int32_t yP, int32_t* out_luma8x8BlkIdx) { *out_luma8x8BlkIdx = 2 * (yP / 8) + (xP / 8); }

/* 6.4.13.4 Derivation process for macroblock and sub-macroblock partition indices */
int derivate_mb_submb_partition_indices(int32_t xP, int32_t yP, MB_TYPE_NAME mbType, MB_TYPE_NAME* subMbType, int32_t* out_mbPartIdx, int32_t* out_subMbPartIdx) {
    int err_code = ERR_OK;

    if (mbType >= I_NxN && mbType <= I_PCM) {
        *out_mbPartIdx = 0;
    } else {
        int32_t mb_part_width = 0;
        int32_t mb_part_height = 0;

        err_code = MbPartWidthHeight(mbType, &mb_part_width, &mb_part_height);
        if (err_code < 0) {
            return err_code;
        }

        *out_mbPartIdx = (16 / mb_part_width) * (yP / mb_part_height) + (xP / mb_part_width);
    }

    if (mbType == B_Skip || mbType == B_Direct_16x16) {
        *out_subMbPartIdx = 2 * ((yP % 8) / 4) + ((xP % 8) / 4);
    } else if (mbType == P_8x8 || mbType == P_8x8ref0 || mbType == B_8x8) {
        MB_TYPE_NAME sub_mb_type = subMbType[*out_mbPartIdx];

        int32_t sub_mb_part_width = 0;
        int32_t sub_mb_part_height = 0;

        err_code = SubMbPartWidthHeight(sub_mb_type, &sub_mb_part_width, &sub_mb_part_height);
        if (err_code < 0) {
            return err_code;
        }

        *out_subMbPartIdx = (8 / sub_mb_part_width) * ((yP % 8) / sub_mb_part_height) + ((xP % 8) / sub_mb_part_width);
    } else {
        *out_subMbPartIdx = 0;
    }

    return ERR_OK;
}