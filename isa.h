#include <iostream>
#include <vector>

#define REGISTERS 32
#define MEMORY 2048


#define CARRY_FLAG 1
#define ZERO_FLAG 1
// TODO: define appropriate


struct state_t {
    int16_t reg[REGISTERS];
    bool flags[8];
    // todo add pointers
};

struct instr_t {
    int opcode;
    int8_t cost;
    int8_t reg_d, reg_r;
    int mode;
    int imm, K;
};
