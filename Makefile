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

clean:
	rm -rf $(OUT_DIR)