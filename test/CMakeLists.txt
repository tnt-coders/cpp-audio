include(Catch)

# Copy data files to the binary directory
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME}_test
    main.cpp
    approx.cpp
    config.cpp
    file.cpp
    io.cpp
    multisignal.cpp
    signal.cpp
    wave_file.cpp
)

target_link_libraries(${PROJECT_NAME}_test
    tnt::${PROJECT_NAME}
    tnt::dsp
    Catch2::Catch2WithMain
)

catch_discover_tests(${PROJECT_NAME}_test)