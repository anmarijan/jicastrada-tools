CC = g++
OBJS = tnt_test.o ../src/tool.o ../src/StradaCmn.o ../src/StradaTNT.o
PROGRAM = tnt_test.exe
CFLAGS  = -std=c++1y -Wall -I../include/
LFLAGS  = -static
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(PROGRAM)

$(OBJS) :


