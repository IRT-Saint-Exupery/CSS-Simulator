#include "ids_p1.h"

#if defined IDS_P1_ENABLED
extern struct P1_struct P1_data;
extern struct P1_struct * P1_struct_ptr;

void init_probe1()
{
	init_probe1_buffer();
	P1_data.buffer_items_nb = 0;
	P1_data.arrival_mean = 0;
	P1_data.last_time_too_quick = 0;
	P1_data.cfdp_is_ongoing = 0;
	for(int i=0; i<64; i++)
	{
		if (i==44)
			P1_data.sa_state_table[i] = 0;
		else
			P1_data.sa_state_table[i] = 1;
		//printf("SA STATE TABLE [%d] = %d\n",i,P1_data.sa_state_table[i]);
	}
	P1_data.bad_crc_count = 0;
}

void init_probe1_buffer()
{
	for(int i=0; i<BUFFER_TIME_SIZE; i++)
	{
		P1_data.buffer_times[i]=0; //replace by memset ?
	}
}

void add_to_buffer1(int64 ticks)
{
	// tete d'écriture avec modulo 10 pour écrire dans buffer + stocker directement dans struct les delta et pas les temps
	/* ADD MUTEX ?*/
	if (P1_data.buffer_items_nb < BUFFER_TIME_SIZE)
	{
		P1_data.buffer_times[P1_data.buffer_items_nb] = ticks;
		P1_data.buffer_items_nb++;
	}
	else
	{
		for (int i = 0; i < BUFFER_TIME_SIZE - 1; i++)
		{
			P1_data.buffer_times[i] = P1_data.buffer_times[i+1];
		}
		P1_data.buffer_times[BUFFER_TIME_SIZE - 1] = ticks;

	}
}

int64 compute_time_mean1()
{
	if (P1_data.buffer_items_nb == 0)
	{
		#ifdef IDS_P1_DEBUG
		printf("No items, no mean ^^\n");
		#endif
		return 0;
	}
	else if (P1_data.buffer_items_nb == 1)
	{
		#ifdef IDS_P1_DEBUG
        	printf("only one item, no arrival rates mean ^^\n");
        	#endif
		return 0;
	}
	else
	{
		int64 sum_arrival_rates = 0;
		
		for (int i = 1; i<P1_data.buffer_items_nb; i++)
		{
			sum_arrival_rates += (P1_data.buffer_times[i]/OS_TIME_TICKS_PER_MSEC - P1_data.buffer_times[i-1]/OS_TIME_TICKS_PER_MSEC);
		}

	//printf("sum arrival rates : %lld, items : %d\n", sum_arrival_rates, P1_data.buffer_items_nb);

		// -a/4 + e/4
		// optimized processing test, fix if buffer size goes up, not worth it for a 10 elements buffer imo
		/*if (P1_data.arrival_mean != 0)
		{	
			uint32 diff_last_element;
			diff_last_element = P1_data.buffer_times[P1_data.buffer_items_nb - 1] - P1_data.buffer_times[P1_data.buffer_items_nb - 2];
			
			uint32 new_mean = 0;

			if (P1_data.buffer_items_nb < BUFFER_TIME_SIZE)
			{
				//we're filling up the buffer	
				new_mean = P1_data.arrival_mean*(P1_data.buffer_items_nb-2)/(P1_data.buffer_items_nb-1); //correct the division of the mean
				new_mean += (diff_last_element/(P1_data.buffer_items_nb-1));
			}
			else
			{
				//buffer is already full, substract value of the element to be removed and add value of the one to be added in buffer
				//HOW TO DO THIS ??
			}
			printf("optimized processing : mean : %d\n", new_mean);
		}*/

		int64 processed_mean = sum_arrival_rates / (P1_data.buffer_items_nb - 1);
		P1_data.arrival_mean = processed_mean;
		return processed_mean;
	}
}

void print_buffer1()
{
	/* ADD MUTEX ???*/
	if (P1_data.buffer_items_nb == 0)
	{
		printf("buffer p1 empty !\n");
	}
	else
	{
		for (int i = 0; i < P1_data.buffer_items_nb; i++)
		{
			printf("buffer p1 [%d] : %lld\n", i, P1_data.buffer_times[i]);
		}
	}
}

