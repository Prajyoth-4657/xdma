/* AUTHOR - K. SAI PRANEETH AND K. SAI PRASANTH
DATE - 4-09-2021
CODE FOR L2/L3 STUB */

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

int fpga_r_fd, fpga_w_fd, file_fd_1, file_w_fd,file_w1_fd,file_w2_fd,file_w3_fd,file_w4_fd;

uint8_t *tx_ptr, *slot_buf,*rx_ptr, *slot_ptr;

void crash(int loc_no) {
	printf("Crash at %d\n", loc_no);
	close(fpga_w_fd);
	close(fpga_r_fd);
	exit(1);
}

void printbuf(int size) {
	for (int i=0; i<size; i++) {
		printf("%x ", rx_ptr[i]); // this prints a hexadecimal number
	}
}

void printbuf_s(int size) {
	for(int i=0; i<size; i++) {
		printf("%x ", slot_buf[i]);
	}
}

static volatile int run = 1;
void sigint_handler(int sig_num) {
	signal(SIGQUIT, sigint_handler); // handle signal interrupts, didn't find sigquit 
	run = 0;
	printf("stopping after next rd-wr cycle\n");
	fflush(stdout); // will basically write any unwritten data
}

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

	rx_ptr = (uint8_t*) malloc((RX_SIZE_MAX+ALIGNMENT)); // reciever pointer allocation in cpu
	tx_ptr = (uint8_t*) malloc((TX_SIZE_MAX+ALIGNMENT)); // What is alignment?
	slot_ptr = (uint8_t*) malloc((SLOT_SIZE_MAX+ALIGNMENT));
	memset(tx_ptr, 0x00, TX_SIZE_MAX + ALIGNMENT); // initialise the values by 0
	memset(rx_ptr, 0x00, RX_SIZE_MAX + ALIGNMENT);
	memset(slot_ptr, 0x00, SLOT_SIZE_MAX + ALIGNMENT);

	file_fd_1 = open("ul_data_out_txt.bin", O_WRONLY | O_TRUNC); // open file with write only and truncate only mode
 	assert(file_fd_1>=0); // check if file is succusfully opened

	int rc, wc,rc1,wc1;
	uint32_t read_len;
    int c=20000,ret,tx_data,cfg_ptr,ul_cfg;
    uint64_t *config_ptr[2000]; 
    uint64_t *arr_ptr[2000];
    uint64_t *dat_ptr[2000]; 
    uint64_t *ul_ptr[2000]; 
    uint8_t *slot_start_arr_ptr, *dl_arr_ptr, *cfg_arr_ptr,*ul_arr_ptr;
    uint16_t dl_msgType, dl_msgSlot,tx_msgType, tx_msgSlot,ul_msgType, ul_msgSlot;
    uint16_t cfg_msgType,cfg_init_hdr,cfg_init_hdr_1,dl_init_hdr,dl_init_hdr_1,tx_init_hdr,tx_init_hdr_1,ul_init_hdr,ul_init_hdr_1, *ptrSFN,*ptrSLOT;
    uint32_t dl_msgLength,tx_msgLength,ul_msgLength,cfg_msgLength;
    /*****************OPEN READ CH****************/
    fpga_r_fd = open(DEVICE_NAME_DEFAULT_READ, O_RDONLY);
    assert(fpga_r_fd>=0);

    /***************OPEN WRITE CH*************/
    fpga_w_fd = open(DEVICE_NAME_DEFAULT_WRITE, O_WRONLY);
    assert(fpga_w_fd>=0);

	/*****************Declaring the I/O variables****************/
    
    FILE* fp;
    FILE* fp1;
    FILE* fp3;
    FILE* fp4;

    /*******************Opening the files to read***************/
    fp = fopen("combined_slot_messages_slot0.bin","rb");
    fp1 = fopen("tx_data.bin","rb");
    fp3 = fopen("config_req.bin","rb");
    fp4 = fopen("ul_tti_req_out.bin","rb");

    /********************** Starting the read func**************/
    for(int slot_id=0; slot_id < 12;slot_id++)
    {
		//config_ptr[slot_id]=(uint64_t *)malloc(c + ALIGNMENT);
		//memset(config_ptr[slot_id], 0x00, c + ALIGNMENT);
		arr_ptr[slot_id] = (uint64_t *)malloc(c + ALIGNMENT);
		memset(arr_ptr[slot_id], 0x00, c + ALIGNMENT );
		dat_ptr[slot_id] = (uint64_t *)malloc(c + ALIGNMENT);	
		memset(dat_ptr[slot_id], 0x00, (c+ALIGNMENT) );
		ul_ptr[slot_id] = (uint64_t *)malloc(c + ALIGNMENT);	
		memset(ul_ptr[slot_id], 0x00, (c+ALIGNMENT) );
		
		/*************DL_TTI msgID,msgLen,msgSLOT ************/
	    // reading from fp byte by byte and writing to arr_ptr
		ret = fread(arr_ptr[slot_id],sizeof(uint8_t),1,fp);			//#0B Phy api msgs
		slot_start_arr_ptr = (uint8_t*)(arr_ptr[slot_id]);
		dl_init_hdr = *((uint8_t*)slot_start_arr_ptr);
		
		ret = fread(slot_start_arr_ptr+1,sizeof(uint8_t),1,fp);			//#1B Fapi Handle
		dl_init_hdr_1 = *((uint8_t*)(slot_start_arr_ptr+1));
		
		ret = fread(slot_start_arr_ptr+2,sizeof(uint16_t),1,fp);		//#2B Msg ID
		dl_msgType = *((uint16_t*)(slot_start_arr_ptr+2));
		
		ret = fread(slot_start_arr_ptr+4,sizeof(uint32_t),1,fp);		//#4B Msg Len
		dl_msgLength = *((uint32_t*)(slot_start_arr_ptr+4));
		
		ret = fread(slot_start_arr_ptr+8,sizeof(uint8_t),dl_msgLength,fp);	//#8B Msg Len
		dl_msgSlot = *((uint16_t*)(slot_start_arr_ptr+10));
		printf("phy_api = %x, fapi_handle = %x,Slot = %d , Message type = %x, Message length = %u\n",dl_init_hdr, dl_init_hdr_1, dl_msgSlot, dl_msgType, dl_msgLength);

		/*************tx_data msgID,msgLen,msgSLOT ************/
		
		tx_data = fread(dat_ptr[slot_id],sizeof(uint8_t),1,fp1);		//#0B Phy api msgs
		dl_arr_ptr = (uint8_t*)(dat_ptr[slot_id]);
		tx_init_hdr = *((uint8_t*)dl_arr_ptr);
		
		tx_data = fread(dl_arr_ptr+1,sizeof(uint8_t),1,fp1);			//#1B Fapi Handle
		tx_init_hdr_1 = *((uint8_t*)(dl_arr_ptr+1));

		tx_data = fread(dl_arr_ptr+2,sizeof(uint16_t),1,fp1);			//#2B Msg ID
		tx_msgType = *((uint16_t*)(dl_arr_ptr+2));
		
		tx_data = fread(dl_arr_ptr+4,sizeof(uint32_t),1,fp1);			//#4B Msg Len
		tx_msgLength = *((uint32_t*)(dl_arr_ptr+4));
		
		tx_data = fread(dl_arr_ptr+8,sizeof(uint8_t),tx_msgLength,fp1);		//#8B Msg Len
		tx_msgSlot = *((uint16_t*)(dl_arr_ptr+10));
		printf("phy_api = %x, fapi_handle = %x, Slot = %d , Message type = %x, Message length = %u\n",tx_init_hdr, tx_init_hdr_1, tx_msgSlot, tx_msgType, tx_msgLength);

		/*************ul_TTI msgID,msgLen,msgSLOT ************/
		ul_cfg = fread(ul_ptr[slot_id],sizeof(uint8_t),1,fp4);
		ul_arr_ptr = (uint8_t*)(ul_ptr[slot_id]);
		ul_init_hdr = *((uint8_t*)ul_arr_ptr);

		ul_cfg = fread(ul_arr_ptr+1,sizeof(uint8_t),1,fp4);			//#1B Fapi Handle
		ul_init_hdr_1 = *((uint8_t*)(ul_arr_ptr+1));

		ul_cfg = fread(ul_arr_ptr+2,sizeof(uint16_t),1,fp4);			//#2B Msg ID
		ul_msgType = *((uint16_t*)ul_arr_ptr+2);
		
		ul_cfg = fread(ul_arr_ptr+4,sizeof(uint32_t),1,fp4);
		ul_msgLength = *((uint32_t*)(ul_arr_ptr+4));
	
		ul_cfg = fread(ul_arr_ptr+8,sizeof(uint8_t),ul_msgLength,fp4);
		ul_msgSlot = *((uint16_t*)(ul_arr_ptr+10));
		printf("phy_api = %x, fapi_handle = %x, Slot = %d , Message type = %x, Message length = %u\n",ul_init_hdr, ul_init_hdr_1, ul_msgSlot, ul_msgType, ul_msgLength);
    }
   	fflush(fp);		// Flushing the pointers
    fflush(fp1);    
	fflush(fp4);

	nfapi_header_t *p5_msg = (nfapi_header_t *) tx_ptr;
	nfapi_header_t *p5_resp = (nfapi_header_t *) rx_ptr;
	sfn_slot_t *slot_resp = (sfn_slot_t *) slot_ptr;
	printf("Starting function\n");


