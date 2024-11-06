#include "stm32l4xx_hal.h"

// Define the Flash memory page to be used for storing the variable
#define FLASH_PAGE_START   0x080F8000  // Starting address of the last Flash sector (Sector 11)
#define FLASH_PAGE_END     0x080FFFFF  // Ending address of Sector 11

#define VARIABLE_ADDRESS   FLASH_PAGE_START  // The address where the variable will be stored

uint32_t myVariable = 12345;  // Initial variable value

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void); // Optional for UART debugging
void Flash_Write(uint32_t address, uint32_t data);
uint32_t Flash_Read(uint32_t address);
void Flash_EraseSector(uint32_t startAddress, uint32_t endAddress);
void Error_Handler(void); // Enhanced error handler

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
        Flash_EraseSector(FLASH_PAGE_START, FLASH_PAGE_END);

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
        // Handle unlocking error
        Error_Handler();
    }

    // Check if Flash is busy before proceeding
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}

    // Write data to the specified Flash address
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data) != HAL_OK)
    {
        // Check the error flags and handle the error
        if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PGERR))
        {
            // Programming error occurred
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR);  // Clear the error flag
            Error_Handler();
        }
        else if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_WRPERR))
        {
            // Write protection error occurred
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);  // Clear the error flag
            Error_Handler();
        }
        else
        {
            // Other errors
            Error_Handler();
        }
    }

    // Ensure the Flash operation is complete (check if it's not busy)
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}

    // Lock the Flash memory again
    if (HAL_FLASH_Lock() != HAL_OK)
    {
        // Handle locking error
        Error_Handler();
    }
}

// Function to read data from Flash memory at a specific address
uint32_t Flash_Read(uint32_t address)
{
    return *(volatile uint32_t*)address;
}

// Function to erase the Flash sector
void Flash_EraseSector(uint32_t startAddress, uint32_t endAddress)
{
    // Check if Flash is busy before starting the erase operation
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}

    // Erase the specified Flash sector (Sector 11 in this case)
    if (FLASH_Erase_Sector(FLASH_SECTOR_11, VOLTAGE_RANGE_3) != HAL_OK)
    {
        // Handle erase error
        Error_Handler();
    }

    // Ensure that the erase operation is complete (check if it's not busy)
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {}
}

// Enhanced error handler function
void Error_Handler(void)
{
    // Here we could trigger a system reset or log the error, depending on your requirements.
    // For now, we'll simply enter an infinite loop to indicate failure.
    while (1)
    {
        // LED flashing or other error indication logic can be added here.
        // You can also use UART or other means to log errors for debugging.
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
