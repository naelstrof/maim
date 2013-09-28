CC=g++
CFLAGS=$(shell imlib2-config --cflags)
LDFLAGS=$(shell imlib2-config --libs)
SOURCES=main.cpp x.cpp options.cpp im.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=maim
BINDIR="usr/bin"

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

install: all
	mkdir -p $(DESTDIR)/$(BINDIR)
	cp $(CURDIR)/slop $(DESTDIR)/$(BINDIR)
