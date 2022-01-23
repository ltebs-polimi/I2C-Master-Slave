/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <stdio.h>
#include "project.h"

#define I2C_BUFFER_SIZE    5
#define PSOC_SLAVE_ADDRESS 0x08
#define MASTER_RD -1
#define MASTER_WR +1

volatile int8_t state = 0x00;

char message[50]; // UART buffer

CY_ISR_PROTO(blink_led);
void printBuffer(uint8_t* buf, uint8_t len);

// SLAVE

int main(void) {
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();
    Timer_1_Start();
    isr_1_StartEx(blink_led);
    Pin_1_Write(0);
    
    uint8_t rData[I2C_BUFFER_SIZE] = {1,2,3,4,5};
    uint8_t wData[I2C_BUFFER_SIZE];
    uint8_t uData[I2C_BUFFER_SIZE];
    I2C_Slave_SlaveInitReadBuf(rData, I2C_BUFFER_SIZE);
    I2C_Slave_SlaveInitWriteBuf(wData, I2C_BUFFER_SIZE);
    
    I2C_Slave_Start();
    
    CyDelay(100);
    
    uint8_t count = 0;
    int8_t action = 0; // +1 for master write, -1 for master read
    
    UART_1_PutString("*** I2C Slave init complete.\r\n");
    sprintf(message, "*** Read buffer pointer = 0x%02x \r\n", *I2C_Slave_slRdBufPtr);
    UART_1_PutString(message);
    
    printBuffer(rData, 5);
    
    while(1) {
        
        if ( state == MASTER_RD ) {
            count = I2C_Slave_SlaveGetReadBufSize();
            sprintf(message, "Master read: %d bytes (%02x) \r\n\r\n", count, I2C_Slave_SlaveStatus());
            UART_1_PutString(message);
            state = 0;
            I2C_Slave_SlaveClearReadBuf();
        }
        if ( state == MASTER_WR ) {
            /* Copy data */
            count = I2C_Slave_SlaveGetWriteBufSize();
            memcpy(uData, wData, count);
            printBuffer(wData, 5);
            memcpy(rData, uData, count);
            printBuffer(rData, 5);
            sprintf(message, "Master write: %d bytes (%02x) \r\n\r\n", count, I2C_Slave_SlaveStatus());
            UART_1_PutString(message);
            state = 0;
            I2C_Slave_SlaveClearWriteBuf();
        }
        
    }
   
    
    return 0;
    
}

void I2C_Slave_ISR_ExitCallback() {
 
    uint8_t stat = I2C_Slave_SlaveStatus();
    sprintf(message, "- I2C status = 0x%02x \r\n", stat);
    UART_1_PutString(message);
    
    /* Master Read Complete */
    if ( stat & I2C_Slave_SSTAT_RD_CMPLT ) {
        I2C_Slave_SlaveClearReadStatus();
        state = MASTER_RD;
    }
    
    /* Master Write Complete */
    if ( stat & I2C_Slave_SSTAT_WR_CMPLT ) {
        I2C_Slave_SlaveClearWriteStatus();
        state = MASTER_WR;
    }
        
    
}

void printBuffer(uint8_t* buf, uint8_t len) {
 
    for ( uint8_t i = 0; i < len; i++ ) {
        sprintf(message, "0x%02x ", buf[i]);
        UART_1_PutString(message);
    }
    UART_1_PutString("\r\n");
    
}

CY_ISR(blink_led) {
 
    Timer_1_ReadStatusRegister();
    Pin_1_Write(!Pin_1_Read());
    
}

/* [] END OF FILE */
