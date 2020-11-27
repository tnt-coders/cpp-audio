include(Catch)

# TODO: Use "configure_file" or another more robust method to copy files
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/sinewave_64bit_100Hz_1000Hz-sample-rate_10-size_1-channels.wav
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/untitled.wav
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(audio_test
    main.cpp
    approx.cpp
    file.cpp
    file_base.cpp
    multisignal.cpp
    signal.cpp
    #TODO: others
)

target_link_libraries(audio_test
    tnt::audio
    tnt::dsp
    Catch2::Catch2WithMain
)

catch_discover_tests(audio_test)