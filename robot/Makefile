.PHONY: default
default:

source_dir     ?= src
build_dir      ?= build
naoqi_path     ?= /opt/naoqi-sdk

CXX             = c++
PROTOC          = protoc
CXXFLAGS_EXTRA += -std=c++11 -Wall -MP -MMD -fPIC
CXXFLAGS_EXTRA += -I$(naoqi_path)/include
CXXFLAGS_EXTRA += -I /usr/include/opencv2

LDFLAGS_EXTRA  += -pthread
LDFLAGS_EXTRA  += -L./
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib/naoqi
LDFLAGS_EXTRA  += -Wl,-rpath,./
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib/naoqi

# Core components
engine_src     = speech_engine.cpp script_engine.cpp behavior_engine.cpp
engine_lib    += boost_signals-mt
engine_lib    += alcommon alproxies alvalue alsoap alerror althread
engine_lib    += qi rttools protobuf

command_src    = core_commands.cpp command.cpp
command_lib   +=

parser_src     = script_parser.cpp command_factory.cpp
parser_lib    +=

protocol_src   = messages.pb.cxx
protocol_lib  += protobuf boost_system-mt

# Main library.
robotutor_src = $(engine_src) $(command_src) $(parser_src) $(protocol_src)
robotutor_lib = $(engine_lib) $(command_lib) $(parser_lib) $(protocol_lib)
robotutor_bin = librobotutor.so

# Server executable.
server_src    = robotutor_server.cpp noise_detector.cpp
server_lib   += alaudio alextractor alcommon alvalue alerror
server_lib   += boost_system-mt robotutor protobuf
server_dep    = $(robotutor_bin)
server_bin    = robotutor-server

# Client executable.
client_src    = robotutor_client.cpp
client_lib    = $(protocol_lib) robotutor
client_dep    = $(robotutor_bin)
client_bin    = robotutor-client


# Presentation plugin.
presentation_src  = commands/presentation_commands.cpp command.cpp
presentation_lib += opencv_core opencv_highgui boost_system-mt protobuf alproxies alvalue robotutor
presentation_bin  = lib/presentation_commands.so

# Behavior plugin.
behavior_src      = commands/behavior_commands.cpp command.cpp
behavior_lib     += boost_system-mt robotutor
behavior_bin      = lib/behavior_commands.so

-include Makefile.local


include Makefile.in
$(call define_library,robotutor)
$(call define_program,server)
$(call define_program,client)
$(call define_library,presentation)
$(call define_library,behavior)

default: $(source_dir)/messages.pb.h $(server_bin) $(robotutor_bin) $(client_bin) $(presentation_bin) $(behavior_bin)


# Call protoc to generate protobuffer C++ files.
$(source_dir)/messages.pb.h: $(source_dir)/messages.proto
	$(suppress) cd '$(source_dir)' && $(PROTOC) --cpp_out='.' 'messages.proto'

$(source_dir)/messages.pb.cc: $(source_dir)/messages.proto

.PHONY: clean_protobuf
clean_protobuf:
	$(suppress) rm -f '$(source_dir)/messages.pb.cc'
	$(suppress) rm -f '$(source_dir)/messages.pb.h'

clean: clean_protobuf

