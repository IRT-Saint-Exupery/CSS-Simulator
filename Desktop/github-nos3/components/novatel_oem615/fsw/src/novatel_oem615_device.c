/*******************************************************************************
** File: novatel_oem615_device.c
**
** Purpose:
**   This file contains the source code for the NOVATEL_OEM615 device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "novatel_oem615_device.h"

static char* saveptr;

/* 
** Command to device
** Note that confirming the echoed response is specific to this implementation
*/
int32_t NOVATEL_OEM615_CommandDevice(uart_info_t* uart_device, uint8_t cmd_code, uint32_t payload)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    uint8_t write_data[NOVATEL_OEM615_DEVICE_CMD_SIZE] = {0};
    uint8_t read_data[NOVATEL_OEM615_DEVICE_DATA_SIZE] = {0};

    payload = CFE_MAKE_BIG32(payload);

    // Prepare command
    write_data[0] = NOVATEL_OEM615_DEVICE_HDR_0;
    write_data[1] = NOVATEL_OEM615_DEVICE_HDR_1;
    write_data[2] = cmd_code;
    write_data[3] = payload >> 24;
    write_data[4] = payload >> 16;
    write_data[5] = payload >> 8;
    write_data[6] = payload;
    write_data[7] = NOVATEL_OEM615_DEVICE_TRAILER_0;
    write_data[8] = NOVATEL_OEM615_DEVICE_TRAILER_1;

    // Flush any prior data 
    status = uart_flush(uart_device);
    if (status == UART_SUCCESS)
    {
        // Write data 
        bytes = uart_write_port(uart_device, write_data, NOVATEL_OEM615_DEVICE_CMD_SIZE);
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_CommandDevice[%d] = ", bytes);
            for (uint32_t i = 0; i < NOVATEL_OEM615_DEVICE_CMD_SIZE; i++)
            {
                OS_printf("%02x", write_data[i]);
            }
            OS_printf("\n");
        #endif
        if (bytes != NOVATEL_OEM615_DEVICE_CMD_SIZE)
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("NOVATEL_OEM615_CommandDevice - uart_write_port returned %d, expected %d \n", bytes, NOVATEL_OEM615_DEVICE_CMD_SIZE);
            #endif
            status = OS_ERROR;
        } // uart_write 
        else
        {
            int32_t bytes_available = uart_bytes_available(uart_device);
            if (bytes_available > 0)
            {
                
                bytes = uart_read_port(uart_device, read_data, NOVATEL_OEM615_DEVICE_CMD_SIZE);
                
                if (bytes != NOVATEL_OEM615_DEVICE_CMD_SIZE)
                {
                    #ifdef NOVATEL_OEM615_CFG_DEBUG
                        OS_printf("NOVATEL_OEM615_CommandDevice - uart_read_port returned %d, expected %d \n", bytes, NOVATEL_OEM615_DEVICE_CMD_SIZE);
                    #endif
                    status = OS_ERROR;
                }
                else
                {
                    if (status == OS_SUCCESS)
                    {
                        // Confirm echoed response 
                        bytes = 0;
                        while ((bytes < (int32_t) NOVATEL_OEM615_DEVICE_CMD_SIZE) && (status == OS_SUCCESS))
                        {
                            if (read_data[bytes] != write_data[bytes])
                            {
                                status = OS_ERROR;
                            }
                            bytes++;
                        }
                    }
                }
            }
            else
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("NOVATEL_OEM615_CommandDevice - uart_bytes_available returned no bytes available!");
                #endif
                status = OS_ERROR;
            }
        }
    } // uart_flush
    else
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
           OS_printf("NOVATEL_OEM615_CommandDevice - uart_flush returned error with status = %d! No uart_write_port performed.", status);
        #endif
        status = OS_ERROR;
    }
    return status;
}




