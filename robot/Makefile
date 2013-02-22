-include Makefile.local

source_dir     ?= src
build_dir      ?= build
naoqi_path     ?= /opt/naoqi-sdk

CXX            ?= c++
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

command_OBJS   := command speech_commands behavior_commands
command_LIBS   :=
command_DIR    := command

parser_OBJS    := command_parser
parser_LIBS    :=
parser_DIR     := parser

engine_OBJS    := speech_engine script_engine behavior_engine
engine_LIBS    += boost_filesystem boost_signals
engine_LIBS    += alcommon alproxies alvalue alsoap alerror althread
engine_LIBS    += qi rttools
engine_DIR     := engine

server_OBJS    := server
server_LIBS    := boost_system
server_DIR     := server

robotutor_COMPONENTS := common parser engine server command
robotutor_BIN        := robotutor

.PHONY: default
default: $(robotutor_BIN)

include Makefile.in


$(call define_components,common command parser engine server)
$(call define_program,robotutor)
