CC=			g++
CFLAGS=		-g -gdwarf-2 -Wall -std=c++11
LD=			g++
LDFLAGS=	-L.
AR=			ar
ARFLAGS=	rcs
TARGETS=	pq

all: $(TARGETS)

pq: main.o client.o server.o schedule.o time.o handle_request.o signal.o empty_scheduler.o
	@echo Linking pq...
	@$(LD) $(LDFLAGS) -o $@ $^

main.o: main.cpp pq.h
	@echo Compiling main.o...
	@$(CC) $(CFLAGS) -o $@ -c $<

time.o: time.cpp pq.h
	@echo Compiling time.o...
	@$(CC) $(CFLAGS) -o $@ -c $<

schedule.o: schedule.cpp pq.h
	@echo Compiling schedule.o...
	@$(CC) $(CFLAGS) -o $@ -c $<

client.o: client.cpp pq.h
	@echo Compiling client.o...
	@$(CC) $(CFLAGS) -o $@ -c $<
	
server.o: server.cpp pq.h
	@echo Compiling server.o...
	@$(CC) $(CFLAGS) -o $@ -c $<

handle_request.o: handle_request.cpp pq.h
	@echo Compiling handle_request.o...
	@$(CC) $(CFLAGS) -o $@ -c $<

signal.o: signal.cpp pq.h
	@echo Compiling signal.o...
	@$(CC) $(CFLAGS) -o $@ -c $<

empty_scheduler.o: empty_scheduler.cpp pq.h
	@echo Compiling empty_scheduler.o...
	@$(CC) $(CFLAGS) -o $@ -c $<


clean:
	@echo Cleaning up
	@rm -f $(TARGETS) *.o
