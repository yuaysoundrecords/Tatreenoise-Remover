cmake_minimum_required(VERSION 3.22)
project(TatreeNoiseRemover VERSION 2.1.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(MSVC AND NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
if(DEFINED JUCE_PATH)
  add_subdirectory("${JUCE_PATH}" "${CMAKE_BINARY_DIR}/JUCE")
else()
  include(FetchContent)
  FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG d6181bde38d858c283c3b7bf699ce6340c050b5d GIT_SHALLOW FALSE)
  FetchContent_MakeAvailable(JUCE)
endif()
set(formats VST3 Standalone)
if(APPLE)
  list(APPEND formats AU)
endif()
juce_add_binary_data(TatreeAssets SOURCES assets/lawyer-colour.png assets/lawyer-deess.png assets/lawyer-hot.png assets/paper.png assets/Anton-Regular.ttf assets/NotoSansThai.ttf)
juce_add_plugin(Tatree
  COMPANY_NAME "Yuay Sound Records" BUNDLE_ID "net.yuaysoundrecords.tatree2"
  PLUGIN_MANUFACTURER_CODE Yuay PLUGIN_CODE Ttr2
  FORMATS ${formats} PRODUCT_NAME "Tatree Noise Remover 2"
  MICROPHONE_PERMISSION_ENABLED TRUE
  MICROPHONE_PERMISSION_TEXT "Tatree processes the selected audio input locally on your Mac."
  IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE VST3_CATEGORIES Fx Restoration)
if(APPLE)
  # AU keeps its original identity for existing sessions. Separate the app and
  # VST3 identities so LaunchServices cannot substitute a non-AU bundle for it.
  foreach(format IN ITEMS Standalone VST3)
    if(format STREQUAL "Standalone")
      set(tatree_bundle_suffix standalone)
    else()
      set(tatree_bundle_suffix vst3)
    endif()
    add_custom_command(TARGET Tatree_${format} POST_BUILD
      COMMAND /usr/libexec/PlistBuddy -c "Set :CFBundleIdentifier net.yuaysoundrecords.tatree2.${tatree_bundle_suffix}" "$<TARGET_BUNDLE_DIR:Tatree_${format}>/Contents/Info.plist"
      COMMAND /usr/bin/codesign --force --sign - "$<TARGET_BUNDLE_DIR:Tatree_${format}>"
      VERBATIM)
  endforeach()
endif()
target_sources(Tatree PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_options(Tatree PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_compile_definitions(Tatree PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(Tatree PRIVATE TatreeAssets juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags)
target_include_directories(Tatree PUBLIC Source "${JUCE_MODULES_DIR}")
juce_add_console_app(TatreeTests PRODUCT_NAME "Tatree tests")
target_sources(TatreeTests PRIVATE tests/Tests.cpp)
target_link_libraries(TatreeTests PRIVATE Tatree TatreeAssets)
enable_testing()
add_test(NAME audio-and-motion COMMAND TatreeTests)
add_test(NAME speech-preservation COMMAND TatreeTests --speech-check "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/jfk.wav")
juce_add_gui_app(TatreeRender PRODUCT_NAME "TatreeRender")
target_sources(TatreeRender PRIVATE tests/Render.cpp)
target_link_libraries(TatreeRender PRIVATE Tatree TatreeAssets)
juce_add_console_app(TatreeOffline PRODUCT_NAME "Tatree offline")
target_sources(TatreeOffline PRIVATE Source/Offline.cpp)
target_link_libraries(TatreeOffline PRIVATE Tatree TatreeAssets)
