cmake_minimum_required(VERSION 2.6)

SET(BOOST_INCLUDE /usr/include/boost-1_38)

project(weberknecht)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -ansi -pedantic -isystem ${BOOST_INCLUDE}")
SET(CMAKE_BUILD_TYPE Debug)


add_subdirectory(src)

include_directories(${WEBERKNECHT_SOURCE_DIR}/src/)

link_directories(${WEBERKNECHT_SOURCE_DIR}/src/irc)
