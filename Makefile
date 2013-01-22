CXXFLAGS_EXTRA :=
LDFLAGS_EXTRA  :=

parser_OBJS    := text_parser command_parser executable_parser
robotutor_OBJS := $(parser_OBJS)
robotutor_LIBS :=

default: robotutor

.PHONY: clean

build/%.o: src/%.cpp | build
	$(CXX) $(CXXGLAGS_EXTRA) $(CXXFLAGS) -c -o '$@' '$<'

robotutor: $(robotutor_OBJS:%=build/%.o)
	$(CXX) $(LDFLAGS_EXTRA) $(LDFLAGS) -o '%@' $^ $(robotutor_LIBS:%=-l%)

build:
	-mkdir build 2>/dev/null

clean:
	-rm -r build  2>/dev/null
	-rm robotutor 2>/dev/null
