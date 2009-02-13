add_subdirectory(irc)
add_subdirectory(sqlite)

add_executable(weberknecht main.cpp weberknecht.cpp userdb.cpp)
target_link_libraries(weberknecht boost_system boost_regex irc sqlite_wrapped)
