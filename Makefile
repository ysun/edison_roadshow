CC := gcc 
CFLAGS := -Wall -O -g
LIBS := -lmraa -lpthread

TARGET = uart #This is the binary name!
SOURCE = $(wildcard *.c)  
OBJS = $(patsubst %.c,%.o,$(SOURCE))  

$(TARGET) : $(OBJS)
	$(CC) $^ $(LIBS) -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm $(TARGET) $(OBJS)

