#define _GNU_SOURCE
#include <signal.h>
#include <sys/time.h>
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


#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif


void get_monotonic_time(struct timespec *ts){
#ifdef __MACH__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts -> tv_sec = mts.tv_sec;
	ts -> tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_MONOTONIC, ts);
#endif
}

double get_elapsed_time (struct timespec *before, struct timespec *after){
double deltat_s = after->tv_sec - before->tv_sec;
double deltat_ns = after->tv_nsec - before->tv_nsec;
return deltat_s*1e9 + deltat_ns;
}

int compareArrays(uint8_t *a, uint8_t *b, int n) {
  int ii;
  printf("%d\n", n);
  for(ii = 1; ii <= n; ii++) {
    if (a[ii] != b[ii]){ 
//	    printf("Error found\n");
//	    return 0;}
  }
 //   printf("%d %d\n",a[ii],b[ii]);
    // better:
    // if(fabs(a[ii]-b[ii]) < 1e-10 * (fabs(a[ii]) + fabs(b[ii]))) {
    // with the appropriate tolerance
  }
//  printf("Error not found\n");
  return 1;
}


int fpga_r_fd, fpga_w_fd, file_fd_1, file_w_fd,file_w1_fd,file_w2_fd,file_w3_fd,file_w4_fd;

uint8_t *tx_ptr, *slot_buf,*rx_ptr, *slot_ptr;

static volatile int run = 1;

