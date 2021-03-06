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


int main(void) {
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_1_Start();
    I2C_Master_Start();
    
    UART_1_PutString("\r\n***** PSoC-PSoC I2C Comm *****\r\n");
    
    // Check if LIS3DH is connected
    uint8_t rval = I2C_Master_MasterSendStart(PSOC_SLAVE_ADDRESS, I2C_Master_WRITE_XFER_MODE);
    if ( rval == I2C_Master_MSTR_NO_ERROR ) {
        UART_1_PutString("PSoC slave found @ address 0x08\r\n");
    }
    I2C_Master_MasterSendStop();
    
    CyDelay(100);
    
    // UART buffer
    char message[50];
    // I2C error return code
    uint8_t error;
    
    /* Functions I2C_Master_MasterReadBuf and I2C_Master_MasterWriteBuf 
       read/write the entire slave buffer, or the first N bytes specified in the function call */

    
    // Read slave buffer
    I2C_Master_MasterClearStatus();
    uint8_t rData[I2C_BUFFER_SIZE];
    error = I2C_Master_MasterReadBuf(PSOC_SLAVE_ADDRESS, rData, I2C_BUFFER_SIZE, I2C_Master_MODE_COMPLETE_XFER);
    while ( I2C_Master_MasterStatus() & I2C_Master_MSTAT_XFER_INP ); // wait for transfer to complete
    if ( error == I2C_Master_MSTR_NO_ERROR ) {
        UART_1_PutString("Data from I2C slave: ");
        for ( uint8_t i = 0; i < I2C_BUFFER_SIZE; i++ ) {
            sprintf(message, "0x%02x ", rData[i]);
            UART_1_PutString(message);
        }
        UART_1_PutString("\r\n");
    }
    I2C_Master_MasterClearReadBuf();
    
    CyDelay(100);
    
    
    
    // Write to slave
    I2C_Master_MasterClearStatus();
    uint8_t wData[I2C_BUFFER_SIZE] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E};
    error = I2C_Master_MasterWriteBuf(PSOC_SLAVE_ADDRESS, wData, I2C_BUFFER_SIZE, I2C_Master_MODE_COMPLETE_XFER);
    while ( I2C_Master_MasterStatus() & I2C_Master_MSTAT_XFER_INP ) ; // wait for transfer to complete
    if ( error == I2C_Master_MSTR_NO_ERROR ) {
        UART_1_PutString("Data written to I2C slave.\r\n");    
    }
    I2C_Master_MasterClearWriteBuf();
    
    CyDelay(100);
    
    
    
    // Read back slave buffer (partial read)
    I2C_Master_MasterClearStatus();
    error = I2C_Master_MasterReadBuf(PSOC_SLAVE_ADDRESS, rData, 5, I2C_Master_MODE_COMPLETE_XFER);
    while ( I2C_Master_MasterStatus() & I2C_Master_MSTAT_XFER_INP ); // wait for transfer to complete
    if ( error == I2C_Master_MSTR_NO_ERROR ) {
        UART_1_PutString("Data from I2C slave: ");
        for ( uint8_t i = 0; i < I2C_BUFFER_SIZE; i++ ) {
            sprintf(message, "0x%02x ", rData[i]);
            UART_1_PutString(message);
        }
        UART_1_PutString("\r\n");
    }
    I2C_Master_MasterClearReadBuf();
    
    
    return 0;
    
}

/* [] END OF FILE */
