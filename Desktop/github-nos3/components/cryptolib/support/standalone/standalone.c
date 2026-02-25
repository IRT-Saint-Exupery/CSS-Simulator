/* Copyright (C) 2009 - 2022 National Aeronautics and Space Administration.
   All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any kind, either expressed, implied, or statutory,
   including, but not limited to, any warranty that the software will conform to specifications, any implied warranties
   of merchantability, fitness for a particular purpose, and freedom from infringement, and any warranty that the
   documentation will conform to the program, or any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or
   consequential damages, arising out of, resulting from, or in any way connected with the software or its
   documentation, whether or not based upon warranty, contract, tort or otherwise, and whether or not loss was sustained
   from, or arose out of the results of, or use of, the software, documentation or services provided hereunder.

   ITC Team
   NASA IV&V
   jstar-development-team@mail.nasa.gov
*/


/*******************************************************************************
** Standalone CryptoLib Implementation
** UDP interfaces to apply / process each frame type and return the result.
*******************************************************************************/

#include "standalone.h"
#include "/home/nos3/Desktop/github-nos3/fsw/osal/src/os/inc/common_types.h"
//#include "/home/nos3/Desktop/github-nos3/fsw/apps/io_lib/fsw/public_inc/tmtf.h"

/* Return codes */
#define TMTF_SUCCESS            (0)
#define TMTF_ERROR             (-1)
#define TMTF_INVALID_POINTER   (-2)
#define TMTF_INVALID_SECHDR    (-3)
#define TMTF_INVALID_LENGTH    (-4)

/* Fixed parameters to compute CRC */
#define TMTF_CRC_INIT_REGISTRY  0xffffU
#define TMTF_CRC_POLYNOMIAL     0x11021UL

/* Max number of virtual channels */
#define TMTF_MAX_VC                 8  

/* Fixed values */
#define TMTF_VERSION                1
#define TMTF_FSH_VERSION            0

#define TMTF_PRIHDR_LENGTH          6  
#define TMTF_SECHDR_MAX_LENGTH      63 
#define TMTF_OCF_LENGTH             4  
#define TMTF_ERR_CTRL_FIELD_LENGTH  2  
#define TMTF_NO_FIRST_HDR_PTR       (0x07 << 8 | 0xFF)
#define TMTF_OID_FIRST_HDR_PTR      (0x07 << 8 | 0xFE)
#define TMTF_WR_CRC(ptr,val)           (((ptr)[0] = (val >> 8) & 0xFF),\
                                        ((ptr)[1] = (val) & 0xFF))

typedef struct
{
    uint8 Id[2];               /* MC id, VC id, OCF flag      */
    uint8 McFrameCount;        /* Master Channel Frame Count  */
    uint8 VcFrameCount;        /* Virtual Channel Frame Count */
    uint8 DataFieldStatus[2];  /* TF Data Field Status        */
} TMTF_PriHdr_t;

static uint16 crcTable[256];


/*
 * Function: TMTF_UpdateErrCtrlField
 *
 */
int32 TMTF_UpdateErrCtrlField(TMTF_PriHdr_t *tfPtr, uint16 offset)
{
    uint16 reg = 0xffffU;
    uint8 *octPtr = NULL;
    //uint8 byte;
    uint32 len = 0;
    uint16_t short_c;
    uint16_t tmp;
    
    if (tfPtr == NULL)
    {
        return TMTF_INVALID_POINTER;
    }

    if (offset < sizeof(TMTF_PriHdr_t))
    {
        return TMTF_INVALID_LENGTH;
    }

    /* First clear the CRC bytes */
   // CFE_PSP_MemSet((void *)((uint8 *)tfPtr + offset), 0x00, 2); // IRT CSS
    ((uint8 *)tfPtr)[offset] = 0;
    ((uint8 *)tfPtr)[offset+1] = 0;
    
    octPtr = (uint8 *) tfPtr;

    //len = offset + 2; IRT CSS
    len = offset;
    while (len--)
    {
        //byte = (reg >> 8) & 0xff;
        //reg = (reg << 8) | *octPtr;
        //reg ^= crcTable[byte];
        //octPtr++;
        
        short_c = 0x00ff & (unsigned short) *octPtr;
	tmp     = (reg >> 8) ^ short_c;
	reg     = (reg << 8) ^ crcTable[tmp];
        octPtr++;
        
    }

    TMTF_WR_CRC((uint8 *)tfPtr + offset, reg);

    return TMTF_SUCCESS;
}


/*
 * Function: TMTF_GenCrcTable
 *
 * Purpose:
 *   Generate the CRC Table for CRC computation
 
 * Arguments:
 *   polynomial  : The polynomial coefficients (eg. 0x1021: x^16 + x^12 + x^5 + 1)
 *
 * Note:
 *   - The highest order coefficient is not required in the polynomial.
 *
 */
