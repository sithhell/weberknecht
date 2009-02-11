add_subdirectory(irc)

add_executable(weberknecht main.cpp)
target_link_libraries(weberknecht boost_system irc)
