CC=gcc

all: run_stub

.PHONY: clean

clean:
		rm -f run_stub *.o

run_stub: L2_L1_stub_rx_tx.cpp l2_l1_stub.h
		$(CC) L2_L1_stub_rx_tx.cpp -o run_stub