
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sched.h>
#include "l2_l1_stub.h"


int compareArrays(uint8_t *a, uint8_t *b, int n) {
  int ii;
  for(ii = 1; ii <= n; ii++) {
    if (a[ii] != b[ii]) return 0;
    // better:
    // if(fabs(a[ii]-b[ii]) < 1e-10 * (fabs(a[ii]) + fabs(b[ii]))) {
    // with the appropriate tolerance
  }
  return 1;
}


int fpga_r_fd, fpga_w_fd, file_fd_1, file_w_fd,file_w1_fd,file_w2_fd,file_w3_fd,file_w4_fd;

uint8_t *tx_ptr, *slot_buf,*rx_ptr, *slot_ptr;

static volatile int run = 1;

int main() {
	cpu_set_t mask;
  CPU_ZERO(&mask);
	CPU_SET(3, &mask); // run on the cpu core 3
	int result = sched_setaffinity(0, sizeof(mask), &mask); 
	long nproc = sysconf(_SC_NPROCESSORS_ONLN);
	printf("Active CPUs: "); // print what are the active cpus
	for (int i=0;i<nproc;i++) 
	  	printf("%d ", CPU_ISSET(i, &mask));
	printf("\n");
  int rx_len, tx_len;
  rx_len = 1000;
  tx_len = 1000;
  //rx_ptr = (uint8_t*) malloc((RX_SIZE_MAX+ALIGNMENT)); // reciever pointer allocation in cpu
	//tx_ptr = (uint8_t*) malloc((TX_SIZE_MAX+ALIGNMENT)); // What is alignment?
	//slot_ptr = (uint8_t*) malloc((SLOT_SIZE_MAX+ALIGNMENT));
	//memset(tx_ptr, 0x00, TX_SIZE_MAX + ALIGNMENT); // initialise the values by 0
	//memset(rx_ptr, 0x00, RX_SIZE_MAX + ALIGNMENT);
	//memset(slot_ptr, 0x00, SLOT_SIZE_MAX + ALIGNMENT);

 
  


	
    /*****************OPEN READ CH****************/
    fpga_r_fd = open(DEVICE_NAME_DEFAULT_READ, O_RDONLY);
    assert(fpga_r_fd>=0);

    /***************OPEN WRITE CH*************/
    fpga_w_fd = open(DEVICE_NAME_DEFAULT_WRITE, O_WRONLY);
    assert(fpga_w_fd>=0);

	  /*****************Declaring the I/O variables****************/
    FILE* fp;
  
    /*******************Opening the files to read***************/
    fp = fopen("data0.bin","rb");
    //assert(fp>=0); 

    /*******************Allocation***************/
  	tx_ptr=(uint8_t *)malloc(tx_len);
	  memset(tx_ptr, 0x00, tx_len);
    rx_ptr=(uint8_t *)malloc(rx_len);
	  memset(rx_ptr, 0x00, rx_len);
  
    /********************** Starting the read func**************/
    rc = read(fp, tx_ptr, tx_len);
    wc = write(fpga_w_fd, tx_ptr, tx_len);
      
    rc = read(fpga_r_fd,rx_ptr,rx_len);
                         
    printf("%d", compareArrays(rx_ptr, tx_ptr, tx_len));
}
    
  
    
    
		
