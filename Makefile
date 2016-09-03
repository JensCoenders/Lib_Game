.PHONY := all libgame
.PRECIOUS:
.SUFFIXES:

# Directory definitions
BIN_DIR := bin/$(CONFIG)
SRC_DIR := src
OUT_DIR := out/$(CONFIG)
INC_DIR := include
LIB_DIR := lib
RES_DIR := resources

# Misc variables
GENERATED_DIRS := $(BIN_DIR) $(LIB_DIR) $(OUT_DIR)
SDL_DLLS := $(addprefix $(BIN_DIR)/,$(notdir $(wildcard $(RES_DIR)/dll/*.dll)))

DIRECTORY_GUARD = -@if not exist $(subst /,\\,$(dir $@)) (mkdir $(subst /,\\,$(dir $@)))
TARGET_GUARD = -@if exist $(subst /,\\,$@) (del $(subst /,\\,$@))

# Compiler flags
DEFAULT_INCLUDE_DIRS := $(INC_DIR)
DEFAULT_CC_FLAGS := -std=c++11
DEFAULT_DEP_FLAGS = -MMD -MP -MT '$@' -MF '$(OUT_DIR)/$*.d'

DEFAULT_LD_LIBS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
DEFAULT_LD_LIB_DIRS := $(LIB_DIR) $(LIB_DIR)/$(CONFIG)

# Variable conversion
DEFAULT_CC_FLAGS := $(addprefix -I,$(DEFAULT_INCLUDE_DIRS)) $(DEFAULT_CC_FLAGS)
DEFAULT_LD_FLAGS := $(addprefix -L,$(DEFAULT_LD_LIB_DIRS))

# Compiler flags based on configuration
ifeq ($(CONFIG),debug)
	DEFAULT_CC_FLAGS += -g3 -O0
else
	DEFAULT_CC_FLAGS += -O3 -Wall -Wextra
endif

all: inc
	$(MAKE) $(LIB_DIR)/$(CONFIG)/libgame.a $(BIN_DIR)/Game.exe

clean::
	-if exist bin\\$(CONFIG) (rmdir /Q /S bin\\$(CONFIG))
	-if exist lib\\$(CONFIG) (rmdir /Q /S lib\\$(CONFIG))
	-if exist out\\$(CONFIG) (rmdir /Q /S out\\$(CONFIG))

# Commandline shorthand targets
jensgame: $(BIN_DIR)/Game.exe

# Rule for generating .o files from .cpp files
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	
	+g++ -c $< -o $@ $(DEFAULT_CC_FLAGS) $(DEFAULT_DEP_FLAGS) $(CUSTOM_CC_FLAGS)

$(GENERATED_DIRS):
	+mkdir $@

include $(SRC_DIR)/Makefile
-include $(wildcard $(OUT_DIR)/**/*.d)