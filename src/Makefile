CC = g++
LEX = flex
YACC = bison
CFLAGS = -Wall -std=gnu++17 -g
LDFLAGS =
INCLUDE = -Iinclude
ifeq ($(shell uname),Darwin)
LIBS    = -ll
else
LIBS    = -lfl
ifeq ($(DEBUG),1)
LDFLAGS += -fsanitize=leak -fsanitize=address -fsanitize=undefined
endif
endif
LIBS    += -ly

SCANNER = scanner
PARSER = parser

ASTDIR = lib/AST/
AST := $(shell find $(ASTDIR) -name '*.cpp')

VISITORDIR = lib/visitor/
VISITOR := $(shell find $(VISITORDIR) -name '*.cpp')

SRC := $(AST) \
	   $(VISITOR)

EXEC = $(PARSER)
OBJS = $(PARSER:=.cpp) \
       $(SCANNER:=.cpp) \
       $(SRC)

OBJDIR := ./obj
$(shell [ -d $(OBJDIR) ] || mkdir -p $(OBJDIR))

# Substitution reference
OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(OBJS)))
DEPS := $(OBJS:%.o=%.d)

all: $(EXEC)

# Static pattern rule
$(SCANNER).cpp: %.cpp: %.l
	$(LEX) -o $@ $<

$(PARSER).cpp: %.cpp: %.y
	$(YACC) -o $@ --defines=parser.h -v $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c -MP -MMD $<

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(INCLUDE) $(LDFLAGS)

clean:
	$(RM) $(DEPS) $(SCANNER:=.cpp) $(PARSER:=.cpp) $(PARSER:=.h) $(OBJS) $(EXEC)

-include $(DEPS)
