all:
	g++ -O3 -D REGISTER_LIMIT=2 -D NUM_RESTARTS=50 -march=native -std=c++14 stoke.cpp -o stoke

run: all
	./stoke ../testcase/exp_testcase.txt

ml7: all
	./stoke ../testcase/ml7_testcase.txt

inc: all
	./stoke ../testcase/inc_testcase.txt

dec: all
	./stoke ../testcase/dec_testcase.txt

two: all
	./stoke ../testcase/two_testcase.txt

sum: all
	./stoke ../testcase/sum_testcase.txt

oth: all
	./stoke ../testcase/oth_testcase.txt

clean:
	rm stoke

p1: all
	./stoke ../hackers-delight/p1.txt

p2: all
	./stoke ../hackers-delight/p2.txt

p3: all
	./stoke ../hackers-delight/p3.txt

p4: all
	./stoke ../hackers-delight/p4.txt

p5: all
	./stoke ../hackers-delight/p5.txt

p6: all
	./stoke ../hackers-delight/p6.txt

p7: all
	./stoke ../hackers-delight/p7.txt

p8: all
	./stoke ../hackers-delight/p8.txt

p9: all
	./stoke ../hackers-delight/p9.txt

p14: all
	./stoke ../hackers-delight/p14_tail.txt

p15: all
	./stoke ../hackers-delight/p15_tail.txt

p17: all
	./stoke ../hackers-delight/p17.txt

