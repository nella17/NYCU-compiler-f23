CC = g++
LEX = flex
YACC = bison
YACC_FLAGS = --defines=parser.h -v 
COVERAGE_DIR = $(CURDIR)/.coverage
GCOVFLAGS = -fprofile-arcs -ftest-coverage
CFLAGS = -Wall -std=gnu++17 -g
LDFLAGS =
INCLUDE = -Iinclude
LIBS    += -ly
ifeq ($(shell uname),Darwin)
LIBS    += -ll
else
LIBS    += -lfl
ifeq ($(DEBUG),1)
LDFLAGS += -fsanitize=leak -fsanitize=address -fsanitize=undefined
endif
endif
ifeq ($(TRACE),1)
CFLAGS 		+= -DTRACE
YACC_FLAGS 	+= --debug
endif

SCANNER = scanner
PARSER = parser

SRCDIR = lib/
SRC := $(shell find $(SRCDIR) -name '*.cpp')

EXEC = compiler
SRCS = $(PARSER:=.cpp) \
       $(SCANNER:=.cpp) \
       $(SRC)

OBJDIR := ./obj
$(shell [ -d $(OBJDIR) ] || mkdir -p $(OBJDIR))

# Substitution reference
OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
DEPS := $(OBJS:%.o=%.d)

all: $(EXEC)

# Static pattern rule
$(SCANNER).cpp: %.cpp: %.l $(PARSER).cpp
	$(LEX) -o $@ $<

$(PARSER).cpp: %.cpp: %.y
	$(YACC) -o $@ $(YACC_FLAGS) $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c -MP -MMD $<

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(INCLUDE) $(LDFLAGS)

#
# Using Gcov to collect coverage data and Lcov to generate HTML report.
#

coverage: LDFLAGS += -O0 $(GCOVFLAGS)
coverage: $(EXEC)
	@command -v gcov >/dev/null 2>&1 || { echo >&2 "gcov is not installed, consider installing it first"; exit 1; }
	$(MAKE) -C ../test/
	$(MAKE) -C ../board/ compile
	gcov $(OBJS)

coverage-report: coverage
	@command -v lcov >/dev/null 2>&1 || { echo >&2 "lcov is not installed, consider installing it first"; exit 1; }
	mkdir -p $(COVERAGE_DIR)
	lcov --capture --directory $(CURDIR) --output-file $(COVERAGE_DIR)/coverage.info
	genhtml $(COVERAGE_DIR)/coverage.info --output-directory $(COVERAGE_DIR)
	@echo "Open $(COVERAGE_DIR)/index.html in your browser to view the coverage report."

clean:
	$(RM) -r $(DEPS) $(SCANNER:=.cpp) $(PARSER:=.cpp) $(PARSER:=.h) $(OBJS) $(EXEC) \
		$(OBJS:.o=.gcda) $(OBJS:.o=.gcno) *.gcov $(COVERAGE_DIR)

-include $(DEPS)

.PHONY: all clean coverage coverage-report