char is_rate_allowed(OS_time_t simtime)
{
	add_to_buffer1(simtime.ticks);

	char return_value = true;
	
	/* If there was another packet before, begin packet rate checks */
	if (P1_data.buffer_items_nb > 1)
	{
		int64 mean = compute_time_mean1();///OS_TIME_TICKS_PER_MSEC;
		//printf("mean : %lld\n",mean);
		#if defined IDS_P2_ENABLED && defined CFDP_FLOOD_LIFT
		sem_wait(&(P1_struct_ptr->cfdp_sem));
		//If there is a CFDP transaction, lift the threshold temporarily - TODO - ADD THE TEMPORARILY ASPECT -- bring back cfdp_is_ongoing to false directly after finishing counting ?
		if(!P1_struct_ptr->cfdp_is_ongoing)
		{
		#endif
			if (mean < (int64) P1_DOS_THRESHOLD)
			{
				//print_buffer1();
				return_value = false;
			}
			else
			{
				//printf("mean : %lld\n",mean);
			}
		#if defined IDS_P2_ENABLED && defined CFDP_FLOOD_LIFT
		}
		sem_post(&(P1_struct_ptr->cfdp_sem));
		#endif
	}
	return return_value;
}

char is_CRC_correct(uint8_t* data_buf, int frame_size)
{
	uint16_t crc_computed;
	uint16_t crc_received;
	
	//compute CRC on buf minus the CRC field and then compare it
	crc_computed = Crypto_Calc_CRC16(data_buf, frame_size - 2);
	crc_received = (data_buf[frame_size-2]<<8) | data_buf[frame_size-1];
	
	//printf("crc computed : %02hx, received : %02hx\n", crc_computed, crc_received);

	if (crc_computed == crc_received)
		return true;
	else
	{
		//printf("crc computed : %02hx, received : %02hx\n", crc_computed, crc_received);
		//TODO - ADD MUTEX ?
		P1_data.bad_crc_count++;
		return false;
	}
}

char spi_is_initialized(uint16_t spi)
{
	if (spi > 63)
	{
		printf("Weird, spi > 63 in spi_is_initialized\n");
		return false;
	}

	if (P1_data.sa_state_table[spi]==0)
		return false;
	else
		return true;
}

char is_len_correct(int32 readBytes, uint16_t spi)
{
	int payload_len;
	int res;

	if (spi != 11)
	{
		//we do not know other SPI using block cipher for now so every other SPI is assumed to not be using blocks of fixed length, so not suspicious to have any length
		return true;
	}
	else
	{
		if (readBytes > 10)
		{
			//payload_len = readBytes - 10; //10 is the combined headers length (TC Primary Header, Segment Header?, and Security Header)
			payload_len = readBytes - (TC_FRAME_HEADER_SIZE + SEGMENT_HDR_SIZE + SPI_LEN + IV_SIZE_CBC + PAD_LEN_CBC /*IV_SIZE_TC + MAC_SIZE */ + FECF_SIZE);
			printf("payload_len : %d\n",payload_len);
			//return payload_len % 16 /*AES has a 128-bit block size = 16 bytes*/ == 0; //test if is a multiple of 16 and then cast the result to bool
			res = payload_len % 16;
			printf("len res : %d\n",res);
			return res == 0;
		}
		else
			return false;
	}
}

unsigned int get_bad_crc_count()
{
	return 0;//P1_data.bad_crc_count;
}

#endif

#if defined LOGGING_P1_ENABLED

void update_Log_P1(FILE * log_file,OS_time_t simtime, int32 bytes_read, uint8 *buffer, unsigned int seconds, struct timespec *realtime, int8_t ids_output, uint8_t attack_tag)
{	
	time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

	fprintf(log_file, "%lld,%d.%06ld,%d,%d,%d,", simtime.ticks, seconds, realtime->tv_nsec/1000, bytes_read, attack_tag, ids_output);
	for (int i = 0; i< bytes_read; i++) {
		fprintf(log_file,"%02x", buffer[i]);
	}
	fprintf(log_file,",%s", asctime(timeinfo));
	fflush(log_file);
	
	#if defined IDS_P1_DEBUG && defined IDS_P1_ENABLED
	printf("[P1] buffer :\n");
	print_buffer1();
	#endif
		
	#if defined IDS_P1_DEBUG && defined IDS_P1_ENABLED
	printf("[P1] PSP ticks : %lld\n", simtime.ticks);
	//print_buffer1();
	int64 mean = compute_time_mean1();
	printf("[P1] mean 1: %lld\n", mean);
	printf("\n");
	#endif
}

#endif