void TMTF_GenCrcTable(uint32 polynomial)
{
    uint32 remainder = 0;
    uint16 topbit = 1 << 15;
    uint16 val = 0;
    uint8 bit = 0;

    for (val = 0; val < 256; ++val)
    {
        /* The first remainder (16-bit) is the divident. */
        remainder = val << 8;
        
        /* Perform modulo-2 division, one bit at a time */
        for (bit = 0; bit < 8; ++bit)
        {
            /* If the remainder has topbit, divide by polynomial */
            if (remainder & topbit)
            {
                remainder = (uint16)(remainder << 1) ^ (uint16)(polynomial);
            }
            else
            {
                remainder = remainder << 1;
            }
        }

        crcTable[val] = (uint16)remainder;       
    }
}


/*
** Global Variables
*/
static volatile uint8_t keepRunning = CRYPTO_LIB_SUCCESS;
static volatile uint8_t tc_seq_num = 0;
static volatile uint8_t tc_vcid = CRYPTO_STANDALONE_FRAMING_VCID;
static volatile uint8_t tc_debug = 1;

unsigned int satId = 0;


/* 
** Functions
*/
int32_t crypto_standalone_check_number_arguments(int actual, int expected)
{
    int32_t status = CRYPTO_LIB_SUCCESS;
    if (actual != expected)
    {
        status = CRYPTO_LIB_ERROR;
        printf("Invalid command format or number of arguments, type 'help' for more info\n");
    }
    return status;
}

void crypto_standalone_to_lower(char* str)
{
    char* ptr = str;
    while(*ptr)
    {
        *ptr = tolower((unsigned char) *ptr);
        ptr++;
    }
    return;
}

void crypto_standalone_print_help(void)
{
    printf(CRYPTO_PROMPT "command [args]\n"
            "----------------------------------------------------------------------\n"
            "exit                               - Exit app                         \n"
            "help                               - Display help                     \n"
            "noop                               - No operation command to device   \n"
            "reset                              - Reset CryptoLib                  \n"
            "tc                                 - Toggle TC debug prints           \n"
            "vcid #                             - Change active TC virtual channel \n"
            "\n"
        );   
}

int32_t crypto_standalone_get_command(const char* str)
{
    int32_t status = CRYPTO_CMD_UNKNOWN;
    char lcmd[CRYPTO_MAX_INPUT_TOKEN_SIZE];
    
    strncpy(lcmd, str, CRYPTO_MAX_INPUT_TOKEN_SIZE);
    crypto_standalone_to_lower(lcmd);

    if(strcmp(lcmd, "help") == 0) 
    {
        status = CRYPTO_CMD_HELP;
    }
    else if(strcmp(lcmd, "exit") == 0) 
    {
        status = CRYPTO_CMD_EXIT;
    }
    else if(strcmp(lcmd, "noop") == 0) 
    {
        status = CRYPTO_CMD_NOOP;
    }
    else if(strcmp(lcmd, "reset") == 0) 
    {
        status = CRYPTO_CMD_RESET;
    }
    else if(strcmp(lcmd, "vcid") == 0) 
    {
        status = CRYPTO_CMD_VCID;
    }
    else if(strcmp(lcmd, "tc") == 0) 
    {
        status = CRYPTO_CMD_TC_DEBUG;
    }
    return status;
}

int32_t crypto_standalone_process_command(int32_t cc, int32_t num_tokens, char* tokens)
{
    int32_t status = CRYPTO_LIB_SUCCESS;

    /* Process command */
    switch(cc) 
    {	
        case CRYPTO_CMD_HELP:
            crypto_standalone_print_help();
            break;
        
        case CRYPTO_CMD_EXIT:
            keepRunning = CRYPTO_LIB_ERROR;
            break;

        case CRYPTO_CMD_NOOP:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                printf("NOOP command success\n");
            }
            break;
        
        case CRYPTO_CMD_RESET:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                status = crypto_reset();
                printf("Reset command received\n");
            }
            break;

        case CRYPTO_CMD_VCID:
            if (crypto_standalone_check_number_arguments(num_tokens, 1) == CRYPTO_LIB_SUCCESS)
            {
                uint8_t vcid = (uint8_t) atoi(&tokens[0]);
                /* Confirm new VCID valid */
                if (vcid < 64)
                {
                    SaInterface sa_if = get_sa_interface_inmemory();
                    SecurityAssociation_t* test_association = NULL;
                    sa_if->sa_get_from_spi(vcid, &test_association);
                    
                    /* Handle special case for VCID */
                    if(vcid == 1)
                    {
                        printf("Special case for VCID 1! \n");
                        vcid = 0;
                    }

                    if ((test_association->sa_state == SA_OPERATIONAL) &&
                        (test_association->gvcid_blk.mapid == TYPE_TC) &&
                        (test_association->gvcid_blk.scid == satId)) // TODO
                        // (test_association->gvcid_blk.scid == SCID))
                    {
                        tc_vcid = vcid;
                        printf("Changed active virtual channel (VCID) to %d \n", tc_vcid);
                    }
                    else
                    {
                        printf("Error - virtual channel (VCID) %d is invalid! Sticking with prior vcid %d \n", vcid, tc_vcid);
                    }
                }
                else
                {
                    printf("Error - virtual channl (VCID) %d must be less than 64! Sticking with prior vcid %d \n", vcid, tc_vcid);
                }
            }
            break;

        case CRYPTO_CMD_TC_DEBUG:
            if (crypto_standalone_check_number_arguments(num_tokens, 0) == CRYPTO_LIB_SUCCESS)
            {
                if (tc_debug == 0)
                {
                    tc_debug = 1;
                    printf("Enabled TC debug prints! \n");
                }
                else
                {
                    tc_debug = 1;
                    printf("Disabled TC debug prints! \n");
                }
            }
            break;
        
        default: 
            printf("Invalid command format, type 'help' for more info\n");
            status = CRYPTO_LIB_ERROR;
            break;
    }

    return status;
}

