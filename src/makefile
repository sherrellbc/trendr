#  Project Name
PROJECT=trendr

#  Type of CPU/MODEL in target hardware
CPU=cortex-m4
MODEL=mk20dx256

# Location of the Teensy Loader
LOADER=../tools/teensy_loader_cli/teensy_loader_cli

#  Build the list of object files needed
OBJECTS	=   $(PROJECT).o \
	        sysinit.o \
	        crt0.o    \
            uart.o

# Top-level directory of the toolchain
TOOLPATH = /home/sherrbc1/.programs/gcc-arm-none-eabi-5_4-2016q2

# Top-level source directory relative to the makefile
TEENSY3X_BASEPATH = ..


#  Describe the various include and source directories needed.
#  These usually point to files from whatever distribution
#  you are using (such as Freescale examples).  This can also
#  include paths to any needed GCC includes or libraries.
TEENSY3X_INC     = $(TEENSY3X_BASEPATH)/include
GCC_INC          = $(TOOLPATH)/arm-none-eabi/include


# Additional source directories. Separate paths with ':'
VPATH = $(TEENSY3X_BASEPATH)/common
				
#  List of directories to be searched for include files during compilation
INCDIRS  = -I$(GCC_INC)
INCDIRS += -I$(TEENSY3X_INC)
INCDIRS += -I$(TEENSY3X_BASEPATH)/src/include

# Name and path to the linker script
LSCRIPT = $(TEENSY3X_BASEPATH)/common/Teensy31_flash.ld

OPTIMIZATION = 0
DEBUG = -g

#  List the directories to be searched for libraries during linking.
LIBDIRS  = -L$(TOOLPATH)/arm-none-eabi/lib/ -L$(TOOLPATH)/arm-none-eabi/lib/armv7e-m/ -L$(TOOLPATH)/lib/gcc/arm-none-eabi/5.4.1/armv7e-m/softfp
LIBS = -lm -lc_nano -lnosys -lgcc

#  Compiler options
GCFLAGS = -Wall -fno-common -mcpu=$(CPU) -mthumb -O$(OPTIMIZATION) $(DEBUG) -DF_CPU=72000000 --specs=nano.specs -mfloat-abi=softfp
GCFLAGS += $(INCDIRS)

#  Assembler options
ASFLAGS = -mcpu=$(CPU)

#  Linker options
LDFLAGS  = -nostdlib -nostartfiles -Map=$(PROJECT).map -T$(LSCRIPT) -static
LDFLAGS += --cref
LDFLAGS += $(LIBDIRS)
LDFLAGS += $(LIBS)


#  Tools paths
BINDIR = $(TOOLPATH)/bin

CC = $(BINDIR)/arm-none-eabi-gcc
AS = $(BINDIR)/arm-none-eabi-as
AR = $(BINDIR)/arm-none-eabi-ar
LD = $(BINDIR)/arm-none-eabi-ld
OBJCOPY = $(BINDIR)/arm-none-eabi-objcopy
SIZE = $(BINDIR)/arm-none-eabi-size
OBJDUMP = $(BINDIR)/arm-none-eabi-objdump

#########################################################################

all:: $(PROJECT).hex $(PROJECT).bin stats dump

$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary -j .text -j .data $(PROJECT).elf $(PROJECT).bin

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -R .stack -O ihex $(PROJECT).elf $(PROJECT).hex

#  Linker invocation
$(PROJECT).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(PROJECT).elf

stats: $(PROJECT).elf
	$(SIZE) $(PROJECT).elf
	
dump: $(PROJECT).elf
	$(OBJDUMP) -h $(PROJECT).elf	

clean:
	rm *.hex *.elf *.map *.bin *.lst *.o 

install:
	$(LOADER) --mcu=$(MODEL) -vw $(PROJECT).hex
	
#########################################################################
.c.o :
	@echo Compiling $<, writing to $@...
	$(CC) $(GCFLAGS) -c $< -o $@ > $(basename $@).lst
    
.cpp.o :
	@echo Compiling $<, writing to $@...
	$(CC) $(GCFLAGS) -c $<

.s.o :
	@echo Assembling $<, writing to $@...
	$(AS) $(ASFLAGS) -o $@ $<  > $(basename $@).lst
#########################################################################
