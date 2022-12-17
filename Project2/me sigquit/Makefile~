#
# In order to execute this "Makefile" just type "make"
#

OBJS	= main.o dateandlist.o hashtable.o kouvades.o topk.o tree.o functions.o
OBJS2	= worker.o dateandlist.o hashtable.o kouvades.o topk.o tree.o functions.o
SOURCE	= main.c worker.c
#HEADER  = mylib.h hashlib.h kouvas.h dentro.h query.h topk.h heap.h
OUT  	= diseaseAggregator
OUT2  	= worker
CC	= gcc
FLAGS   = -g -c
# -g option enables debugging mode
# -c flag generates object code for separate files

$(OUT): $(OBJS) $(OBJS2)
	$(CC) -g $(OBJS) -o $(OUT)
	$(CC) -g $(OBJS2) -o $(OUT2)


# create/compile the individual files >>separately<<
main.o: main.c
	$(CC) $(FLAGS) main.c

worker.o: worker.c
	$(CC) $(FLAGS) worker.c

dateandlist.o: dateandlist.c
	$(CC) $(FLAGS) dateandlist.c

hashtable.o: hashtable.c
	$(CC) $(FLAGS) hashtable.c

kouvades.o: kouvades.c
	$(CC) $(FLAGS) kouvades.c

topk.o: topk.c
	$(CC) $(FLAGS) topk.c

tree.o: tree.c
	$(CC) $(FLAGS) tree.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

run:
	./diseaseAggregator -w 3 -b 100 -i fakelos

# clean house
clean:
	rm -f $(OBJS) $(OUT) $(OBJS2) $(OUT2)
