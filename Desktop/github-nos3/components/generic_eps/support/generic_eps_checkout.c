/*******************************************************************************
** File: generic_eps_checkout.c
**
** Purpose:
**   This checkout can be run without cFS and is used to quickly develop and 
**   test functions required for a specific component.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_eps_checkout.h"


/*
** Global Variables
*/
i2c_bus_info_t Generic_epsI2C;
GENERIC_EPS_Device_HK_tlm_t Generic_epsHK;

/*
** Component Functions
*/
void print_help(void) 
{
    printf(PROMPT "command [args]\n"
        "---------------------------------------------------------------------\n"
        "help                               - Display help                    \n"
        "exit                               - Exit app                        \n"
        "hk                                 - Request device housekeeping     \n"
        "  h                                - ^                               \n"
        "switch # #                         - Switch [0-7] [0x00 off, 0xAA on]\n"
        "  s # #                            - ^                               \n"
        "\n"
    );
}


int get_command(const char* str)
{
    int status = CMD_UNKNOWN;
    char lcmd[MAX_INPUT_TOKEN_SIZE];
    strncpy(lcmd, str, MAX_INPUT_TOKEN_SIZE);

    /* Convert command to lower case */
    to_lower(lcmd);

    if(strcmp(lcmd, "help") == 0) 
    {
        status = CMD_HELP;
    }
    else if(strcmp(lcmd, "exit") == 0) 
    {
        status = CMD_EXIT;
    }
    else if(strcmp(lcmd, "hk") == 0) 
    {
        status = CMD_HK;
    }
    else if(strcmp(lcmd, "h") == 0) 
    {
        status = CMD_HK;
    }
    else if(strcmp(lcmd, "switch") == 0) 
    {
        status = CMD_SWITCH;
    }
    else if(strcmp(lcmd, "s") == 0) 
    {
        status = CMD_SWITCH;
    }
    return status;
}


int process_command(int cc, int num_tokens, char* tokens)
{
    int32_t status = OS_SUCCESS;
    int32_t exit_status = OS_SUCCESS;
    uint8_t switch_num = 0;
    uint8_t value = 0;

    /* Process command */
    switch(cc) 
    {	
        case CMD_HELP:
            print_help();
            break;
        
        case CMD_EXIT:
            exit_status = OS_ERROR;
            break;

        case CMD_HK:
            if (check_number_arguments(num_tokens, 1) == OS_SUCCESS)
            {
                status = GENERIC_EPS_RequestHK(Generic_epsI2C.handle, &Generic_epsHK);
                if (status == OS_SUCCESS)
                {
                    OS_printf("GENERIC_EPS_RequestHK command success\n");
                }
                else
                {
                    OS_printf("GENERIC_EPS_RequestHK command failed!\n");
                }
            }
            break;

        case CMD_SWITCH:
            if (check_number_arguments(num_tokens, 3) == OS_SUCCESS)
            {
                switch_num = atoi(&tokens[0]);
                value = atoi(&tokens[1]);
                /* Check switch number valid */
                if (switch_num < 8)
                {
                    /* Check value valid */
                    if ((value == 0x00) || (value == 0xAA))
                    {
                        status = GENERIC_EPS_CommandSwitch(Generic_epsI2C.handle, switch_num, value, &Generic_epsHK);
                        if (status == OS_SUCCESS)
                        {
                            OS_printf("GENERIC_EPS_CommandSwitch command success\n");
                        }
                        else
                        {
                            OS_printf("GENERIC_EPS_CommandSwitch command failed!\n");
                        }
                    }
                    else
                    {
                        OS_printf("GENERIC_EPS_CommandSwitch value invalid!\n");
                    }
                }
                else
                {
                    OS_printf("GENERIC_EPS_CommandSwitch switch number invalid!\n");
                }


                
            }
            break;
        
        default: 
            OS_printf("Invalid command format, type 'help' for more info\n");
            break;
    }
    return exit_status;
}


int main(int argc, char *argv[]) 
{
    int status = OS_SUCCESS;
    char input_buf[MAX_INPUT_BUF];
    char input_tokens[MAX_INPUT_TOKENS][MAX_INPUT_TOKEN_SIZE];
    int num_input_tokens;
    int cmd;    
    char* token_ptr;
    uint8_t run_status = OS_SUCCESS;

    /* Open device specific protocols */
    Generic_epsI2C.handle = GENERIC_EPS_CFG_I2C_HANDLE;
    Generic_epsI2C.addr = GENERIC_EPS_CFG_I2C_ADDRESS;
    Generic_epsI2C.isOpen = I2C_CLOSED;
    Generic_epsI2C.speed = GENERIC_EPS_CFG_I2C_SPEED;
    status = i2c_master_init(&Generic_epsI2C);
    if (status == OS_SUCCESS)
    {
        printf("I2C device 0x%02x configured with speed %d \n", Generic_epsI2C.addr, Generic_epsI2C.speed);
    }
    else
    {
        printf("I2C device 0x%02x failed to initialize! \n", Generic_epsI2C.addr);
        run_status = OS_ERROR;
    }

    /* Main loop */
    print_help();
    while(run_status == OS_SUCCESS) 
    {
        num_input_tokens = -1;
        cmd = CMD_UNKNOWN;

        /* Read user input */
        printf(PROMPT);
        fgets(input_buf, MAX_INPUT_BUF, stdin);

        /* Tokenize line buffer */
        token_ptr = strtok(input_buf, " \t\n");
        while((num_input_tokens < MAX_INPUT_TOKENS) && (token_ptr != NULL)) 
        {
            if(num_input_tokens == -1) 
            {
                /* First token is command */
                cmd = get_command(token_ptr);
            }
            else 
            {
                strncpy(input_tokens[num_input_tokens], token_ptr, MAX_INPUT_TOKEN_SIZE);
            }
            token_ptr = strtok(NULL, " \t\n");
            num_input_tokens++;
        }

        /* Process command if valid */
        if(num_input_tokens >= 0)
        {
            /* Process command */
            run_status = process_command(cmd, num_input_tokens, token_ptr);
        }
    }

    OS_printf("Cleanly exiting generic_eps application...\n\n"); 
    return 1;
}


/*
** Generic Functions
*/
int check_number_arguments(int actual, int expected)
{
    int status = OS_SUCCESS;
    if (actual != expected)
    {
        status = OS_ERROR;
        OS_printf("Invalid command format, type 'help' for more info\n");
    }
    return status;
}

void to_lower(char* str)
{
    char* ptr = str;
    while(*ptr)
    {
        *ptr = tolower((unsigned char) *ptr);
        ptr++;
    }
    return;
}

