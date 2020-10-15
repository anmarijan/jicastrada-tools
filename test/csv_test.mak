CC = g++
OBJS = csv_test.o ../src/tool.o
PROGRAM = csv_test.exe
CFLAGS  = -std=c++1y -Wall -I../include/
LFLAGS  = -static
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(PROGRAM)

$(OBJS) :