int32_t crypto_standalone_udp_init(udp_info_t* sock, int32_t port)
{
    int status = CRYPTO_LIB_SUCCESS;
    int optval;
    socklen_t optlen;

    sock->port = port;

    /* Create */
    sock->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(sock->sockfd == -1)
    {
        printf("udp_init:  Socket create error port %d", sock->port);
    }

    /* Bind */
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    saddr.sin_port = htons(sock->port);   
    status = bind(sock->sockfd, (struct sockaddr *) &saddr, sizeof(saddr));
    if (status != 0)
    {
        printf(" udp_init:  Socker bind error with port %d", sock->port);
        status = CRYPTO_LIB_ERROR;
    }

    /* Keep Alive */
    optval = 1;
    optlen = sizeof(optval);
    setsockopt(sock->sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);    

    return status;
}

int32_t crypto_reset(void)
{
    int32_t status;

    status = Crypto_Shutdown();
    if(status != CRYPTO_LIB_SUCCESS)
    {
        printf("CryptoLib initialization failed with error %d \n", status);
    }

    status = Crypto_Init_TC_Unit_Test();
    // TODO: CryptoLib appears to be looking at the second byte and not specficially the SCID bits
    if(status != CRYPTO_LIB_SUCCESS)
    {
        printf("CryptoLib initialization failed with error %d \n", status);
    }

    return status;
}

void crypto_standalone_tc_frame(uint8_t* in_data, uint16_t in_length, uint8_t* out_data, uint16_t* out_length)
{

#define fecsize 2  //TODO

    /* TC Length */
    //*out_length = (uint16_t) CRYPTO_STANDALONE_FRAMING_TC_DATA_LEN + 6;

    /* TC Header */
    //out_data[0] = 0x20;
    //out_data[1] = CRYPTO_STANDALONE_FRAMING_SCID;
    //out_data[2] = ((tc_vcid << 2) & 0xFC) | (((uint16_t) CRYPTO_STANDALONE_FRAMING_TC_DATA_LEN >> 8) & 0x03);
    //out_data[3] = (uint16_t) CRYPTO_STANDALONE_FRAMING_TC_DATA_LEN & 0x00FF;
    //out_data[4] = tc_seq_num++; 
    
    /* TC Length */  //TODO to avoid filling the TC frame with 0
    *out_length = in_length + 6 + fecsize;

    /* TC Header */
    out_data[0] = 0x20;
    out_data[1] = CRYPTO_STANDALONE_FRAMING_SCID;
    out_data[2] = ((tc_vcid << 2) & 0xFC) | (((*out_length - 1) >> 8) & 0x03);
    out_data[3] = (*out_length - 1) & 0x00FF;
    out_data[4] = tc_seq_num++;
   
    /* Segement Header */
    //out_data[5] = 0x00;
    out_data[5] = 0xC0;  // TODO unsegmented packet 

    /* SDLS Header */

    /* TC Data */
    memcpy(&out_data[6], in_data, in_length);

    /* SDLS Trailer */
}

