IDIR=include .
CXX=mpic++
CFLAGS=-Wall -std=c++11
INCLUDES= $(foreach d, $(IDIR), -I$d)
DEPS=Matrix.h Node.h ClientNode.h ServerNode.h Common.h

LDIR=lib

ODIR=obj

SRCS=Matrix.cpp Node.cpp ClientNode.cpp ServerNode.cpp code.cpp

_OBJ = Matrix.o Node.o ClientNode.o ServerNode.o code.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

OBJS=$(SRCS:.cpp=.o)

.cpp.o:
	$(CXX)  -c $< -o $@  $(INCLUDES) $(CFLAGS)

TARGET = code

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $@ $^ 
	
.PHONY: depend clean
	
clean:
	rm -f *.o *~ core $(INCDIR)/*~ $(TARGET)
	
depend: $(SRCS)
	makedepend $(INCLUDES) $^
