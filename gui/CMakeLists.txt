project (paogui VERSION "1.99" DESCRIPTION "paogui")

find_package(Qt5Widgets REQUIRED)
find_package(CGAL COMPONENTS Qt5 Core Debug REQUIRED)

include(${CGAL_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(paogui
                       main.cpp
                       mainwindow.cpp
                       InputGraphicsItem.cpp
                       TriangleGraphicsItem.cpp
                       )
target_link_libraries(paogui ${QT_LIBRARIES})
target_link_libraries(paogui ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
TARGET_LINK_LIBRARIES( paogui
                       paolib
                       trianglelib )

target_include_directories(paogui PRIVATE ../triangle)
target_include_directories(paogui PRIVATE ../pao/inc)
target_include_directories(paogui PRIVATE ../pao/src)

qt5_use_modules(paogui Widgets)
