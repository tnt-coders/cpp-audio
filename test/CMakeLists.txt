include(GoogleTest)

add_executable(audio_test
    file_test.cpp
    main.cpp
)

target_link_libraries(audio_test
    tnt::audio
    GTest::GTest
)

gtest_discover_tests(audio_test)