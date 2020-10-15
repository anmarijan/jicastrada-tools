CC = g++
OBJS = par_test.o ../src/StradaPAR.o ../src/tool.o
PROGRAM = par_test.exe
# GIT_HASH = $(shell git log -1 --format="%h")
# CFLAGS  = -std=c++1y -Wall -DGITVERSION=\"$(GIT_HASH)\"
CFLAGS  = -std=c++1y -Wall -I../include/
LFLAGS  = -static
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC)  $(OBJS)  $(LFLAGS) -o $(PROGRAM)

$(OBJS) :

clean:
	rm -f *.o $(PROGRAM)