/******************PARAM req*****************/
	printf("\nPARAM msg\n");
	p5_msg->phy_api_msgs = 0x1;
	p5_msg->fapi_handle = 0;
	p5_msg->msg_id = PARAM_REQ_TAG;
	p5_msg->msg_len = PARAM_REQ_LEN;

	for (int i=0;i<HEADER_LEN;i++) {
	printf("%x ", tx_ptr[i]);
	}
	printf("\n");

	wc = write(fpga_w_fd, tx_ptr, HEADER_LEN); // ?
	printf("written PARAM req.\n");
	printf("bytes tx:%d\n", wc);
	/************1st read from FPGA ********/
	rc = read(fpga_r_fd, rx_ptr, HEADER_LEN);
	printf("bytes rx:%d\n", rc);

	if (p5_resp->msg_id == PARAM_RESP_TAG)
	printf("Received PARAM_RESP_TAG\n");
	else {
		printbuf(HEADER_LEN);
		crash(0);
	}

	read_len = (int) p5_resp->msg_len;
	printf("Received PARAM_RESP_LEN:%d\n",read_len);

	printf("Recv PARAM_RESP:");
	printbuf(HEADER_LEN);
	int rx_len = read_len+8;
	printf("\nnew read len:%d\n",rx_len);
	/************2nd read from FPGA ********/ // what is 2nd read?
 	rc = read(fpga_r_fd,rx_ptr,rx_len);
	printf("2nd read:%d\n",rc);
	// ***********Writing FPGA data to file *******
	// wc = write(file_w_fd,rx_ptr,rx_len);
	// printf("writing into file:%d\n",wc);

	/*if (rc != rx_len) {
		printf("\nPARAM_RESP read fail\n");
		crash(1);
	}*/

	printbuf(rc); //printing buffer values
	printf("\n");

 /****************************CONFIG req msgID,msgLen******************/
	printf("\nCONFIG msg\n");
	config_ptr[0]=(uint64_t *)malloc(c + ALIGNMENT);
	memset(config_ptr[0], 0x00, c + ALIGNMENT);

	cfg_ptr = fread(config_ptr,sizeof(uint8_t),1,fp3);			//#0B Phy api msgs
	cfg_arr_ptr = (uint8_t*)(config_ptr);

	cfg_init_hdr = *((uint8_t*)cfg_arr_ptr);
	cfg_ptr = fread(cfg_arr_ptr+1,sizeof(uint8_t),1,fp3);			//#1B Fapi Handle
	cfg_init_hdr_1 = *((uint8_t*)(cfg_arr_ptr+1));
	cfg_ptr = fread(cfg_arr_ptr+2,sizeof(uint16_t),1,fp3);			//#2B Msg ID
	cfg_msgType = *((uint16_t*)(cfg_arr_ptr+1));
	cfg_ptr = fread(cfg_arr_ptr+4,sizeof(uint32_t),1,fp3);			//#4B Msg Len
	cfg_msgLength = *((uint32_t*)(cfg_arr_ptr+4));
	cfg_ptr = fread(cfg_arr_ptr+8,sizeof(uint8_t),cfg_msgLength,fp3);
	printf("phy_api = %x, fapi_handle = %x, Message type = %x, Message length = %x\n",cfg_init_hdr, cfg_init_hdr_1, cfg_msgType, cfg_msgLength);

	cfg_arr_ptr = (uint8_t*)(config_ptr);
	cfg_msgLength = *((uint32_t*)(cfg_arr_ptr+4));

	int config_len =cfg_msgLength+8;
	//printf("msg_length:%d",config_len);
	wc = write(fpga_w_fd, config_ptr, config_len);
	printf("written CONFIG req.\n");
	printf("bytes tx:%d\n", wc);
	wc = write(file_fd_1,config_ptr, config_len);
	printf("written CONFIG req into a file.\n");
