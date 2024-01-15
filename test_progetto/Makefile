.PHONY: clean all

all: floor rand fibonacci sqrt eqn2 inssort inssort2 sqrt2

floor: callfloor.o floor.o
	g++ -o floor callfloor.o floor.o

callfloor.o: callfloor.cpp
	g++ -c callfloor.cpp

floor.o: floor.k
	../kcomp floor.k 2> floor.ll
	./tobinary floor.ll
	
rand: callrand.o floor.o rand.o
	g++ -o rand callrand.o floor.o rand.o

callrand.o: callrand.cpp
	g++ -c callrand.cpp

rand.o:	rand.k
	../kcomp rand.k 2> rand.ll
	./tobinary rand.ll

fibonacci: fibonacciIt.o callfibo.o
	g++ -o fibonacci callfibo.o fibonacciIt.o

callfibo.o:	callfibo.cpp
	g++ -c callfibo.cpp
	
fibonacciIt.o:	fibonacciIt.k
	../kcomp fibonacciIt.k 2> fibonacciIt.ll
	./tobinary fibonacciIt.ll
	
sqrt: callsqrt.o sqrt.o
	g++ -o sqrt callsqrt.o sqrt.o

callsqrt.o: callsqrt.cpp
	g++ -c callsqrt.cpp

sqrt.o:	sqrt.k
	../kcomp sqrt.k 2> sqrt.ll
	./tobinary sqrt.ll
	
eqn2: calleqn2.o sqrt.o eqn2.o
	g++ -o eqn2 calleqn2.o sqrt.o eqn2.o

calleqn2.o: calleqn2.cpp
	g++ -c calleqn2.cpp

eqn2.o:	eqn2.k
	../kcomp eqn2.k 2> eqn2.ll
	./tobinary eqn2.ll
	
inssort: inssort.o time_and_print.o rand.o
	g++ -o inssort inssort.o time_and_print.o rand.o

time_and_print.o: time_and_print.cpp
	g++ -c time_and_print.cpp

inssort.o:	inssort.k
	../kcomp inssort.k 2> inssort.ll
	./tobinary inssort.ll
	
inssort2: inssort2.o time_and_print.o rand.o
	g++ -o inssort2 inssort2.o time_and_print.o rand.o

inssort2.o:	inssort2.k
	../kcomp inssort2.k 2> inssort2.ll
	./tobinary inssort2.ll	
	
sqrt2: callsqrt.o sqrt2.o
	g++ -o sqrt2 callsqrt.o sqrt2.o

sqrt2.o:	sqrt2.k
	../kcomp sqrt2.k 2> sqrt2.ll
	./tobinary sqrt2.ll
	
sqrt3: callsqrt.o sqrt3.o
	g++ -o sqrt3 callsqrt.o sqrt3.o

sqrt3.o:	sqrt3.k
	../kcomp sqrt3.k 2> sqrt3.ll
	./tobinary sqrt3.ll
	
clean:
	rm -f floor rand fibonacci sqrt eqn2 inssort inssort2 sqrt2 sqrt3 *~ *.o *.s *.bc *.ll
