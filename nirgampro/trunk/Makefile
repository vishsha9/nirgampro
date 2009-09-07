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

CORE_SRCS = ${shell ls src/core/*.cpp} \
	${shell ls src/core/*/*.cpp}

ROUTER_SRCS = ${shell ls src/router/*.cpp}

WIRE_SRCS = ${shell ls src/wireModel/*.cpp}

MAIN_SRCS = src/main.cpp \
	src/default.cpp
	
CORE_OBJS = $(CORE_SRCS:.cpp=.o)

APP_OBJS = $(APP_SRCS:.cpp=.o)

ROUTER_OBJS = $(ROUTER_SRCS:.cpp=.o)

MAIN_OBJS = ${MAIN_SRCS:.cpp=.o}

WIRE_OBJS= ${WIRE_SRCS:.cpp=.o}

#APP_LIB = $(subst src,lib,$(APP_OBJS:.o=.so))

#ROUTER_LIB = $(subst src,lib,$(ROUTER_OBJS:.o=.so))

include Makefile.defs
# DO NOT DELETE
