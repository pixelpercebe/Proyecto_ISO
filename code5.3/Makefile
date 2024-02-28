CC=gcc
CFLAGS=-c
LDFLAGS=-lssl -lcrypto
SOURCES=insertar_fichero.c my-sha256-repo.c sha256-ehu-lib.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mysha256-repo

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
