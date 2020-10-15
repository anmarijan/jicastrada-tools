# ---------------------------------------------------------------------------
BCB = C:\Program Files\CodeGear\RAD Studio\6.0
# ---------------------------------------------------------------------------
ROOT     = C:\Project
PROJECT  = gad_test.exe
OBJFILES = gad_test.obj
RESFILES = 
RESDEPEN = $(RESFILES)
LIBFILES = $(ROOT)\lib\strada.lib $(ROOT)\lib\MyTools.lib
DEFFILE = 
# ---------------------------------------------------------------------------
!if $d(DEBUG)
CFLAG1 = -6 -Hc -w-par -Od -b- -k -y -v -vi- -c -tWC -tWM- -tW- -x
!else
CFLAG1 = -DNDEBUG -6 -Hc -O2 -w-par -b- -vi -c -tWC -tWM- -tW- -x
!endif
CFLAG2 = -I$(ROOT) 
RFLAGS = -i"$(BCB)\include"
LFLAGS = -L"$(BCB)\lib\obj";"$(BCB)\lib" -Tpe -Gn -x -v 
IFLAGS = 
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0x32.obj $(OBJFILES)
ALLRES = $(RESFILES) 
ALLLIB = $(LIBFILES) import32.lib cw32.lib
# ---------------------------------------------------------------------------
.autodepend

$(PROJECT): $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES) 
!

.cpp.obj:
    bcc32 $(CFLAG1) $(CFLAG2) -o$* $* 

#-----------------------------------------------------------------------------
