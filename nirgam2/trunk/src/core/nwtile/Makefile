TARGET_ARCH = linux

# compiler
CC     = g++ 

# debug flag
DEBUG  = -g
OTHER  = -Wall
LDFLAG = -rdynamic

#CFLAGS = $(DEBUG) $(OTHER) $(LDFLAG)
CFLAGS = $(DEBUG) $(LDFLAG)

all: nwtile.so

nwtile.so: $(NWTILE_OBJS)
	$(CC) -shared -o nwtile.so $(NWTILE_OBJS)

NWTILE_SRC = NWTile.cpp
	
NWTILE_OBJS = $(NWTILE_SRC:.cpp=.o)

.cpp.o:
	$(CC) $(CFLAGS) $(INCDIR) -o $@ -c $<

.cc.o:
	$(CC) $(CFLAGS) $(INCDIR) -o $@ -c $<