int32_t NOVATEL_OEM615_CommandDeviceCustom(uart_info_t* uart_device, uint8_t cmd_code, int8_t log_type, int8_t period_option)
{
    int32_t status = OS_SUCCESS;
    int32_t command_length = 0;
    //uint8_t* write_data;
    char* log_type_str;
    char* period_option_str;
    uint8_t write_data[4096] = {0}; // IRT CSS 

    // log
    if (cmd_code == 4)
    {
        command_length = sizeof(NOVATEL_OEM615_LOG) + sizeof(NOVATEL_OEM615_PORT);
        switch (log_type)
        {
            case 0: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_BESTXYZA);
                log_type_str = NOVATEL_OEM615_LOGTYPE_BESTXYZA;
                break;
            case 1: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_GPGGAA);
                log_type_str = NOVATEL_OEM615_LOGTYPE_GPGGAA;
                break;
            case 2: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_RANGECMPA);
                log_type_str = NOVATEL_OEM615_LOGTYPE_RANGECMPA;
                break;
            case 3: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_BESTXYZB);
                log_type_str = NOVATEL_OEM615_LOGTYPE_BESTXYZB;
                break;
            case 4: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_RANGECMPB);
                log_type_str = NOVATEL_OEM615_LOGTYPE_RANGECMPB;
                break;
            default:
                status = OS_ERROR;
                break;
        }
        switch (period_option)
        {
            case 0: command_length += sizeof(NOVATEL_OEM615_PERIOD_OPTION_0);
                period_option_str = NOVATEL_OEM615_PERIOD_OPTION_0;
                break;
            case 1: command_length += sizeof(NOVATEL_OEM615_PERIOD_OPTION_1);
                period_option_str = NOVATEL_OEM615_PERIOD_OPTION_1;
                break;
            case 2: command_length += sizeof(NOVATEL_OEM615_PERIOD_OPTION_2);
                period_option_str = NOVATEL_OEM615_PERIOD_OPTION_2;
                break;
            case 3: command_length += sizeof(NOVATEL_OEM615_PERIOD_OPTION_3);
                period_option_str = NOVATEL_OEM615_PERIOD_OPTION_3;
                break;
            case 4: command_length += sizeof(NOVATEL_OEM615_PERIOD_OPTION_4);
                period_option_str = NOVATEL_OEM615_PERIOD_OPTION_4;
                break;
            case 5: command_length += sizeof(NOVATEL_OEM615_PERIOD_OPTION_5);
                period_option_str = NOVATEL_OEM615_PERIOD_OPTION_5;
                break;
            default:
                status = OS_ERROR;
                break;
        }
        if (status == OS_SUCCESS)
        {
            //write_data = (uint8_t*)calloc(command_length, sizeof(uint8_t));
            strcpy((char*)write_data, NOVATEL_OEM615_LOG);
            strcat((char*)write_data, NOVATEL_OEM615_PORT);
            strcat((char*)write_data, log_type_str);
            strcat((char*)write_data, period_option_str);
        }
    }
    // unlog
    else if (cmd_code == 5)
    {
        command_length = sizeof(NOVATEL_OEM615_UNLOG) + sizeof(NOVATEL_OEM615_PORT);
        switch (log_type)
        {
            case 0: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_BESTXYZA);
                log_type_str = NOVATEL_OEM615_LOGTYPE_BESTXYZA;
                break;
            case 1: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_GPGGAA);
                log_type_str = NOVATEL_OEM615_LOGTYPE_GPGGAA;
                break;
            case 2: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_RANGECMPA);
                log_type_str = NOVATEL_OEM615_LOGTYPE_RANGECMPA;
                break;
            case 3: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_BESTXYZB);
                log_type_str = NOVATEL_OEM615_LOGTYPE_BESTXYZB;
                break;
            case 4: command_length += sizeof(NOVATEL_OEM615_LOGTYPE_RANGECMPB);
                log_type_str = NOVATEL_OEM615_LOGTYPE_RANGECMPB;
                break;
            default:
                status = OS_ERROR;
                break;
        }
        if (status == OS_SUCCESS)
        {
            //write_data = (uint8_t*)calloc(command_length, sizeof(uint8_t));
            strcpy((char*)write_data, NOVATEL_OEM615_UNLOG);
            strcat((char*)write_data, NOVATEL_OEM615_PORT);
            strcat((char*)write_data, log_type_str);
        }
    }
    // unlogall
    else if (cmd_code == 6)
    {
        command_length = sizeof(NOVATEL_OEM615_UNLOGALL);
        //write_data = (uint8_t*)calloc(command_length, sizeof(uint8_t));
        strcpy((char*)write_data, NOVATEL_OEM615_UNLOGALL);
    }
    // serialconfig
    else if (cmd_code == 7)
    {
        command_length = sizeof(NOVATEL_OEM615_SERIALCONFIG);
        //write_data = (uint8_t*)calloc(command_length, sizeof(uint8_t));
        strcpy((char*)write_data, NOVATEL_OEM615_SERIALCONFIG);
    }

    // Flush any prior data 
    if (status == OS_SUCCESS)
    {
        status = uart_flush(uart_device);
        if (status == OS_SUCCESS)
        {
            // Write data 
            int32_t bytes = uart_write_port(uart_device, write_data, command_length);
            //free(write_data);
        } // uart_flush
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("NOVATEL_OEM615_CommandDeviceCustom - uart_flush returned error with status = %d! No uart_write_port performed.", status);
            #endif
            status = OS_ERROR;
        }
    }
    return status;
}

