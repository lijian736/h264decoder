#ifndef _H_H264_LOCATIONS_NEIGHBOURS_H_
#define _H_H264_LOCATIONS_NEIGHBOURS_H_

#include <stdint.h>

#include "h264_defs.h"
#include "h264_error.h"

/**
 * @brief Inverse macroblock scanning process
 * @see 6.4.1 Inverse macroblock scanning process
 *
 * Input to this process is a macroblock address mbAddr.
 *
 * Output of this process is the location ( x, y ) of the upper-left luma sample for the macroblock with address mbAddr relative to the upper-left sample of the picture
 *
 * @param mbAddr the macroblock address
 * @param PicWidthInSamplesL picture width for the luma component
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param is_frame_macroblock if the MbaffFrameFlag is 1, is the current macroblock a frame macroblock ?
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 */
void inverse_macroblock_scanning_process(int32_t mbAddr, int32_t PicWidthInSamplesL, int32_t MbaffFrameFlag, int32_t is_frame_macroblock, int32_t* out_x, int32_t* out_y);

/**
 * @brief Inverse macroblock partition scanning process
 * @see 6.4.2.1 Inverse macroblock partition scanning process
 * @see Table 7-13 – Macroblock type values 0 to 4 for P and SP slices
 * @see Table 7-14 – Macroblock type values 0 to 22 for B slices
 *
 * Input to this process is the index of a macroblock partition mbPartIdx.
 *
 * Output of this process is the location ( x, y ) of the upper-left luma sample for the macroblock partition mbPartIdx relative to the upper-left sample of the macroblock.
 *
 * @param mbPartIdx the macroblock partition index
 * @param mb_type the macroblock partition type
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 * @return int 0 on success, negative value on error
 */
inline int inverse_macroblock_partition_scanning_process(int32_t mbPartIdx, MB_TYPE_NAME mb_type, int32_t* out_x, int32_t* out_y);

/**
 * @brief Inverse sub-macroblock partition scanning process
 * @see 6.4.2.2 Inverse sub-macroblock partition scanning process
 *
 * Inputs to this process are the index of a macroblock partition mbPartIdx and the index of a sub-macroblock partition subMbPartIdx.
 *
 * Output of this process is the location ( x, y ) of the upper-left luma sample for the sub-macroblock partition subMbPartIdx relative to the upper-left sample of the
 * sub-macroblock.
 *
 * Note: There is a slightly difference. the input parameter is sub_mb_type_of_mbPartIdx, not the macroblock partition index mbPartIdx
 *
 * @param mb_type the macroblock type name
 * @param sub_mb_type_of_mbPartIdx the sub-macroblock type name of mbPartIdx
 * @param subMbPartIdx the sub-macroblock partition index
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 * @return int 0 on success, negative value on error
 */
inline int inverse_sub_macroblock_partition_scanning_process(MB_TYPE_NAME mb_type, MB_TYPE_NAME sub_mb_type_of_mbPartIdx, int32_t subMbPartIdx, int32_t* out_x, int32_t* out_y);

/**
 * @brief Inverse 4x4 luma block scanning process
 * @see 6.4.3 Inverse 4x4 luma block scanning process
 *
 * Input to this process is the index of a 4x4 luma block luma4x4BlkIdx.
 *
 * Output of this process is the location ( x, y ) of the upper-left luma sample for the 4x4 luma block with index luma4x4BlkIdx relative to the upper-left luma sample of the
 * macroblock.
 *
 * @param luma4x4BlkIdx the 4x4 luma block index
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 */
inline void inverse_4x4_luma_block_scanning_process(int32_t luma4x4BlkIdx, int32_t* out_x, int32_t* out_y);

/**
 * @brief Inverse 4x4 Cb or Cr block scanning process for ChromaArrayType equal to 3
 * @see 6.4.4 Inverse 4x4 Cb or Cr block scanning process for ChromaArrayType equal to 3
 *
 * This process is only invoked when ChromaArrayType is equal to 3.
 *
 * @param cbcr4x4BlkIdx the 4x4 Cb or Cr block index
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 */
inline void inverse_4x4_Cb_Cr_block_scanning_process(int32_t cbcr4x4BlkIdx, int32_t* out_x, int32_t* out_y);

