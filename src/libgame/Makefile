LIBGAME_DIR := libgame
LIBGAME_SRCDIR := $(SRC_DIR)/$(LIBGAME_DIR)
LIBGAME_OUTDIR := $(OUT_DIR)/$(LIBGAME_DIR)

# Get sources and set objects
LIBGAME_SRCS := $(wildcard $(LIBGAME_SRCDIR)/*.cpp)
LIBGAME_OBJS := $(subst $(LIBGAME_SRCDIR),$(LIBGAME_OUTDIR),$(patsubst %.cpp,%.o,$(LIBGAME_SRCS)))

# Rule for building libgame.dll and libgame.a
$(BIN_DIR)/libgame.dll: $(LIBGAME_OBJS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	-@if not exist lib\\$(CONFIG) (mkdir lib\\$(CONFIG))
	-@if exist $(LIB_DIR)/libgame.a (del $(LIB_DIR)/libgame.a)
	
	+g++ -o $@ $^ -shared -Wl,--out-implib,$(LIB_DIR)/libgame.a $(DEFAULT_LD_FLAGS) $(DEFAULT_LD_LIBS)
