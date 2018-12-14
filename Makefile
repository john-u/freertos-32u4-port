# -*- Makefile -*-

PORT=/dev/ttyACM0

F_CPU=16000000
MCU=atmega32u4

ADC_PATH=./basic-analog

FREERTOS_PATH=./FreeRTOS
FREERTOS_PORT_PATH=$(FREERTOS_PATH)/Source/portable/gcc/ATmega32U4
FREERTOS_MEM_PATH=$(FREERTOS_PATH)/Source/portable/MemMang

HOUGH_PATH=./hough

TASKS_PATH=./project-tasks

CSRCS = ./main.c \
$(ADC_PATH)/basic-analog.c \
$(HOUGH_PATH)/hough.c \
$(FREERTOS_PATH)/Source/tasks.c \
$(FREERTOS_PATH)/Source/queue.c \
$(FREERTOS_PATH)/Source/list.c \
$(FREERTOS_PORT_PATH)/port.c \
$(FREERTOS_MEM_PATH)/heap_4.c \
$(TASKS_PATH)/project-tasks.c

INC_PATH = . \
$(ADC_PATH)/ \
$(HOUGH_PATH)/ \
$(FREERTOS_PATH)/Source/include/ \
$(FREERTOS_PORT_PATH)/ \
$(FREERTOS_MEM_PATH)/ \
$(TASKS_PATH)/

CFLAGS= -g -Wall -mcall-prologues -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os $(INC_PATH:%=-I%)

LDFLAGS=-Wl,-gc-sections -Wl,-relax

CC=avr-gcc

OBJECT_FILES=$(CSRCS:.c=.o)

TARGET=main

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj

%.hex: %.obj
	avr-objcopy -R .eeprom -O ihex $< $@

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

program: $(TARGET).hex
	avrdude -p $(MCU) -c avr109 -P $(PORT) -U flash:w:$(TARGET).hex