/************1st read from FPGA ********/
	rc = read(fpga_r_fd, rx_ptr, HEADER_LEN);
	printf("bytes rx:%d\n", rc);
	printbuf(HEADER_LEN);

	if (p5_resp->msg_id == CONFIG_RESP_TAG)
		printf("Received CONFIG_RESP");
	else
		crash(2);

	read_len = (int) p5_resp->msg_len;
	printf("\nRecv msg:%d\n ",read_len);
	printbuf(HEADER_LEN);
/************2nd read from FPGA ********/
	rc = read(fpga_r_fd, rx_ptr, read_len+8);
	printf("\nconfig_resp 2nd read:%d\n",rc);

	/*if (rc != read_len) {
	 	printf("CONFIG_RESP read fail\n");
	 	crash(3);
	 }*/
 	printbuf(rc); printf("\n");
	fflush(fp3);  // Flushing the pointers

/****************************Start req*******************************************/
	printf("\nSTART_REQ msg\n");
	p5_msg->phy_api_msgs = 0x1;
	p5_msg->fapi_handle = 0;
	p5_msg->msg_id = START_REQ_TAG;
	p5_msg->msg_len = 0;

	wc = write(fpga_w_fd, tx_ptr, HEADER_LEN);
	printf("start req is Written\n");


