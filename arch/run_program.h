#include "isa.h"
#include "arith_logic.h"
#include "data_transfer.h"
#include "bit_instr.h"
#include <vector>
// using namespace std;

float to_fixed_point(int8_t num) {
    float prod = 8;
    float ans = 0;
    for(int i = 7; i >= 0; --i, prod *= 0.5) {
        ans += prod * ((num >> i) & 1);
    }
    return ans;
}

void run_program(state_t &state, const std::vector<instr_t> &program) {
    for(auto &instr: program) {
        if(instr.opcode <= 32)
            run_arith_logic(state, instr);
        else if(100 <= instr.opcode and instr.opcode <= 110)
            run_dt_instruction(state, instr);
        else if(120 <= instr.opcode and instr.opcode <= 126) {
            run_bits(state, instr);
        }
        // for(int i = 0; i < 5; ++i) {
        //     printf("%d: %f\n", i, to_fixed_point(state.reg[i]));
        // }
        // printf("----\n");
    }
}


// Depreciated in favour of the .asm format
// find examples of this in parser/
void parser(const char filename[], vector<instr_t> &program) {
    FILE *fin;
    fin = fopen(filename, "r");
    char opcode[16];
    int a, b;
    while(fscanf(fin, "%s\n", opcode) != EOF) {
        printf("%s ", opcode);
        fscanf(fin, "%d, %d\n", &a, &b);
        printf("%d, %d\n", a, b);
        if(strcmp(opcode, "MOV") == 0) {
            instr_t mov_instr;
            program.push_back(mov_instr);
            program.back().opcode = MOV;
            program.back().reg_d = a;
            program.back().reg_r = b;
        }
        else if(strcmp(opcode, "LDI") == 0) {
            instr_t ldi_instr;
            program.push_back(ldi_instr);
            program.back().opcode = LDI;
            program.back().reg_d = a;
            program.back().K = b;
        }
        else if(strcmp(opcode, "ADD") == 0) {
            instr_t add_instr;
            program.push_back(add_instr);
            program.back().opcode = ADD;
            program.back().reg_d = a;
            program.back().reg_r = b;
        }
        else if(strcmp(opcode, "SUB") == 0) {
            instr_t add_instr;
            program.push_back(add_instr);
            program.back().opcode = SUB;
            program.back().reg_d = a;
            program.back().reg_r = b;
        }
    }
    fclose(fin);
}

// this runs the code from a .asm file
// that is generated after compiling a .pk file
// by the parser
void read_program_from_asm(char* filename, vector<instr_t> &program) {
    FILE *fin;
    char opcode[8];
    int rr, rd;
    fin = fopen(filename, "r");
    while(fscanf(fin, "%s rd:%d rr:%d\n", opcode, &rd, &rr) != EOF) {
        printf("%s, %d, %d\n", opcode, rd, rr); // for DEBUG
        instr_t instr;
        instr.reg_d = rd;
        instr.reg_r = rr;
        if(strcmp(opcode, "ADD") == 0) {
            instr.opcode = ADD;
        } else if(strcmp(opcode, "SUB") == 0) {
            instr.opcode = SUB;
        } else if(strcmp(opcode, "MOV") == 0) {
            instr.opcode = MOV;
        } else if(strcmp(opcode, "ADC") == 0) {
            instr.opcode = ADC;
        } else {
            assert(false);
        }
        program.push_back(instr);
    }
    fclose(fin);
}

//
//
// int main() {
//     state_t state;
//     memset(state.mem, 0, sizeof(state.mem));
//     memset(state.reg, 0, sizeof(state.reg));
//     vector<instr_t> program;
//     parser("sample_program.txt", program);
//     cout << program.size() << "\n";
//     run_program(state, program);
//     for(int i = 0; i < 5; ++i) {
//         printf("%d: %f\n", i, to_fixed_point(state.reg[i]));
//         // print_bitform(state.reg[i]);
//     }
//     return 0;
// }
