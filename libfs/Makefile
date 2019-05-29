# Target library
lib := libfs.a
objects := disk.o fs.o

CC      := gcc
CFLAGS  := -Wall -Werror

# Generate dependencies
DEPFLAGS = -MMD -MF $(@:.o=.d)

# Include dependencies
deps := $(patsubst %.o,%.d,$(objects))
-include $(deps)

all: $(lib)

$(lib): $(objects)
	ar rcs $(lib) $(objects)

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	@echo "CLEANING FILES"
	rm -f $(lib) $(objects) $(deps)
