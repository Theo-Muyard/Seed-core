# | ================================================ |
# 			NAMES
# | ================================================ |

NAME		=	seed_test
BUILD_DIR	=	build/tests
SEED_ARCHIVE	=	seed_core.a

# | ================================================ |
# 			COMPILATION
# | ================================================ |

CC		=	cc
CFLAGS		=	-Wall -Wextra -Werror

# | ================================================ |
# 			INCLUDES
# | ================================================ |
INCLUDES	=	-I includes -I tests

# | ================================================ |
# 			SOURCE FILES
# | ================================================ |

TESTS_SRC	=	tests/tools.c

MANAGER_SRC	=	tests/core/TEST_manager.c

DISPATCHER_SRC	=	tests/core/TEST_dispatcher.c

WRITING_SRC	=	tests/systems/writing/TEST_writing_internal.c \
			tests/systems/writing/TEST_writing_commands.c \
			tests/systems/writing/TEST_writing_main.c

FS_SRC		=	tests/systems/filesystem/TEST_fs.c


# | ================================================ |
# 			OBJ FILES
# | ================================================ |

TEST_OBJ	=	$(addprefix $(BUILD_DIR)/, $(notdir $(TESTS_SRC:.c=.o)))

MANAGER_OBJ	=	$(addprefix $(BUILD_DIR)/, $(notdir $(MANAGER_SRC:.c=.o)))

DISPATCHER_OBJ	=	$(addprefix $(BUILD_DIR)/, $(notdir $(DISPATCHER_SRC:.c=.o)))

WRITING_OBJ	=	$(addprefix $(BUILD_DIR)/, $(notdir $(WRITING_SRC:.c=.o)))

FS_OBJ		=	$(addprefix $(BUILD_DIR)/, $(notdir $(FS_SRC:.c=.o)))

# | ================================================ |
# 			COLORS / WIDTH
# | ================================================ |

COL_WIDTH	=	30
RED		=	\033[31m
GREEN		=	\033[32m
BLUE		=	\033[34m
WHITE		=	\033[37m

# | ================================================ |
# 			COMPILE FUNCTION
# | ================================================ |

define COMPILE_OBJ
$(BUILD_DIR)/$(notdir $(1:.c=.o)): $(1) | $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $$< -o $$@ $(INCLUDES)
	@printf "$(BLUE)%-$(COL_WIDTH)s$(WHITE): ✔️\n" "$$(notdir $$@)"
endef

# | ================================================ |
# 			MAKE RULE
# | ================================================ |

all:
	@echo "$(BLUE)Usage$(WHITE): make test TARGET=<target>"

# | ================================================ |
# 			RUN RULE
# | ================================================ |

run:
	./$(NAME)

# | ================================================ |
# 			TEST TARGETS
# | ================================================ |

manager: $(TEST_OBJ) $(MANAGER_OBJ)
	@$(CC) $(CFLAGS) $(TEST_OBJ) $(MANAGER_OBJ) $(SEED_ARCHIVE) -o $(NAME)
	@echo "$(GREEN)Done$(WHITE)."

dispatcher: $(TEST_OBJ) $(DISPATCHER_OBJ)
	@$(CC) $(CFLAGS) $(TEST_OBJ) $(DISPATCHER_OBJ) $(SEED_ARCHIVE) -o $(NAME)
	@echo "$(GREEN)Done$(WHITE)."

writing: $(TEST_OBJ) $(WRITING_OBJ)
	@$(CC) $(CFLAGS) $(TEST_OBJ) $(WRITING_OBJ) $(SEED_ARCHIVE) -o $(NAME)
	@echo "$(GREEN)Done$(WHITE)."

fs: $(TEST_OBJ) $(FS_OBJ)
	@$(CC) $(CFLAGS) $(TEST_OBJ) $(FS_OBJ) $(SEED_ARCHIVE) -o $(NAME)
	@echo "$(GREEN)Done$(WHITE)."

# | ================================================ |
# 			DIRECTORY
# | ================================================ |

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@printf "$(BLUE)%-$(COL_WIDTH)s$(WHITE): ✔️\n" "$(BUILD_DIR) (directory)"

# | ================================================ |
# 			OBJECTS
# | ================================================ |

$(foreach src, $(TESTS_SRC), $(eval $(call COMPILE_OBJ,$(src))))
$(foreach src, $(MANAGER_SRC), $(eval $(call COMPILE_OBJ,$(src))))
$(foreach src, $(DISPATCHER_SRC), $(eval $(call COMPILE_OBJ,$(src))))
$(foreach src, $(WRITING_SRC), $(eval $(call COMPILE_OBJ,$(src))))
$(foreach src, $(FS_SRC), $(eval $(call COMPILE_OBJ,$(src))))

.PHONY: all run manager dispatcher writing