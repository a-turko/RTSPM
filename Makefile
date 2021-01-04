CXXFLAGS= -std=c++17 -Wall -Wextra -Wshadow 
lasan = -fsanitize=address -fsanitize=address -fsanitize=undefined -static-libasan

objects=memreport.o rtspm.o kmp.o
executables=kmp-mem rtspm-mem kmp rtspm gen

all: $(executables)

%.o: %.cpp
	g++ -c -D MEMREPORT $(CXXFLAGS) $< -o $@ 

rtspm-mem: memreport.o rtspm.o
	g++ rtspm.o memreport.o -o $@
	rm rtspm.o

kmp-mem: memreport.o kmp.o
	g++ kmp.o memreport.o -o $@
	rm kmp.o

kmp: kmp.cpp
	g++ $(CXXFLAGS) $< -o $@

rtspm: rtspm.cpp
	g++ $(CXXFLAGS) $(lasan) $< -o $@

clean:
	rm $(objects) $(executables)
