CC = g++
OBJS = tntintck.o
PROGRAM = tntintck.exe
CFLAGS  = -std=c++1y -Wall -I../../include/
LFLAGS  = -static -L../../lib -lstradatool
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(PROGRAM)

$(OBJS) :