void* crypto_standalone_tc_apply(void* sock)
{
    int32_t status = CRYPTO_LIB_SUCCESS;
    udp_info_t* tc_sock = (udp_info_t*) sock;
    
    uint8_t tc_apply_in[TC_MAX_FRAME_SIZE];
    uint16_t tc_in_len = 0;
    uint8_t* tc_out_ptr;
    uint16_t tc_out_len = 0;
    uint16_t fecf;
    
    #ifdef CRYPTO_STANDALONE_HANDLE_FRAMING
        uint8_t tc_framed[TC_MAX_FRAME_SIZE];
    #endif

    struct sockaddr_in rcv_addr;
    struct sockaddr_in fwd_addr;
    int sockaddr_size = sizeof(struct sockaddr_in);

    fwd_addr.sin_family = AF_INET;
    fwd_addr.sin_addr.s_addr = inet_addr("192.168.100.5");
    fwd_addr.sin_port = htons(TC_APPLY_FWD_PORT);

    /* Prepare */
    memset(tc_apply_in, 0x00, sizeof(tc_apply_in));

    while(keepRunning == CRYPTO_LIB_SUCCESS)
    {
        /* Receive */
        status = recvfrom(tc_sock->sockfd, tc_apply_in, sizeof(tc_apply_in), 0, (struct sockaddr*) &rcv_addr, (socklen_t*) &sockaddr_size);
       
	//printf ("************************ received by standalone with NBcar = %d \n",status); // IRT CSS
	
        if (status != -1)
        {
            tc_in_len = status;
            
           if (tc_debug == 1) {
                printf("crypto_standalone_tc_apply - received[%d]: 0x", tc_in_len);
                for(int i = 0; i < status; i++)
                {
                    printf("%02x", tc_apply_in[i]);
                }
                printf("\n");
           }
            
            // Intercept special packets for CFDP Uplink 0x18b3 Fc = 30  : 18 B3 C0 00 00 89 1E .. ..   
            // use external CFDP library to send the file (this is a work around as COSMOS-NASA CF do not work properly)   // only CFDP Class 1 supported for now !    
           if ((tc_apply_in[0] == 0x18) && (tc_apply_in[1] == 0xb3) && (tc_apply_in[6] == 0x1e) ) {
           
                   printf("CFDP Request for File Uplink. \n");
                    
		   char shell_command[512];
		   char source_file[65];
		   char dest_file[65];
		   		   
		   source_file[64] = 0;
		   dest_file[64] = 0;
		   
		   memmove(source_file, tc_apply_in + 9, 64); //read source file 
		   
		   memmove(dest_file, tc_apply_in + 9 + 64, 64); // read dest file 
		   
		   if (tc_apply_in[8] == 0x01) { // class 2
		     sprintf(shell_command, 
		     "python3 /home/nos3/Desktop/github-nos3/gsw/scripts/python-cfdp-main/examples/spacepacket_transport/local_send_file_class2.py %s %s %d %d &",source_file,dest_file,TC_APPLY_PORTS[satId],0xb3); 
		   }
		   
		   else if (tc_apply_in[8] == 0x00) { // class 1
		     sprintf(shell_command, 
		     "python3 /home/nos3/Desktop/github-nos3/gsw/scripts/python-cfdp-main/examples/spacepacket_transport/local_send_file_class1.py %s %s %d %d &",source_file,dest_file,TC_APPLY_PORTS[satId],0xb3); 
		   }
		   
		   else {
		     printf("Error: Bad CFDP Class Specified. \n");
		   }
		    
		   system(shell_command);
		   
		   continue;                    
           }
            

            /* Frame */
            #ifdef CRYPTO_STANDALONE_HANDLE_FRAMING
                crypto_standalone_tc_frame(tc_apply_in, tc_in_len, tc_framed, &tc_out_len);
                memcpy(tc_apply_in, tc_framed, tc_out_len);
                tc_in_len = tc_out_len;
                tc_out_len = 0;
                if (tc_debug == 1)
                {
                    printf("crypto_standalone_tc_apply - framed[%d]: 0x", tc_in_len);
                    for(int i = 0; i < tc_in_len; i++)
                    {
                        printf("%02x", tc_apply_in[i]);
                    }
                    printf("\n");
                }
            #endif

            /* Process */
            status = Crypto_TC_ApplySecurity(tc_apply_in, tc_in_len, &tc_out_ptr, &tc_out_len);
            if (status == CRYPTO_LIB_SUCCESS)
            {
              /* Calculate FECF*/
              fecf = Crypto_Calc_FECF(tc_out_ptr, tc_out_len - 2);
              tc_out_ptr[tc_out_len - 2] = (uint8_t) ((fecf & 0xFF00) >> 8);
              tc_out_ptr[tc_out_len - 1] = (uint8_t) (fecf & 0x00FF);
            
                if (tc_debug == 1)
                {
                    printf("crypto_standalone_tc_apply - status = %d, encrypted[%d]: 0x", status, tc_out_len);
                    for(int i = 0; i < tc_out_len; i++)
                    {
                        printf("%02x", tc_out_ptr[i]);
                    }
                    printf("\n");
                }

                /* Reply */
                status = sendto(tc_sock->sockfd, tc_out_ptr, tc_out_len, 0, (struct sockaddr*) &fwd_addr, sizeof(fwd_addr));
                if ((status == -1) || (status != tc_out_len))
                {
                    printf("crypto_standalone_tc_apply - Reply error %d \n", status);
                }
            }
            else
            {
                printf("crypto_standalone_tc_apply - ApplySecurity error %d \n", status);
            }

            /* Reset */
            memset(tc_apply_in, 0x00, sizeof(tc_apply_in));
            tc_in_len = 0;
            tc_out_len = 0;
            if (!tc_out_ptr) free(tc_out_ptr);
            if (tc_debug == 1)
            {
                printf("\n");
            }
        }

        /* Delay */
        usleep(100);
    }
    close(tc_sock->port);
    return tc_sock;
}

