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

CY_ISR_PROTO(blink_led);

int main(void) {
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2C_Slave_Start();
    UART_1_Start();
    Timer_1_Start();
    isr_1_StartEx(blink_led);
    Pin_1_Write(0);
    
    uint8_t rData[I2C_BUFFER_SIZE] = {1,2,3,4,5};
    uint8_t wData[I2C_BUFFER_SIZE];
    uint8_t uData[2*I2C_BUFFER_SIZE];
    I2C_Slave_SlaveInitReadBuf(rData, I2C_BUFFER_SIZE);
    I2C_Slave_SlaveInitWriteBuf(wData, I2C_BUFFER_SIZE);
    
    char message[50]; // UART buffer
    
    CyDelay(100);
    
    uint8_t count = 0;
    int8_t action = 0; // +1 for master write, -1 for master read
    
    UART_1_PutString("I2C Slave init complete.\r\n");
    sprintf(message, "Read buffer pointer = 0x%02x \r\n", *I2C_Slave_slRdBufPtr);
    UART_1_PutString(message);
    
    while(1) {
        
        /* Master Write */
        if ( 0u != (I2C_Slave_SlaveStatus() & I2C_Slave_SSTAT_WR_CMPLT) ) {
            count = I2C_Slave_SlaveGetWriteBufSize(); 
            I2C_Slave_SlaveClearWriteStatus();
            for ( uint8_t i = 0; i < count; i++ ) {
                uData[i] = wData[i];
            }
            I2C_Slave_SlaveClearWriteBuf();
            action = MASTER_WR;
        }
        
        /* Master Read */
        if ( 0u != (I2C_Slave_SlaveStatus() & I2C_Slave_SSTAT_RD_CMPLT) ) {
            count = I2C_Slave_SlaveGetReadBufSize();
            I2C_Slave_SlaveClearReadStatus();
            I2C_Slave_SlaveClearReadBuf();
            action = MASTER_RD;
        }
        
        /* Handle master write operation */
        if ( action == MASTER_WR ) {
            UART_1_PutString("Master write complete.\r\n");
            action = 0;       
            memcpy(rData, uData, count);     // copy user data to rData buffer
            memset(uData, 0, sizeof(uData)); // reset user data
        }
        /* Handle master read operation */
        if ( action == MASTER_RD ) {
            UART_1_PutString("Master read complete.\r\n");
            action = 0;
        }
        
    }
   
    
    return 0;
    
}

CY_ISR(blink_led) {
 
    Timer_1_ReadStatusRegister();
    Pin_1_Write(!Pin_1_Read());
    
}

/* [] END OF FILE */
