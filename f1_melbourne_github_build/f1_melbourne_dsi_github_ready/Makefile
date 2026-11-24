# Nintendo DS / DSi homebrew Makefile
.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM is not set. Use the devkitPro devkitARM environment.")
endif

include $(DEVKITARM)/ds_rules

TARGET := f1_melbourne_dsi
BUILD := build
SOURCES := source
INCLUDES :=

ARCH := -marm
CFLAGS := -g -Wall -Wextra -O2 -ffunction-sections -fdata-sections $(ARCH) -DARM9
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)
LDFLAGS = -g
LIBS := -lnds9 -lm
LIBDIRS := $(LIBNDS) $(PORTLIBS)

.PHONY: all clean
all: $(TARGET).nds

clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).nds
