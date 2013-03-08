source_dir     ?= src
build_dir      ?= build
naoqi_path     ?= /opt/naoqi-sdk

CXX            := c++
PROTOC         := protoc
CXXFLAGS_EXTRA += -std=c++11 -Wall -MP -MMD
CXXFLAGS_EXTRA += -I$(naoqi_path)/include
CXXFLAGS_EXTRA += -I /usr/include/opencv2

LDFLAGS_EXTRA  += -pthread
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib/naoqi
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib/naoqi

server_OBJS    := server noise_detector
server_LIBS    := alaudio alextractor
server_DIR     := .

client_OBJS    := client
client_LIBS    :=
client_DIR     := .

command_OBJS   := command speech_commands behavior_commands presentation_commands
command_LIBS   :=
command_DIR    := command

parser_OBJS    := command_parser
parser_LIBS    :=
parser_DIR     := parser

engine_OBJS    := speech_engine script_engine behavior_engine server_engine
engine_LIBS    += boost_filesystem-mt boost_signals-mt
engine_LIBS    += alcommon alproxies alvalue alsoap alerror althread
engine_LIBS    += qi rttools protobuf opencv_core opencv_highgui
engine_DIR     := engine

protocol_OBJS  := messages.pb
protocol_LIBS  += protobuf
protocol_DIR   += protocol

server_COMPONENTS := server parser engine protocol command
server_BIN        := robotutor-server

client_COMPONENTS := client protocol
client_BIN        := robotutor-client

-include Makefile.local

.PHONY: default
default: $(source_dir)/$(protocol_DIR)/messages.pb.h $(server_BIN) $(client_BIN)

include Makefile.in

# Call protoc to generate protobuffer C++ files.
$(source_dir)/$(protocol_DIR)/messages.pb.h: $(source_dir)/$(protocol_DIR)/messages.proto
	$(suppress) cd '$(source_dir)/$(protocol_DIR)' && $(PROTOC) --cpp_out='.' 'messages.proto'

$(source_dir)/$(protocol_DIR)/messages.pb.cc: $(source_dir)/$(protocol_DIR)/messages.proto
	
$(source_dir)/$(engine_DIR)/server_engine.hpp: $(source_dir)/$(protocol_DIR)/messages.pb.h

clean_protobuf:
	$(suppress) rm -f '$(source_dir)/$(protocol_DIR)/messages.pb.cc'
	$(suppress) rm -f '$(source_dir)/$(protocol_DIR)/messages.pb.h'

clean: clean_protobuf


$(call define_components,client server command parser engine protocol)
$(call define_program,server)
$(call define_program,client)
