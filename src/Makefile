# Get sources and set objects
GAME_EXEC_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
GAME_EXEC_OBJS := $(subst $(SRC_DIR),$(OUT_DIR),$(patsubst %.cpp,%.o,$(GAME_EXEC_SRCS))) $(OUT_DIR)/resources.o

GAME_VERSION_FILE := $(SRC_DIR)/testgame.h

# Rule for generating game executable
$(BIN_DIR)/Game.exe: $(GAME_EXEC_OBJS) $(LIB_DIR)/$(CONFIG)/libgame.a $(SDL_DLLS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+g++ -o $@ $(filter %.o,$^) $(DEFAULT_LD_FLAGS) -lgame $(DEFAULT_LD_LIBS)

# Rule for updating version in resource file
$(RES_DIR)/resources.rc: $(GAME_VERSION_FILE)
#	+powershell.exe -File "scripts\update_version.ps1" -ResourceFile $@ -VersionFile $<

# Rule for building resource file
$(OUT_DIR)/resources.o: $(RES_DIR)/resources.rc
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+windres $(subst /,\\,$<) -o $(subst /,\\,$@)
	
# Rule for copying DLL files to bin directory
$(BIN_DIR)/%.dll: $(RES_DIR)/dll/%.dll
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+copy $(subst /,\\,$<) $(subst /,\\,$@) >NUL

include $(SRC_DIR)/libgame/Makefile