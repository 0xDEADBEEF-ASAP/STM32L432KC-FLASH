#include "stm32l4xx_hal.h"

// Define the Flash memory page to be used for storing the variable
#define FLASH_PAGE_START   0x080F8000  // Starting address of the last Flash sector (Sector 11)
#define FLASH_PAGE_END     0x080FFFFF  // Ending address of Sector 11

#define VARIABLE_ADDRESS   FLASH_PAGE_START  // The address where the variable will be stored

uint32_t myVariable = 12345;  // Initial variable value

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);  // Optional for UART debugging
void Flash_Write(uint32_t address, uint32_t data);
uint32_t Flash_Read(uint32_t address);
void Flash_EraseLastSector(void);
void Error_Handler(uint32_t errorCode);  // Enhanced error handler

int main(void)
{
    // Initialize HAL Library
    HAL_Init();

    // Configure the system clock
    SystemClock_Config();

    // Initialize GPIO (if needed)
    MX_GPIO_Init();

    // Optional: Initialize UART for debugging
    MX_USART2_UART_Init();
    
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
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        Error_Handler(1);  // 1 indicates an unlock failure
    }

    // Wait until the Flash is not busy before proceeding
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}

    // Write data to the specified Flash address
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data) != HAL_OK)
    {
        // Check for errors during programming
        if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PGERR))  // Programming error
        {
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR);  // Clear the error flag
            Error_Handler(2);  // 2 indicates programming error
        }
        else if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_WRPERR))  // Write protection error
        {
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);  // Clear the error flag
            Error_Handler(3);  // 3 indicates write protection error
        }
        else
        {
            Error_Handler(4);  // 4 for unknown errors
        }
    }

    // Ensure the Flash operation is complete (check if it's not busy)
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}

    // Lock the Flash memory again
    if (HAL_FLASH_Lock() != HAL_OK)
    {
        Error_Handler(5);  // 5 indicates lock failure
    }
}

// Function to read data from Flash memory at a specific address
uint32_t Flash_Read(uint32_t address)
{
    return *(volatile uint32_t*)address;
}

// Function to erase the last Flash sector (Sector 11 in STM32L432KC)
void Flash_EraseLastSector(void)
{
    // Ensure the Flash is not busy before starting the erase operation
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}

    // Erase the last Flash sector (Sector 11) with voltage range 3
    FLASH_Erase_Sector(FLASH_SECTOR_11, VOLTAGE_RANGE_3);

    // Check if the erase operation was successful
    if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) == RESET)
    {
        Error_Handler(6);  // 6 indicates erase failure
    }

    // Wait until the erase operation is complete
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}

    // Clear the end of operation flag
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
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

// Optional system clock configuration (change as needed)
void SystemClock_Config(void)
{
    // System Clock Configuration (depends on your application and setup)
    // This will need to be set up according to your MCU's needs.
}

// Optional GPIO initialization (if needed)
static void MX_GPIO_Init(void)
{
    // GPIO Initialization (if needed)
}

// Optional UART initialization (if needed for debugging)
static void MX_USART2_UART_Init(void)
{
    // USART Initialization for debugging via UART (optional)
}