/**
 * @brief Inverse 8x8 luma block scanning process
 * @see 6.4.5 Inverse 8x8 luma block scanning process
 *
 * Input to this process is the index of an 8x8 luma block luma8x8BlkIdx.
 *
 * Output of this process is the location ( x, y ) of the upper-left luma sample for the 8x8 luma block with index luma8x8BlkIdx relative to the upper-left luma sample of the
 * macroblock.
 *
 * @param luma8x8BlkIdx the 8x8 luma block index
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 */
inline void inverse_8x8_luma_block_scanning_process(int32_t luma8x8BlkIdx, int32_t* out_x, int32_t* out_y);

/**
 * @brief Inverse 8x8 Cb or Cr block scanning process for ChromaArrayType equal to 3
 * @see 6.4.6 Inverse 8x8 Cb or Cr block scanning process for ChromaArrayType equal to 3
 *
 * This process is only invoked when ChromaArrayType is equal to 3.
 *
 * @param cbcr8x8BlkIdx the 8x8 Cb or Cr block index
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 */
inline void inverse_8x8_Cb_Cr_block_scanning_process(int32_t cbcr8x8BlkIdx, int32_t* out_x, int32_t* out_y);

/**
 * @brief Inverse 4x4 chroma block scanning process
 * @see 6.4.7 Inverse 4x4 chroma block scanning process
 *
 * Input to this process is the index of a 4x4 chroma block chroma4x4BlkIdx.
 *
 * Output of this process is the location ( x, y ) of the upper-left chroma sample for a 4x4 chroma block with index chroma4x4BlkIdx relative to the upper-left chroma sample of the
 * macroblock.
 *
 * @param chroma4x4BlkIdx the 4x4 chroma block index
 * @param out_x output parameter. the x location
 * @param out_y output parameter. the y location
 */
inline void inverse_4x4_chroma_block_scanning_process(int32_t chroma4x4BlkIdx, int32_t* out_x, int32_t* out_y);

/**
 * @brief Derivation process of the availability for macroblock addresses
 * @see 6.4.8 Derivation process of the availability for macroblock addresses
 *
 * Input to this process is a macroblock address mbAddr.
 * Output of this process is the availability of the macroblock mbAddr.
 *    NOTE – The meaning of availability is determined when this process is invoked.
 *
 * The macroblock is marked as available, unless any of the following conditions are true, in which case the macroblock is marked as not available:
 *  – mbAddr < 0,
 *  – mbAddr > CurrMbAddr,
 *  – the macroblock with address mbAddr belongs to a different slice than the macroblock with address CurrMbAddr.
 *
 * @param mbAddr the macroblock address
 * @param mbAddr_slice_id the id of the slice which the macroblock with address mbAddr belongs to
 * @param CurrMbAddr the current macroblock address
 * @param CurrMbAddr_slice_id the id of the slice which the macroblock with address CurrMbAddr belongs to
 * @return int 0: false, 1: true
 */
inline int is_available_of_macroblock(int32_t mbAddr, int32_t mbAddr_slice_id, int32_t CurrMbAddr, int32_t CurrMbAddr_slice_id);

/**
 * @brief Derivation process for neighbouring macroblock addresses and their availability
 * @see 6.4.9 Derivation process for neighbouring macroblock addresses and their availability
 *
 * This process can only be invoked when MbaffFrameFlag is equal to 0.
 *
 * The outputs of this process are:
 *   – mbAddrA: the address and availability status of the macroblock to the left of the current macroblock,
 *   – mbAddrB: the address and availability status of the macroblock above the current macroblock,
 *   – mbAddrC: the address and availability status of the macroblock above-right of the current macroblock,
 *   – mbAddrD: the address and availability status of the macroblock above-left of the current macroblock.
 *
 * If the macroblock output address is less than 0, then the macroblock is NOT available
 *
 * @param CurrMbAddr the current macroblock address
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param out_mbAddrA output parameter. the address and availability status of the macroblock to the left of the current macroblock,
 * @param out_mbAddrB output parameter. the address and availability status of the macroblock above the current macroblock,
 * @param out_mbAddrC output parameter. the address and availability status of the macroblock above-right of the current macroblock,
 * @param out_mbAddrD output parameter. the address and availability status of the macroblock above-left of the current macroblock.
 */
