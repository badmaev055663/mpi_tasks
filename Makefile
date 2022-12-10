HEADERS = common.h
SRC = common.c
CC = mpicc
OUT_DIR = out
default: prepare

prepare:
	rm -rf $(OUT_DIR)
	mkdir $(OUT_DIR)

task1: task1.c $(HEADERS) $(SRC)
	$(CC) $(SRC) task1.c -o $(OUT_DIR)/task1

task2: task2.c $(HEADERS) $(SRC)
	$(CC) $(SRC) task2.c -o $(OUT_DIR)/task2

task3: task3.c $(HEADERS) $(SRC)
	$(CC) $(SRC) task3.c -o $(OUT_DIR)/task3

task5: task5.c $(HEADERS) $(SRC)
	$(CC) $(SRC) task5.c -o $(OUT_DIR)/task5

clean:
	rm -rf $(OUT_DIR)