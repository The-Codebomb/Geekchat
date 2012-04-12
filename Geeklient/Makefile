CC = gcc

FILES = src/networking.o src/main.o

src/networking.o: src/networking.cpp
src/main.o: src/main.cpp

default:
	$(CC) $(FILES) -o geeklient

all: default

clean:
	rm $(FILES)