void crypto_standalone_tm_frame(uint8_t* in_data, uint16_t in_length, uint8_t* out_data, uint16_t* out_length)
{
    /* TM Length */
    *out_length = (uint16_t) in_length - 10;

    /* TM Header */
    memcpy(out_data, &in_data[10], in_length - 10);
}

// TODO 
typedef struct
{
	unsigned char synchro[4];
	unsigned char Id[6];
} ccsdsTmTfHeader_t;

#define TMTF_RD_SCID(hdr)          	   (((hdr)->Id[0]  & 0x3F) << 4) | (((hdr)->Id[1] & 0xF0) >> 4)


// Definition of the CCSDS TM Transfer Frame Header
typedef struct __attribute__((packed)) {
       uint8_t synchro[4];
    uint8_t version_number : 2;                // CCSDS version number (usually 0)
    uint16_t spacecraft_id : 10;               // Spacecraft ID
    uint8_t virtual_channel_id : 3;            // Virtual Channel ID (VCID)
    uint8_t op_control : 1;              // op control
    uint8_t master_frame_count : 8;            // MFC count modulo 256
    uint8_t virtual_channel_count : 8;   // VC count modulo 256
    uint8_t flags : 5;                                // 5 bits: unused here
    uint16_t first_header_pointer : 11;  // Pointer to the first header in the frame data field
} CCSDS_TM_TransferFrameHeader;

#define SPP_MAX_SIZE      0xFFFF
#define SECURITY_HEADER_LENGTH 0
#define HAS_FEC    1


