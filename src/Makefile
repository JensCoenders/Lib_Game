# Get sources and set objects
GAME_EXEC_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
GAME_EXEC_OBJS := $(subst $(SRC_DIR),$(OUT_DIR),$(patsubst %.cpp,%.o,$(GAME_EXEC_SRCS))) $(OUT_DIR)/resources.o

# Rule for generating game executable
$(BIN_DIR)/Game.exe: $(GAME_EXEC_OBJS) $(BIN_DIR)/libgame.dll $(SDL_DLLS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	
	+g++ -o $@ $(filter %.o,$^) $(DEFAULT_LD_FLAGS) -lgame $(DEFAULT_LD_LIBS)
	
# Rule for generating windows resource files
$(OUT_DIR)/%.o: $(RES_DIR)/%.rc
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	
	+windres $(subst /,\\,$<) -o $(subst /,\\,$@)
	
# Rule for copying DLL files to bin directory
$(BIN_DIR)/%.dll: $(RES_DIR)/dll/%.dll
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	
	+copy $(subst /,\\,$<) $(subst /,\\,$@) >NUL

include $(SRC_DIR)/libgame/Makefile