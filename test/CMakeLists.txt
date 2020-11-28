include(Catch)

# Copy data files to the binary directory
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(audio_test
    main.cpp
    approx.cpp
    config.cpp
    file.cpp
    io.cpp
    multisignal.cpp
    signal.cpp
    wave_file.cpp
    #TODO: others
)

target_link_libraries(audio_test
    tnt::audio
    tnt::dsp
    Catch2::Catch2WithMain
)

catch_discover_tests(audio_test)