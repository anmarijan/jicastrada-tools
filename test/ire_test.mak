CC = g++
OBJS = ire_test.o ../src/tool.o ../src/StradaCmn.o ../src/StradaINT.o ../src/StradaIRE.o
PROGRAM = ire_test.exe
CFLAGS  = -std=c++1y -Wall -I../include/
LFLAGS  = -static
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(PROGRAM)

$(OBJS) :


