CPP = g++

OBJECTS = src/networking.o src/main.o src/ui.o
TARGET = geeklient
FILES = $(OBJECTS) $(TARGET)

all: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(TARGET)

src/networking.o: 
	$(CPP) -c src/networking.cpp -o src/networking.o $(DEFINES)
src/main.o: 
	$(CPP) -c src/main.cpp -o src/main.o $(DEFINES)
qt:
	$(CPP) -c src/qt/chatwindow.cpp -o src/ui.o $(DEFINES)

clean:
	rm $(FILES)
