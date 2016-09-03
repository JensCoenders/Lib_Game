.PHONY += libgame

LIBGAME_DIR := libgame
LIBGAME_SRCDIR := $(SRC_DIR)/$(LIBGAME_DIR)
LIBGAME_OUTDIR := $(OUT_DIR)/$(LIBGAME_DIR)

# Get sources and set objects
LIBGAME_SRCS := $(wildcard $(LIBGAME_SRCDIR)/*.cpp) $(LIBGAME_SRCDIR)/shm/shm_generated.cpp
LIBGAME_OBJS := $(subst $(LIBGAME_SRCDIR),$(LIBGAME_OUTDIR),$(patsubst %.cpp,%.o,$(LIBGAME_SRCS)))

libgame: $(LIB_DIR)/$(CONFIG)/libgame.a

# Rule for building libgame.dll and libgame.a
$(LIB_DIR)/$(CONFIG)/libgame.a: $(LIBGAME_OBJS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	-@if not exist $(subst /,\\,$(BIN_DIR)) (mkdir $(subst /,\\,$(BIN_DIR)))
	-@if exist $(subst /,\\,$(BIN_DIR)/libgame.dll) (del $(subst /,\\,$(BIN_DIR)/libgame.dll))
	
	+g++ -o $(BIN_DIR)/libgame.dll $^ -shared -Wl,--out-implib,$@ $(DEFAULT_LD_FLAGS) $(DEFAULT_LD_LIBS)

$(LIBGAME_OBJS): $(LIBGAME_SRCDIR)/shm/shm_generated.h

# Rule for generating shm_generated.h and shm_generated.cpp
$(LIBGAME_SRCDIR)/shm/shm_generated.h $(LIBGAME_SRCDIR)/shm/shm_generated.cpp: $(LIBGAME_SRCDIR)/shm/shm.dat
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+java -jar scripts/ShmGenerator.jar --input $(LIBGAME_SRCDIR)/shm/shm.dat --output-prefix shm_generated \
		--output-dir $(LIBGAME_SRCDIR)/shm
	
clean::
	-if exist $(LIBGAME_SRCDIR)\\shm\\shm_generated.h) (del /Q $(LIBGAME_SRCDIR)\\shm\\shm_generated.h)
	-if exist $(LIBGAME_SRCDIR)\\shm\\shm_generated.cpp) (del /Q $(LIBGAME_SRCDIR)\\shm\\shm_generated.cpp)
	-if exist $(LIB_DIR)\\$(CONFIG)\\libgame.a) (del /Q $(LIB_DIR)\\$(CONFIG)\\libgame.a)