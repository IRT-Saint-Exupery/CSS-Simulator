#include "ids_p0.h"

void update_Log_P0(FILE * log_file,OS_time_t simtime, int32 bytes_read, uint8 *buffer, unsigned int seconds, struct timespec *realtime, int8_t ids_output, uint8_t attack_tag)
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
}

