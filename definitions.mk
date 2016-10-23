.PHONY := all clean inc
.PRECIOUS:
.SUFFIXES:

# general directories
BINDIR := bin
SRCDIR :=
OUTDIR :=
ROOT_SRCDIR := src
ROOT_OUTDIR := out

DEPDIR := dep
RESDIR := resources
SCRIPTSDIR := scripts

# automatic directory variables
CONFIG_BINDIR := $(BINDIR)/$(CONFIG)
CONFIG_OUTDIR := $(ROOT_OUTDIR)/$(CONFIG)

# global variables
G_BINARY_FILES :=
G_INC_DIRS := $(DEPDIR)/include 
G_LIBGAME_OBJS :=
G_JENSGAME_OBJS :=

# targets
JENSGAME := $(CONFIG_BINDIR)/Game.exe
LIBGAME := $(CONFIG_BINDIR)/libgame.a

# misc variables
DIRECTORY_GUARD = -@if not exist $(subst /,\\,$(dir $@)) (mkdir $(subst /,\\,$(dir $@)))
TARGET_GUARD = -@if exist $(subst /,\\,$@) (del $(subst /,\\,$@))

# _copy_file macro: $(1) = name of global variable, $(2) = target directory, $(3) = source directory, $(4) = file name
define _copy_file
$(1) += $$(addprefix $(2)/,$(4))

$(2)/$(4): $(3)/$(4)
	$$(DIRECTORY_GUARD)
	$$(TARGET_GUARD)
	+copy $$(subst /,\\,$$<) $$(dir $$(subst /,\\,$$@)) >NUL
endef

# _subdir_include macro: $(1) = new directory (subdirectory)
define _subdir_include
# set local directories
PSRCDIR-$(1) := $$(SRCDIR)
POUTDIR-$(1) := $$(OUTDIR)

ifeq ($$(SRCDIR),)
SRCDIR := $(1)
OUTDIR := $(CONFIG_OUTDIR)
else
SRCDIR := $$(SRCDIR)/$(1)
OUTDIR := $$(OUTDIR)/$(1)
endif

# reset local variables
BINARY_FILES :=
INC_DIRS :=
LIBGAME_OBJS :=
JENSGAME_OBJS :=
SUBDIRS :=

# include subdirectory makefile
include $$(SRCDIR)/Makefile

# process local variables
$$(foreach file,$$(BINARY_FILES),$$(eval $$(call _copy_file,G_BINARY_FILES,$$(CONFIG_BINDIR),$$(SRCDIR),$$(file))))
$$(foreach directory,$$(INC_DIRS),$$(eval G_INC_DIRS += $$(PSRCDIR-$(1))/$$(directory)))
$$(foreach obj,$$(LIBGAME_OBJS),$$(eval G_LIBGAME_OBJS += $$(OUTDIR)/$$(obj)))
$$(foreach obj,$$(JENSGAME_OBJS),$$(eval G_JENSGAME_OBJS += $$(OUTDIR)/$$(obj)))

$$(call subdir_include,$$(SUBDIRS))

# reset local directories
SRCDIR := $$(PSRCDIR-$(1))
OUTDIR := $$(POUTDIR-$(1))
endef

# subdir_include macro: $(1) = space-seperated list of subdirectories to include
define subdir_include
$(foreach directory,$(1),$(eval $(call _subdir_include,$$(directory))))
endef