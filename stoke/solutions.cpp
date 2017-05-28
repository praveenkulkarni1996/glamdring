
int
testcase_two(testcase_t testcase) {
    vector<instr_t> program (1);
    for(instr_t &instr: program) {
      instr.opcode = LSL;
      instr.reg_d = 0;
    }
    int error = total_reg_error(testcase, program);
    printf("error = %d\n", error);
    return 0;
}

int
testcase_p1(testcase_t testcase) {
    vector<instr_t> program (3);
    /* MOV R0 to R1 */
    /* DEC R1 */
    /* AND R1, R0 */
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].reg_d = 1;
    program[1].opcode = DEC;

    program[2].opcode = AND;
    program[2].reg_r = 1;
    program[2].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p1: error = %d\n", error);
    return error;
}

int
testcase_p2(testcase_t testcase) {
    // is power of 2
    vector<instr_t> program (3);
    /* MOV R0 to R1 */
    /* INC R1 */
    /* AND R1, R0 */
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].reg_d = 1;
    program[1].opcode = INC;

    program[2].opcode = AND;
    program[2].reg_r = 1;
    program[2].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p2: error = %d\n", error);
    return error;
}

int
testcase_p3(testcase_t testcase) {
    vector<instr_t> program (2);
    /* SUB R0 from R1 */
    /* AND R1, R0 */
    program[0].opcode = SUB;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].opcode = AND;
    program[1].reg_r = 1;
    program[1].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p3: error = %d\n", error);
    return error;
}

int
testcase_p4(testcase_t testcase) {
    // is power of 2
    vector<instr_t> program (3);
    /* MOV R0 to R1 */
    /* DEC R1 */
    /* AND R1, R0 */
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].reg_d = 1;
    program[1].opcode = DEC;

    program[2].opcode = EOR;
    program[2].reg_r = 1;
    program[2].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p4: error = %d\n", error);
    return error;
}

int
testcase_p17(testcase_t testcase) {
    vector<instr_t> program (5);
    // o1 = x - 1
    // o2 = o1 | x
    // o3 = o2 + 1
    // res = o3 & x
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].opcode = DEC;
    program[1].reg_d = 1;

    program[2].opcode = OR;
    program[2].reg_r = 0;
    program[2].reg_d = 1;

    program[3].opcode = INC;
    program[3].reg_d = 1;

    program[4].opcode = AND;
    program[4].reg_r = 1;
    program[4].reg_d = 0;
    int error = total_reg_error(testcase, program);
    printf("p17: error = %d\n", error);
    return error;
}
