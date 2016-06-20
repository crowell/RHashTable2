CFLAGS += $(shell pkg-config --cflags r_util) -g
LDFLAGS += $(shell pkg-config --libs r_util)

ht:
	$(CC) $(CFLAGS) ht.c -o ht $(LDFLAGS)

test:
	$(CC) $(CFLAGS) ht.c ht_test.c -o ht_test $(LDFLAGS)
