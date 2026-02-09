# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6

TARGET=juno_image_simple.axf


CC=armclang
LD=armlink

# Select build rules based on Windows or Unix
ifdef WINDIR
DONE=@if exist $(1) echo Build completed.
RM=if exist $(1) del /q $(1)
SHELL=$(WINDIR)\system32\cmd.exe
else
ifdef windir
DONE=@if exist $(1) echo Build completed.
RM=if exist $(1) del /q $(1)
SHELL=$(windir)\system32\cmd.exe
else
DONE=@if [ -f $(1) ]; then echo Build completed.; fi
RM=rm -f $(1)
endif
endif

all: $(TARGET)
	$(call DONE,$(TARGET))

rebuild: clean all

clean:
	$(call RM,*.o)
	$(call RM,$(TARGET))

CFLAGS=--target=aarch64-arm-none-eabi -march=armv8-a -g -ffreestanding -nostdinc -nostdlib

$(TARGET): a64_main.o scatter.scat
	$(LD) a64_main.o --scatter=scatter.scat --entry a64_main -o $(TARGET) --info=totals --info=unused

a64_main.o: a64_main.S
	$(CC) -c $(CFLAGS) a64_main.S
