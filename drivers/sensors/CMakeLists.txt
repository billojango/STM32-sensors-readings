set(LIB_NAME sensors)

add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
        src/HX711.cpp
)

target_include_directories(${LIB_NAME} INTERFACE 
        include/
)

target_link_libraries(${LIB_NAME} INTERFACE mbed-os)
