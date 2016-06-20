CFLAGS += $(shell pkg-config --cflags r_util)
LDFLAGS += $(shell pkg-config --libs r_util)

all:
	$(CC) $(CFLAGS) ht.c -o ht $(LDFLAGS)
