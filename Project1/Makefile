#
# In order to execute this "Makefile" just type "make"
#

OBJS 	= main.o functions.o hashtable.o kouvades.o tree.o queries.o topk.o
SOURCE	= main.c functions.c hashtable.c kouvades.c tree.c queries.c topk.c
HEADER  = mylib.h hashlib.h kouvas.h dentro.h query.h topk.h heap.h
OUT  	= diseaseMonitor
CC	= gcc
FLAGS   = -g -c
# -g option enables debugging mode
# -c flag generates object code for separate files

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

# create/compile the individual files >>separately<<
main.o: main.c
	$(CC) $(FLAGS) main.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

hashtable.o: hashtable.c
	$(CC) $(FLAGS) hashtable.c

kouvades.o: kouvades.c
	$(CC) $(FLAGS) kouvades.c

tree.o: tree.c
	$(CC) $(FLAGS) tree.c

queries.o: queries.c
	$(CC) $(FLAGS) queries.c

topk.o: topk.c
	$(CC) $(FLAGS) topk.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
