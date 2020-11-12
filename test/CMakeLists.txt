include(GoogleTest)

# TODO: Use "configure_file" or another more robust method to copy files
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/sinewave_100Hz.wav
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(audio_test
    main.cpp
    file_test.cpp
    wav_file_test.cpp
)

target_link_libraries(audio_test
    tnt::audio
    CONAN_PKG::gtest
)

gtest_discover_tests(audio_test)