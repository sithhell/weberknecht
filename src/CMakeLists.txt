add_subdirectory(irc)
add_subdirectory(sqlite)

add_executable(weberknecht main.cpp weberknecht.cpp quotes.cpp words.cpp)
target_link_libraries(weberknecht boost_system boost_regex irc sqlite3 sqlite_wrapped)
