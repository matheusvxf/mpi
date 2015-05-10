CXX=mpic++
CFLAGS=-Wall -std=c++11

DEPS=Matrix.h Node.h ClientNode.h ServerNode.h Common.h

IDIR=include .

LDIR=lib

ODIR=obj

SDIR=source

BDIR=bin

INCLUDES= $(foreach d, $(IDIR), -I$d)

_SRC=Matrix.cpp Node.cpp ClientNode.cpp ServerNode.cpp code.cpp
SRC = $(patsubst %, $(SDIR)/%, $(_OBJ))

_OBJ = Matrix.o Node.o ClientNode.o ServerNode.o code.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

OBJS=$(_SRC:.cpp=.o)

$(ODIR)/%.o:$(SDIR)/%.cpp
	$(CXX) -c $< -o $@  $(INCLUDES) $(CFLAGS)

TARGET = code

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(BDIR)/$@ $^ 
	
.PHONY: depend clean
	
clean:
	rm -f *.o $(ODIR)/*.o *~ core $(INCDIR)/*~ $(TARGET)
	
depend: $(SRCS)
	makedepend $(INCLUDES) $^
