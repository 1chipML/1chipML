CC=gcc

CFLAGS += -g # adds debugging information
CFLAGS += -Wall # turns on most compiler warnings
CFLAGS += -std=c99 # C99
CFLAGS += -I./../utils # included header files
CFLAGS += -D_DEFAULT_SOURCE # Defines required for serial

# loaded libraries
LDLIBS += -lm # Math library

all: image_processing

image_processing: image_processing.c ../utils/bitmap.c ../utils/serial_port.c
	$(CC) $(CFLAGS) $^ -o $@.out $(LDLIBS)

clean:
	rm -rf *.out
	find . -name "*.bmp" -not -name "1chipML_color32.bmp" -exec rm -rf {} +