inline void neighbouring_mb_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA, int32_t* out_mbAddrB, int32_t* out_mbAddrC,
                                                 int32_t* out_mbAddrD);
inline void neighbouring_mb_A_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA);
inline void neighbouring_mb_B_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrB);
inline void neighbouring_mb_C_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrC);
inline void neighbouring_mb_D_address_availability(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrD);

/**
 * @brief Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames
 * @see 6.4.10 Derivation process for neighbouring macroblock addresses and their availability in MBAFF frames
 *
 * This process can only be invoked when MbaffFrameFlag is equal to 1.
 *
 * If the macroblock output address is less than 0, then the macroblock is NOT available
 *
 * @param CurrMbAddr the current macroblock address
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param out_mbAddrA output parameter. the address and availability status of the top macroblock of the macroblock pair to the left of the current macroblock pair
 * @param out_mbAddrB output parameter. the address and availability status of the top macroblock of the macroblock pair above the current macroblock pair
 * @param out_mbAddrC output parameter. the address and availability status of the top macroblock of the macroblock pair above-right of the current macroblock pair
 * @param out_mbAddrD output parameter. the address and availability status of the top macroblock of the macroblock pair above-left of the current macroblock pair
 */
inline void neighbouring_mb_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA, int32_t* out_mbAddrB,
                                                                int32_t* out_mbAddrC, int32_t* out_mbAddrD);
inline void neighbouring_mb_A_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrA);
inline void neighbouring_mb_B_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrB);
inline void neighbouring_mb_C_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrC);
inline void neighbouring_mb_D_address_availability_in_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* out_mbAddrD);

/**
 * @brief Derivation process for neighbouring macroblocks
 * @see 6.4.11.1 Derivation process for neighbouring macroblocks
 *
 * Outputs of this process are:
 * –  mbAddrA: the address of the macroblock to the left of the current macroblock and its availability status,
 * –  mbAddrB: the address of the macroblock above the current macroblock and its availability status.
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param is_chroma is chroma
 * @param MbWidthC the macroblock chroma width. it is used when is_chroma is true
 * @param MbHeightC the macroblock chroma height. it is used when is_chroma is true
 * @param out_mbAddrA output parameter. the left macroblock address
 * @param out_mbAddrB output parameter. the upper macroblock address
 */
void neighbouring_macroblocks(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                              int32_t is_chroma, int32_t MbWidthC, int32_t MbHeightC, int32_t* out_mbAddrA, int32_t* out_mbAddrB);

/**
 * @brief Derivation process for neighbouring 8x8 luma block
 * @see 6.4.11.2 Derivation process for neighbouring 8x8 luma block
 *
 * Input to this process is an 8x8 luma block index luma8x8BlkIdx.
 * The luma8x8BlkIdx specifies the 8x8 luma blocks of a macroblock in a raster scan.
 *
 * Outputs of this process are:
 *  – mbAddrA: either equal to CurrMbAddr or the address of the macroblock to the left of the current macroblock and its availability status,
 *  – luma8x8BlkIdxA: the index of the 8x8 luma block to the left of the 8x8 block with index luma8x8BlkIdx and its availability status,
 *  – mbAddrB: either equal to CurrMbAddr or the address of the macroblock above the current macroblock and its availability status,
 *  – luma8x8BlkIdxB: the index of the 8x8 luma block above the 8x8 block with index luma8x8BlkIdx and its availability status.
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param luma8x8BlkIdx the 8x8 luma block index
 * @param out_mbAddrA output parameter. the left macroblock address
 * @param out_luma8x8BlkIdxA output parameter. the 8x8 luma block index to the left of the 8x8 block with index luma8x8BlkIdx
 * @param out_mbAddrB output parameter. the upper macroblock address
 * @param out_luma8x8BlkIdxB output parameter. the 8x8 luma block index above the 8x8 block with index luma8x8BlkIdx
 */
