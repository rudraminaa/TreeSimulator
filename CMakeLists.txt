cmake_minimum_required(VERSION 3.16)
project(TreeSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/core/BSTree.cpp
    src/core/AVLTree.cpp
    src/gui/MainWindow.cpp
    src/gui/TreeScene.cpp
    src/gui/NodeItem.cpp
    src/gui/EdgeItem.cpp
    src/gui/PlusButtonItem.cpp
)

set(HEADERS
    src/core/BSTNode.h
    src/core/BSTree.h
    src/core/AVLTree.h
    src/gui/MainWindow.h
    src/gui/TreeScene.h
    src/gui/NodeItem.h
    src/gui/EdgeItem.h
    src/gui/PlusButtonItem.h
)

set(RESOURCES
    resources/resources.qrc
)

add_executable(TreeSimulator WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(TreeSimulator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(TreeSimulator PRIVATE Qt6::Widgets)
