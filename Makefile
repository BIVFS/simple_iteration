TARGET=calc

CC=g++

CFLAGS=-c -std=c++14 -Wall -Wextra -pedantic -g -O0 -I. # -Werror

LFLAGS =
LIBDEP = -pthread

SOURCES=main.cpp \
	my_calc.cpp

OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) $(LIBDEP) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(TARGET)
