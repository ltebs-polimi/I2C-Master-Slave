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

// UART buffer
char message[50];
volatile uint8_t state = 0x00;

int main(void) {
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_1_Start();
    I2C_Master_Start();
    Pin_1_Write(1);
    
    UART_1_PutString("\r\n***** PSoC-PSoC I2C Comm *****\r\n");
    
    // Check if LIS3DH is connected
    uint8_t rval = I2C_Master_MasterSendStart(PSOC_SLAVE_ADDRESS, I2C_Master_WRITE_XFER_MODE);
    if ( rval == I2C_Master_MSTR_NO_ERROR ) {
        UART_1_PutString("PSoC slave found @ address 0x08\r\n");
    }
    I2C_Master_MasterSendStop();
    
    CyDelay(100);
    

    // I2C error return code
    uint8_t error;
    
    /* Functions I2C_Master_MasterReadBuf and I2C_Master_MasterWriteBuf 
       read/write the entire slave buffer, or the first N bytes specified in the function call */
    
    // One-time write
    I2C_Master_MasterClearStatus();
    uint8_t wData[I2C_BUFFER_SIZE] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E};
    error = I2C_Master_MasterWriteBuf(PSOC_SLAVE_ADDRESS, wData, I2C_BUFFER_SIZE, I2C_Master_MODE_COMPLETE_XFER);
    while ( I2C_Master_MasterStatus() & I2C_Master_MSTAT_XFER_INP ) ; // wait for transfer to complete
    if ( error == I2C_Master_MSTR_NO_ERROR ) {
        UART_1_PutString("Data written to I2C slave.\r\n");    
    }
    I2C_Master_MasterClearWriteBuf();
    I2C_Master_MasterClearStatus();
    
    CyDelay(100);
    
    uint8_t rData[5];
    
    state = 0x00;
    
    uint8_t readCount = 0, count = 0, nbyteRead = 1;
    
    // Period read (use a timer ISR instead of a CyDelay)
    while(1) {
        
        if ( state == 0x00 ) {
            I2C_Master_MasterClearStatus();
            error = I2C_Master_MasterReadBuf(PSOC_SLAVE_ADDRESS, rData, nbyteRead, I2C_Master_MODE_COMPLETE_XFER);
        
            if ( error != I2C_Master_MSTR_NO_ERROR ) {
                UART_1_PutString("I2C error\r\n");
            } 
            else {
                state = 0x01; // xfer in progress
            }
        }
        
        
        if ( state != 0x00 ) {
            
            /* Read complete */
            if ( state == 0x02 ) {
                count = I2C_Master_MasterGetReadBufSize();
                sprintf(message, "(%d) Data from I2C slave: ", count);
                UART_1_PutString(message);
                for ( uint8_t i = 0; i < count; i++ ) {
                    sprintf(message, "0x%02x ", rData[i]);
                    UART_1_PutString(message);
                }
                UART_1_PutString("\r\n\r\n");
                I2C_Master_MasterClearReadBuf();  
                state = 0x00;
                CyDelay(1500);
                readCount++;
                nbyteRead = ++nbyteRead > 5 ? 1 : nbyteRead; // Avoid overflow (read up to 5 bytes)
            }
            
        }
        
        
        
    }
    
    return 0;
    
}

void I2C_Master_ISR_ExitCallback() {
           
    uint8_t stat = I2C_Master_MasterStatus();
    sprintf(message, "- 0x%02x \r\n", stat);
    UART_1_PutString(message);
    
    /* Master Read Complete */
    if ( stat & I2C_Master_MSTAT_RD_CMPLT ) {
        state = 0x02;
    }
    
    /* Master Write Complete */
    if ( stat & I2C_Master_MSTAT_WR_CMPLT ) {
        state = 0x03;
    }
    
}

/* [] END OF FILE */
