CC = g++
OBJS = iresum.o
PROGRAM = iresum.exe

DEST = ../../bin/
CFLAGS  = -std=c++1y -Wall -I../../include/
LFLAGS  = -static -L../../lib -lstradatool -lboost_program_options-mt
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(DEST)$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(DEST)$(PROGRAM)

.PHONY: clean
clean:
	rm -f *.o
