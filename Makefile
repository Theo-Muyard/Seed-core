# | ================================================ |
# 						NAMES
# | ================================================ |

NAME		=	seed_core.a
TEST		=	seed_test
BUILD_DIR	=	build

# | ================================================ |
# 					COMPILATION
# | ================================================ |

CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -g3
AR			=	ar
FLAGS		=	rcs

# | ================================================ |
# 					INCLUDES
# | ================================================ |

INCLUDES	=	-I includes #-I includes/core -I includes/systems/writing

# | ================================================ |
# 					SOURCE FILES
# | ================================================ |

SRC			=	core/manager.c \
				core/dispatcher.c \
\
				tools/memory.c \
				tools/systems.c \
\
				systems/writing/_internal.c \
				systems/writing/commands.c \
				systems/writing/system.c \
\
				systems/filesystem/_internal.c \
				systems/filesystem/_os.c \
				systems/filesystem/commands.c \
				systems/filesystem/system.c \

# | ================================================ |
# 					OBJ FILES
# | ================================================ |

OBJ			=	$(addprefix $(BUILD_DIR)/, $(SRC:.c=.o))

# | ================================================ |
# 					COLORS / WIDTH
# | ================================================ |

COL_WIDTH	=	35
RED			=	\033[31m
GREEN		=	\033[32m
BLUE		=	\033[34m
WHITE		=	\033[37m

# | ================================================ |
# 					MAKE RULE
# | ================================================ |

all: $(NAME)

$(NAME): $(BUILD_DIR) $(OBJ)
	@$(AR) $(FLAGS) $(NAME) $(OBJ)
	@echo "$(GREEN)Done$(WHITE)."

# | ================================================ |
# 					CLEAN RULE
# | ================================================ |

clean:
	@rm -rf $(BUILD_DIR)
	@echo "$(RED)Cleaned$(WHITE)."

# | ================================================ |
# 					FCLEAN RULE
# | ================================================ |

fclean:
	@rm -rf $(BUILD_DIR) $(NAME) $(TEST)
	@echo "$(RED)Fcleaned$(WHITE)."

# | ================================================ |
# 					RE RULE
# | ================================================ |

re: fclean all

# | ================================================ |
# 					TESTS RULES
# | ================================================ |

test:
	@$(MAKE) -s $(TARGET) -f tests.mk

# | ================================================ |
# 					DIRECTORY
# | ================================================ |

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@printf "$(BLUE)%-$(COL_WIDTH)s$(WHITE): ✔️\n" "$(BUILD_DIR) (directory)"

# | ================================================ |
# 					OBJECTS
# | ================================================ |

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)
	@printf "$(BLUE)%-$(COL_WIDTH)s$(WHITE): ✔️\n" "$(patsubst $(BUILD_DIR)/%,%,$@)"

.PHONY: all clean fclean re test