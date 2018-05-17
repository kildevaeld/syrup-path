
set(SOURCES
    main.c
    path.c
)

add_executable(path_test ${SOURCES})
target_link_libraries(path_test syrup-path unity)
add_test(path_test path_test)