void neighbouring_8x8_luma_block(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                                 int32_t luma8x8BlkIdx, int32_t* out_mbAddrA, int32_t* out_luma8x8BlkIdxA, int32_t* out_mbAddrB, int32_t* out_luma8x8BlkIdxB);

/**
 * @brief Derivation process for neighbouring 8x8 chroma blocks for ChromaArrayType equal to 3
 * @see 6.4.11.3 Derivation process for neighbouring 8x8 chroma blocks for ChromaArrayType equal to 3
 *
 * This process is only invoked when ChromaArrayType is equal to 3.
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param MbWidthC the macroblock chroma width
 * @param MbHeightC the macroblock chroma height
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param cbcr8x8BlkIdx the 8x8 Cb or Cr block index
 * @param out_mbAddrA output parameter. the left macroblock address
 * @param out_cbcr8x8BlkIdxA output parameter. the 8x8 Cb or Cr block index to the left of the 8x8 block with index cbcr8x8BlkIdx
 * @param out_mbAddrB output parameter. the upper macroblock address
 * @param out_cbcr8x8BlkIdxB output parameter. the 8x8 Cb or Cr block index above the 8x8 block with index cbcr8x8BlkIdx
 */
void neighbouring_8x8_chroma_block(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t MbWidthC, int32_t MbHeightC,
                                   int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t cbcr8x8BlkIdx, int32_t* out_mbAddrA, int32_t* out_cbcr8x8BlkIdxA, int32_t* out_mbAddrB,
                                   int32_t* out_cbcr8x8BlkIdxB);

/**
 * @brief Derivation process for neighbouring 4x4 luma blocks
 * @see 6.4.11.4 Derivation process for neighbouring 4x4 luma blocks
 *
 * Input to this process is a 4x4 luma block index luma4x4BlkIdx.
 *
 * Outputs of this process are:
 *  – mbAddrA: either equal to CurrMbAddr or the address of the macroblock to the left of the current macroblock and its availability status,
 *  – luma4x4BlkIdxA: the index of the 4x4 luma block to the left of the 4x4 block with index luma4x4BlkIdx and its availability status,
 *  – mbAddrB: either equal to CurrMbAddr or the address of the macroblock above the current macroblock and its availability status,
 *  – luma4x4BlkIdxB: the index of the 4x4 luma block above the 4x4 block with index luma4x4BlkIdx and its availability status.
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param luma4x4BlkIdx the 4x4 luma block index
 * @param out_mbAddrA output parameter. the left macroblock address
 * @param out_luma4x4BlkIdxA output parameter. the 4x4 luma block index to the left of the 4x4 block with index luma4x4BlkIdx
 * @param out_mbAddrB output parameter. the upper macroblock address
 * @param out_luma4x4BlkIdxB output parameter. the 4x4 luma block index above the 4x4 block with index luma4x4BlkIdx
 */
void neighbouring_4x4_luma_block(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                                 int32_t luma4x4BlkIdx, int32_t* out_mbAddrA, int32_t* out_luma4x4BlkIdxA, int32_t* out_mbAddrB, int32_t* out_luma4x4BlkIdxB);

