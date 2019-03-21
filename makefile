GCC			= gcc
FLGS		= -Wextra -g

SRC_DIR	= src
OBJ_DIR	= obj
INCLUDE	= lib

SRC	= $(wildcard $(SRC_DIR)/*.c)
LIB	= $(wildcard $(INCLUDE)/*.h)
OBJ	= $(subst src,obj,$(patsubst %.c,%.o,$(SRC)))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(GCC) $(FLGS) -c $^ -I\$(INCLUDE)/ -o $@

detect: $(OBJ)
	$(GCC) $(FLGS) $^ -o $@

clean:
	$(RM) $(OBJ)