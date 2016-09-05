.PHONY := all clean inc
.PRECIOUS:
.SUFFIXES:

# Directory definitions
BINDIR := bin
INCDIR := include
LIBDIR := lib
SRCDIR_ROOT := src
OUTDIR_ROOT := out

DEPDIR := dep
SCRIPTSDIR := scripts
RESDIR := resources

CONF_BINDIR := $(BINDIR)/$(CONFIG)
CONF_LIBDIR := $(LIBDIR)/$(CONFIG)
CONF_OUTDIR := $(OUTDIR_ROOT)/$(CONFIG)

# Target variables
LIBGAME := $(CONF_LIBDIR)/libgame.a
JENSGAME := $(CONF_BINDIR)/Game.exe

# Compiler flags
DEFAULT_INC_DIRS := $(INCDIR) $(DEPDIR)/include
DEFAULT_CPP_FLAGS = -std=c++11 -MMD -MP -MT '$@' -MF '$(CONF_OUTDIR)/$*.d'

SDL_LIBS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
DEFAULT_LIB_DIRS := $(LIBDIR) $(CONF_LIBDIR)

# Compiler flags based on configuration
ifeq ($(CONFIG),debug)
	DEFAULT_CPP_FLAGS += -g3 -O0
else
	DEFAULT_CPP_FLAGS += -O3 -Wall -Wextra
endif

# Global variables
G_INC_HEADERS :=
G_LIBGAME_OBJS :=

# Misc variables
DIRECTORY_GUARD = -@if not exist $(subst /,\\,$(dir $@)) (mkdir $(subst /,\\,$(dir $@)))
TARGET_GUARD = -@if exist $(subst /,\\,$@) (del $(subst /,\\,$@))

# Variable conversion
DEFAULT_CPP_FLAGS += $(addprefix -I,$(DEFAULT_INC_DIRS))
DEFAULT_LD_FLAGS := $(addprefix -L,$(DEFAULT_LIB_DIRS)) $(SDL_LIBS)

# Macros
define _include_header
G_INC_HEADERS += $$(addprefix $(INCDIR)/,$(2))

$(INCDIR)/$(2): $(1)/$(2)
	$$(DIRECTORY_GUARD)
	$$(TARGET_GUARD)
	+mklink /H $$(subst /,\\,$$@) $$(subst /,\\,$$<) >NUL
endef

define _subdir_include
PSRCDIR-$(1) := $$(SRCDIR)
POUTDIR-$(1) := $$(OUTDIR)

ifeq ($(1),$(SRCDIR_ROOT))
SRCDIR := $(SRCDIR_ROOT)
OUTDIR := $(CONF_OUTDIR)
else
SRCDIR := $$(SRCDIR)/$(1)
OUTDIR := $$(OUTDIR)/$(1)
endif

INC_HEADERS :=
LIBGAME_OBJS :=
SUBDIRS :=

include $$(SRCDIR)/Makefile

$$(foreach incHeader,$$(INC_HEADERS),$$(eval $$(call _include_header,$$(SRCDIR),$$(incHeader))))
G_LIBGAME_OBJS += $$(addprefix $$(OUTDIR)/,$$(LIBGAME_OBJS))
$$(call subdir_include,$$(SUBDIRS))

SRCDIR := $$(PSRCDIR-$(1))
OUTDIR := $$(POUTDIR-$(1))
endef

define subdir_include
$(foreach subDir,$(1),$(eval $(call _subdir_include,$(subDir))))
endef

$(call subdir_include,src)
include $(wildcard $(OUTDIR_ROOT)/**/*.d)

all: $(LIBGAME) $(JENSGAME)

inc: $(G_INC_HEADERS)

# Rule for building libgame.dll and libgame.a
$(LIBGAME): $(G_LIBGAME_OBJS) $(SDL_LIB_TARGETS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	-@if not exist $(subst /,\\,$(CONF_BINDIR)) (mkdir $(subst /,\\,$(CONF_BINDIR)))
	-@if exist $(subst /,\\,$(CONF_BINDIR)/libgame.dll) (del $(subst /,\\,$(CONF_BINDIR)/libgame.dll))
	
	+g++ -o $(CONF_BINDIR)/libgame.dll $^ -shared -Wl,--out-implib,$@ $(addprefix -L,$(DEFAULT_LIB_DIRS)) $(SDL_LIBS)

$(G_LIBGAME_OBJS): | inc

# Rule for generating .o files from .cpp files
$(CONF_OUTDIR)/%.o: $(SRCDIR_ROOT)/%.cpp
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+g++ -c $< -o $@ $(DEFAULT_CPP_FLAGS) $(CUSTOM_CPP_FLAGS)

# Rule for moving dependency libraries to lib directory
$(LIBDIR)/%.lib: $(DEPDIR)/lib/%.lib
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+copy $(subst /,\\,$<) $(subst /,\\,$@) >NUL

clean::
	-if exist $(subst /,\\,$(BINDIR)) (rmdir /Q /S $(subst /,\\,$(BINDIR)))
	-if exist $(subst /,\\,$(INCDIR)) (rmdir /Q /S $(subst /,\\,$(INCDIR)))
	-if exist $(subst /,\\,$(LIBDIR)) (rmdir /Q /S $(subst /,\\,$(LIBDIR)))
	-if exist $(subst /,\\,$(OUTDIR_ROOT)) (rmdir /Q /S $(subst /,\\,$(OUTDIR_ROOT)))
