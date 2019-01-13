#ifndef __ads1110_h_
#define __ads1110_h_

// you must define the slave address.
// it can be determined based on the external marking
#define ED0     0x48
#define ED1     0x49
#define ED2     0x4a
#define ED3     0x4b
#define ED4     0x4c
#define ED5     0x4d
#define ED6     0x4e
#define ED7     0x4f

// configuration register bits
#define PGA_1   0x0
#define PGA_2   0x1
#define PGA_4   0x2
#define PGA_8   0x3
#define BITS_12 0x0
#define BITS_14 0x4
#define BITS_15 0x8
#define BITS_16 0xc
#define SC      0x10
#define ST      0x80

// if the chip only does a single conversion, the state machine can reach these
#define STATE_STANDBY 0x0
#define STATE_CONVERT 0x1

struct ads1110 {
    int16_t conv_raw;   // adc raw output
    int16_t conv;       // int voltage value *10^4
    uint8_t config;     // adc configuration register
    uint8_t state;      // state machine parameter
};

struct ads1110 eadc;

uint8_t ads1110_read(const uint8_t slave_addr, struct ads1110 *adc);
void ads1110_convert(struct ads1110 *adc);
uint8_t ads1110_config(const uint8_t slave_addr, const uint8_t val);
uint8_t ads1110_init(const uint8_t slave_address, const uint8_t config_reg);


#endif
