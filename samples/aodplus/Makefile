CC = g++
OBJS = aodplus.o calcaod.o fratar.o
PROGRAM = aodplus.exe

DEST = ../../bin/
CFLAGS  = -std=c++1y -Wall -I../../include/
LFLAGS  = -static -L../../lib -lstradatool
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(DEST)$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(DEST)$(PROGRAM)

.PHONY: clean
clean:
	rm -f *.o