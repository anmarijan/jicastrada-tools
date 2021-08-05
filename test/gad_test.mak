CC = g++
OBJS = gad_test.o ../src/tool.o ../src/StradaCmn.o ../src/StradaGAD.o
PROGRAM = gad_test.exe
CFLAGS  = -std=c++1y -Wall -I../include/
LFLAGS  = -static
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(PROGRAM)

$(OBJS) :


