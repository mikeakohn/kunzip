CC?=gcc
CXX?=g++
#DEBUG_INFLATE=-DDEBUG_INFLATE
#DEBUG_ZIP=-DDEBUG_ZIP
FLAGS=-s -O3 -Wall $(DEBUG_INFLATE) $(DEBUG_ZIP)
OBJECTS=fileio.o kinflate.o zipfile.o
VPATH=../src

WINPREFIX=i686-w64-mingw32-
WINDRES=$(WINPREFIX)windres
DLLWRAP=$(WINPREFIX)dllwrap

default: $(OBJECTS)
	$(CC) -o ../kunzip ../src/kunzip.c $(OBJECTS) $(FLAGS)

%.o: %.cxx %.h
	$(CXX) -c $< -o $*.o \
	   $(CFLAGS)

old:
	$(CC) -c fileio.c $(FLAGS)
	$(CC) -c kinflate.c $(FLAGS) -DZIP
	$(CC) -c zipfile.c $(FLAGS)
	$(CC) -o kunzip kunzip.c fileio.o kinflate.o zipfile.o $(FLAGS)

dll:
	$(WINPREFIX)$(CC) -c fileio.c $(FLAGS) -m32 -mrtd -DWINDOWS
	$(WINPREFIX)$(CC) -c kinflate.c $(FLAGS) -DZIP -m32 -mrtd -DWINDOWS
	$(WINPREFIX)$(CC) -c zipfile.c $(FLAGS) -DQUIET -m32 -mrtd -DWINDOWS
	$(WINDRES) -i kunzip.rc -o kunzipres.o
	$(DLLWRAP) --def kunzip.def --dllname kunzip -o kunzip.dll \
	        fileio.o kinflate.o kunzipres.o zipfile.o $(FLAGS)

lib:
	$(CC) -c fileio.c $(FLAGS)
	$(CC) -c kinflate.c $(FLAGS) -DZIP
	$(CC) -c zipfile.c $(FLAGS)
	$(CC) -o libkunzip.so -shared fileio.o kinflate.o zipfile.o $(FLAGS)

clean:
	@if [ -e fileio.o ]; then rm fileio.o; fi
	@if [ -e kinflate.o ]; then rm kinflate.o; fi
	@if [ -e zipfile.o ]; then rm zipfile.o; fi
	@if [ -e kunzipres.o ]; then rm kunzipres.o; fi
	@if [ -e kunzip.exe ]; then rm kunzip.exe; fi
	@if [ -e libkunzip.so ]; then rm libkunzip.so; fi
	@if [ -e kunzip.dll ]; then rm kunzip.dll; fi
	@if [ -e kunzip ]; then rm kunzip; fi
	@echo "Clean!"



