all:
	g++ -O3 -std=c++14 error.cpp -o error

run: all
	./error ../testcase/exp_testcase.txt

ml7: all
	./error ../testcase/ml7_testcase.txt

inc: all
	./error ../testcase/inc_testcase.txt

dec: all
	./error ../testcase/dec_testcase.txt

clean:
	rm error
