# The name of your project (used to name the compiled .hex file)
TARGET = $(notdir $(CURDIR))

# The teensy version to use, 30, 31, or LC
TEENSY = 31

# Set to 24000000, 48000000, 72000000, or 96000000 to set CPU core speed
TEENSY_CORE_SPEED = 72000000

# configurable options
OPTIONS = -DUSB_SERIAL -DLAYOUT_US_ENGLISH -D'TEST_AP_SSID="testap"' -D'TEST_AP_PASSWD="simplepass"'
#OPTIONS += -DJSON_DEBUG
#OPTIONS += -DESP8266_DEBUG
#OPTIONS += -DEXECUTE_UNIT_TESTS
#OPTIONS += -DSFS_DEBUG
#OPTIONS += -DMEM_DEBUG
#OPTIONS += -DEEPROM_ERASE

# directory to build in
BUILDDIR = $(abspath $(CURDIR)/build)

# path location for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH = $(CURDIR)/tools

# path location for Teensy 3 core
COREPATH = teensy3

# additional libraries
LIBRARYPATH = libraries

# path location for the arm-none-eabi compiler
COMPILERPATH = $(TOOLSPATH)/arm/bin

# CPPFLAGS = compiler options for C and C++
CPPFLAGS =  -Wall -g -Os -mthumb -ffunction-sections -fdata-sections -nostdlib \
            -MMD $(OPTIONS) -DTEENSYDUINO=124 -DF_CPU=$(TEENSY_CORE_SPEED) -Isrc \
            -Isrc/include/ -I$(COREPATH)

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections -mthumb

# additional libraries to link
LIBS = -lm -lc

# compiler options specific to teensy version
ifeq ($(TEENSY), 30)
    CPPFLAGS += -D__MK20DX128__ -mcpu=cortex-m4
    LDSCRIPT = $(COREPATH)/mk20dx128.ld
    LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
else
    ifeq ($(TEENSY), 31)
        CPPFLAGS += -D__MK20DX256__ -mcpu=cortex-m4
        LDSCRIPT = $(COREPATH)/mk20dx256.ld
        LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
    else
        ifeq ($(TEENSY), LC)
            CPPFLAGS += -D__MKL26Z64__ -mcpu=cortex-m0plus
            LDSCRIPT = $(COREPATH)/mkl26z64.ld
            LDFLAGS += -mcpu=cortex-m0plus -T$(LDSCRIPT)
            LIBS += -larm_cortexM0l_math
        else
            $(error Invalid setting for TEENSY)
        endif
    endif
endif

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size
STRIP = $(abspath $(COMPILERPATH))/arm-none-eabi-strip

# automatically create lists of the sources and objects
LC_FILES := $(wildcard $(LIBRARYPATH)/*/*.c)
LCPP_FILES := $(wildcard $(LIBRARYPATH)/*/*.cpp)
TC_FILES := $(wildcard $(COREPATH)/*.c)
TCPP_FILES := $(wildcard $(COREPATH)/*.cpp)
CPP_FILES := $(wildcard src/*.cpp)
CPP_FILES += $(wildcard src/*/*.cpp)
C_FILES := $(wildcard src/*.c)
C_FILES += $(wildcard src/*/*.c)

# include paths for libraries
L_INC := $(foreach lib,$(filter %/, $(wildcard $(LIBRARYPATH)/*/)), -I$(lib))

SOURCES :=  $(C_FILES:.c=.o) \
            $(CPP_FILES:.cpp=.o) \
            $(TC_FILES:.c=.o) \
            $(TCPP_FILES:.cpp=.o) \
            $(LC_FILES:.c=.o) \
            $(LCPP_FILES:.cpp=.o)

OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))

all: $(OBJS) cli-strip hex
build: $(TARGET).elf
hex: $(TARGET).hex
install: do_proj post_compile reboot
install-cli: do_cli post_compile reboot

post_compile: $(TARGET).hex
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

# Build the main target firmware
do_proj: $(OBJS) cli-strip hex 
cli-strip:
	$(STRIP) --strip-symbol main $(BUILDDIR)/src/cli.o

# Build the CLI firmware
do_cli: $(OBJS) targ-strip hex
targ-strip:
	$(STRIP) --strip-symbol main $(BUILDDIR)/src/trendr.o


$(BUILDDIR)/%.o: %.c
	@echo "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

# Build targets
$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

%.hex: %.elf
	@echo "[HEX]\t$@"
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(TARGET).elf" "$(TARGET).hex"
