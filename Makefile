CC = g++

CFLAGS = -std=c++11 -O3
#CFLAGS += -Wall -pedantic

INCLUDES = -I.

OBJDIR = obj
BINDIR = bin
SRCDIR = src

OBJFILES := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp))


MKDIR_P = mkdir -p

.PHONY: directories

all: clean directories gdp

directories: ${OBJDIR} ${BINDIR}

${OBJDIR}:
	${MKDIR_P} ${OBJDIR}
${BINDIR}:
	${MKDIR_P} ${BINDIR}


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)
	
gdp: ${OBJFILES}
	$(CC) $(CFLAGS) $(INCLUDES)  -o ${BINDIR}/gdp $(OBJFILES)

clean:
	rm -f ${BINDIR}/gdp
	rm -f $(OBJDIR)/*.o

	

