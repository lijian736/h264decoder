#ifndef _H_H264_RBSP_H_
#define _H_H264_RBSP_H_

#include <stddef.h>
#include <stdint.h>

#include "h264_error.h"
#include "h264_defs.h"

/**
 * @brief Extract the NALU RBSP (Raw Byte Sequence Payload)
 * @see 7.4.1 NAL unit semantics
 * @see 7.4.1.1 Encapsulation of an SODB within an RBSP (informative)
 *
 * Within the NAL unit, the following three-byte sequences shall not occur at any byte-aligned position:
 * - 0x000000
 * - 0x000001
 * - 0x000002
 *
 * Within the NAL unit, any four-byte sequence that starts with 0x000003 other than the following sequences shall not
 * occur at any byte-aligned position:
 * - 0x00000300
 * - 0x00000301
 * - 0x00000302
 * - 0x00000303
 *
 * @param nalu the NALU buffer pointer
 * @param nalu_len the NALU buffer length
 * @param rbsp in/out parameter. the RBSP buffer pointer
 * @return int the actual RBSP length extracted from NALU, negative value on error
 */
int extract_nalu_rbsp(const uint8_t* nalu, size_t nalu_len, uint8_t* rbsp);

/**
 * @brief Extract the NALU RBSP (Raw Byte Sequence Payload)
 * @see 7.4.1 NAL unit semantics
 * @see 7.4.1.1 Encapsulation of an SODB within an RBSP (informative)
 *
 * @param nalu the NALU buffer pointer
 * @param nalu_len the NALU buffer length
 * @param rbsp in/out parameter. the RBSP buffer pointer
 * @return int the actual RBSP length extracted from NALU
 */
int extract_nalu_rbsp_simple(const uint8_t* nalu, size_t nalu_len, uint8_t* rbsp);

/**
 * @brief The RBSP bits reader from a specified buffer
 * @see ITU-T H.264 9 Parsing process and 7.2 Specification of syntax functions, categories, and descriptors
 *
 */
typedef struct {
    uint8_t* start;   /* the start position(inclusive) */
    uint8_t* end;     /* the end position(exclusive) */
    uint8_t* current; /* the current position */
    int bits_left;    /* the bits left in the current byte, default value is 8 */
} RBSPReader;

/**
 * @brief check if there are n bits left in the RBSPReader
 *
 * @param reader the RBSPReader
 * @param n n bits
 * @return int: 1 is available, 0 is not available
 */
int is_n_bits_available(RBSPReader* reader, int n);

/**
 * @brief check if the bits left in the RBSPReader is byte aligned
 *
 * @param reader the RBSPReader
 * @return int: 1 is aligned, 0 is not aligned
 */
int is_byte_aligned(RBSPReader* reader);

/**
 * @brief check if the RBSPReader read ended successfully
 *
 * @param reader the RBSPReader
 * @return int 1: end successfully, 0: end failed
 */
int is_end_valid(RBSPReader* reader);

/**
 * @brief read the byte aligned pointer object
 *
 * @param reader the RBSPReader
 * @param ptr out parameter, the ptr
 * @param length the aligned data length, it may be a negative value when the current pointer exceeds the ends of the reader
 */
void read_byte_aligned_pointer(RBSPReader* reader, uint8_t** ptr, int* length);

/**
 * @brief skip n bits of the RBSPReader
 *
 * @param reader the RBSPReader
 * @param n the n bits
 */
void skip_n_bits(RBSPReader* reader, int n);

/**
 * @brief check if the RBSPReader has reached its end
 *
 * @param reader the RBSPReader
 * @return int: 1 is end, 0 is not end
 */
int is_end_of_reader(RBSPReader* reader);

/**
 * @brief read fixed-pattern bit string using n bits written (from left to right) with the left bit first
 * @see 7.2 Specification of syntax functions, categories, and descriptors
 *
 * @param reader the RBSPReader
 * @param n n bits
 * @return uint32_t
 */
uint32_t read_f(RBSPReader* reader, int n);

/**
 * @brief read unsigned integer using n bits
 * @see 7.2 Specification of syntax functions, categories, and descriptors
 *
 * @param reader the RBSPReader
 * @param n n bits
 * @return uint32_t
 */
uint32_t read_u(RBSPReader* reader, int n);
uint32_t read_u8(RBSPReader* reader);
uint32_t read_u16(RBSPReader* reader);
uint32_t read_u32(RBSPReader* reader);

/**
 * @brief read unsigned integer Exp-Golomb-coded syntax element with the left bit first
 * @see 7.2 Specification of syntax functions, categories, and descriptors
 * @see 9.1 Parsing process for Exp-Golomb codes
 *
 * @param reader the RBSPReader
 * @return uint32_t
 */
uint32_t read_ue(RBSPReader* reader);

/**
 * @brief read signed integer Exp-Golomb-coded syntax element with the left bit first
 * @see 7.2 Specification of syntax functions, categories, and descriptors
 * @see 9.1 Parsing process for Exp-Golomb codes
 *
 * @param reader the RBSPReader
 * @return int32_t
 */
int32_t read_se(RBSPReader* reader);

/**
 * @brief read the truncat Exp-Golomb-codec syntax element
 * 
 * @param reader the RBSPReader
 * @param range the range
 * @return uint32_t 
 */
uint32_t read_te(RBSPReader* reader, int32_t range);

/**
 * @brief Mapping process for coded block pattern
 * @see 9.1.2 Mapping process for coded block pattern
 * @see Table 9-4 – Assignment of codeNum to values of coded_block_pattern for macroblock prediction modes
 * 
 * Input to this process is codeNum as specified in clause 9.1.
 * Output of this process is a value of the syntax element coded_block_pattern coded as me(v)
 * 
 * @param reader the RBSPReader
 * @param ChromaArrayType the chroma array type
 * @param pred_mode the prediction mode
 * @return int32_t the value of coded block pattern. -1 on error
 */
int32_t read_me(RBSPReader* reader, int32_t ChromaArrayType, H264_MB_PART_PRED_MODE pred_mode);

/**
 * @brief Peek 1 bit value
 *
 * @param reader the RBSPReader
 * @return uint8_t
 */
uint8_t peek_u1(RBSPReader* reader);

#endif