cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS
include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(rtos_project C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable
add_executable(rtos_project 
    src/main.c
)

pico_set_program_name(rtos_project "rtos_project")
pico_set_program_version(rtos_project "1.0")

# Enable stdio output over USB
pico_enable_stdio_uart(rtos_project 0)
pico_enable_stdio_usb(rtos_project 1)

# Add the standard library to the build
target_link_libraries(rtos_project
    pico_stdlib
    hardware_pwm
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# Add the standard include files to the build
target_include_directories(rtos_project PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(rtos_project)