void* crypto_standalone_tm_process(void* sock)
{
    udp_info_t* tm_sock = (udp_info_t*) sock;
    struct sockaddr_in rcv_addr;
    struct sockaddr_in fwd_addr;
    int sockaddr_size = sizeof(struct sockaddr_in);
        
    CCSDS_TM_TransferFrameHeader *tfHeader;
      uint8_t discontinuous = 1;                                   // True if discontinuous master frame counts
       uint8_t opControl;
       uint16_t firstHeaderPointer;
       uint8_t prevMasterFrameCount = 0;
    int32_t status = CRYPTO_LIB_SUCCESS;
    uint8_t recvBuffer[TM_FRAME_DATA_SIZE];
    uint8_t *source;
    uint8_t *dest;
       uint16_t  sizeToTransfer;                // From the Transfer Frame to the bucket

       uint8_t sppBucket[SPP_MAX_SIZE];  // a bucket for received packets
       uint16_t sppNbBytes;                  // nb bytes in the spp bucket
    uint16_t sppLen = 0;                       // Spp length as per header declaration
    uint8_t* sppPtr;

    fwd_addr.sin_family = AF_INET;
    fwd_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    fwd_addr.sin_port = htons(TM_PROCESS_FWD_PORT);
    
    TMTF_GenCrcTable(TMTF_CRC_POLYNOMIAL);

    // Receive a frame
    // If it is not an Idle frame
    //   Transfer the data into the bucket (treat discontinuity if any)
    //   Get out complete spps from the bucket

    while(keepRunning == CRYPTO_LIB_SUCCESS)
    {
        /* Receive Frame*/
        status = recvfrom(tm_sock->sockfd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*) &rcv_addr, (socklen_t*) &sockaddr_size);

        if (status != -1)
        {
             tfHeader = (CCSDS_TM_TransferFrameHeader *)recvBuffer;
             
                         
             if (HAS_FEC) {
                    
                    //uint16_t fec =  Crypto_Calc_FECF(recvBuffer+4, status-4); // not working                    
                    uint16_t fecinframe = (recvBuffer[status-2] << 8) + recvBuffer[status-1];                    
                    TMTF_UpdateErrCtrlField((TMTF_PriHdr_t*) (recvBuffer+4), status-6);
                    uint16_t fecinframe2 = (recvBuffer[status-2] << 8) + recvBuffer[status-1]; // Algo CRC-16/CCITT-FALSE / CRCITT_FFFF
                    
                    if (fecinframe != fecinframe2) { 
                          printf ("TMTF FEC error\n");
                          discontinuous = 1;
                          sppNbBytes = 0; // Flush any bytes from the spps buffer
                   prevMasterFrameCount = tfHeader->master_frame_count;
                   continue;
                    }
             }
            
             opControl = recvBuffer[5]&1;                                                                                                //     tfHeader->op_control not used due to the endianess
             uint8_t clcwSize = (opControl == 1)?4:0;
             if (tfHeader->master_frame_count != ((prevMasterFrameCount + 1)&0xFF)) {
                    discontinuous = 1;
                    sppNbBytes = 0; // Flush any bytes from the spps buffer
             }
             firstHeaderPointer = ((recvBuffer[8]&0x7) << 8) + recvBuffer[9];                                         // tfHeader->first_header_pointer not used due to the endianess.
             if (firstHeaderPointer == 0b11111111110) {  // This is an Idle Transfer Frame
                    prevMasterFrameCount = tfHeader->master_frame_count;
                    continue;                                                                      // Wait for the next frame
             }
             dest = sppBucket + sppNbBytes;
             if (discontinuous == 0) {
                    source = recvBuffer + sizeof (CCSDS_TM_TransferFrameHeader) + SECURITY_HEADER_LENGTH;
             }
             else {
                    if (firstHeaderPointer == 0b11111111111) {   // In case of discontinuity and no start packet, then ignore and wait for the next frame.
                          prevMasterFrameCount = tfHeader->master_frame_count;
                          continue;
                    }
                    source = recvBuffer + sizeof (CCSDS_TM_TransferFrameHeader) + SECURITY_HEADER_LENGTH + firstHeaderPointer; // Restart on the next spp start.
             }
             prevMasterFrameCount = tfHeader->master_frame_count;
             discontinuous = 0;

             sizeToTransfer = status - (source - recvBuffer) -(HAS_FEC?2:0) -clcwSize;

             // Transfer packets data into packet buffer (including idle packets)
             memcpy (dest, source, sizeToTransfer);

                    sppNbBytes += sizeToTransfer;


            /* Space Packet Protocol Loop */
             // Send the complete packets (if any) from the packets bucket
            sppPtr = sppBucket;

            while (sppNbBytes > 5)
            {
            
            //IRT CSS //use external CFDP library for this type of TC ! //TODO // temporary solution as COSMOS-NASA CF not working properly for CFDP layer.
            if ((sppPtr[0] == 0x18) && (sppPtr[1] == 0xb6) && ((sppPtr[6] == 0x24)||(sppPtr[6] == 0x34)||(sppPtr[6] == 0x20)||(sppPtr[6] == 0x30)) ) { //24 34 class 1 //20 30 class 2
            //if ((sppPtr[0] == 0x18) && (sppPtr[1] == 0xb6) && ((sppPtr[6] == 0x24)||(sppPtr[6] == 0x34)) ) { //24 34 class 1 
            	sppLen = ((sppPtr[4] << 8) | sppPtr[5]) + 7;
            	int32 status;  
    		static bool first = true;    		
            	static struct sockaddr_in fwd_addrCFDP;
                         
             	printf("CFDP Downlink packets received of len %d and transferred to CFDP Server of SAT %d \n",sppLen, satId);
                					
		if(first == true) {	
		  	fwd_addrCFDP.sin_family = AF_INET;
	   		fwd_addrCFDP.sin_addr.s_addr = inet_addr("127.0.0.1");
	   		int portCFDP = 4913 + 100*satId;  //TODO // 5013 , 5113, 5213, ... see launch_mcs.sh
	    		fwd_addrCFDP.sin_port = htons(portCFDP);
	    		//fwd_addrCFDP.sin_port = htons(TM_PROCESS_FWD_PORT);
	    		first = false;
    		}
    		   				
		status = sendto(tm_sock->sockfd, sppPtr, sppLen, 0, (struct sockaddr*) &fwd_addrCFDP, sizeof(fwd_addrCFDP));
		
               if (status != sppLen)
               {
                      printf("crypto_standalone_tm_process - error in sending to external CFDP server %d \n", status);
               }
		                                                
               sppPtr = &sppPtr[sppLen];
               sppNbBytes = sppNbBytes - sppLen; 
                           
            }
            
           
            else {
                        
		    // intercept TLM alert from IDS   // this is a proto //TODO // see also IDS/gsw IDS_DATA_TLM
		    if ((sppPtr[0] == 0x09) && (sppPtr[1] == 0xFB)) {	    
			//printf(" ********* IDS telemetry received! ****** \n");
		        // send mitigation based on IDS TLM  
			// if MODULE = 5 and SENDING_TASK=CAM then malicious camera on board --> mitigation = resend legit arducam.so      
			if (sppPtr[16] == 5) { //MODULE = 5 //IDS DETECTION
			            printf(" ********* IDS Attack Detection ****** Message = ");			      
		                    bool isfound = false;                 
		                    for (int i = 18; i < 36; ++i) {   
		                            printf("%c",sppPtr[i]);
					    if (toupper(sppPtr[i]) == 'C' && toupper(sppPtr[i+1]) == 'A' && toupper(sppPtr[i+2]) == 'M') { // search for "CAM"
	     					   isfound  = true;	     					   	     					   
	      				           break;
	    				    }	    				    
				     }
				     printf("\n");
				    
				    // we simulate here the operator reaction to an attack // to be improved //TODO			     			    
				    if (isfound) {  // SENDING_TASK contains CAM  // mitigation is not systematic !!!!
					static int lastcall = 0;
					int now = time(0);
					printf(" ********* Malicious CAM detected by IDS! \n");
					if (rand() % 2) { // once every 2 ! //TODO // to be improved			        
						if (lastcall == 0 || now - lastcall > 60) {
						   printf(" ********* Sending mitigation (reload arducam.so) ! ****** \n");
	   					   // add some random time before application of 
	   					   // mitigation to represent operator response (included in bash script)
						   system("python3 /home/nos3/CSS_Attacks/Restore_CAM.py &");
						   lastcall = now;
						}			       
					}
			            }
		      }				
		    }

                                 
                if ((sppPtr[0] >= 0x08) && (sppPtr[0] < 0x10))  // Test if it is a TM packet and not idle.
                {
                    sppLen = ((sppPtr[4] << 8) | sppPtr[5]) + 7;
                    if (sppLen <= sppNbBytes) {// test is the whole packet is in the bucket
                                       
                                       static int imageCount = 0;

                                       // Save the arducam telemetry as a jpg file in /tmp 
                                       if ((sppPtr[0] == 0x08) && (sppPtr[1] == 0xC9)) {
                                       
                                          int isNew = 0;
                                          bool isFinished = false;
                                       
                                              if ((sppPtr[16] == 0xFF && sppPtr[17] == 0xD8) && // start of jpg image
                                                  (!strncmp((char*)sppPtr+22,"JFIF",4) || !strncmp((char*)sppPtr+22,"Exif",4))) { // marker for jpg format        
                                              	 	isNew = 1;
                                              	 	imageCount++;
                                              	 	printf(" ********* start of photo stream detected ! \n");
                                              }
                                           
					       for (int i = 0; i + 1 < sppLen; ++i) {
					    		if (sppPtr[i] == 0xFF && sppPtr[i + 1] == 0xD9) {
								printf(" ********* end of photo stream detected ! \n");
								isFinished = true;
								break; // stop at first JPEG EOI marker
					    		}
					       }
                                                                                                                                     
                                              char filename[512];
                                              sprintf(filename,"/tmp/ArducamSat%d_%d.jpg", satId, imageCount); // write images in /tmp   //TODO check for file rename !
                                              FILE* fp = fopen(filename,isNew?"w":"a");
                                              
                                              for(int i = 16; i < sppLen-10 ; i++)
                   			       {
                        		           fprintf(fp, "%c", sppPtr[i]);
                   			       }
                   			       
                   			       fclose(fp);
                   			       
                   			       if (isFinished) {
                   			       	char command[1024];
                   			       	sprintf(command, "mv %s /tmp/ArducamSat%d.jpg", filename, satId);
                   			       	system(command);
                   			       	printf(" ********* New photo saved in /tmp ! \n");
                   			       }
                   			       
                   			}
                   			      	                          						                                    
                                       #ifdef CRYPTO_STANDALONE_TM_PROCESS_DEBUG
                                              printf("crypto_standalone_tm_process - SPP[%d]: 0x", spp_len);
                                              for(int i = 0; i < spp_len; i++)
                                              {
                                                    printf("%02x", sppPtr[i]);
                                              }
                                              printf("\n");
                                       #endif
                                       status = sendto(tm_sock->sockfd, sppPtr, sppLen, 0, (struct sockaddr*) &fwd_addr, sizeof(fwd_addr));
                                       if ((status == -1) || (status != sppLen))
                                       {
                                              printf("crypto_standalone_tm_process - Reply error %d \n", status);
                                       }
                                       sppPtr = &sppPtr[sppLen];
                                       sppNbBytes = sppNbBytes - sppLen;
                    }
                    else {
                          break; // the packet in the buket is not yet complete. Wait for the next frame
                    }
                }
                else {
                   // only idle packets in the bucket. Clean the bucket
                   sppNbBytes = 0;
                }
            }
            
            }
            
             // shift the bucket, to keep the remaining part of spp (if any)
             if (sppNbBytes != 0) {
                    memcpy (sppBucket, sppPtr, sppNbBytes);
             }

            #ifdef CRYPTO_STANDALONE_TM_PROCESS_DEBUG
                printf("\n");
            #endif
        }

        /* Delay */
        usleep(100);
    }
    close(tm_sock->port);
    return tm_sock;
}

