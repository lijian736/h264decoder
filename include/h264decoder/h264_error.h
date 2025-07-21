#ifndef _H_H264_ERROR_H_
#define _H_H264_ERROR_H_

/* OK */
#define ERR_OK 0

/* out of memory */
#define ERR_OOM (-1)
/* not implemented*/
#define ERR_NOT_IMPL (-2)
/* invalid parameter */
#define ERR_INVALID_PARAM (-3)

/* end of stream */
#define ERR_EOS (-2000)
/* invalid RBSP */
#define ERR_INVALID_RBSP (-2001)
/* RBSP invalid three-byte sequences
 * @see 7.4.1 NAL unit semantics */
#define ERR_RBSP_INVALID_3_BYTE (-2002)
/* RBSP invalid four-byte sequence
 * @see 7.4.1 NAL unit semantics */
#define ERR_RBSP_INVALID_4_BYTE (-2003)
/* invalid SPS*/
#define ERR_INVALID_SPS (-2004)
/* invalid SPS id*/
#define ERR_INVALID_SPS_ID (-2005)
/* invalid SPS param */
#define ERR_INVALID_SPS_PARAM (-2006)
/* invalid PPS*/
#define ERR_INVALID_PPS (-2007)
/* invalid PPS ID*/
#define ERR_INVALID_PPS_ID (-2008)
/* invalid PPS param */
#define ERR_INVALID_PPS_PARAM (-2009)
/* no sps parsed*/
#define ERR_NO_SPS_PARSED (-2010)
/* no pps parsed*/
#define ERR_NO_PPS_PARSED (-2011)
/* invalid HRD*/
#define ERR_INVALID_HRD (-2012)
/* invalid slice */
#define ERR_INVALID_SLICE (-2013)
/* no main picture for auxiliary slice */
#define ERR_NO_MAIN_PIC_4_AUXILIARY (-2014)

/* invalid VUI param*/
#define ERR_INVALID_VUI_PARAM (-2015)
/* invalid HRD param*/
#define ERR_INVALID_HRD_PARAM (-2016)

/* invalid scaling list*/
#define ERR_INVALID_SCALING (-2017)

/* invalid slice type for IDR */
#define ERR_INVALID_SLICE_4_IDR (-2018)

/* invalid slice param*/
#define ERR_INVALID_SLICE_PARAM (-2019)

/* invalid reference picture list modification param */
#define ERR_INVALID_REF_PIC_LIST_PARAM (-2020)

/* invalid reference picture list mvc modification param */
#define ERR_INVALID_REF_PIC_LIST_MVC_PARAM (-2021)

/* invalid prediction weight table */
#define ERR_INVALID_PRE_WEIGHT_TABLE (-2022)

/* invalid decoded reference picture marking */
#define ERR_INVALID_DEC_REF_PIC_MARKING (-2023)

/* no active SPS*/
#define ERR_NO_ACTIVE_SPS (-2024)

/* no active PPS*/
#define ERR_NO_ACTIVE_PPS (-2025)

/* invalid slice data */
#define ERR_INVALID_SLICE_DATA (-2026)

/* invalid cabac context index */
#define ERR_INVALID_CABAC_CONTEXT_INDEX (-2027)

/* invalid next macroblock address */
#define ERR_INVALID_NEXT_MB_ADDR (-2028)

/* decode bin error*/
#define ERR_DECODE_BIN (-2029)

/* decode bypass error */
#define ERR_DECODE_BYPASS (-2030)

/* decode terminate error */
#define ERR_DECODE_TERMINATE (-2031)

/* decode decision error */
#define ERR_DECODE_DECISION (-2032)

/* macroblock not available*/
#define ERR_MB_NOT_AVAILABLE (-2033)

/* no slice header*/
#define ERR_NO_SLICE_HEADER (-2034)

/* unsupport slice type for binarization*/
#define ERR_UNSUPPORT_SLICE_TYPE_OF_BINARIZATION  (-2035)

/* invalid slice type*/
#define ERR_INVALID_SLICE_TYPE (-2036)

/* unrecognized mb in slice*/
#define ERR_UNRECOGNIZED_MB (-2037)

/* unknown mb partition prediction mode */
#define ERR_UNKNOWN_MB_PART_PRED_MODE (-2038)

/* invalid coded_block_pattern*/
#define ERR_INVALID_CODED_BLOCK_PATTERN (-2039)

/* invalid ctxIdxOffset for coded_block_pattern*/
#define ERR_INVALID_CTXIDXOFFSET_4_CODED_BLOCK_PATTERN  (-2040)

#endif