#include <iostream>
#include <fstream>
#include "../run_program.h"

typedef pair<int8_t, int8_t> testpoint;

int8_t popcount(uint8_t num) {
    int8_t ans = 0;
    while(num != 0) {
        ans += (num & 1);
        num >>= 1;
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

int main() {
    vector<testpoint> testcase;
    vector<instr_t> program (1);

    program[0].opcode = ADD;
    program[0].reg_d = 0;
    program[0].reg_r = 0;

    /*
    program[1].opcode = ADD;
    program[1].reg_r = 1;
    program[1].reg_d = 0;
    */

    read_testcase("../testcase/exp_testcase.txt", testcase);
    std::cout << total_reg_error(testcase, program) << "\n";
}
