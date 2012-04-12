CC = gcc

OBJECTS += src/main.o
TARGET = geekserver

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

src/main.o: 
	$(CC) -c src/main.cpp -o src/main.o

clean:
	rm $(FILES) $(TARGET)
