CC = gcc

OBJECTS += src/networking.o src/main.o
TARGET = geeklient

all: src/networking.o src/main.o
	$(CC) $(OBJECTS) -o $(TARGET)

src/networking.o: 
	$(CC) -c src/networking.cpp -o src/networking.o
src/main.o: 
	$(CC) -c src/main.cpp -o src/main.o

clean:
	rm $(FILES) $(TARGET)
