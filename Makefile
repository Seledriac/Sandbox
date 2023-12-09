CC = g++
RM	= rm -f

INCLUDE_DIRS = -I"Libs\freeglut\include"
LIB_DIRS = -L"Libs\freeglut\lib\x64"

FLAGS_BUILD = -m64
FLAGS_OPTIMIZATION = -O3
FLAGS_OPENMP = -fopenmp
FLAGS_DEBUG = -g
FLAGS_WARNING = -Wall -Wextra

UNAME := $(shell uname)
ifeq ($(UNAME), Windows_NT)
FLAGS_GLUT = -lfreeglut -lopengl32 -lglu32
else ifeq ($(UNAME), MINGW32_NT-6.2)
FLAGS_GLUT = -lfreeglut -lopengl32 -lglu32
else
FLAGS_GLUT = -lGL -lglut -lGLU -lX11 -lm
endif

CFLAGS = ${FLAGS_BUILD} ${INCLUDE_DIRS} ${LIB_DIRS} ${FLAGS_GLUT} ${FLAGS_OPENMP} ${FLAGS_WARNING} ${FLAGS_DEBUG} ${FLAGS_OPTIMIZATION}
SOURCES = $(wildcard *.cpp) $(wildcard Libs/tb/*.cpp) $(wildcard Util/*.cpp) $(wildcard Projects/*/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECS = main.exe


all :: $(EXECS)

main.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS)

.cpp.o :
	$(CC) -c $< -o $*.o $(INCLUDES) $(CFLAGS)

clean ::
	$(RM) $(OBJECTS) $(EXECS) depend

depend::
	$(CC) -MM *.cpp > depend

include depend
