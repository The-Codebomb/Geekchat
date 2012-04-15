CPP = g++

OBJECTS = src/networking.o src/main.o
TARGET = geeklient
FILES = $(OBJECTS) $(TARGET)

all: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(TARGET)

src/networking.o: 
	$(CPP) -c src/networking.cpp -o src/networking.o
src/main.o: 
	$(CPP) -c src/main.cpp -o src/main.o

clean:
	rm $(FILES)
