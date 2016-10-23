include definitions.mk

# flags
CPPFLAGS = -std=c++11 -MMD -MP -MT '$@' -MF '$(CONFIG_OUTDIR)/$*.d'
SDL_CPPFLAGS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

LIB_DIRS := $(CONFIG_BINDIR) $(DEPDIR)/lib

ifeq ($(CONFIG),release)
	CPPFLAGS += -O3 -Wall -Wextra
	RELEASE_FLAGS := -static-libgcc -static-libstdc++
else
	CPPFLAGS += -g3 -O0 -DGAME_DEBUG
endif

# include all dependency files
include $(shell dir /s /b $(subst /,\\,$(CONFIG_OUTDIR)/*.d))

# include makefiles
$(call subdir_include,$(DEPDIR)/dll src)

all: jensgame $(G_BINARY_FILES)

# rule for building libgame.dll and libgame.a
$(LIBGAME): $(G_LIBGAME_OBJS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	-@if not exist $(subst /,\\,$(CONFIG_BINDIR)) (mkdir $(subst /,\\,$(CONFIG_BINDIR)))
	-@if exist $(subst /,\\,$(CONFIG_BINDIR)/libgame.dll) (del $(subst /,\\,$(CONFIG_BINDIR)/libgame.dll))
	
	+g++ -o $(CONFIG_BINDIR)/libgame.dll $^ -shared -Wl,--out-implib,$@ $(addprefix -L,$(LIB_DIRS)) $(SDL_CPPFLAGS)

# rule for generating the game executable
$(JENSGAME): $(G_JENSGAME_OBJS) $(LIBGAME)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+g++ -o $@ $(filter %.o,$^) $(addprefix -L,$(LIB_DIRS)) $(RELEASE_FLAGS) -lgame $(SDL_CPPFLAGS)

# rule for generating .o files from .cpp files
$(CONFIG_OUTDIR)/%.o: $(ROOT_SRCDIR)/%.cpp
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+g++ -c $< -o $@ $(addprefix -I,$(G_INC_DIRS)) $(CPPFLAGS)

clean::
	-if exist $(subst /,\\,$(BINDIR)) (rmdir /Q /S $(subst /,\\,$(BINDIR)))
	-if exist $(subst /,\\,$(ROOT_OUTDIR)) (rmdir /Q /S $(subst /,\\,$(ROOT_OUTDIR)))
