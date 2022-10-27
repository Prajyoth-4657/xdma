/* AUTHOR - K. SAI PRANEETH AND K. SAI PRASANTH
DATE - 4-08-2021
CODE FOR L2/L3 STUB */
#define DEVICE_NAME_DEFAULT_WRITE "/dev/xdma0_h2c_0"
#define DEVICE_NAME_DEFAULT_READ "/dev/xdma0_c2h_0"

#define DEVICE_NAME_DEFAULT_WRITE1 "/dev/xdma0_c2h_1"
#define DEVICE_NAME_DEFAULT_WRITE2 "/dev/xdma0_c2h_2"

#define ALIGNMENT 4096

#define RX_SIZE_MAX 20000
#define TX_SIZE_MAX 16384
#define DAT_SIZE_MAX 2000000
#define CONFIG_SIZE_MAX 20000
#define SLOT_SIZE_MAX 32

typedef struct {
        uint8_t phy_api_msgs;
        uint8_t fapi_handle;
        uint16_t msg_id;
        uint32_t msg_len;
} __attribute__((packed, aligned(1))) nfapi_header_t;

typedef struct {
        uint16_t sfn;
        uint16_t slot;
} __attribute__((packed, aligned(1))) sfn_slot_t;


#define HEADER_LEN 8

#define PARAM_REQ_TAG 0x00
#define PARAM_REQ_LEN 0

#define PARAM_RESP_TAG 0x01
#define PARAM_RESP_LEN 0x1

#define CONFIG_REQ_TAG 0x02
#define CONFIG_REQ_LEN 0x42

#define CONFIG_RESP_TAG 0x03
#define CONFIG_RESP_LEN 0x4

#define START_REQ_TAG 0x04
#define START_REQ_LEN 0

#define SLOT_IND_TAG 0x82
#define SLOT_IND_LEN 4

#define STOP_REQ_TAG 0x05
#define STOP_REQ_LEN 0x00
