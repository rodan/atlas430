
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/reference_libs_msp430fr5x
//  license:         GNU GPLv3

/**
 * \file
 *         Header file for the data conversion library
 * \author
 *         Petre Rodan <2b4eda@subdimension.ro>
 */

/**
* \addtogroup Library
* @{ */

/**
 * \defgroup lib_convert Data conversion library
 * @{
  simple functions for converting data between different formats

  to be used when the libc implementation takes too much space
**/

#ifndef __LIB_CONVERT_H__
#define __LIB_CONVERT_H__

#define  CONV_BASE_2_BUF_SZ  19 ///< buffer size needed for converting number into string in binary representation
#define CONV_BASE_10_BUF_SZ  12 ///< buffer size needed for converting number into string in decimal representation (-2147483648 + ending 0 is 12 bytes long)
#define CONV_BASE_16_BUF_SZ  12 ///< buffer size needed for converting number into string in hexadecimal representation

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1 ///< define EXIT_FAILURE just in case stdlib.h is not included
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0 ///< define EXIT_FAILURE just in case stdlib.h is not included
#endif

typedef enum {
    PAD_NONE,       ///< no padding
    PAD_ZEROES,     ///< pad with zeroes
    PAD_SPACES,     ///< pad with spaces
} pad_type;         ///< padding type used by prepend_padding()

