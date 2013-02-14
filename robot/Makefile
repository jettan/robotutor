CXX            := c++
CXXFLAGS_EXTRA := -std=c++11 -I/opt/naoqi-sdk/include -Wall -MP -MMD
LDFLAGS_EXTRA  := -pthread
LDFLAGS_EXTRA  += -L /opt/naoqi-sdk/lib
LDFLAGS_EXTRA  += -L /opt/naoqi-sdk/lib/naoqi
LDFLAGS_EXTRA  += -Wl,-rpath=/opt/naoqi-sdk/lib
LDFLAGS_EXTRA  += -Wl,-rpath=/opt/naoqi-sdk/lib/naoqi

source_dir     := src
build_dir      := build

common_OBJS    := test
common_LIBS    :=
common_DIR     := .

command_OBJS   := command text_command
command_LIBS   :=
command_DIR    := command

parser_OBJS    := command_parser
parser_LIBS    :=
parser_DIR     := parser

engine_OBJS    := speech_engine script_engine
engine_LIBS    := alcommon alproxies alvalue alsoap alerror althread
engine_LIBS    += qi rttools boost_filesystem
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