/**
 * @brief Derivation process for neighbouring 4x4 chroma blocks
 * @see 6.4.11.5 Derivation process for neighbouring 4x4 chroma blocks
 *
 * This clause is only invoked when ChromaArrayType is equal to 1 or 2.
 *
 * Input to this process is a 4x4 chroma block index chroma4x4BlkIdx
 *
 * Outputs of this process are:
 *  – mbAddrA (either equal to CurrMbAddr or the address of the macroblock to the left of the current macroblock) and its availability status,
 *  – chroma4x4BlkIdxA (the index of the 4x4 chroma block to the left of the 4x4 chroma block with index chroma4x4BlkIdx) and its availability status,
 *  – mbAddrB (either equal to CurrMbAddr or the address of the macroblock above the current macroblock) and its availability status,
 *  – chroma4x4BlkIdxB (the index of the 4x4 chroma block above the 4x4 chroma block with index chroma4x4BlkIdx) and its availability status.
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param MbWidthC the macroblock chroma width
 * @param MbHeightC the macroblock chroma height
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param chroma4x4BlkIdx the 4x4 chroma block index
 * @param out_mbAddrA output parameter. the left macroblock address
 * @param out_chroma4x4BlkIdxA output parameter. the 4x4 chroma block index to the left of the 4x4 block with index chroma4x4BlkIdx
 * @param out_mbAddrB output parameter. the upper macroblock address
 * @param out_chroma4x4BlkIdxB output parameter. the 4x4 chroma block index above the 4x4 block with index chroma4x4BlkIdx
 */
void neighbouring_4x4_chroma_block_ChromaArrayType_12(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t MbWidthC,
                                                      int32_t MbHeightC, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t chroma4x4BlkIdx, int32_t* out_mbAddrA,
                                                      int32_t* out_chroma4x4BlkIdxA, int32_t* out_mbAddrB, int32_t* out_chroma4x4BlkIdxB);

/**
 * @brief Derivation process for neighbouring 4x4 chroma blocks for ChromaArrayType equal to 3
 * @see 6.4.11.6 Derivation process for neighbouring 4x4 chroma blocks for ChromaArrayType equal to 3
 *
 * This process is only invoked when ChromaArrayType is equal to 3.
 *
 * The derivation process for neighbouring 4x4 chroma block in 4:4:4 chroma format is identical to the derivation process for neighbouring 4x4 luma block as specified in
 * clause 6.4.11.4 when substituting the term "luma" with the term "Cb" or the term "Cr", and substituting the term "luma4x4BlkIdx" with the term "cb4x4BlkIdx" or the term
 * "cr4x4BlkIdx" in all places in clause 6.4.11.4.
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param MbWidthC the macroblock chroma width
 * @param MbHeightC the macroblock chroma height
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param chroma4x4BlkIdx the 4x4 chroma block index
 * @param out_mbAddrA output parameter. the left macroblock address
 * @param out_chroma4x4BlkIdxA output parameter. the 4x4 chroma block index to the left of the 4x4 block with index chroma4x4BlkIdx
 * @param out_mbAddrB output parameter. the upper macroblock address
 * @param out_chroma4x4BlkIdxB output parameter. the 4x4 chroma block index above the 4x4 block with index chroma4x4BlkIdx
 */
void neighbouring_4x4_chroma_block_ChromaArrayType_3(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t MbWidthC,
                                                     int32_t MbHeightC, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t chroma4x4BlkIdx, int32_t* out_mbAddrA,
                                                     int32_t* out_chroma4x4BlkIdxA, int32_t* out_mbAddrB, int32_t* out_chroma4x4BlkIdxB);

