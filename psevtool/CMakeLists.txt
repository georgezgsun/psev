cmake_minimum_required(VERSION 3.1)
project(psevtool)

file(GLOB READER_SRCS *.cpp)

set(EXTRA_LIBS archiver)

set(Boost_USE_STATIC_LIBS on)
set(Boost_USE_MULTITHREADED on)
set(Boost_USE_STATIC_RUNTIME off)

find_package(Boost 1.55.0 REQUIRED
            COMPONENTS
            system
            filesystem
            )
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found, please install it")
else()
    include_directories(${Boost_INCLUDE_DIRS})
    set(EXTRA_LIBS ${EXTRA_LIBS} ${Boost_LIBRARIES})
endif ()


add_executable(${PROJECT_NAME} ${READER_SRCS})
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS})