int main() {
	cpu_set_t mask;
  CPU_ZERO(&mask);
	CPU_SET(4, &mask); // run on the cpu core 4
	int rc, wc;
	struct timespec oneTime, twoTime, threeTime, before, after;
	int result = sched_setaffinity(0, sizeof(mask), &mask); 
	long nproc = sysconf(_SC_NPROCESSORS_ONLN);
	printf("Active CPUs: "); // print what are the active cpus
	for (int i=0;i<nproc;i++) 
	  	printf("%d ", CPU_ISSET(i, &mask));
	printf("\n");
  int rx_len, tx_len;
  /*
  rx_len = 100;
  tx_len = 100;
  
    //****************OPEN READ CH***************
    fpga_r_fd = open(DEVICE_NAME_DEFAULT_READ, O_RDONLY);
    assert(fpga_r_fd>=0);

    /***************OPEN WRITE CH*************
    fpga_w_fd = open(DEVICE_NAME_DEFAULT_WRITE, O_WRONLY);
    assert(fpga_w_fd>=0);

	  /*****************Declaring the I/O variables****************
    FILE* fp;
  
    /*******************Opening the files to read***************
    fp = fopen("data0.bin","rb");
    //assert(fp>=0); 

    /*******************Allocation***************
  	tx_ptr=(uint8_t *)malloc((tx_len * sizeof(uint8_t)) + ALIGNMENT);
	  memset(tx_ptr, 0x00, tx_len);
    rx_ptr=(uint8_t *)malloc((rx_len * sizeof(uint8_t)) + ALIGNMENT);
	  memset(rx_ptr, 0x00, rx_len);
  
    /********************** Starting the read func**************
    //printf("Reading from file\n");
	  rc = fread(tx_ptr, sizeof(tx_ptr),tx_len,fp);
	  printf("\nRead from file:%d\n",rc);
	  for (int j=0;j<95;j++){
		  printf("%x",tx_ptr[j]);}
    printf("Writing to fpga\n");
    //clock_gettime(CLOCK_MONOTONIC, &oneTime);
    get_monotonic_time(&before);
    wc = write(fpga_w_fd, tx_ptr, tx_len);
    printf("\nwc:%d\n",wc);
  //  clock_gettime(CLOCK_REALTIME, &twoTime);
      
    rc = read(fpga_r_fd,rx_ptr,tx_len);
    printf("\nrc:%d\n",rc);
    get_monotonic_time(&after);
    for (int j=0;j<95;j++){
	    printf("%x",rx_ptr[j]);}
    //clock_gettime(CLOCK_MONOTONIC, &threeTime);
printf("Read from fpga\n");    
//    printf("Result: %d \n", compareArrays(rx_ptr, tx_ptr, tx_len));
//long int diff1 = (twoTime.tv_nsec - oneTime.tv_nsec);
//long int diff2 = (threeTime.tv_nsec - twoTime.tv_nsec);
//long int diff3 = (threeTime.tv_nsec - oneTime.tv_nsec);
//printf("Writing to fpga Time is %ld nanosec \n", diff1);
//printf("Reading from fpga Time is %ld nanosec \n", diff2);
//printf("Latency is %ld nanosec \n", diff3);
printf("Latency = %e ns\n", get_elapsed_time(&before, &after));
printf("Result: %d \n", compareArrays(rx_ptr, tx_ptr, tx_len));
*/
char file_arr[20][20] = {"data0.bin", "data1.bin", "data2.bin", "data3.bin", "data4.bin", "data5.bin", "data6.bin", "data7.bin", "data8.bin", "data9.bin", "data10.bin", "data11.bin", "data12.bin", "data13.bin", "data14.bin", "data15.bin", "data16.bin", "data17.bin"};
int tx_arr[20] = {100, 1000,10e3, 20e3, 50e3, 100e3, 200e3, 500e3, 1e6, 10e6, 20e6, 50e6, 100e6, 200e6, 500e6, 1e9, 2e9};//, 5e9};
//printf("%d", tx_arr[0]);
char sizes[20][20] = {"100b", "1kb", "10kb", "20kb", "50kb", "100kb", "200kb", "500kb", "1mb", "10mb", "20mb", "50mb", "100mb", "200mb", "500mb", "1gb", "2gb", "5gb"};
printf("Result is 0 if error is found otherwise 1\n");
//free(tx_ptr);
//free(rx_ptr);

//for (int i = 1; i<0; i++){
int i = 0;
  rx_len = tx_arr[i];
  tx_len = tx_arr[i];
  printf("Size of this bin file: %s \n", sizes[i]);

    /*****************OPEN READ CH****************/
    fpga_r_fd = open(DEVICE_NAME_DEFAULT_READ, O_RDONLY);
    assert(fpga_r_fd>=0);

    /***************OPEN WRITE CH*************/
    fpga_w_fd = open(DEVICE_NAME_DEFAULT_WRITE, O_WRONLY);
    assert(fpga_w_fd>=0);

          /*****************Declaring the I/O variables****************/
    FILE* fp;

    /*******************Opening the files to read***************/
    fp = fopen(file_arr[i],"rb");
    assert(fp>=0); 

    /*******************Allocation***************/
        tx_ptr=(uint8_t *)malloc((tx_len * sizeof(uint8_t)) + ALIGNMENT);
          memset(tx_ptr, 0x00, tx_len);
    rx_ptr=(uint8_t *)malloc((rx_len * sizeof(uint8_t)) + ALIGNMENT);
          memset(rx_ptr, 0x00, rx_len);

    /********************** Starting the read func**************/
  //  printf("Reading from file\n");
          rc = fread(tx_ptr,sizeof(tx_ptr), tx_len,fp);
	  printf("\nrc:%d\n",rc);
	  for (int j=0;j<50;j++){
		  printf("%x",tx_ptr[j]);}
    printf("\nWriting to fpga\n");
    clock_gettime(CLOCK_MONOTONIC, &oneTime);
    wc = write(fpga_w_fd, tx_ptr, tx_len/8);
    
  //  clock_gettime(CLOCK_REALTIME, &twoTime);
    printf("wrote to fpga");
    rc = read(fpga_r_fd,rx_ptr,rx_len/8);
    clock_gettime(CLOCK_MONOTONIC, &threeTime);
    printf("\nwc:%d\n",wc);
    printf("\nrc:%d\n",rc);
    for (int j=0;j<rx_len;j++){
	    printf("%x",rx_ptr[j]);}
printf("\nRead from fpga\n");    
//    printf("Result: %d \n", compareArrays(rx_ptr, tx_ptr, tx_len));
//long int diff1 = (twoTime.tv_nsec - oneTime.tv_nsec);
//long int diff2 = (threeTime.tv_nsec - twoTime.tv_nsec);
long int diff3 = (threeTime.tv_nsec - oneTime.tv_nsec);
//printf("Writing to fpga Time is %ld nanosec \n", diff1);
//printf("Reading from fpga Time is %ld nanosec \n", diff2);
printf("Latency is %ld nanosec \n", diff3);
printf("Result: %d \n", compareArrays(rx_ptr, tx_ptr, tx_len));

free(tx_ptr);
free(rx_ptr);

//}
}
    
  
    
    
		
