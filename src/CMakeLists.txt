tnt_project_AddLibrary(${PROJECT_NAME}
  TARGET
    audio
  SOURCES
    file.cpp
    signal.cpp
    wav_file.cpp
)

target_compile_features(audio PUBLIC cxx_std_17)

target_link_libraries(audio
    tnt::dsp
)