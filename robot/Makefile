CXX            := c++
CXXFLAGS_EXTRA := -std=c++11 -I/opt/naoqi-sdk/include
LDFLAGS_EXTRA  := -pthread
LDFLAGS_EXTRA  += -L /opt/naoqi-sdk/lib
LDFLAGS_EXTRA  += -L /opt/naoqi-sdk/lib/naoqi

source_dir     := src
build_dir      := build

common_OBJS    := executable test
common_LIBS    :=
common_DIR     := .

parser_OBJS    := text_parser command_parser executable_parser
parser_LIBS    :=
parser_DIR     := parser

engine_OBJS    := speech_engine
engine_LIBS    := alcommon alproxies alvalue alsoap alerror althread
engine_LIBS    += qi rttools boost_filesystem
engine_DIR     := engine

server_OBJS    := server
server_LIBS    := boost_system
server_DIR     := server

robotutor_COMPONENTS := common parser engine server
robotutor_BIN        := robotutor

.PHONY: default
default: $(robotutor_BIN)

include Makefile.in


$(call define_components,common parser engine server)
$(call define_program,robotutor)
