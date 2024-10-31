# Define target
set(target barcode_lib)





add_library(${target}
        barcode.c
        barcode.h
        )
    
target_include_directories(barcode_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# pull in common dependencies
target_link_libraries(barcode_lib
    pico_stdlib 
    )