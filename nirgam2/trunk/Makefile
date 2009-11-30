# makefile

# architecture
TARGET_ARCH = linux

# compiler
CC     = g++ 

# debug flag
DEBUG  = -g
OTHER  = -Wall
LDFLAG = -rdynamic

#CFLAGS = $(DEBUG) $(OTHER) $(LDFLAG)
CFLAGS = $(DEBUG) $(LDFLAG)

MODULE = nirgam

APP_SRCS = ${shell ls src/app/*.cpp}

CORE_SRCS = ${shell ls src/core/*.cpp}	\
			${shell ls src/core/tracker/*.cpp}

TILECOM = 	src/core/nwtile/Controller.cpp \
			src/core/nwtile/fifo.cpp	\
			src/core/nwtile/InputChannel.cpp	\
			src/core/nwtile/OutputChannel.cpp	\
			src/core/nwtile/VCAllocator.cpp	

TILE_SRCS = $(TILECOM)	\
			src/core/nwtile/NWTile.cpp

ROUTER_SRCS = ${shell ls src/router/*.cpp}

WIRE_SRCS = ${shell ls src/wireModel/*.cpp}

MAIN_SRCS = src/main.cpp \
	src/global.cpp	\
	src/config.cpp	\
	src/dirtool.cpp	\
	src/cfgLoader.cpp
	
CORE_OBJS = ${CORE_SRCS:.cpp=.o}

TILE_OBJS = ${TILE_SRCS:.cpp=.o}

APP_OBJS = ${APP_SRCS:.cpp=.o}

ROUTER_OBJS = ${ROUTER_SRCS:.cpp=.o}

MAIN_OBJS = ${MAIN_SRCS:.cpp=.o}

WIRE_OBJS= ${WIRE_SRCS:.cpp=.o}

#APP_LIB = $(subst src,lib,$(APP_OBJS:.o=.so))

#ROUTER_LIB = $(subst src,lib,$(ROUTER_OBJS:.o=.so))

include Makefile.defs
# DO NOT DELETE
