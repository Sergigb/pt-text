#
# Copyright (C) 2023 Sergi Garcia Bordils
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU
# General Public License as published by the Free Software Foundation, either version 3 of the 
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program. If not,
# see <https://www.gnu.org/licenses/>. 
#

CXX := g++
CC := gcc
INC := -I/usr/include/freetype2
CXXFLAGS := -Wall -Wextra -Werror -pedantic -ubsan -MMD -std=c++11
LDLIBS :=  -lGL -lGLEW -lglfw -lfreetype
OBJPATH := bin
EXECPATH := bin

ifdef RELEASE
	CXXFLAGS := $(CXXFLAGS) -O3 -no-pie
else
	CXXFLAGS := $(CXXFLAGS) -ggdb -DDEBUG
endif

ifdef SAVE_STB
	CXXFLAGS := $(CXXFLAGS) -DSAVE_STB
	INC := $(INC) -I./include/
endif

CXXFLAGS := $(INC) $(CXXFLAGS)

# text rendering
TEXT_SRCS := $(shell find src/ -name "*.cpp")
TEXT_OBJS := $(foreach source, $(TEXT_SRCS), $(OBJPATH)/$(source:.cpp=.o))

DEPENDS_TEXT = ${TEXT_OBJS:.o=.d}

# demo
MAIN_APP_SRCS := $(wildcard example/*.cpp)
MAIN_OBJS := $(foreach source, $(MAIN_APP_SRCS), $(OBJPATH)/$(source:.cpp=.o))
MAIN_OBJS := $(MAIN_OBJS) $(TEXT_OBJS)

DEPENDS = $(DEPENDS_TEXT) ${MAIN_OBJS:.o=.d}

.PHONY: clean

all: main

main: $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) $(MAIN_OBJS) -o $(EXECPATH)/main $(LDLIBS)

$(OBJPATH)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	rm -rf $(OBJPATH)/* $(EXECPATH)/*

-include ${DEPENDS}
