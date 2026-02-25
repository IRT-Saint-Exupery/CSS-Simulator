/*******************************************************************************
** File: novatel_oem615_child.h
**
** Purpose:
**   This is the header file for the NOVATEL_OEM615 child task.
**
*******************************************************************************/
#ifndef _NOVATEL_OEM615_CHILD_H_
#define _NOVATEL_OEM615_CHILD_H_

#include "novatel_oem615_app.h"

#define NOVATEL_OEM615_READ_TIMEOUT            250 // default 1 ? // IRT CSS
#define NOVATEL_OEM615_UART_BUFFER_SIZE        1024

void NOVATEL_OEM615_ChildTask(void);
void NOVATEL_OEM615_ProcessData(void);

#endif