/**
 * @brief Derivation process for neighbouring partitions
 * @see 6.4.11.7 Derivation process for neighbouring partitions
 *
 * Inputs to this process are:
 *  – a macroblock partition index mbPartIdx
 *  – a current sub-macroblock type currSubMbType
 *  – a sub-macroblock partition index subMbPartIdx
 *
 * Outputs of this process are:
 * – mbAddrA\mbPartIdxA\subMbPartIdxA: specifying the macroblock or sub-macroblock partition to the left of the current macroblock and its availability status, or the
 * sub-macroblock partition CurrMbAddr\mbPartIdx\subMbPartIdx and its availability status, – mbAddrB\mbPartIdxB\subMbPartIdxB: specifying the macroblock or sub-macroblock partition
 * above the current macroblock and its availability status, or the sub-macroblock partition CurrMbAddr\mbPartIdx\subMbPartIdx and its availability status, –
 * mbAddrC\mbPartIdxC\subMbPartIdxC: specifying the macroblock or sub-macroblock partition to the right-above of the current macroblock and its availability status, or the
 * sub-macroblock partition CurrMbAddr\mbPartIdx\subMbPartIdx and its availability status, – mbAddrD\mbPartIdxD\subMbPartIdxD: specifying the macroblock or sub-macroblock partition
 * to the left-above of the current macroblock and its availability status, or the sub-macroblock partition CurrMbAddr\mbPartIdx\subMbPartIdx and its availability status.
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param mbPartIdx the macroblock partition index
 * @param currSubMbType the current sub-macroblock type
 * @param subMbPartIdx the sub-macroblock partition index
 * @param subMbPartType the sub-macroblock partition type
 * @param out_mbAddrA output parameter. macroblock address
 * @param out_mbPartIdxA output parameter. macroblock partition index
 * @param out_subMbPartIdxA output parameter. sub-macroblock partition index
 * @param out_mbAddrB output parameter. macroblock address
 * @param out_mbPartIdxB output parameter. macroblock partition index
 * @param out_subMbPartIdxB output parameter. sub-macroblock partition index
 * @param out_mbAddrC output parameter. macroblock address
 * @param out_mbPartIdxC output parameter. macroblock partition index
 * @param out_subMbPartIdxC output parameter. sub-macroblock partition index
 * @param out_mbAddrD output parameter. macroblock address
 * @param out_mbPartIdxD output parameter. macroblock partition index
 * @param out_subMbPartIdxD output parameter. sub-macroblock partition index
 * @return int 0 on success, negative value on error
 */
int neighbouring_partitions_pre(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags,
                            int32_t mbPartIdx, MB_TYPE_NAME currSubMbType, int32_t subMbPartIdx, MB_TYPE_NAME subMbPartType, int32_t* out_mbAddrA, int32_t* out_mbPartIdxA,
                            int32_t* out_subMbPartIdxA, int32_t* out_mbAddrB, int32_t* out_mbPartIdxB, int32_t* out_subMbPartIdxB, int32_t* out_mbAddrC, int32_t* out_mbPartIdxC,
                            int32_t* out_subMbPartIdxC, int32_t* out_mbAddrD, int32_t* out_mbPartIdxD, int32_t* out_subMbPartIdxD);
int neighbouring_partitions_post();

/**
 * @brief Derivation process for neighbouring locations
 * @see 6.4.12 Derivation process for neighbouring locations
 *
 * Input to this process is a luma or chroma location ( xN, yN ) expressed relative to the upper left corner of the current macroblock.
 * Outputs of this process are:
 *     - mbAddrN: either equal to CurrMbAddr or to the address of neighbouring macroblock that contains (xN, yN) and its availability status,
 *     - ( xW, yW ): the location (xN, yN) expressed relative to the upper-left corner of the macroblock mbAddrN (rather than relative to the upper-left corner of the current
 * macroblock).
 *
 *
 * @param MbaffFrameFlag the MbaffFrameFlag
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock. this parameter is used when MbaffFrameFlag is 1
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array. this parameter is used when MbaffFrameFlag is 1
 * @param xN input x location relative to the upper left corner of the current macroblock
 * @param yN input y location relative to the upper left corner of the current macroblock
 * @param maxW maximum values of the location components xN and xW
 * @param maxH maximum values of the location components yN and yW
 * @param out_mbAddrN_type output parameter. the mbAddrN relative location type and availability
 * @param out_mbAddrN output parameter. the mbAddrN address
 * @param out_xW output parameter. the location xN expressed relative to the upper-left corner of the macroblock out_mbAddrN
 * @param out_yW output parameter. the location yN expressed relative to the upper-left corner of the macroblock out_mbAddrN
 */
