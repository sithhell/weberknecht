cmake_minimum_required(VERSION 2.6)
project(weberknecht)

add_subdirectory(src)

include_directories(${WEBERKNECHT_SOURCE_DIR}/src/)
link_directories(${WEBERKNECHT_SOURCE_DIR}/src/irc)
