project (pao VERSION "1.99" DESCRIPTION "pao")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(pao
                       main.cpp
                       )
TARGET_LINK_LIBRARIES( pao
                       paolib
                       triangle
                        )
target_include_directories(pao PRIVATE ../pao/triangle)
target_include_directories(pao PRIVATE ../pao/inc)
target_include_directories(pao PRIVATE ../pao/src)
