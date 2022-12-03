CC = g++
RM	= rm -f

INCLUDE_DIRS = -I"freeglut\include"
LIB_DIRS = -L"freeglut\lib\x64"

FLAGS_BUILD = -m64
FLAGS_OPTIMIZATION = -O2
FLAGS_OPENMP = -fopenmp
FLAGS_GLUT = -lfreeglut -lopengl32 -lglu32
FLAGS_WARNING = -W -Wall -g

CFLAGS = ${FLAGS_BUILD} ${INCLUDE_DIRS} ${LIB_DIRS} ${FLAGS_GLUT} ${FLAGS_OPENMP} ${FLAGS_WARNING} ${FLAGS_OPTIMIZATION}
SOURCES = $(wildcard *.cpp) $(wildcard tb/*.cpp) $(wildcard math/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECS = main.exe


all :: $(EXECS)

main.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS)

.cpp.o :
	$(CC) -c $< -o $*.o $(INCLUDES) $(CFLAGS)

clean ::
	$(RM) $(OBJECTS) $(EXECS)

depend::
	$(CC) -MM *.cpp > depend

include depend
