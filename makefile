GCC			= gcc
FLGS		= -Wall -g

SRC_DIR	= src
OBJ_DIR	= obj
INCLUDE	= lib

SRC	= $(wildcard $(SRC_DIR)/*.c)
LIB	= $(wildcard $(INCLUDE)/*.h)
OBJ	= $(patsubst %.c,%.o,$(SRC))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(GCC) $(FLGS) -c $^ -o $@

detect: $(OBJ)
	$(GCC) $(FLGS) $^ -o $@

clean:
	$(RM) $(OBJ)