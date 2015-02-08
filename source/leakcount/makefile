# ---- FILE HEADER ----------------------------------------------------------
# project: ece3220 project1
# file: makefile
# author: elias abderhalden
# date: 2015-02-08
# ---------------------------------------------------------------------------
# notes:	commands
# 			make - build
# 			make cl - delete unnecessary files
# ---------------------------------------------------------------------------


c = gcc
cn = gcc -g -Wall
cof = -O2 -DNDEBUG
sl = $(c) -fPIC -shared
a = ar
af = -cvq
an = $(a) $(af)




project: leakcount


leakcount : leakcount.o list.o
	$(cn) leakcount.o list.o -o leakcount -ldl
leakcount.o : leakcount.c list.h
	$(cn) -c leakcount.c -lm
memory_shim.so : memory_shim.c
	$(c) -Wall -fPIC -shared -o memory_shim.so memory_shim.c -ldl


clearscreen :
	clear


t : $(wildcard t*)

t0 :
t1 :
t2 :
t3 :
t4 :
t5 :
t6 :
t7 :
t8 :
t9 :


%.o : %.c
	$(cn) -c -o $@ $<


cl :
	find . -type f -not -name '*.c' -not -name '*.h' -not -name 'makefile' | xargs rm




