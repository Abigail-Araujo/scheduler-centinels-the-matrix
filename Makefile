CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CFLAGS += -pthread

TARGET = sentinel-dispatcher
SOURCES = main.c readCentinels.c planificador.c colas.c queue.c log.c metrics.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

clean:
	rm -f $(TARGET)
