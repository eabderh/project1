# ---- FILE HEADER ----------------------------------------------------------
# project: project1
# file: makefile
# student/author: elias abderhalden
# date: 2014-10-03
# ---------------------------------------------------------------------------
# class: ece3220 fall 2014
# instructor: jacob sorber
# assignment: project 1: tracing, sytem calls, and processes 
# ---------------------------------------------------------------------------
# notes:	commands
# 			make - build programs
# 			make clean - delete object files, executable, and core
# ---------------------------------------------------------------------------

cc = gcc
gflag = -g -Wall
glib = -lm
lib = ar
lib_flag = rvs


project1 : leakcount memory_shim.so sctracer


leakcount : leakcount.o list.o
	$(cc) $(gflag) leakcount.o list.o -o leakcount -ldl
leakcount.o : leakcount.c list.h
	$(cc) $(gflag) -c leakcount.c $(glib)
memory_shim.so : memory_shim.c
	$(cc) -Wall -fPIC -shared -o memory_shim.so memory_shim.c -ldl


sctracer : sctracer.o list.o
	$(cc) $(gflag) sctracer.o list.o -o sctracer
sctracer.o : sctracer.c list.h
	$(cc) $(gflag) -c sctracer.c


list.o : list.c list.h
	$(cc) $(gflag) -c list.c



t : lctest trtest

lctest : lctest.o
	$(cc) $(gflag) lctest.o -o lctest
lctest.o : lctest.c
	$(cc) $(gflag) -c lctest.c

trtest : trtest.o
	$(cc) $(gflag) trtest.o -o trtest
trtest.o : trtest.c
	$(cc) $(gflag) -c trtest.c


cl :
	rm -f core *.o *.so leakcount sctracer
ct :
	rm -f *test
da :
	rm -f *.a


