# Target library
lib := libfs.a
objects := disk.o fs.o

CC      := gcc
CFLAGS  := -Wall -Werror

all: $(lib)

$(lib): $(objects)
	ar rcs $(lib) $(objects)

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "CLEANING FILES"
	rm -f $(lib) $(objects)
