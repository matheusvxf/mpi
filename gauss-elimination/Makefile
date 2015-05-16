CXX=mpic++
MKDIR_P = mkdir -p
CFLAGS=-Wall -std=c++11 -g

DEPS=Matrix.h Node.h ClientNode.h ServerNode.h Common.h TestFramework.h

IDIR=include .

LDIR=lib

ODIR=obj

SDIR=source

BDIR=bin

INCLUDES= $(foreach d, $(IDIR), -I$d)

_OBJ = Matrix.o Node.o ClientNode.o ServerNode.o TestFramework.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

OBJS=$(_SRC:.cpp=.o)

$(ODIR)/%.o:$(SDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $< -o $@  $(INCLUDES) $(CFLAGS)

TARGET = code

TARGET_TESTGEN = testgen

all: $(TARGET) $(TARGET_TESTGEN)

$(TARGET): $(OBJ) $(ODIR)/code.o
	@mkdir -p $(BDIR)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(BDIR)/$@ $^ 

$(TARGET_TESTGEN): $(OBJ) $(ODIR)/TestGen.o
	@mkdir -p $(BDIR)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(BDIR)/$@ $^

-include $(INCLUDES)

.PHONY: depend clean
	
clean:
	rm -f *.o $(ODIR)/*.o *~ core $(INCDIR)/*~ $(TARGET)
	
depend: $(SRCS)
	makedepend $(INCLUDES) $^
