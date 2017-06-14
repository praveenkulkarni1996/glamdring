// Copyright 2017, Praveen Kulkarni,
// written on 14 June

#include "../arch/run_program.h"

int main(int argc, char* argv[]) {
    state_t state;
    std::vector<instr_t> program;
    // read the program from .asm
    read_program_from_asm(argv[1], program);
    // and run it
    run_program(state, program);
}

