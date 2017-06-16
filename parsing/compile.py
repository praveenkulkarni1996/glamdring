import json
import pprint
from classes import ProgramLine, Instruction


def reading_config_file(filename):
    with open(filename) as configfile:
        config_data = json.load(configfile)
    pprint.pprint(config_data)
    return config_data


def reading_program_file(filename):
    programs = []
    with open(filename) as programfile:
        for line in programfile:
            programs.append(line.strip())
    return programs


def print_program(assembly_code, asm_outputfile):
    with open(asm_outputfile, 'w') as asm_out:
        for assembly_instr in assembly_code:
            print(assembly_instr, file=asm_out)


def build_8bit_mapping(config):

    # depending upon the architechture more temp variables will be needed
    # 8 bit needs 1 temp variable
    # 16 bit needs 2 temp variables
    # 32 bit needs 3 temp variables
    temp_vars_by_arch = {
        8: {'__temp__'},
        16: {'__temp0__', '__temp1__'},
        32: {'__temp0__', '__temp1__', '__temp2__', '__temp3__'}
    }

    input_vars = set(config['input'])
    output_vars = set(config['output'])
    temp_vars = temp_vars_by_arch[config['arch']]

    vars_set = input_vars | output_vars | temp_vars
    mapping = dict(zip(list(vars_set), range(len(vars_set))))
    return mapping


def compile_line_8(programline, mapping):
    assembly = []
    temp_register = mapping['__temp__']
    operation_to_opcode = {'+': 'ADD', '-': 'SUB'}
    # c = a <op> b
    # first move the temp register
    assembly.append(Instruction(opcode='MOV',
                                rd=temp_register,
                                rr=mapping[programline.a]))
    # apply the operation to the temp register
    assembly.append(Instruction(opcode=operation_to_opcode[programline.op],
                                rd=temp_register,
                                rr=mapping[programline.b]))
    # move the result to the destination register
    assembly.append(Instruction(opcode='MOV',
                                rd=mapping[programline.c],
                                rr=temp_register))
    return assembly


def compile_prog_8(program, config):
    assert config['arch'] == 8
    assembly_code = []
    mapping = build_8bit_mapping(config)
    for line in program:
        programline = ProgramLine(line)
        programline.parse()
        assembly_code += compile_line_8(programline, mapping)
    return assembly_code


def build_16bit_mapping(config):
    input_vars = set(config['input'])
    output_vars = set(config['output'])
    temp_vars = set({'__temp__'})

    variables = list(input_vars | output_vars | temp_vars)

    mapping = {'low': {}, 'high': {}}
    for index, var in enumerate(variables):
        mapping['low'][var] = index
        mapping['high'][var] = index + len(variables)

    return mapping


def compile_line_16(programline, mapping):
    # assembly = []
    # vars
    low, high = mapping['low'], mapping['high']
    temp_low, temp_high = low['__temp__'], high['__temp__']

    if programline.op == '+':
        # move the low bits into the assembly register
        return [
            Instruction(opcode='MOV', rd=temp_low, rr=low[programline.a]),
            Instruction(opcode='ADD', rd=temp_low, rr=low[programline.b]),
            Instruction(opcode='MOV', rd=low[programline.c], rr=temp_low),
            # carry is currently in the CARRY flag,
            # MOV does not affect the flags
            Instruction(opcode='MOV', rd=temp_high, rr=high[programline.a]),
            Instruction(opcode='ADC', rd=temp_high, rr=high[programline.b]),
            Instruction(opcode='MOV', rd=high[programline.c], rr=temp_high)
        ]
    else:
        raise NotImplementedError
    return [None]


def compile_prog_16(program, config):
    assert config['arch'] == 16
    mapping = build_16bit_mapping(config)
    assembly_code = []
    for line in program:
        programline = ProgramLine(line)
        programline.parse()
        assembly_code += compile_line_16(programline, mapping)
    pprint.pprint(mapping)
    pprint.pprint(assembly_code)
    return assembly_code


def main():
    config = reading_config_file('config.json')
    program = reading_program_file('adding.pk')

    # read architecture from the configfile

    # compile the code
    if config['arch'] == 8:
        assembly_code = compile_prog_8(program, config)
        # TODO: move this line outside if
    elif config['arch'] == 16:
        assembly_code = compile_prog_16(program, config)
    else:
        raise NotImplementedError
    print_program(assembly_code, 'adding.asm')


if __name__ == '__main__':
    main()