void neighbouring_locations(int32_t MbaffFrameFlag, int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t xN,
                            int32_t yN, int32_t maxW, int32_t maxH, RELATIVE_LOCATION_TYPE* out_mbAddrN_type, int32_t* out_mbAddrN, int32_t* out_xW, int32_t* out_yW);

/**
 * @brief Specification for neighbouring locations in fields and non-MBAFF frames
 * @see 6.4.12.1 Specification for neighbouring locations in fields and non-MBAFF frames
 *
 * The specifications in this clause are applied when MbaffFrameFlag is equal to 0.
 *
 * The derivation process for neighbouring macroblock addresses and their availability in clause 6.4.9 is invoked with mbAddrA, mbAddrB, mbAddrC, and mbAddrD as well as their
 * availability status as the output.
 *
 * @param CurrMbAddr the current macroblock address
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param xN input x location relative to the upper left corner of the current macroblock
 * @param yN input y location relative to the upper left corner of the current macroblock
 * @param maxW max width
 * @param maxH max height
 * @param out_mbAddrN_type output parameter. the mbAddrN relative location type and availability
 * @param out_mbAddrN output parameter. the mbAddrN address
 * @param out_xW output parameter. the location xN expressed relative to the upper-left corner of the macroblock out_mbAddrN
 * @param out_yW output parameter. the location yN expressed relative to the upper-left corner of the macroblock out_mbAddrN
 */
void neighbouring_locations_in_frame_non_MBAFF_frame(int32_t CurrMbAddr, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t xN, int32_t yN, int32_t maxW, int32_t maxH,
                                                     RELATIVE_LOCATION_TYPE* out_mbAddrN_type, int32_t* out_mbAddrN, int32_t* out_xW, int32_t* out_yW);

/**
 * @brief Specification for neighbouring locations in MBAFF frames
 * @see 6.4.12.2 Specification for neighbouring locations in MBAFF frames
 *
 * The specifications in this clause are applied when MbaffFrameFlag is equal to 1.
 *
 * The derivation process for neighbouring macroblock addresses and their availability in clause 6.4.10 is invoked with mbAddrA, mbAddrB, mbAddrC, and mbAddrD as well as their
 * availability status as the output.
 *
 * @param CurrMbAddr the current macroblock address
 * @param currMbFrameFlag whether the current macroblock is a frame macroblock.
 * @param PicWidthInMbs the picture width in macroblocks
 * @param mb_slice_ids the macroblock slices id array
 * @param mb_frame_flags the macroblock frame flags array.
 * @param xN input x location relative to the upper left corner of the current macroblock
 * @param yN input y location relative to the upper left corner of the current macroblock
 * @param maxW max width
 * @param maxH max height
 * @param out_mbAddrN_type output parameter. the mbAddrN relative location type and availability
 * @param out_mbAddrN output parameter. the mbAddrN address
 * @param out_xW output parameter. the location xN expressed relative to the upper-left corner of the macroblock out_mbAddrN
 * @param out_yW output parameter. the location yN expressed relative to the upper-left corner of the macroblock out_mbAddrN
 */
void neighbouring_locations_in_MBAFF_frame(int32_t CurrMbAddr, int32_t currMbFrameFlag, int32_t PicWidthInMbs, int32_t* mb_slice_ids, int32_t* mb_frame_flags, int32_t xN,
                                           int32_t yN, int32_t maxW, int32_t maxH, RELATIVE_LOCATION_TYPE* out_mbAddrN_type, int32_t* out_mbAddrN, int32_t* out_xW,
                                           int32_t* out_yW);

