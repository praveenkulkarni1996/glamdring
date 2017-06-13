class ProgramLine(object):
    def __init__(self, line):
        self.line = line
        self.op = None
        self.a = None
        self.b = None
        self.c = None

    def parse(self):
        self.c, _, self.a, self.op, self.b = self.line.split(' ')

    def __repr__(self):
        return {
            'op': self.op,
            'a': self.a,
            'b': self.b,
            'c': self.c,
            'line': self.line
        }.__repr__()


class Program(object):
    def __init__(self, program):
        self.program = program


class Instruction(object):
    def __init__(self, opcode, rr, rd):
        self.opcode = opcode
        self.rr = rr
        self.rd = rd

    def __repr__(self):
        return('{}\trd:{}\trr:{}'.format(self.opcode, self.rd, self.rr))
