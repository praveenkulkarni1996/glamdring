#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "../run_program.h"

typedef pair<int8_t, int8_t> testpoint;
double beta = 2.0;

const int REGISTER_LIMIT = 2;
const int NUM_RESTARTS = 10;

int8_t popcount(uint8_t num) {
    int8_t ans = 0;
    int8_t penalty = 1;
    while(num != 0) {
        ans += (penalty) * (num & 1);
        num >>= 1;
        penalty++;
    }
    return ans;
}

void read_testcase(const char filename[], vector<testpoint> &testcase) {
    ifstream fin(filename);
    int x, y;
    while(fin >> x >> y) {
        testcase.push_back({(int8_t)x, (int8_t)y});
    }
    fin.close();
}


void clear_state(state_t &state) {
    memset(state.reg, 0, sizeof(state.reg));
    return;
    memset(state.mem, 0, sizeof(state.mem));
    memset(state.flags, 0, sizeof(state.flags));
}

int total_reg_error(const vector<testpoint> &testcase, 
        const vector<instr_t> &program) {
    state_t state;
    int error = 0;
    for(auto &tp: testcase) {
        clear_state(state);
        state.reg[0] = tp.first;
        run_program(state, program);
        error += popcount((uint8_t)(state.reg[0] ^ tp.second));
    }
    return error;
}

inline void change_opcode_2_registers(instr_t &instr) {
    const int opcodes[] = {MOV, ADD, SUB/*, ADC, SBC, AND, OR, EOR*/};
    const int num_opcodes = sizeof(opcodes) / sizeof(int);
    // cout << "(" << instr.opcode;
    instr.opcode = opcodes[rand() % num_opcodes];
    // cout << ", " << instr.opcode << ")\n";
}

inline void change_opcode(instr_t &instr) {
    switch(instr.opcode) {
        case MOV:
        case ADD:
        case SUB:
        case ADC:
        case SBC:
        case AND:
        case OR:
        case EOR:
            change_opcode_2_registers(instr);
            break;
    }
}

inline void change_dest_register(instr_t &instr) {
    switch(instr.opcode) {
        case MOV:
        case ADD:
        case ADC:
        case SUB:
        case SBC:
        case AND:
        case OR:
        case EOR:
            // source register can be r0 .. r3
            instr.reg_d = rand () % REGISTER_LIMIT;
    }
}

inline void change_src_register(instr_t &instr) {
    switch(instr.opcode) {
        case MOV:
        case ADD:
        case SUB:
        case ADC:
        case SBC:
        case AND:
        case OR:
        case EOR:
            // source register can be r0 .. r3
            instr.reg_r = rand () % REGISTER_LIMIT;
    }
}


void mcmc_search(const vector<testpoint> &testcase, 
        vector<instr_t> &program) {

    int prevcost = total_reg_error(testcase, program);
    for(int i = 0; i < 10000; ++i) {
        int prog_index = rand() % program.size();
        // printf("pi = %d:\t", prog_index);
        instr_t previnstr = program[prog_index];

        int decider = rand () % 100;
        if(decider < 90) {
            // printf("op:");
            change_opcode(program[prog_index]);
        }
        else if(decider < 95) {
            // printf("dr:");
            change_dest_register(program[prog_index]);
        }
        else {
            // printf("sr:");
            change_src_register(program[prog_index]);
        }

        int newcost = total_reg_error(testcase, program);

        // printf("\t%d ---> %d\n", prevcost, newcost);
        float accept = min(1.0, exp(-beta * newcost / prevcost));
        // printf("acc = %lf\t", accept);

        if(rand() % 100000 < accept * 100000) {
            prevcost = newcost;
            //printf("ACCEPTED\n");
        }
        else {
            program[prog_index] = previnstr;
            // printf("REJECTED\n");
        }
    }
}

void print_instr(const instr_t &instr) {
    switch(instr.opcode) {
        case ADD:
            printf("ADD %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case SUB:
            printf("SUB %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case MOV:
            printf("MOV %d %d\n", instr.reg_r, instr.reg_d);
            break;
        default:
            printf("WTF!!!");

    }
}


void test_subtraction() {
    state_t state;
    clear_state(state);
    state.reg[0] = 32;
    state.reg[1] = 64;


    instr_t instr;
    instr.opcode = SUB;
    instr.reg_d = 0;
    instr.reg_r = 1;

    print_bitform(state.reg[0]);
    print_bitform(state.reg[1]);
    run_arith_logic(state, instr);
    print_bitform(state.reg[0]);
    print_bitform(state.reg[1]);

}

int main() {
    srand(time(NULL));
    vector<testpoint> testcase;
    vector<instr_t> program(10);
    read_testcase("../testcase/ml7_testcase.txt", testcase);

    int opcodes [] = {ADD, SUB, MOV};
    int num_opcodes = sizeof(opcodes) / sizeof(int);
    for(int restarts = 0; restarts < NUM_RESTARTS; ++restarts) {
        for(auto &instr: program) {
            instr.opcode = opcodes[rand() % num_opcodes];
            instr.reg_d = rand() % REGISTER_LIMIT;
            instr.reg_r = rand() % REGISTER_LIMIT;
        }
        cout << "first = " << total_reg_error(testcase, program) << "\n";
        mcmc_search(testcase, program);
        cout << "final = " << total_reg_error(testcase, program) << "\n";
        for(auto &instr: program) {
            print_instr(instr);
        }
    }
}