/**
 * @brief Derivation process for 4x4 luma block indices
 * @see 6.4.13.1 Derivation process for 4x4 luma block indices
 *
 * @see 6.4.3 Inverse 4x4 luma block scanning process
 * @see Figure 6-10 Scan for 4x4 luma blocks
 *
 * Input to this process is a luma location ( xP, yP ) relative to the upper-left luma sample of a macroblock.
 * Output of this process is a 4x4 luma block index luma4x4BlkIdx.
 * The 4x4 luma block index luma4x4BlkIdx is derived by
 *     luma4x4BlkIdx = 8 * ( yP / 8 ) + 4 * ( xP / 8 ) + 2 * ( ( yP % 8 ) / 4 ) + ( ( xP % 8 ) / 4 )
 *
 * @param xP the x location relative to the upper-left luma sample of a macroblock
 * @param yP the y location relative to the upper-left luma sample of a macroblock
 * @param out_luma4x4BlkIdx output parameter. the 4x4 luma block index
 */
inline void derivate_4x4_luma_block_indices(int32_t xP, int32_t yP, int32_t* out_luma4x4BlkIdx);

/**
 * @brief Derivation process for 4x4 chroma block indices
 * @see 6.4.13.2 Derivation process for 4x4 chroma block indices
 *
 * This clause is only invoked when ChromaArrayType is equal to 1 or 2.
 * Input to this process is a chroma location ( xP, yP ) relative to the upper-left chroma sample of a macroblock.
 * Output of this process is a 4x4 chroma block index chroma4x4BlkIdx.
 * The 4x4 chroma block index chroma4x4BlkIdx is derived by
 *     chroma4x4BlkIdx = 2 * ( yP / 4 ) + ( xP / 4 )
 *
 * @param xP the x location relative to the upper-left chroma sample of a macroblock
 * @param yP the y location relative to the upper-left chroma sample of a macroblock
 * @param out_chroma4x4BlkIdx output parameter. the 4x4 chroma block index
 */
inline void derivate_4x4_chroma_block_indices(int32_t xP, int32_t yP, int32_t* out_chroma4x4BlkIdx);

/**
 * @brief Derivation process for 8x8 luma block indices
 * @see 6.4.13.3 Derivation process for 8x8 luma block indices
 *
 * Input to this process is a luma location ( xP, yP ) relative to the upper-left luma sample of a macroblock.
 * Outputs of this process is an 8x8 luma block index luma8x8BlkIdx.
 * The 8x8 luma block index luma8x8BlkIdx is derived by
 *     luma8x8BlkIdx = 2 * ( yP / 8 ) + ( xP / 8 )
 *
 * @param xP the x location relative to the upper-left luma sample of a macroblock
 * @param yP the y location relative to the upper-left luma sample of a macroblock
 * @param out_luma8x8BlkIdx output parameter. the 8x8 luma block index
 */
inline void derivate_8x8_luma_block_indices(int32_t xP, int32_t yP, int32_t* out_luma8x8BlkIdx);

/**
 * @brief Derivation process for macroblock and sub-macroblock partition indices
 * @see 6.4.13.4 Derivation process for macroblock and sub-macroblock partition indices
 *
 * Inputs to this process are:
 *  – a luma location ( xP, yP ) relative to the upper-left luma sample of a macroblock,
 *  – a macroblock type mbType,
 *  – when mbType is equal to P_8x8, P_8x8ref0, or B_8x8, a list of sub-macroblock types subMbType with 4 elements.
 *
 * Outputs of this process are:
 *  – a macroblock partition index mbPartIdx,
 *  – a sub-macroblock partition index subMbPartIdx.

 * @param xP the x location relative to the upper-left luma sample of a macroblock
 * @param yP the y location relative to the upper-left luma sample of a macroblock
 * @param mbType the macroblock type
 * @param subMbType the sub-macroblock type with 4 elements when mbType is equal to P_8x8, P_8x8ref0, or B_8x8
 * @param out_mbPartIdx output parameter. the macroblock partition index
 * @param out_subMbPartIdx output parameter. the sub-macroblock partition index
 * @return int 0 on success, negative value on error
 */
inline int derivate_mb_submb_partition_indices(int32_t xP, int32_t yP, MB_TYPE_NAME mbType, MB_TYPE_NAME* subMbType, int32_t* out_mbPartIdx, int32_t* out_subMbPartIdx);

#endif