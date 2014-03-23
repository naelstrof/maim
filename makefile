CC=g++
CFLAGS=$(shell imlib2-config --cflags)
LDFLAGS=-L/usr/lib -lImlib2 -lX11
SOURCES=main.cpp x.cpp options.cpp im.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=maim
BINDIR="/usr/bin"

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
        $(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
        rm -rf $(OBJECTS) $(EXECUTABLE)

install: all
        mkdir -p $(DESTDIR)$(BINDIR)
        cp $(CURDIR)/$(EXECUTABLE) $(DESTDIR)$(BINDIR)
