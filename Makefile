CXX     = g++
AR      = ar
ARFLAGS = crs
CDFLAGS = -std=c++1y -Wall -Iinclude/

OBJDIR = src/
OBJS= $(OBJDIR)tool.o \
      $(OBJDIR)StradaCmn.o \
      $(OBJDIR)StradaINT.o \
      $(OBJDIR)StradaIRE.o \
      $(OBJDIR)StradaTNT.o \
      $(OBJDIR)StradaPAR.o \
      $(OBJDIR)StradaTPA.o \
      $(OBJDIR)StradaAOD.o \
      $(OBJDIR)StradaZXY.o \
      $(OBJDIR)StradaGAD.o \
      $(OBJDIR)StradaDRE.o \
      $(OBJDIR)pch.o
LFILE = lib/libstradatool.a

all: $(LFILE) $(OBJS)

.cpp.o:
	$(CXX) $(CDFLAGS) -c $< -o $@

$(LFILE) : $(OBJS)
	$(AR) $(ARFLAGS) $(LFILE) $(OBJS)


