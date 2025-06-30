file(GLOB_RECURSE srcs "main.cpp" "src/*.cpp" "src/*.c")

idf_component_register(SRCS "${srcs}"
                        PRIV_REQUIRES usb    
                        INCLUDE_DIRS "./include" ".")