#ifdef __cplusplus
extern "C" {
#endif

// string functions

/** return a binary string for an uint16_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_2_BUF_SZ] needs to be pre-allocated
    @param val uint16_t value
    @return pointer to the string
*/
char *_utob(char *buf, const uint16_t val);

/** return a hex string for an uint32_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_16_BUF_SZ] needs to be pre-allocated
    @param val uint32_t value
    @return pointer to the string
*/
char *_utoh(char *buf, const uint32_t val);

/** return a hex string for an uint8_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_16_BUF_SZ] needs to be pre-allocated
    @param val uint32_t value
    @return pointer to the string
*/
char *_utoh8(char *buf, const uint32_t val);

/** return a hex string for an uint16_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_16_BUF_SZ] needs to be pre-allocated
    @param val uint32_t value
    @return pointer to the string
*/
char *_utoh16(char *buf, const uint32_t val);

/** return a hex string for an uint32_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_16_BUF_SZ] needs to be pre-allocated
    @param val uint32_t value
    @return pointer to the string
*/
char *_utoh32(char *buf, const uint32_t val);

/** return a padded hex string for an uint32_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_16_BUF_SZ] needs to be pre-allocated
    @param val uint32_t value
    @param pad_size padding size
    @return pointer to the string
*/
char *_utorh(char *buf, const uint32_t val, const uint8_t pad_size);

/** return a decimal string for an uint32_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_10_BUF_SZ] needs to be pre-allocated
    @param val uint32_t value
    @return pointer to the string
*/
char *_utoa(char *buf, const uint32_t val);

/** return a decimal string for an int32_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_10_BUF_SZ] needs to be pre-allocated
    @param val int32_t value
    @return pointer to the string
*/
char *_itoa(char *buf, const int32_t val);

/** return a decimal string for an int16_t integer
    @param buf temporary buffer used to build up the string. buf[CONV_BASE_10_BUF_SZ] needs to be pre-allocated
    @param val int32_t value
    @return pointer to the string
*/
char *_i16toa(char *buf, const int16_t val);

/** return a string with prefixed padding
    @param buf pointer to originally allocated buffer
    @param converted_buf pointer within the originally allocated buffer where the conversion is stored
    @param padding_type type of padding
    @param target_len target total length of resulting string
    @return pointer to the resulting string. *converted_buf is returned on parameter error
*/
char *prepend_padding(char *buf, char *converted_buf, const pad_type padding_type, const uint8_t target_len);

/** convert a string to a 16bit unsigned integer
    @param str pointer to string to be converted
    @param out pointer to resulting uint16_t value [min .. max]
    @param seek how many chars to skip [0 .. len-1]
    @param len length of string to parse
    @param min minimal value for result
    @param max maximum value for result
    @return EXIT_FAILURE if resulting value is outside the limits, EXIT_SUCCESS otherwise
*/
uint8_t str_to_uint8(char *str, uint8_t * out, const uint8_t seek,
                      const uint8_t len, const uint8_t min, const uint8_t max);

/** convert a string to a 16bit unsigned integer
    @param str pointer to string to be converted
    @param out pointer to resulting uint16_t value [min .. max]
    @param seek how many chars to skip [0 .. len-1]
    @param len length of string to parse
    @param min minimal value for result
    @param max maximum value for result
    @return EXIT_FAILURE if resulting value is outside the limits, EXIT_SUCCESS otherwise
*/
uint8_t str_to_uint16(char *str, uint16_t * out, const uint8_t seek,
                      const uint8_t len, const uint16_t min, const uint16_t max);

/** convert a string to a 32bit unsigned integer
    @param str pointer to string to be converted
    @param out pointer to resulting uint32_t value [min .. max]
    @param seek how many chars to skip [0 .. len-1]
    @param len length of string to parse
    @param min minimal value for result
    @param max maximum value for result
    @return EXIT_FAILURE if resulting value is outside the limits, EXIT_SUCCESS otherwise
*/
uint8_t str_to_uint32(char *str, uint32_t * out, const uint8_t seek,
                      const uint8_t len, const uint32_t min, const uint32_t max);

/** convert a string to a 32bit signed integer
    @param str pointer to string to be converted
    @param out pointer to resulting int32_t value [min .. max]
    @param seek how many chars to skip [0 .. len-1]
    @param len length of string to parse
    @param min minimal value for result
    @param max maximum value for result
    @return EXIT_FAILURE if resulting value is outside the limits, EXIT_SUCCESS otherwise
*/
uint8_t str_to_int32(char *str, int32_t * out, const uint8_t seek,
                      const uint8_t len, const int32_t min, const int32_t max);

/** convert a hex string to a 8bit unsigned integer
    @param str pointer to string to be converted
    @param out pointer to resulting uint8_t value [min .. max]
    @param seek how many chars to skip [0 .. len-1]
    @param len length of string to parse
    @param min minimal value for result
    @param max maximum value for result
    @return EXIT_FAILURE if resulting value is outside the limits, EXIT_SUCCESS otherwise
*/
uint8_t hstr_to_uint8(char *str, uint8_t *out, const uint8_t seek,
                      const uint8_t len, const uint8_t min, const uint16_t max);


/** convert a hex string to a 16bit unsigned integer
    @param str pointer to string to be converted
    @param out pointer to resulting uint16_t value [min .. max]
    @param seek how many chars to skip [0 .. len-1]
    @param len length of string to parse
    @param min minimal value for result
    @param max maximum value for result
    @return EXIT_FAILURE if resulting value is outside the limits, EXIT_SUCCESS otherwise
*/
uint8_t hstr_to_uint16(char *str, uint16_t *out, const uint8_t seek,
                      const uint8_t len, const uint16_t min, const uint16_t max);

/** convert in-memory structure to an ascii string (ex 0x14 -> 0x31 0x34)
    @param data_in input mem structure
    @param data_out ascii output
    @param len in-mem length
    @return void
*/
void mem2ascii(uint8_t * data_in, uint8_t * data_out, uint8_t len);

/** convert an ascii string (ex 0x31 0x34 -> 0x14)
    @param data_in input mem structure
    @param data_out ascii output
    @param len in-mem length
*/
void ascii2mem(uint8_t * data_in, uint8_t * data_out, uint8_t len);

/** convert a base-10 integer value to a binary-coded decimal (BCD)
    @param val input decimal value to be converted
    @return BCD conversion
*/
uint8_t dec_to_bcd(const uint8_t val);

/** convert a binary-coded decimal (BCD) to a base-10 integer value
    @param val BCD integer to be converted
    @return input decimal value to be converted
*/
uint8_t bcd_to_dec(const uint8_t val);

/** convert hex-ascii string into little endian uint32_t
    0x31 0x34 -> 0x14
    @param buf input
    @param seek skip these many characters
    @param len keep converting these many chars
    @return little endian uint32_t representation
*/
uint32_t _wiretou32(char *buf, const uint16_t seek, const uint16_t len);

/** flip bytes in a two byte WORD
    0xaabb -> 0xbbaa
    @param val input
    @return flipped result
*/
uint16_t _flip_u16(const uint16_t val);

/** flip bytes in a four byte DWORD
    0xaabbccdd -> 0xddccbbaa
    @param val input
    @return flipped result
*/
uint32_t _flip_u32(const uint32_t val);

#ifdef __cplusplus
}
#endif

#endif
///\}
