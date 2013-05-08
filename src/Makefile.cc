LIB_HOME:=/home/bus/gyf/lib
HEAD:=-I $(LIB_HOME)/include
LIB:=-L $(LIB_HOME)/lib -lnetcdf -Wl,-rpath=$(LIB_HOME)/lib
CC:=g++
CFLAGS:= -O3 $(HEAD) $(LIB)
CXXFLAGS:= -O3 $(HEAD) $(LIB)

#Find files containing main function
#GREPCMD:=grep -l 'int main\s*(.*)' *.c

#EXESOURCES:=$(shell $(GREPCMD))
#EXEOBJECTS:=$(patsubst %.c,%.o,$(EXEOBJECTS))
#ALL_BIN:=$(patsubst %.c,%,$(EXESOURCES))
#GREPCMD:=grep -L 'int main\s*(.*)' *.c
#NONEXESOURCES:=$(shell $(GREPCMD))
#NONEXEOBJECTS:=$(patsubst %.c,%.o,$(NONEXESOURCES))
#ALLSOURCE:=$(wildcard *.c)
#ALLOBJS:=$(patsubst %.c,%.o,$(SOURCE))
ALL_BIN:=nc2csv statistic sorted bsearch
NONEXEOBJECTS=nc2csv.o statistic.o sorted.o bsearch.o
all:$(ALL_BIN)
nc2csv:
	$(CC) $(patsubst %,%.c,$@) $^ -o $@ $(CFLAGS) 
statistic:
	$(CC) $(patsubst %,%.cc,$@) $^ -o $@ $(CFLAGS) 
sorted:
	$(CC) $(patsubst %,%.c,$@) $^ -o $@ $(CFLAGS) 
bsearch:
	$(CC) $(patsubst %,%.c,$@) $^ -o $@ $(CFLAGS) 
#$(ALL_BIN):$(NONEXEOBJECTS)
#	$(CC) $(patsubst %,%.c,$@) $^ -o $@ $(CFLAGS) 
#%.o:%.c
#	$(CC) -c $< -o $@ $(HEAD)
nc2csv.o:nc2csv.c
	$(CC) -c $< -o $@ $(HEAD)
statistic.o:statistic.cc
	$(CC) -c $< -o $@ $(HEAD)
sorted.o:sorted.c
	$(CC) -c $< -o $@ $(HEAD)
besearch.o:bsearch.c
	$(CC) -c $< -o $@ $(HEAD)
	
.PHONY:clean
LSCMD= ls *.o $(ALL_BIN)
EXISTOBJ=$(shell $(LSCMD))
clean:
	rm $(EXISTOBJ)