void crypto_standalone_cleanup(const int signal)
{
    if (signal == SIGINT)
    {
        printf("\n");
        printf("Received CTRL+C, cleaning up... \n");
    }
    /* Signal threads to stop */
    keepRunning = CRYPTO_LIB_ERROR;
    exit(signal);
    return;
}

int main(int argc, char* argv[])
{
    int32_t status = CRYPTO_LIB_SUCCESS;
    
    
    char input_buf[CRYPTO_MAX_INPUT_BUF];
    char input_tokens[CRYPTO_MAX_INPUT_TOKENS][CRYPTO_MAX_INPUT_TOKEN_SIZE];
    int num_input_tokens;
    int cmd;    
    char* token_ptr;

    udp_info_t tc_apply;
    udp_info_t tm_process;
    pthread_t tc_apply_thread;
    pthread_t tm_process_thread;

    if (argc != 2)
    {
      printf ("Usage: standalone satId! \n");
      exit (-1);
    }
    satId = atoi(argv[1]);
    
    if (satId <= 0 || satId > NB_SATELLITES) {
      printf ("Bad satId : %d . Shall be in [1 - %d]", satId, NB_SATELLITES);
      exit (-1);
    }
    
    Crypto_Init_SetScId(satId); // Init Crypto with satId

    printf("Starting CryptoLib in standalone mode for Satellite %d ! \n", satId);
    printf("  %d ==> TC  ==> %d \n", TC_APPLY_PORT, TC_APPLY_FWD_PORT);
    printf("  %d ==> TM  ==> %d \n", TM_PROCESS_PORT, TM_PROCESS_FWD_PORT);

    printf("\n");
     
    /* Initialize CryptoLib */
    status = crypto_reset();
    if(status != CRYPTO_LIB_SUCCESS)
    {
        printf("CryptoLib initialization failed with error %d \n", status);
        keepRunning = CRYPTO_LIB_ERROR;
    }
    
    /* Initialize sockets */
    if (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        status = crypto_standalone_udp_init(&tc_apply, TC_APPLY_PORT);
        if (status != CRYPTO_LIB_SUCCESS)
        {
            printf("crypto_standalone_udp_init tc_apply failed with status %d \n", status);
            keepRunning = CRYPTO_LIB_ERROR;
        }
        else
        {
            status = crypto_standalone_udp_init(&tm_process, TM_PROCESS_PORT);
            if (status != CRYPTO_LIB_SUCCESS)
            {
                printf("crypto_standalone_udp_init tm_process failed with status %d \n", status);
                keepRunning = CRYPTO_LIB_ERROR;
            }
        }
    }

    /* Catch CTRL+C */
    signal(SIGINT, crypto_standalone_cleanup);

    /* Start threads */
    if (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        status = pthread_create(&tc_apply_thread, NULL, *crypto_standalone_tc_apply, &tc_apply);
        if (status < 0)
        {
            perror("Failed to create tc_apply_thread thread");
            keepRunning = CRYPTO_LIB_ERROR;
        }
        else
        {
            status = pthread_create(&tm_process_thread, NULL, *crypto_standalone_tm_process, &tm_process);
            if (status < 0)
            {
                perror("Failed to create tm_process_thread thread");
                keepRunning = CRYPTO_LIB_ERROR;
            }
        }
    }

    /* Main loop */
    while (keepRunning == CRYPTO_LIB_SUCCESS)
    {
        num_input_tokens = -1;
        cmd = CRYPTO_CMD_UNKNOWN;

        /* Read user input */
        printf(CRYPTO_PROMPT);
        fgets(input_buf, CRYPTO_MAX_INPUT_BUF, stdin);

        /* Tokenize line buffer */
        token_ptr = strtok(input_buf, " \t\n");
        while ((num_input_tokens < CRYPTO_MAX_INPUT_TOKENS) && (token_ptr != NULL)) 
        {
            if (num_input_tokens == -1) 
            {
                /* First token is command */
                cmd = crypto_standalone_get_command(token_ptr);
                //printf("CMD = %s %d\n",token_ptr,cmd);
            }
            else 
            {
                strncpy(input_tokens[num_input_tokens], token_ptr, CRYPTO_MAX_INPUT_TOKEN_SIZE);
                //printf("Token[%d] = %s\n",num_input_tokens,token_ptr);
            }
            token_ptr = strtok(NULL, " \t\n");
            num_input_tokens++;
        }

        /* Process command if valid */
        if(num_input_tokens >= 0)
        {
            crypto_standalone_process_command(cmd, num_input_tokens, &input_tokens[0][0]);
        }
    }

    /* Cleanup */
    close(tc_apply.port);
    close(tm_process.port);

    Crypto_Shutdown();
    
    printf("\n");
    exit(status);
}
