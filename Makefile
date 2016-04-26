# build client-timer executable when user executes "make"

LDFLAGS += -lrt
CFLAGS += -std=gnu99

APP_NAME = client-timer
OBJ = client-timer.o
OBJ += timeout-handler.o

$(APP_NAME): $(OBJ)
	$(CC) $^ -o $(APP_NAME) $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

# remove object files and executable when user executes "make clean"
clean:
	rm *.o $(APP_NAME)
