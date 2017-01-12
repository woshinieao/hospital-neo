CC=gcc
CXX=g++
AR=ar
LD=gcc
CFLAGS  = -I ../include -I/usr/include
LDFLAGS = -O -Wall -lpthread -lm -L/usr/local/lib -L../bin -L/usr/lib -lfcgi 
TOP_DIR =..
LIB_DIR =$(TOP_DIR)/bin
PROG = cgi.fcgi

CONFIG=debug
ifeq ($(CONFIG), debug)
CC_OPTS+=-g
DEFINE=-DDEBUG
OBJ_DIR =$(LIB_DIR)/debug
else
CC_OPTS+=-O3
DEFINE=
OBJ_DIR =$(LIB_DIR)/release
endif

SOURCES = cgi.c util.c json.c
OBJECTS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCES)) 

target:$(PROG) 
	@echo "+----------------------------------------------------+"
	@echo "+       building cgi program                      +"
	@echo "+----------------------------------------------------+"
	@date
	@echo ""
$(PROG):$(OBJECTS)
	$(CC)  $(CFLAGS)  $^ -o $@ $(LDFLAGS)
#	mv cgi $(OUT_DIR) -f
	cp $(PROG) /var/www/media/cgi-bin/ -f

$(OBJ_DIR)/%.o:%.c
	$(CC) $(CFLAGS) $(CC_OPTS) -c $^ -o $@


clean:
	rm $(OBJECTS) $(PROG) -rf
