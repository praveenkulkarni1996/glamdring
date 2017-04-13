// #include "isa.h"
#include "arith_logic.h"
#include "data_transfer.h"
#include <vector>

void run_program(state_t &state, std::vector<instr_t> program) {
    for(auto &instr: program) {
        if(instr.opcode <= 32)
            run_arith_logic(state, instr);
        else if(instr.opcode >= 100)
            run_dt_instruction(state, instr);
    }
}


int main() {
    vector<instr_t> instr (2);

    instr[0].opcode = LDI;
    instr[0].K = 32;
    instr[0].reg_d = 16;

    instr[1].opcode = MOV;
    instr[1].reg_r = 16;
    instr[1].reg_d = 0;

    state_t state;


    print_bitform(state.reg[instr[1].reg_d]);
    run_program(state, instr);
    print_bitform(state.reg[instr[1].reg_d]);

}