/*
** Request housekeeping command
*/
int32_t NOVATEL_OEM615_RequestHK(uart_info_t* uart_device, NOVATEL_OEM615_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[NOVATEL_OEM615_DEVICE_HK_SIZE] = {0};

    /* Command device to send HK */
    status = NOVATEL_OEM615_CommandDevice(uart_device, NOVATEL_OEM615_DEVICE_REQ_HK_CMD, 0);
    if (status == OS_SUCCESS)
    {
        /* Read HK data */
        status = NOVATEL_OEM615_ReadHK(uart_device, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_RequestHK = ");
                for (uint32_t i = 0; i < sizeof(read_data); i++)
                {
                    OS_printf("%02x", read_data[i]);
                }
                OS_printf("\n");
            #endif

            /* Verify data header and trailer */
            if ((read_data[0]  == NOVATEL_OEM615_DEVICE_HDR_0)     && 
                (read_data[1]  == NOVATEL_OEM615_DEVICE_HDR_1)     && 
                (read_data[14] == NOVATEL_OEM615_DEVICE_TRAILER_0) && 
                (read_data[15] == NOVATEL_OEM615_DEVICE_TRAILER_1) )
            {
                data->DeviceCounter  = read_data[2] << 24;
                data->DeviceCounter |= read_data[3] << 16;
                data->DeviceCounter |= read_data[4] << 8;
                data->DeviceCounter |= read_data[5];

                data->DeviceConfig  = read_data[6] << 24;
                data->DeviceConfig |= read_data[7] << 16;
                data->DeviceConfig |= read_data[8] << 8;
                data->DeviceConfig |= read_data[9];

                data->DeviceStatus  = read_data[10] << 24;
                data->DeviceStatus |= read_data[11] << 16;
                data->DeviceStatus |= read_data[12] << 8;
                data->DeviceStatus |= read_data[13];

                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
                    OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                    OS_printf("  Config  = 0x%08x      \n", data->DeviceConfig);
                    OS_printf("  Status  = 0x%08x      \n", data->DeviceStatus);
                    OS_printf("  Trailer = 0x%02x%02x  \n", read_data[14], read_data[15]);
                #endif
            }
            else
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  NOVATEL_OEM615_RequestHK: NOVATEL_OEM615_ReadHK returned data with either invalid header [0x%02x%02x] or invalid trailer [0x%02x%02x]!\n", read_data[0], read_data[1], read_data[14], read_data[15]);
                #endif 
                status = OS_ERROR;
            }
        }
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_RequestHK: Invalid data read from NOVATEL_OEM615_ReadHK!\n");
            #endif 
            status = OS_ERROR;
        } /* NOVATEL_OEM615_ReadHK */
    
    }
    else
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_RequestHK: NOVATEL_OEM615_CommandDevice reported error %d \n", status);
        #endif 
    }
    return status;
}

