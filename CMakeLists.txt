# I target a recent cmake, it shouldn't be a problem on a dev machine
cmake_minimum_required(VERSION 3.7)
# Project name
project("My Firmware")

# Product filename
set(PRODUCT_NAME my_firmware)

## AVR Chip Configuration
set(F_CPU 8000000UL)
# CPU, you can find the list here:
# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
set(MCU atmega8)
# Default Baudrate for UART, read avr include/util/setbaud.h for usage
set(BAUD 9600)
# The programmer to use, read avrdude manual for list
set(PROG_TYPE linuxgpio)

set(SSH_LOGIN pi@cato.local)

set(AVRDUDE ssh ${SSH_LOGIN} sudo avrdude)

# AVR Fuses, must be in concordance with your hardware and F_CPU
# http://eleccelerator.com/fusecalc/fusecalc.php
set(H_FUSE 0xd9)
set(L_FUSE 0xff)


# Use AVR GCC toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)


# Pass defines to compiler
add_definitions(
    -DF_CPU=${F_CPU}
    -DBAUD=${BAUD}
)
# mmcu MUST be passed to bot the compiler and linker, this handle the linker
set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

add_compile_options(
    -mmcu=${MCU} # MCU
    -Os # optimize
)

file(GLOB SRC_FILES "src/*.c") # Load all files in src folder

SET_SOURCE_FILES_PROPERTIES(src/usbdrv/usbdrvasm.S PROPERTIES LANGUAGE C)

# Create one target
add_executable(${PRODUCT_NAME} ${SRC_FILES})

# Rename the output to .elf as we will create multiple files
set_target_properties(${PRODUCT_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)

# Strip binary for upload
add_custom_target(strip ALL avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex DEPENDS strip)
# Transform binary into hex file, this is the eeprom part (empty if you don't
# use eeprom static variables)
add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)

add_custom_target(scp bash -lc 'scp ${PRODUCT_NAME}.hex ${SSH_LOGIN}:/tmp/${PRODUCT_NAME}.hex' DEPENDS hex)

# Upload the firmware with avrdude
add_custom_target(upload bash -lc '${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} -U flash:w:/tmp/${PRODUCT_NAME}.hex' DEPENDS scp)

# Upload the eeprom with avrdude
add_custom_target(upload_eeprom bash -lc '${AVRDUDE} -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep' DEPENDS eeprom)

# Burn fuses
add_custom_target(fuses bash -lc '${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m')

# -v
add_custom_target(view bash -lc '${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} -v')

# Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")
