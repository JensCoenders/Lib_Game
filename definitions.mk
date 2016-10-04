.PHONY := all clean inc
.PRECIOUS:
.SUFFIXES:

# general directories
BINDIR := bin
INCDIR := include
LIBDIR := lib
ROOT_SRCDIR := src
ROOT_OUTDIR := out

DEPDIR := dep
RESDIR := resources
SCRIPTSDIR := scripts

# automatic directory variables
CONFIG_BINDIR := $(BINDIR)/$(CONFIG)
CONFIG_LIBDIR := $(LIBDIR)/$(CONFIG)
CONFIG_OUTDIR := $(ROOT_OUTDIR)/$(CONFIG)

# target variables
JENSGAME := $(CONFIG_BINDIR)/Game.exe
LIBGAME := $(CONFIG_LIBDIR)/libgame.a

# global variables
G_INC_HEADERS :=
G_LIBGAME_OBJS :=

# compiler flags
INC_DIRS := $(INCDIR) $(DEPDIR)/include
CPPFLAGS = -std=c++11 -MMD -MP -MT '$@' -MF '$(CONFIG_OUTDIR)/$*.d'
LIB_DIRS := $(LIBDIR) $(DEPDIR)/lib

ifeq ($(CONFIG),debug)
	CPPFLAGS += -g3 -O0 -DGAME_DEBUG
 else
	CPPFLAGS += -O3 -Wall -Wextra
 endif

# misc variables
DIRECTORY_GUARD = -@if not exist $(subst /,\\,$(dir $@)) (mkdir $(subst /,\\,$(dir $@)))
TARGET_GUARD = -@if exist $(subst /,\\,$@) (del $(subst /,\\,$@))
SDL_FLAGS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# _copy_file macro: $(1) = name of global variable, $(2) = target directory, $(3) = source directory, $(4) = file name
define _copy_file
$(1) += $$(addprefix $(2)/,$(4))

$(2)/$(4): $(3)/$(4)
	$$(DIRECTORY_GUARD)
	$$(TARGET_GUARD)
	+mklink /H $$(subst /,\\,$$@) $$(subst /,\\,$$<) >NUL
endef

# _subdir_include macro: $(1) = new directory (subdirectory)
define _subdir_include
# set local directories
PSRCDIR-$(1) := $$(SRCDIR)
POUTDIR-$(1) := $$(OUTDIR)

ifeq ($(1),$(ROOT_SRCDIR))
SRCDIR := $(ROOT_SRCDIR)
OUTDIR := $(CONFIG_OUTDIR)
else
SRCDIR := $$(SRCDIR)/$(1)
OUTDIR := $$(OUTDIR)/$(1)
endif

# reset local variables
INC_HEADERS :=
LIBGAME_OBJS :=
SUBDIRS :=

# include subdirectory makefile
include $$(SRCDIR)/Makefile

# process local variables
$$(foreach header,$$(INC_HEADERS),$$(eval $$(call _copy_file,G_INC_HEADERS,$$(INCDIR),$$(SRCDIR),$$(header))))
$$(foreach obj,$$(LIBGAME_OBJS),$$(eval G_LIBGAME_OBJS += $$(OUTDIR)/$$(obj)))

$$(call subdir_include,$$(SUBDIRS))

# reset local directories
SRCDIR := $$(PSRCDIR-$(1))
OUTDIR := $$(POUTDIR-$(1))
endef

# subdir_include macro: $(1) = space-seperated list of subdirectories to include
define subdir_include
$(foreach directory,$(1),$(eval $(call _subdir_include,$$(directory))))
endef