/************************ Actual P7 messages starts here **********************/
	printf("Starting reading slot_ind\n");
	int sfn_no,slot_no, s=1,slot=0,file_fd_1,file_fd_2; 
	//file_fd_1 = open("ul_data_out_txt.bin", O_WRONLY | O_TRUNC);
 	//assert(file_fd_1>=0);

    //file_fd_2 = open("dl_data_out_txt.bin", O_WRONLY | O_TRUNC);
    //assert(file_fd_2>=0);

	signal(SIGQUIT, sigint_handler);	// To terminate the program when the ctrl+c is caught

	while (run) {
		//run++;
		for (s=0;s<20;s++){
		rc = read(fpga_r_fd, rx_ptr, HEADER_LEN);
		read_len = (int) p5_resp->msg_len;
		rc = read(fpga_r_fd, slot_ptr, read_len);
		sfn_no = (int) slot_resp->sfn;
		//printf("sfn_no:%d\n", sfn_no);
		slot_no = (int) slot_resp->slot;
		//printf("slot_no:%d\n", slot_no);

		//// Dl msgs ////
		if(slot_no <= 11 ){ 	
			//////////////// dl tti req ////////////////
			slot_start_arr_ptr = (uint8_t*)(arr_ptr[s]);
			dl_msgLength = *((uint32_t*)(slot_start_arr_ptr+4));
			int newLength =dl_msgLength+8;
			wc = write(fpga_w_fd, arr_ptr[s], newLength);
			//printf("dl_tti_req:%d\n", wc);

			/////////////// Tx data req//////////////////
		 	dl_arr_ptr = (uint8_t*)(dat_ptr[s]);
			tx_msgLength = *((uint32_t*)(dl_arr_ptr+4));
			int newLength_dat =tx_msgLength+8;
			wc = write(fpga_w_fd, dat_ptr[s], newLength_dat);
			//printf("tx_data_req:%d\n", wc);
		}

		//// Dummy slot ////
		else if(slot_no == 12){
			//printf("No_data : 12\n");
		}

		//// Ul msgs ////
		else if(slot_no >= 13 && slot_no <=19){

		/////////////// TX_TTI_REQ//////////////////
		 	ul_arr_ptr = (uint8_t*)(ul_ptr[0]);
		 	 ptrSFN = ((uint16_t*)(ul_arr_ptr+8));	// This is to add the input slot tick to the dl tti,tx_data, ul_tti and send it to the chain
			*ptrSFN = (uint16_t) slot_resp->sfn;
			ptrSLOT = ((uint16_t*)(ul_arr_ptr+10));
			*ptrSLOT = (uint16_t) slot_resp->slot;

			ul_msgLength = *((uint32_t*)(ul_arr_ptr+4));
			int newLength_dat_ul =ul_msgLength+8;
			wc = write(fpga_w_fd, ul_ptr[0], newLength_dat_ul);
			//printf("ul_tti_req:%d\n", wc);
	      }			
	   }
	}

	printf("Stub terminated, Stopped sending data");

	printf("\n");
}