/* 
** Read HK from device
*/
int32_t NOVATEL_OEM615_ReadHK(uart_info_t* uart_device, uint8_t* read_data, uint8_t data_length)
{
    int32_t status = OS_ERROR;
    int32_t bytes = 0;
    int32_t bytes_available = 0;
    uint8_t temp_read_data[4096] = {0};

    if (data_length < 4)
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_ReadHK: Structure data length provided below expected HK header + trailer size! \n");
        #endif
    }
    else
    {
        /* check how many bytes are waiting on the uart */
        bytes_available = uart_bytes_available(uart_device);
        if (bytes_available > 0)
        {
            //uint8_t* temp_read_data = (uint8_t*)calloc(bytes_available, sizeof(uint8_t));
            /* Read all existing data on uart port */
            bytes = uart_read_port(uart_device, temp_read_data, bytes_available);
            if (bytes != bytes_available)
            {
                //free(temp_read_data);
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  NOVATEL_OEM615_ReadHK: Bytes read != to requested! \n");
                #endif
            }
            else
            {
                /* search uart data for header+trailer signifying start of HK gps packet */
                int j = 0;
                for (int i=0;i<(bytes-data_length);i++)
                {
                    if ((temp_read_data[i]  == NOVATEL_OEM615_DEVICE_HDR_0)     && 
                    (temp_read_data[i+1]  == NOVATEL_OEM615_DEVICE_HDR_1)     && 
                    (temp_read_data[i+data_length-2] == NOVATEL_OEM615_DEVICE_TRAILER_0) && 
                    (temp_read_data[i+data_length-1] == NOVATEL_OEM615_DEVICE_TRAILER_1) )
                    {
                        //OS_printf(" HK HEADER + TRAILER FOUND \n"); //IRT CSS
                        for (int j=0;j<data_length;j++)
                        {
                            //read_data[j] = read_data[i+j];
                            read_data[j] = temp_read_data[i+j]; // ?? IRT CSS

                        }
                        status = OS_SUCCESS;
                        //free(temp_read_data); //IRT CSS
                        return status;
                    }
                }
                if (status != OS_SUCCESS)
                {
                    //free(temp_read_data);
                    #ifdef NOVATEL_OEM615_CFG_DEBUG
                        OS_printf("  NOVATEL_OEM615_ReadHK: No DEAD header + BEEF trailer found when reading uart port! \n");
                    #endif
                }
            }
        }
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_ReadHK: No data available when attempting to read from uart port! \n");
            #endif
        }
    }
    return status;
}

