#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "libmem.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Xil_MMAP() - Map physical address to userspace using mmap intermediary with /dev/mem. 
 *              MMAP creates userspace intermediary buffers to other memory locations. MMAP users can write/read 
 *              to/from the intermediaries and MMAP can sync changes to the buffers with the memory they are mapped
 *              to. Anything mapped by MMAP must be page aligned. This function calculates the page offset of the
 *              requested address and then creates mapped page buffers using mmap that users may interface to. 
 *
 * Inputs:      _off_t address: Requested address
 *              size_t length:  Length of memory request
 *
 * Outputs:     returns void *: Address offset into mapped page. Corresponds to requested physical address. 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void *Xil_MMAP(__off_t address, size_t length){
	size_t 	pagesize    = sysconf(_SC_PAGE_SIZE); 
	__off_t	page_base   = (address / pagesize) * pagesize; 
	__off_t	page_offset = address - page_base; 
	unsigned char *sysaddr; 
	int fd, error;  

	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
		return NULL; 

	if((sysaddr = mmap(NULL, page_offset + length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, page_base)) == MAP_FAILED){
        error = errno; 
        OS_printf("MMAP ERROR = %d\n", error); 
		sysaddr = NULL; 
    }
	close(fd); 

	return sysaddr + page_offset; 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * Xil_MMAP_close() -   Sync page buffers and unmap previously mapped memory address with MMAP.
 *                      Mapped intermediary buffers must be freed since they are dynamically allocated with MMAP. 
 *                      The kernel periodically syncs mapped buffer changes. This should be done explicitly prior
 *                      to close however just to make sure. The msync() function does this. 
 *                      TODO: Normally the flag MS_SYNC given to msync tells it to wait for completion of the sync
 *                      before moving forward. This is currently broken and results in msync returning EINVAL. As
 *                      an alternative, we're just waiting explicitly for it to finish. 
 *  
 * Inputs:              __off_t shared_addr:    Mapped buffer address previously returned by MMAP
 *                      __off_t address:        Physical address previously mapped by MMAP 
 *                      size_t length:          Data length previously mapped by MMAP
 *                      int read:               Whether this buffer was used for writing
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Xil_MMAP_close(__off_t shared_addr, __off_t address, size_t length, int read){
	size_t 	pagesize    = sysconf(_SC_PAGE_SIZE); 
	__off_t	page_base   = (address / pagesize) * pagesize; 
	__off_t	page_offset = address - page_base; 
    int     error; 

    if(!read){
        if(msync((void *)(shared_addr - page_offset),  page_offset + length, MS_ASYNC) != 0){
            error = errno; 
            OS_printf("MSYNC ERROR = %d\n", error); 
        }
        usleep(10000); 
    }

    if(munmap((void *)(shared_addr - page_offset), page_offset + length)){
        error = errno; 
        OS_printf("MUNMAP ERROR = %d\n", error); 
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * Xil_MMAP_Out32() -   Write 32-bit register from userspace. 
 *
 * Inputs:              unsigned int Addr:      Register address to write to
 *                      unsigned int Value:     32-bit value to write 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Xil_MMAP_Out32(unsigned int Addr, unsigned int Value){
	volatile unsigned int   *LocalAddr;

	if((LocalAddr = Xil_MMAP(Addr, 4)) == NULL)
		return; 
	
    *LocalAddr = Value; 

    Xil_MMAP_close((unsigned long int)LocalAddr, Addr, 4, 0); 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * Xil_MMAP_In32() -    Read 32-bit register from userspace.
 *
 * Inputs:              unsigned int Addr:      Register address to read from
 * Outputs:             returns unsigned int:   Data from register 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
unsigned int Xil_MMAP_In32(unsigned int Addr){
	volatile unsigned int  *LocalAddr;
    unsigned int           res; 

	if((LocalAddr = Xil_MMAP(Addr, 4)) == NULL)
		return -1; 

    res = *LocalAddr; 

    Xil_MMAP_close((unsigned long int)LocalAddr, Addr, 4, 1); 
    return res; 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * devmem_write() - Higher-level function to write physical memory from user-space. 
 *
 * Inputs:          unsigned int addr:  Address to write to
 *                  unsigned char *in:  Input data to write
 *                  int length:         Length of input data
 *
 * Outputs:         returns int:        Length of data written on success, -1 on failure
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t devmem_write(uint32_t addr, uint8_t *in, int32_t length){
	volatile unsigned char *LocalAddr;
    int                     byte;  

	if((LocalAddr = Xil_MMAP(addr, length)) == NULL)
		return -1;  

    for(byte = 0; byte < length; byte++)
        LocalAddr[byte] = in[byte]; 

    Xil_MMAP_close((unsigned long int)LocalAddr, addr, length, 0); 

    return length; 
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * devmem_read() -  Higher-level function to read physical memory from user-space. 
 *
 * Inputs:          unsigned int addr:  Address to read from
 *                  unsigned char *out: Output buffer to read into
 *                  int length:         Length of data to read
 *
 * Outputs:         returns int:        Length of data read on success, -1 on failure  
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t devmem_read(uint32_t addr, uint8_t *out, int32_t length){
	volatile unsigned char *LocalAddr;
    int                     byte;

	if((LocalAddr = Xil_MMAP(addr, length)) == NULL)
		return -1;  

    for(byte = 0; byte < length; byte++)
        out[byte] = LocalAddr[byte]; 

    Xil_MMAP_close((unsigned long int)LocalAddr, addr, length, 1); 

    return length; 
}
