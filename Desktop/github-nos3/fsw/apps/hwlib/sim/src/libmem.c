#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "libmem.h"

#ifdef __cplusplus
extern "C" {
#endif

pthread_mutex_t mutex;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * get_shared_mem() -   Creates a shared memory segment and attaches that segment to an unused address.
 * 
 * Inputs:              uint32_t address:   Requested address
 *                      uint32_t length:    Length of memory request
 *
 * Outputs:             returns void *:     Address containing the shared memory segment. 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void *get_shared_mem(uint32_t address, uint32_t length){

    pthread_mutex_lock(&mutex);

    // Creates shared memory segment or returns the identifier of the previously created segment.
    int32_t shmid = shmget(address, length, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (shmid == -1) {
        printf("LIBMEM SHMGET ERROR %d\n", errno);
        return NULL;
    }

    // Attaches shared memory segment to an unused page-aligned address chosen by the system
    uint32_t *sh_mem_addr = (uint32_t*)shmat(shmid, NULL, 0);
    if (sh_mem_addr == (uint32_t*)(-1)) {
        printf("LIBMEM SHMAT ERROR %d\n", errno);
        return NULL;
    }

    return sh_mem_addr;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * detach_shared_mem() -   Detaches the shared memory segment.
 *  
 * Inputs:                 uint32_t:    Address of the shared memory segment.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void detach_shared_mem(uint32_t *shared_addr){
    while (shmdt((void*)shared_addr) == -1) {
        printf("LIBMEM SHMDT ERROR %d\n", errno);
    }
    pthread_mutex_unlock(&mutex);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * devmem_write() - Higher-level function to write to shared memory. 
 *
 * Inputs:          uint32_t addr:      Address to write to
 *                  uint8_t *in:        Input data to write
 *                  int32_t length:     Length of input data
 *
 * Outputs:         returns int32_t:    Length of data written on success, -1 on failure
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t devmem_write(uint32_t addr, uint8_t *in, int32_t length){
	uint32_t *local_addr;
    uint8_t byte;  

	if((local_addr = (uint32_t*)get_shared_mem(addr, length)) == NULL) {
		return -1;  
    }

    for(byte = 0; byte < length; byte++) {
        local_addr[byte] = in[byte]; 
    }

    detach_shared_mem(local_addr); 

    return length; 
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * devmem_read() -  Higher-level function to read from shared memory. 
 *
 * Inputs:          uint32_t addr:      Address to read from
 *                  uint8_t *out:       Output buffer to read into
 *                  int32_t length:     Length of data to read
 *
 * Outputs:         returns int32_t:    Length of data read on success, -1 on failure  
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t devmem_read(uint32_t addr, uint8_t *out, int32_t length){
	uint32_t *local_addr;
    uint8_t byte;

	if((local_addr = (uint32_t*)get_shared_mem(addr, length)) == NULL) {
		return -1;  
    }

    for(byte = 0; byte < length; byte++) {
        out[byte] = local_addr[byte]; 
    }

    detach_shared_mem(local_addr); 

    return length; 
}

#ifdef __cplusplus
}
#endif
