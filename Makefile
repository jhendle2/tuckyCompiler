CC:=gcc
CFLAGS:=-Wall

SRC:=./src
OBJ:=./obj

SRCS:=$(wildcard $(SRC)/*.c)
OBJS:=$(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
HDRS:=$(wildcard $(SRC)/*.h)

INCLUDE:=-I$(SRC)
APP:=tucky

$(OBJ)/%.o: $(SRC)/%.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDE) -g -O0 -c -o $@ $<

$(APP): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDE) -g -O0    -o $@ $(OBJS)

$(APP)-release:
	$(CC) $(CFLAGS) $(INCLUDE)    -O2    -o $@ $(SRCS)

EXAMPLE:=./examples/hello_world.c
# EXAMPLE:=./examples/functions.c
# EXAMPLE:=./examples/tokens.c
# EXAMPLE:=./src/main.c
test: $(APP)
# ./$(APP) -g --werror -f $(EXAMPLE) -o hello_world
	./$(APP) -f $(EXAMPLE) -o $(patsubst %.c,%,$(EXAMPLE))
	qbe -o $(patsubst %.c,%,$(EXAMPLE)).s $(patsubst %.c,%,$(EXAMPLE)).ssa
	cc -o $(patsubst %.c,%,$(EXAMPLE)) $(patsubst %.c,%,$(EXAMPLE)).s
	./$(patsubst %.c,%,$(EXAMPLE))

clean:
	rm -rf $(OBJ)/*.o $(APP)

valgrind: $(APP)
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(APP) -g --werror -f $(EXAMPLE) -o hello_world