/*
** Request data command
*/
int32_t NOVATEL_OEM615_RequestData(uart_info_t* uart_device, NOVATEL_OEM615_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    int32_t bytes_available = 0;
    char *token;
    uint8_t temp_read_data[4096]; // = {0}; //IRT CSS

    status = NOVATEL_OEM615_CommandDevice(uart_device, NOVATEL_OEM615_DEVICE_REQ_DATA_CMD, 0);
    if (status == OS_SUCCESS)
    {
        /* check how many bytes are waiting on the uart */
        bytes_available = uart_bytes_available(uart_device);
        if (bytes_available > 0)
        {
            //uint8_t* temp_read_data = (uint8_t*)calloc(bytes_available, sizeof(uint8_t));
            /* Read all existing data on uart port */
            bytes = uart_read_port(uart_device, temp_read_data, bytes_available);
            if (bytes != bytes_available)
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  NOVATEL_OEM615_RequestData: Bytes read != to requested! \n");
                #endif
                status = OS_ERROR;
                //free(temp_read_data);
            }
            else
            {
                /* search uart data for token signifying start of bestxyza gps data packet */
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf(" ALL UART BYTES READ FROM BUFFER: \n");
                    for (int i=0;i<bytes;i++)
                    {
                        OS_printf("%02x", temp_read_data[i]);
                    }
                    OS_printf(" DONE PRINTING ALL UART BYTES READ FROM BUFFER \n");
                #endif
                token = strtok_r((char*)temp_read_data, ",", &saveptr);
                if ((token != NULL) && (strncmp(token, "#BESTXYZA", 9) == 0)) 
                {
                    #ifdef NOVATEL_OEM615_CFG_DEBUG
                        OS_printf(" DATA TOKEN FOUND = %s\n", token);
                    #endif
                    
                    NOVATEL_OEM615_ParseBestXYZA(data);
                    
                }
                else
                {
                    #ifdef NOVATEL_OEM615_CFG_DEBUG
                        OS_printf("  NOVATEL_OEM615_RequestData: No #BESTXYZA token found when reading uart port! \n");
                    #endif
                    status = OS_ERROR;
                    
                }
                //free(temp_read_data); //IRT CSS
            }
         // Flush any prior data 
        //uart_flush(uart_device); //IRT CSS
        }
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_RequestData: No data available when attempting to read from uart port! \n");
            #endif
            status = OS_ERROR;
        }
    }
    else
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_RequestData: CommandDevice returned error with status = %d! \n", status);
        #endif
        status = OS_ERROR;
    }
    return status;
}

/*
** Request data command for child task
*/
int32_t NOVATEL_OEM615_ChildProcessReadData(uart_info_t* uart_device, NOVATEL_OEM615_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    int32_t bytes_available = 0;
    char *token;
    uint8_t temp_read_data[4096] = {0}; //IRT CSS
    

    /* check how many bytes are waiting on the uart */
    bytes_available = uart_bytes_available(uart_device);
    if (bytes_available > 0)
    {
    
    if ((unsigned int) bytes_available > sizeof(temp_read_data)) { // IRT CSS
    	printf("GPS: ERROR too many bytes in ChildProcessData = %d \n", bytes_available);
    	status = OS_ERROR;
    	return status;
    }
        //uint8_t* temp_read_data = (uint8_t*)calloc(bytes_available, sizeof(uint8_t));
        /* Read all existing data on uart port */
        bytes = uart_read_port(uart_device, temp_read_data, bytes_available);
        
        if (bytes != bytes_available)
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_ChildProcessReadData: Bytes read != to requested! \n");
            #endif
            status = OS_ERROR;
            //free(temp_read_data);
        }
        else
        {
            /* search uart data for token signifying start of bestxyza gps data packet */
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf(" ALL UART BYTES READ FROM BUFFER: \n");
                for (int i=0;i<bytes;i++)
                {
                    OS_printf("%02x", temp_read_data[i]);
                }
                OS_printf(" DONE PRINTING ALL UART BYTES READ FROM BUFFER \n");
            #endif
            
            token = strtok_r((char*)temp_read_data, ",", &saveptr);
            
            if ((token != NULL) && (strncmp(token, "#BESTXYZA", 9) == 0)) 
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf(" DATA TOKEN FOUND = %s\n", token);
                #endif
                
                NOVATEL_OEM615_ParseBestXYZA(data);
                
            }
            else
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  NOVATEL_OEM615_ChildProcessReadData: No #BESTXYZA token found when reading uart port! \n");
                #endif
                status = OS_ERROR;
                
            }
            //free(temp_read_data); //IRT CSS
        }
    // Flush any prior data 
    uart_flush(uart_device); //IRT CSS
    }
    else
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_ChildProcessReadData: No data available when attempting to read from uart port! \n");
        #endif
        status = OS_ERROR;
    }
    return status;
}

