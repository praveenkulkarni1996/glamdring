x_r = range(-128, 127)


def ff(x):
    return x*x - x - 2


def g(x):
    return x*x


def test(x):
    return x + x


def mul(x):
    return x * x


def add_one_sqr(x):
    return (x + 1) ** 2


def add_two_mul_add_one(x):
    return (x + 2) * (x + 1)

def test2(x):
    return 2*x*x + 2*x + 2


def test3(x):
    return (x*x) + (3*x) + 5


def test4(x):
    return x**3 + x**2 + x + 1


def test5(x):
    return x**3 + 3*(x**2) + x + 1


def test6(x):
    return x*2 + 6*x + 6

f = test6

for x in x_r:
    if f(x) in x_r:
        print x, f(x)



