source_dir     ?= src
build_dir      ?= build
naoqi_path     ?= /opt/naoqi-sdk

CXX             = c++
PROTOC          = protoc
CXXFLAGS_EXTRA += -std=c++11 -Wall -MP -MMD
CXXFLAGS_EXTRA += -I$(naoqi_path)/include
CXXFLAGS_EXTRA += -I /usr/include/opencv2

LDFLAGS_EXTRA  += -pthread
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib/naoqi
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib/naoqi

engine_src    = speech_engine.cpp script_engine.cpp behavior_engine.cpp
commands_src  = command.cpp commands/speech_commands.cpp commands/behavior_commands.cpp commands/presentation_commands.cpp
parser_src    = script_parser.cpp
protocol_src  = messages.pb.cxx

engine_lib    += boost_signals-mt
engine_lib    += alcommon alproxies alvalue alsoap alerror althread
engine_lib    += qi rttools protobuf
commands_lib  += opencv_core opencv_highgui
parser_lib    +=
protocol_lib  += protobuf boost_system-mt

server_src    = robotutor_server.cpp noise_detector.cpp $(engine_src) $(commands_src) $(parser_src) $(protocol_src)
server_lib    = alaudio alextractor $(engine_lib) $(commands_lib) $(parser_lib) $(protocol_lib)
server_bin    = robotutor-server

client_src    = robotutor_client.cpp $(protocol_src)
client_lib    = $(protocol_lib)
client_bin    = robotutor-client

-include Makefile.local

.PHONY: default
default: $(source_dir)/messages.pb.h $(server_bin) $(client_bin)

include Makefile.in

# Call protoc to generate protobuffer C++ files.
$(source_dir)/messages.pb.h: $(source_dir)/messages.proto
	$(suppress) cd '$(source_dir)' && $(PROTOC) --cpp_out='.' 'messages.proto'

$(source_dir)/messages.pb.cc: $(source_dir)/messages.proto

clean_protobuf:
	$(suppress) rm -f '$(source_dir)/messages.pb.cc'
	$(suppress) rm -f '$(source_dir)/messages.pb.h'

clean: clean_protobuf




$(call define_subdir,commands)
$(call define_program,server)
$(call define_program,client)
