tnt_project_AddLibrary(${PROJECT_NAME} TARGET audio INTERFACE)

target_compile_features(audio INTERFACE cxx_std_17)

target_link_libraries(audio INTERFACE tnt::dsp)