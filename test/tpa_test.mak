CC = g++
OBJS = tpa_test.o ../src/tool.o ../src/StradaTPA.o
PROGRAM = tpa_test.exe
CFLAGS  = -std=c++1y -Wall -I../include/
LFLAGS  = -static
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(PROGRAM)

$(OBJS) :


