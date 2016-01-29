cmake_minimum_required(VERSION 2.8.9)
project (carmin_server)

add_definitions( -DWITH_COOKIES )

file(GLOB SOURCES ./*.cpp)

add_executable(carmin_server ${SOURCES})

include_directories(AFTER 
/home/jl237561/local/include
)

TARGET_LINK_LIBRARIES(carmin_server gsoap++ gsoapssl++ curl )
