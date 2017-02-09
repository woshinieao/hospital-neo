
CC=arm-none-linux-gnueabihf-gcc
CXX=arm-none-linux-gnueabihf-g++
AR=arm-none-linux-gnueabihf-ar
LD=arm-none-linux-gnueabihf-gcc

VERSION = 1.0

#¹¤×÷Ä¿Â¼
TOP_DIR =..
LIB_DIR =$(TOP_DIR)/bin
TARGET  = libcgi.so
TARGETA = libcgi.a

CFLAGS  = -I ../include -fPIC
LDFLAGS = -O -Wall -lpthread 
DEMO_LDFLAGS =-lpthread

#CONFIG=debug
ifeq ($(CONFIG), debug)
CC_OPTS+=-g
DEFINE=-DDEBUG
OBJ_DIR =$(LIB_DIR)/debug
else
CC_OPTS+=-O3
DEFINE=
OBJ_DIR =$(LIB_DIR)/release
endif

SOURCES = fcgi_stdio.c fcgiapp.c os_unix.c json.c

OBJECTS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCES)) 

target: $(TARGET) $(TARGETA)
	@echo "+----------------------------------------------------+"
	@echo "+       building libcgi                              +"
	@echo "+----------------------------------------------------+"
	@date
	@echo ""
	
$(TARGET):$(OBJECTS)
	$(CC)  -shared -fPIC $^ -o $@  $(LIBS)

$(TARGETA):$(OBJECTS)
	$(AR) rcu $@ $^  
	cp $(TARGET) $(LIB_DIR) -f
	
$(OBJ_DIR)/%.o:%.c
	$(CC) $(CFLAGS) $(CC_OPTS) $(DEFINE) -c $^ -o $@
	
$(OBJ_DIR)/fcgio.o:fcgio.cpp
	$(CXX) $(CFLAGS) $(CC_OPTS) $(DEFINE) -c $^ -o $@

clean:
	rm -rf $(OBJ_DIR)/*.o $(LIB_DIR)/$(TARGET)

