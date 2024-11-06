#include "stm32l4xx.h"

// Define the Flash memory page to be used for storing the variable
#define FLASH_PAGE_START   0x080F8000  // Starting address of the last Flash sector (Sector 11)
#define FLASH_PAGE_END     0x080FFFFF  // Ending address of Sector 11

#define VARIABLE_ADDRESS   FLASH_PAGE_START  // The address where the variable will be stored

uint32_t myVariable = 12345;  // Initial variable value

// Function prototypes
void Flash_Write(uint32_t address, uint32_t data);
uint32_t Flash_Read(uint32_t address);
void Flash_EraseLastSector(void);
void Error_Handler(uint32_t errorCode);  // Enhanced error handler

int main(void)
{
    // Step 1: Read value from the Flash memory
    uint32_t flashData = Flash_Read(VARIABLE_ADDRESS);

    // Step 2: Update variable if the read data is different
    if (flashData != myVariable)
    {
        // Erase the Flash sector before writing new data
        Flash_EraseLastSector();

        // Write the new variable to the Flash memory
        Flash_Write(VARIABLE_ADDRESS, myVariable);
    }

    // Infinite loop for normal operation
    while (1)
    {
        // Add any background tasks or idle code here
    }
}

// Function to write data to Flash memory at a specific address
void Flash_Write(uint32_t address, uint32_t data)
{
    // Unlock the Flash memory for write operations
    FLASH->KEYR = 0x45670123;  // Unlock key 1
    FLASH->KEYR = 0xCDEF89AB;  // Unlock key 2

    // Wait until the Flash is not busy before proceeding
    while (FLASH->SR & FLASH_SR_BSY) {}

    // Add NOP instructions for timing, just in case the processor needs to process some operations
    __asm("NOP");
    __asm("NOP");

    // Write data to the specified Flash address (program the word)
    *(volatile uint32_t*)address = data;

    // Check for programming errors
    if (FLASH->SR & FLASH_SR_PGERR)
    {
        // Clear the programming error flag
        FLASH->SR |= FLASH_SR_PGERR;
        Error_Handler(2);  // 2 indicates programming error
    }

    // Wait until the Flash operation is complete
    while (FLASH->SR & FLASH_SR_BSY) {}

    // Lock the Flash memory again
    FLASH->CR |= FLASH_CR_LOCK;
}

// Function to read data from Flash memory at a specific address
uint32_t Flash_Read(uint32_t address)
{
    return *(volatile uint32_t*)address;
}

// Function to erase the last Flash sector (Sector 11 in STM32L432KC)
void Flash_EraseLastSector(void)
{
    // Unlock the Flash memory
    FLASH->KEYR = 0x45670123;  // Unlock key 1
    FLASH->KEYR = 0xCDEF89AB;  // Unlock key 2

    // Wait until the Flash is not busy before starting the erase operation
    while (FLASH->SR & FLASH_SR_BSY) {}

    // Add NOP instructions to help with timing between operations
    __asm("NOP");
    __asm("NOP");

    // Set the sector erase bit in the control register (erase sector 11)
    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR |= FLASH_CR_SNB_5;  // Sector number 11
    FLASH->CR |= FLASH_CR_STRT;   // Start the erase operation

    // Wait until the erase operation is complete
    while (FLASH->SR & FLASH_SR_BSY) {}

    // Clear the end of operation flag
    FLASH->SR |= FLASH_SR_EOP;

    // Lock the Flash memory again
    FLASH->CR |= FLASH_CR_LOCK;
}

// Enhanced error handler function with error code logging
void Error_Handler(uint32_t errorCode)
{
    // Log the error code (in practice, you can use UART to send the code to a terminal or use an LED blink pattern)
    while (1)
    {
        // Flash an LED or use UART for error reporting, based on the errorCode
        // For now, just entering an infinite loop.
        // You can map error codes to specific diagnostic actions for easier debugging.
    }
}
