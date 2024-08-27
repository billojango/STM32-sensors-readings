set(LIB_NAME keypad)

add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
        src/keypad.cpp
)

target_include_directories(${LIB_NAME} INTERFACE 
        include/
)

target_link_libraries(${LIB_NAME} INTERFACE mbed-os)
