#
# In order to execute this "Makefile" just type "make"
#

OBJS	= master.o dateandlist.o hashtable.o kouvades.o topk.o tree.o functions.o
OBJS2	= worker.o dateandlist.o hashtable.o kouvades.o topk.o tree.o functions.o
OBJS3	= whoServer.o whoserverfunctions.o dateandlist.o hashtable.o kouvades.o topk.o tree.o functions.o
OBJS4	= whoClient.o dateandlist.o hashtable.o kouvades.o topk.o tree.o functions.o whoclientfunctions.o
SOURCE	= main.c worker.c whoServer.c whoClient.c
#HEADER  = mylib.h hashlib.h kouvas.h dentro.h query.h topk.h heap.h
OUT  	= master
OUT2  	= worker
OUT3  	= whoServer
OUT4  	= whoClient
CC	= gcc
FLAGS   = -g -c -lpthread
# -g option enables debugging mode
# -c flag generates object code for separate files

$(OUT): $(OBJS) $(OBJS2) $(OBJS3) $(OBJS4)
	$(CC) -g $(OBJS) -o $(OUT)
	$(CC) -g $(OBJS2) -o $(OUT2)
	$(CC) -g $(OBJS3) -o $(OUT3) -lpthread
	$(CC) -g $(OBJS4) -o $(OUT4) -lpthread

# create/compile the individual files >>separately<<
master.o: master.c
	$(CC) $(FLAGS) master.c

worker.o: worker.c
	$(CC) $(FLAGS) worker.c

whoServer.o: whoServer.c
	$(CC) $(FLAGS) whoServer.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

whoClient.o: whoClient.c
	$(CC) $(FLAGS) whoClient.c

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

whoserverfunctions.o: whoserverfunctions.c
	$(CC) $(FLAGS) whoserverfunctions.c

whoclientfunctions.o: whoclientfunctions.c
	$(CC) $(FLAGS) whoclientfunctions.c




run:
	./master -w 4 -b 8 -s 0.0.0.0 -p 4080 -i director

# clean house
clean:
	rm -f $(OBJS) $(OUT) $(OBJS2) $(OUT2) $(OBJS3) $(OUT3) $(OBJS4) $(OUT4)
