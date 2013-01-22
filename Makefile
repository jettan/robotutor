CXXFLAGS_EXTRA :=
LDFLAGS_EXTRA  :=

parser_OBJS    := executable text_parser command_parser executable_parser test
robotutor_OBJS := $(parser_OBJS)
robotutor_LIBS :=

default: robotutor

.PHONY: clean

build/%.o: src/%.cpp Makefile | build
	$(CXX) $(CXXFLAGS_EXTRA) $(CXXFLAGS) -c -o '$@' '$<'

robotutor: $(robotutor_OBJS:%=build/%.o)
	$(CXX) $(LDFLAGS_EXTRA) $(LDFLAGS) -o '$@' $^ $(robotutor_LIBS:%=-l%)

build:
	-mkdir build 2>/dev/null

clean:
	-rm -r build  2>/dev/null
	-rm robotutor 2>/dev/null
