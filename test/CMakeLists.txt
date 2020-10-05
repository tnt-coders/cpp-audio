include(GoogleTest)

add_executable(audio_test
    #TODO: <files>
    main.cpp
)

target_link_libraries(audio_test
    tnt::audio
    CONAN_PKG::gtest
)

gtest_discover_tests(audio_test)