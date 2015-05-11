INCLUDE_DIRS:= device/include cmsis/include ../include ../STM32F0xx_StdPeriph_Driver/inc ../retarget/gcc/include
SOURCE_DIRS:=  device/source ../source ../STM32F0xx_StdPeriph_Driver/src ../retarget/gcc/source

#All the dtandard peripherial library files
SPL_FILES:= $(notdir $(wildcard ../STM32F0xx_StdPeriph_Driver/src/*.c ))
#the C files to compile
CFILES:= system_stm32f0xx.c $(SPL_FILES) main.c serial_stdio.c retarget_STM32F0.c GLCD.c GLCD_STM32F0.c rTouch.c GUI.c
#No optimization C files
NO_OPT_CFILES:= ARM_delay.c
#the assembly files assemble
ASM_FILES:= startup_stm32f051.S
#the target microcontroller
TCHIP=STM32F051
#The compiler to use
CC=arm-none-eabi-gcc
#LINKER OPTIONS
#The linker script for the microcontroller
LD_SCRIPTS:= STM32F051x6_FLASH.ld
#the directories for linker files
LIB_DIRS:= device/linker
#the libraries to link
LIB_NAMES:=
#linker flags
LFLAGS:= $(patsubst %,-L%,$(LIB_DIRS)) $(patsubst %,-l%,$(LIB_NAMES)) -T$(LD_SCRIPTS)
#PREPROCESSOR OPTIONS
#Define macro for all files
DEFINES:= USE_STDPERIPH_DRIVER
DEFINE_FLAGS:=$(patsubst %,-D%,$(DEFINES))
INCLUDE_FLAGS:=$(patsubst %,-I%,$(INCLUDE_DIRS))
ARCHFLAGS:= -mthumb -mcpu=cortex-m0
CFLAGS= $(ARCHFLAGS) -std=c99 -Wall -g -ffunction-sections -fdata-sections
OFLAGS:= -Os
CPPFLAGS= $(INCLUDE_FLAGS) $(DEFINE_FLAGS) -D$(TCHIP)
vpath %.h $(INCLUDE_DIRS)
vpath %.c $(SOURCE_DIRS)
vpath %.S $(SOURCE_DIRS)
vpath %.o objects

COBJECTS:= $(patsubst %.c,%.o,$(CFILES))
NO_OPT_COBJECTS:= $(patsubst %.c,%.o,$(NO_OPT_CFILES))	
ASMOBJECTS:= $(patsubst %.S,%.o,$(ASM_FILES))

all: salida.elf
objects:
	mkdir objects
salida.elf: objects $(COBJECTS) $(ASMOBJECTS) $(NO_OPT_COBJECTS)
	$(CC) -Wl,--gc-sections -specs=nano.specs -u _printf_float $(CFLAGS) $(OFLAGS) objects/*.o $(LFLAGS) -o $@
	arm-none-eabi-size $@ ;
$(COBJECTS): %.o: %.c
	$(CC) $(CFLAGS) $(OFLAGS) $(CPPFLAGS) -c $< -o objects/$@
$(ASMOBJECTS): %.o: %.S
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o objects/$@
$(NO_OPT_COBJECTS): %.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o objects/$@
.PHONY:clean rebuild burn
clean:
	rm -f  *.elf ;
	rm -fr objects ;
rebuild: clean all
burn: salida.elf
	openocd -f board/st_nucleo_f0.cfg -c"init" -c"sleep 300"  -c"program erase salida.elf" -c"sleep 300" -c"reset" -c"shutdown"
debug: salida.elf
	openocd -f board/st_nucleo_f0.cfg &>/dev/null &
	sleep 1 ;
	! echo "reset" | telnet localhost 4444 ;
	! echo "reset halt" | telnet localhost 4444 ;
	sleep 1 ;
	arm-none-eabi-gdb --tui --eval-command="target extended-remote localhost:3333" salida.elf ;
	! echo "reset" | telnet localhost 4444 ;
	sleep 1 ;
	! echo "shutdown" | telnet localhost 4444 ;