#ifndef __IR_COMMON_H__
#define __IR_COMMON_H__

struct ir_tome {
    uint16_t id;           // internal identifier for the current command
    uint16_t delta_size;   // count of elements in *delta
    uint16_t *delta;       // time intervals for each level in one command
    struct ir_tome * next; // next command in the tome
};

#endif
