# | ================================================ |
# 			NAMES
# | ================================================ |

NAME		=	seed_core.a
TEST		=	seed_test
BUILD_DIR	=	build

# | ================================================ |
# 			COMPILATION
# | ================================================ |

CC		=	cc
CFLAGS		=	-Wall -Wextra -Werror -g3
AR		=	ar
FLAGS		=	rcs

# | ================================================ |
# 			INCLUDES
# | ================================================ |

INCLUDES	=	-I includes

# | ================================================ |
# 			SRC FILES
# | ================================================ |

SRC		=	common/memory.c \
			common/systems_registry.c \
\
			core/dispatcher.c \
			core/manager.c \
\
			systems/writing/writing_commands.c \
			systems/writing/writing_internal.c \
			systems/writing/writing_system.c \
\
			systems/filesystem/fs_commands.c \
			systems/filesystem/fs_common.c \
			systems/filesystem/fs_os.c \
			systems/filesystem/fs_system.c \
			systems/filesystem/fs_vfs.c \
			systems/filesystem/fs_watcher_internal.c \
			systems/filesystem/fs_watcher.c \

# | ================================================ |
# 			OBJ FILES
# | ================================================ |

OBJ		=	$(addprefix $(BUILD_DIR)/, $(SRC:.c=.o))

# | ================================================ |
# 			COLORS
# | ================================================ |

COL_WIDTH	=	45
RED		=	\033[31m
GREEN		=	\033[32m
BLUE		=	\033[34m
WHITE		=	\033[37m

# | ================================================ |
# 			MAKE RULE
# | ================================================ |

all: $(NAME)

$(NAME): $(BUILD_DIR) $(OBJ)
	@$(AR) $(FLAGS) $(NAME) $(OBJ)
	@echo -e "$(GREEN)Done$(WHITE)."

# | ================================================ |
# 			CLEAN RULE
# | ================================================ |

clean:
	@rm -rf $(BUILD_DIR)
	@echo -e "$(RED)Cleaned$(WHITE)."

# | ================================================ |
# 			FCLEAN RULE
# | ================================================ |

fclean:
	@rm -rf $(BUILD_DIR) $(NAME) $(TEST)
	@echo -e "$(RED)Fcleaned$(WHITE)."

# | ================================================ |
# 			RE RULE
# | ================================================ |

re: fclean all

# | ================================================ |
# 			TEST RULES
# | ================================================ |

test:
	@$(MAKE) -s $(TARGET) -f tests.mk

# | ================================================ |
# 			DIRECTORY
# | ================================================ |

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@printf "$(BLUE)%-$(COL_WIDTH)s$(WHITE): ✔️\n" "$(BUILD_DIR) (directory)"

# | ================================================ |
# 			OBJECTS
# | ================================================ |

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)
	@printf "$(BLUE)%-$(COL_WIDTH)s$(WHITE): ✔️\n" "$(patsubst $(BUILD_DIR)/%,%,$@)"

.PHONY: all clean fclean re test