/************************************************************************
** Parse NovAtel OEM615 BESTXYZA log data
*************************************************************************/
// Reference:  Section 1.1.1, p. 24, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
// Reference:  Section 3.2.17, pp. 420-422, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
void NOVATEL_OEM615_ParseBestXYZA(NOVATEL_OEM615_Device_Data_tlm_t* device_data_struct)
{
    device_data_struct->Weeks = 0;
    device_data_struct->SecondsIntoWeek = 0;
    device_data_struct->Fractions = 0.0;
    device_data_struct->ECEFX = 0.0;
    device_data_struct->ECEFY = 0.0;
    device_data_struct->ECEFZ = 0.0;
    device_data_struct->VelX = 0.0;
    device_data_struct->VelY = 0.0;
    device_data_struct->VelZ = 0.0;
    char *token;

    token = strtok_r(NULL, ",; ", &saveptr); // Port
    token = strtok_r(NULL, ",; ", &saveptr); // Sequence #
    token = strtok_r(NULL, ",; ", &saveptr); // % Idle Time
    token = strtok_r(NULL, ",; ", &saveptr); // Time Status
    token = strtok_r(NULL, ",; ", &saveptr); // Week
    if (token != NULL) device_data_struct->Weeks = atol(token);
    token = strtok_r(NULL, ",; ", &saveptr); // Seconds
    if (token != NULL) {
        device_data_struct->Fractions = atof(token);
        device_data_struct->SecondsIntoWeek = (uint32_t)device_data_struct->Fractions;
        device_data_struct->Fractions -= device_data_struct->SecondsIntoWeek;
    }
    token = strtok_r(NULL, ",; ", &saveptr); // Receiver Status
    token = strtok_r(NULL, ",; ", &saveptr); // Reserved
    token = strtok_r(NULL, ",; ", &saveptr); // Receiver s/w Version

    // Now the data
    token = strtok_r(NULL, ",; ", &saveptr); // P-sol status
    token = strtok_r(NULL, ",; ", &saveptr); // pos type
    token = strtok_r(NULL, ",; ", &saveptr); // P-X (m)
    if (token != NULL) device_data_struct->ECEFX = atof(token);
    token = strtok_r(NULL, ",; ", &saveptr); // P-Y (m)
    if (token != NULL) device_data_struct->ECEFY = atof(token);
    token = strtok_r(NULL, ",; ", &saveptr); // P-Z (m)
    if (token != NULL) device_data_struct->ECEFZ = atof(token);
    token = strtok_r(NULL, ",; ", &saveptr); // P-X sigma
    token = strtok_r(NULL, ",; ", &saveptr); // P-Y sigma
    token = strtok_r(NULL, ",; ", &saveptr); // P-Z sigma
    token = strtok_r(NULL, ",; ", &saveptr); // V-sol status
    token = strtok_r(NULL, ",; ", &saveptr); // vel type
    token = strtok_r(NULL, ",; ", &saveptr); // V-X (m/s
    if (token != NULL) device_data_struct->VelX = atof(token);
    token = strtok_r(NULL, ",; ", &saveptr); // V-Y (m/s)
    if (token != NULL) device_data_struct->VelY = atof(token);
    token = strtok_r(NULL, ",; ", &saveptr); // V-Z (m/s)
    if (token != NULL) device_data_struct->VelZ = atof(token);

    #ifdef NOVATEL_OEM615_CFG_DEBUG
        OS_printf("  Weeks = %d\n", device_data_struct->Weeks);
        OS_printf("  SecondsIntoWeek = %d\n", device_data_struct->SecondsIntoWeek);
        OS_printf("  Fractions = %f\n", device_data_struct->Fractions);
        OS_printf("  ECEFX = %f\n", device_data_struct->ECEFX);
        OS_printf("  ECEFY = %f\n", device_data_struct->ECEFY);
        OS_printf("  ECEFZ = %f\n", device_data_struct->ECEFZ);
        OS_printf("  VelX = %f\n", device_data_struct->VelX);
        OS_printf("  VelY = %f\n", device_data_struct->VelY);
        OS_printf("  VelZ = %f\n", device_data_struct->VelZ);
    #endif

} /* NAV_ParseOEM615Bestxyza */
