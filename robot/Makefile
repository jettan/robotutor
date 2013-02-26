-include Makefile.local

source_dir     ?= src
build_dir      ?= build
naoqi_path     ?= /opt/naoqi-sdk

CXX            := c++
PROTOC         := protoc
CXXFLAGS_EXTRA += -std=c++11 -Wall -MP -MMD
CXXFLAGS_EXTRA += -I$(naoqi_path)/include

LDFLAGS_EXTRA  += -pthread
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib
LDFLAGS_EXTRA  += -L$(naoqi_path)/lib/naoqi
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib
LDFLAGS_EXTRA  += -Wl,-rpath,$(naoqi_path)/lib/naoqi

common_OBJS    := main
common_LIBS    :=
common_DIR     := .

command_OBJS   := command speech_commands behavior_commands presentation_commands
command_LIBS   :=
command_DIR    := command

parser_OBJS    := command_parser
parser_LIBS    :=
parser_DIR     := parser

engine_OBJS    := speech_engine script_engine behavior_engine server_engine
engine_LIBS    += boost_filesystem boost_signals
engine_LIBS    += alcommon alproxies alvalue alsoap alerror althread
engine_LIBS    += qi rttools protobuf
engine_DIR     := engine

protocol_OBJS  := messages.pb
protocol_LIBS  +=
protocol_DIR   += protocol

robotutor_COMPONENTS := common parser engine protocol command
robotutor_BIN        := robotutor

.PHONY: default
default: $(source_dir)/$(protocol_DIR)/messages.pb.h $(robotutor_BIN)

include Makefile.in

# Call protoc to generate protobuffer C++ files.
$(source_dir)/$(protocol_DIR)/messages.pb.h: $(source_dir)/$(protocol_DIR)/messages.proto
	@tput bold
	@tput setf 1
	@echo "Generating protobuf files"
	@tput sgr0
	@cd '$(source_dir)/$(protocol_DIR)' && $(PROTOC) --cpp_out='.' 'messages.proto'

$(source_dir)/$(protocol_DIR)/messages.pb.cc: $(source_dir)/$(protocol_DIR)/messages.proto
	
$(source_dir)/$(engine_DIR)/server_engine.hpp: $(source_dir)/$(protocol_DIR)/messages.pb.h

clean_protobuf:
	@tput bold
	@tput setf 4
	@echo "Removing generated protobuf files"
	@tput sgr0
	@rm -f '$(source_dir)/$(protocol_DIR)/messages.pb.cc'
	@rm -f '$(source_dir)/$(protocol_DIR)/messages.pb.h'

clean: clean_protobuf


$(call define_components,common command parser engine protocol)
$(call define_program,robotutor)
