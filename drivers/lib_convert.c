
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "lib_convert.h"

// ###############################################
// #
// #  string functions
// #

uint8_t str_to_uint8(char *str, uint8_t * out, const uint8_t seek,
                     const uint8_t len, const uint8_t min, const uint8_t max)
{
    uint32_t val = 0, pow = 1;
    uint8_t i;

    // pow() is missing in msp gcc, so we improvise
    for (i = 0; i < len - 1; i++) {
        pow *= 10;
    }
    for (i = 0; i < len; i++) {
        if ((str[seek + i] > 47) && (str[seek + i] < 58)) {
            val += (str[seek + i] - 48) * pow;
        }
        pow /= 10;
    }
    if ((val >= min) && (val <= max)) {
        *out = val;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

uint8_t str_to_uint16(char *str, uint16_t * out, const uint8_t seek,
                      const uint8_t len, const uint16_t min, const uint16_t max)
{
    uint16_t val = 0;
    uint32_t pow = 1;
    uint8_t i, c;

    for (i = len + 1; i > seek; i--) {
        c = str[i - 1] - 48;
        if (c < 10) {
            val += c * pow;
            pow *= 10;
        } else {
            if (val) {
                // if we already have a number and this char is unexpected
                break;
            }
        }
    }

    if ((val >= min) && (val <= max)) {
        *out = val;
    } else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

uint8_t str_to_uint32(char *str, uint32_t * out, const uint8_t seek,
                      const uint8_t len, const uint32_t min, const uint32_t max)
{
    uint32_t val = 0, pow = 1;
    uint8_t i;

    // pow() is missing in msp gcc, so we improvise
    for (i = 0; i < len - 1; i++) {
        pow *= 10;
    }
    for (i = 0; i < len; i++) {
        if ((str[seek + i] > 47) && (str[seek + i] < 58)) {
            val += (str[seek + i] - 48) * pow;
        } else {
            val /= 10;
        }
        pow /= 10;
    }
    if ((val >= min) && (val <= max)) {
        *out = val;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

uint8_t str_to_int32(char *str, int32_t * out, const uint8_t seek,
                     const uint8_t len, const int32_t min, const int32_t max)
{
    int32_t val = 0;
    uint32_t pow = 1;
    uint8_t i;
    int8_t sign = 1;

    // pow() is missing in msp gcc, so we improvise
    for (i = 0; i < len - 1; i++) {
        pow *= 10;
    }
    for (i = 0; i < len; i++) {
        if ((str[seek + i] > 47) && (str[seek + i] < 58)) {
            val += (str[seek + i] - 48) * pow;
        } else if (str[seek + i] == 45) {
            sign = -1;
        } else {
            val /= 10;
        }
        pow /= 10;
    }
    if ((val >= min) && (val <= max)) {
        *out = val * sign;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

uint8_t hstr_to_uint8(char *str, uint8_t * out, const uint8_t seek,
                      const uint8_t len, const uint8_t min, const uint16_t max)
{
    uint8_t val = 0;
    uint32_t pow = 1;
    uint8_t i;

    // pow() is missing in msp gcc, so we improvise
    for (i = seek; i < len; i++) {
        pow *= 16;
    }

    for (i = seek; i < len + 1; i++) {
        if ((str[i] > 47) && (str[i] < 58)) {
            // 0 - 9
            val += (str[i] - 48) * pow;
        } else if ((str[i] > 96) && (str[i] < 103)) {
            // a - f
            val += (str[i] - 87) * pow;
        } else if ((str[i] > 64) && (str[i] < 71)) {
            // A - F
            val += (str[i] - 55) * pow;
        }
        pow /= 16;
    }

    if ((val >= min) && (val <= max)) {
        *out = val;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

uint8_t hstr_to_uint16(char *str, uint16_t * out, const uint8_t seek,
                       const uint8_t len, const uint16_t min, const uint16_t max)
{
    uint16_t val = 0;
    uint32_t pow = 1;
    uint8_t i;

    // pow() is missing in msp gcc, so we improvise
    for (i = seek; i < len; i++) {
        pow *= 16;
    }

    for (i = seek; i < len + 1; i++) {
        if ((str[i] > 47) && (str[i] < 58)) {
            // 0 - 9
            val += (str[i] - 48) * pow;
        } else if ((str[i] > 96) && (str[i] < 103)) {
            // a - f
            val += (str[i] - 87) * pow;
        } else if ((str[i] > 64) && (str[i] < 71)) {
            // A - F
            val += (str[i] - 55) * pow;
        }
        pow /= 16;
    }

    if ((val >= min) && (val <= max)) {
        *out = val;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

static uint16_t const hex_ascii[16] =
    { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65,
    0x66
};

char *_utoh(char *buf, const uint32_t val)
{
    char *p = buf + (CONV_BASE_16_BUF_SZ - 1);   // the very end of the buffer
    uint32_t m = val;
    uint8_t i = 0;

    *p = '\0';

    if (val == 0) {
        p -= 1;
        memcpy(p, &hex_ascii[0], sizeof(uint8_t));
    }
    // groups of 8 bits
    while (m > 0 || (i & 1)) {
        p -= 1;
        memcpy(p, &hex_ascii[m & 0xf], sizeof(uint8_t));
        m >>= 4;
        i++;
    }

    p -= 2;
    memcpy(p, "0x", sizeof(uint16_t));

    return p;
}

char *_utoh8(char *buf, const uint32_t val)
{
    char *p = buf + (CONV_BASE_16_BUF_SZ - 1);   // the very end of the buffer
    uint32_t m = val;
    uint8_t i = 0;

    *p = '\0';

    if (val == 0) {
        p -= 2;
        memcpy(p, &hex_ascii[0], sizeof(uint8_t));
        memcpy(p + 1, &hex_ascii[0], sizeof(uint8_t));
    }
    // groups of 8 bits
    while (m > 0 || (i & 1)) {
        p -= 1;
        memcpy(p, &hex_ascii[m & 0xf], sizeof(uint8_t));
        m >>= 4;
        i++;
    }

    return p;
}

char *_utoh16(char *buf, const uint32_t val)
{
    char *p = buf + (CONV_BASE_16_BUF_SZ - 1);   // the very end of the buffer
    uint32_t m = val;
    uint8_t i = 0;

    *p = '\0';

    if (val == 0) {
        p -= 4;
        memcpy(p, &hex_ascii[0], sizeof(uint8_t));
        memcpy(p + 1, &hex_ascii[0], sizeof(uint8_t));
        memcpy(p + 2, &hex_ascii[0], sizeof(uint8_t));
        memcpy(p + 3, &hex_ascii[0], sizeof(uint8_t));
        return p;
    }
    // groups of 8 bits
    while (m > 0 || (i & 1)) {
        p -= 1;
        memcpy(p, &hex_ascii[m & 0xf], sizeof(uint8_t));
        m >>= 4;
        i++;
    }

    if (val < 0x100) {
        p -= 2;
        memcpy(p, &hex_ascii[0], sizeof(uint8_t));
        memcpy(p + 1, &hex_ascii[0], sizeof(uint8_t));
    }

    return p;
}

char *_utoh32(char *buf, const uint32_t val)
{
    char *p = buf + (CONV_BASE_16_BUF_SZ - 1);     // the very end of the buffer
    uint32_t m = val;
    uint8_t i = 0;

    *p = '\0';

    p -= 8;
    for (i=0;i<8;i++) {
        *(p + i) = 0x30;
    }

    p += 8;
    // groups of 8 bits
    while (m > 0 || (i & 1)) {
        p -= 1;
        memcpy(p, &hex_ascii[m & 0xf], sizeof(uint8_t));
        m >>= 4;
        i++;
    }

    return buf + 3;
}

static uint16_t const caps_hex_ascii[16] =
    { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x46
};

char *_utorh(char *buf, const uint32_t val, const uint8_t pad_size)
{
    char *p = &buf[11];
    uint32_t m = val;
    uint8_t i = 0;
    uint8_t pc = 0;

    *p = '\0';

    if (val == 0) {
        for (pc = 0; pc < pad_size; pc++) {
            memcpy(p - (pc + 1), &caps_hex_ascii[0], sizeof(uint8_t));
        }
        p -= pad_size;
        return p;
    }
    // groups of 8 bits
    //while (m > 0 || (i & 1))
    while (m > 0 || (i < pad_size)) {
        p -= 1;
        memcpy(p, &caps_hex_ascii[m & 0xf], sizeof(uint8_t));
        m >>= 4;
        i++;
    }

    return p;
}

#ifdef USE_ITOA_LUT

static uint16_t const dec_ascii[10] =
    { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };
static uint16_t const bin_ascii[2] = { 0x30, 0x31 };

char *_uint32toa(char *buf, const uint32_t val)
{
    char *p = buf + (CONV_BASE_10_BUF_SZ - 1);  // the very end of the buffer
    uint32_t m = val;

    *p = '\0';

    while (m >= 10) {
        uint32_t const old = m;

        p -= 1;
        m /= 10;
        memcpy(p, &dec_ascii[old - (m * 10)], sizeof(uint8_t));
    }

    p -= 1;
    memcpy(p, &dec_ascii[m], sizeof(uint8_t));

    return p;
}

char *_uint16toa(char *buf, const uint16_t val)
{
    char *p = buf + (CONV_BASE_10_BUF_SZ - 1);  // the very end of the buffer
    uint16_t m = val;

    *p = '\0';

    while (m >= 10) {
        uint32_t const old = m;

        p -= 1;
        m /= 10;
        memcpy(p, &dec_ascii[old - (m * 10)], sizeof(uint8_t));
    }

    p -= 1;
    memcpy(p, &dec_ascii[m], sizeof(uint8_t));

    return p;
}

char *_utob(char *buf, const uint16_t val)
{
    char *p = buf + (CONV_BASE_2_BUF_SZ - 1);   // the very end of the buffer
    uint16_t m = val;
    uint8_t i = 0;

    *p = '\0';

    if (val == 0) {
        p -= 1;
        memcpy(p, &bin_ascii[0], sizeof(uint8_t));
    }
    // groups of 8bits
    while (m > 0 || (i & 7)) {
        if (m > 0 && !(i & 7)) {
            p -= 1;
            *p = ' ';
        }
        p -= 1;
        memcpy(p, &bin_ascii[m & 0x1], sizeof(uint8_t));
        m >>= 1;
        i++;
    }

    return p;
}
#else

char *_uint32toa(char *buf, const uint32_t val)
{
    char *p = buf + (CONV_BASE_10_BUF_SZ - 1);  // the very end of the buffer
    uint32_t m = val;

    *p = '\0';

    if (val == 0) {
        p -= 1;
        *p = '0';
    }

    while (m > 0) {
        p -= 1;
        *p = (m % 10) + '0';
        m /= 10;
    }

    return p;
}

char *_uint16toa(char *buf, const uint16_t val)
{
    char *p = buf + (CONV_BASE_10_BUF_SZ - 1);  // the very end of the buffer
    uint32_t m = val;

    *p = '\0';

    if (val == 0) {
        p -= 1;
        *p = '0';
    }

    while (m > 0) {
        p -= 1;
        *p = (m % 10) + '0';
        m /= 10;
    }

    return p;
}

char *_utob(char *buf, const uint16_t val)
{
    char *p = buf + (CONV_BASE_2_BUF_SZ - 1);   // the very end of the buffer
    uint16_t m = val;
    uint8_t i = 0;

    *p = '\0';

    if (val == 0) {
        p -= 1;
        *p = '0';
    }
    // groups of 8bits
    while (m > 0 || (i & 7)) {
        if (m > 0 && !(i & 7)) {
            p -= 1;
            *p = ' ';
        }
        p -= 1;
        *p = (m & 0x1) + '0';
        m >>= 1;
        i++;
    }

    return p;
}

#endif

char *_utoa(char *buf, const uint32_t val)
{
    return _uint32toa(buf, val);
}

char *_itoa(char *buf, const int32_t val)
{
    char *p;
    if (val >= 0) {
        return _uint32toa(buf, val);
    } else {
        p = _uint32toa(buf, val * -1);
        *(p - 1) = '-';
        return p - 1;
    }
}

char *_i16toa(char *buf, const int16_t val)
{
    char *p;
    if (val >= 0) {
        return _uint16toa(buf, val);
    } else {
        p = _uint16toa(buf, val * -1);
        *(p - 1) = '-';
        return p - 1;
    }
}

char *prepend_padding(char *buf, char *converted_buf, const pad_type padding_type,
                      const uint8_t target_len)
{
    uint8_t conv_len;
    uint8_t buf_pos;
    uint8_t cnt;
    uint8_t padding_char = '0';

    conv_len = strlen(converted_buf);
    buf_pos = converted_buf - buf;

    // if not enough buffer space is available for the prepend
    if (buf_pos < target_len - conv_len) {
        return converted_buf;
    }
    // if the converted string is already longer than the target length
    if (target_len <= conv_len) {
        return converted_buf;
    }

    if (padding_type == PAD_SPACES) {
        padding_char = ' ';
    }

    for (cnt = 0; cnt < target_len - conv_len; cnt++) {
        *(buf + buf_pos - cnt - 1) = padding_char;
    }

    return buf + buf_pos - cnt;
}

void mem2ascii(uint8_t * data_in, uint8_t * data_out, uint8_t len)
{
    uint8_t byte;
    while (len) {
        byte = *data_in;
        byte >>= 4;
        byte &= 0x0f;
        if (byte > (uint8_t) 9) {
            byte += 0x37;
        } else {
            byte += 0x30;
        }
        *(data_out++) = byte;
        byte = *(data_in++);
        byte &= 0x0f;
        if (byte > (uint8_t) 9) {
            byte += 0x37;
        } else {
            byte += 0x30;
        }
        *(data_out++) = byte;
        len--;
    }
}

uint8_t dec_to_bcd(const uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

uint8_t bcd_to_dec(const uint8_t val)
{
    return ((val & 0xf0) >> 4) * 10 + (val & 0x0f);
}

uint16_t _flip_u16(const uint16_t val)
{
    uint16_t tmp;
    tmp = val << 8;

    return ((val >> 8) | tmp);
}

uint32_t _flip_u32(const uint32_t val)
{
    uint32_t a, b, c, d;

    a = (val & 0xff000000) >> 24;
    b = (val & 0x00ff0000) >> 8;
    c = (val & 0x0000ff00) << 8;
    d = (val & 0x000000ff) << 24;

    return (a | b | c | d);
}

uint32_t _wiretou32(char *buf, const uint16_t seek, const uint16_t len)
{
    uint32_t val = 0, c = 0;    //, pow = 1;
    uint8_t i, buf0;
    //char itoa_buf[18];

    buf0 = buf[seek];

    if ((buf0 < 0x30) || (buf0 > 0x46) || (buf0 == 0x40)) {
        // binary input
        for (i = 0; i < len; i++) {
            c = buf[seek + len - i - 1];
            val |= c << (i * 8);
        }
        return val;
    }
    // ascii(hex) input
    for (i = 0; i < len / 2; i++) {
        c = buf[seek + i * 2];
        if (c < 0x40) {
            c -= 48;
        } else {
            c -= 55;
        }
        val |= c << (i * 8 + 4);
        c = buf[seek + i * 2 + 1];
        if (c < 0x40) {
            c -= 48;
        } else {
            c -= 55;
        }
        val |= c << (i * 8);
    }

/*
    uart0_print(" val ");
    uart0_print(_utoh(&itoa_buf[0], val));
    uart0_print("\r\n");
*/
    return val;
}
