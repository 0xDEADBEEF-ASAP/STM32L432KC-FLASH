    .section .text
    .global _start

_start:
    // Initialize the stack pointer (for bare-metal or minimal startup)
    LDR     R0, =0x20020000      // Load stack pointer address
    MOV     SP, R0               // Set stack pointer

    // Main program starts here
    BL      main

    // Infinite loop to prevent the processor from exiting
    b       .

// Define the flash memory address and other constants
FLASH_PAGE_START   .equ    0x080F8000   // Starting address of the last Flash sector (Sector 11)
FLASH_PAGE_END     .equ    0x080FFFFF   // Ending address of Sector 11
VARIABLE_ADDRESS   .equ    FLASH_PAGE_START

myVariable          .word   12345         // Initial variable value

main:
    // Step 1: Read value from Flash memory
    LDR     R1, =VARIABLE_ADDRESS
    BL      Flash_Read

    // Store the read value into R0 (to compare later)
    MOV     R2, R0               // Store flash data in R2

    // Step 2: Compare the Flash value with myVariable
    LDR     R1, =myVariable      // Load the address of myVariable
    LDR     R1, [R1]             // Load the value of myVariable
    CMP     R2, R1               // Compare Flash value with myVariable
    BEQ     no_update            // If they are equal, skip update

    // Step 3: Erase Flash sector before writing
    BL      Flash_EraseLastSector

    // Step 4: Write the new value to Flash
    LDR     R1, =myVariable      // Load the address of myVariable
    LDR     R1, [R1]             // Load the value of myVariable
    BL      Flash_Write

no_update:
    // Infinite loop
    b       .

// Flash Write function
Flash_Write:
    // R1 = address, R2 = data

    // Unlock Flash memory
    LDR     R0, =0x40022000       // FLASH base address
    LDR     R3, =0x45670123       // Unlock key 1
    STR     R3, [R0, #0x08]       // Write unlock key 1 to FLASH_KEYR
    LDR     R3, =0xCDEF89AB       // Unlock key 2
    STR     R3, [R0, #0x08]       // Write unlock key 2 to FLASH_KEYR

    // Wait until FLASH is not busy
wait_flash_busy:
    LDR     R3, [R0, #0x0C]       // Read FLASH_SR
    TST     R3, #0x01             // Check if BSY bit is set (Flash busy)
    BNE     wait_flash_busy       // If BSY is set, wait

    // Write data to Flash
    STR     R2, [R1]              // Store the data in the flash address (R1 = address, R2 = data)

    // Wait until FLASH is not busy again
wait_flash_busy2:
    LDR     R3, [R0, #0x0C]       // Read FLASH_SR
    TST     R3, #0x01             // Check if BSY bit is set
    BNE     wait_flash_busy2      // If BSY is set, wait

    // Lock Flash memory after writing
    LDR     R3, [R0, #0x10]       // Read FLASH_CR
    ORR     R3, R3, #0x80         // Set LOCK bit
    STR     R3, [R0, #0x10]       // Write back to FLASH_CR

    BX      LR

// Flash Read function
Flash_Read:
    // R1 = address, return value in R0
    LDR     R0, [R1]              // Load the data at address R1 (Flash address)
    BX      LR

// Flash Erase function (erase last sector)
Flash_EraseLastSector:
    // R1 = address (not used, we erase sector 11 specifically)

    // Unlock Flash memory
    LDR     R0, =0x40022000       // FLASH base address
    LDR     R3, =0x45670123       // Unlock key 1
    STR     R3, [R0, #0x08]       // Write unlock key 1 to FLASH_KEYR
    LDR     R3, =0xCDEF89AB       // Unlock key 2
    STR     R3, [R0, #0x08]       // Write unlock key 2 to FLASH_KEYR

    // Wait until FLASH is not busy
wait_flash_busy3:
    LDR     R3, [R0, #0x0C]       // Read FLASH_SR
    TST     R3, #0x01             // Check if BSY bit is set
    BNE     wait_flash_busy3      // If BSY is set, wait

    // Start erasing sector 11 (sector number 11 corresponds to SNB_5)
    LDR     R3, [R0, #0x10]       // Read FLASH_CR
    BIC     R3, R3, #0x3F         // Clear SNB bits
    ORR     R3, R3, #0x20         // Set SNB_5 (sector 11)
    STR     R3, [R0, #0x10]       // Write back to FLASH_CR

    // Enable sector erase (SER) and start erasure
    LDR     R3, [R0, #0x10]       // Read FLASH_CR
    ORR     R3, R3, #0x02         // Set SER bit
    STR     R3, [R0, #0x10]       // Write back to FLASH_CR

    LDR     R3, [R0, #0x10]       // Read FLASH_CR again
    ORR     R3, R3, #0x40         // Set ST_R bit to start the erase operation
    STR     R3, [R0, #0x10]       // Write back to FLASH_CR

    // Wait until FLASH is not busy
wait_flash_busy4:
    LDR     R3, [R0, #0x0C]       // Read FLASH_SR
    TST     R3, #0x01             // Check if BSY bit is set
    BNE     wait_flash_busy4      // If BSY is set, wait

    // Clear EOP flag after operation is complete
    LDR     R3, [R0, #0x0C]       // Read FLASH_SR
    ORR     R3, R3, #0x20         // Set EOP flag
    STR     R3, [R0, #0x0C]       // Write back to FLASH_SR

    // Lock Flash memory after erasing
    LDR     R3, [R0, #0x10]       // Read FLASH_CR
    ORR     R3, R3, #0x80         // Set LOCK bit
    STR     R3, [R0, #0x10]       // Write back to FLASH_CR

    BX      LR
