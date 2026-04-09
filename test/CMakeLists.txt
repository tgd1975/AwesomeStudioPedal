set(PROD_SOURCES
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/profile.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/profile_manager.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/event_dispatcher.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/send.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/action.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/config_loader.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/serial_action.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/non_send_action.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/littlefs_file_system.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/serial_logger.cpp
    ${CMAKE_SOURCE_DIR}/lib/hardware/esp32/src/button.cpp
    ${CMAKE_SOURCE_DIR}/test/fakes/arduino_shim.cpp
)

add_executable(pedal_tests
    unit/test_event_dispatcher.cpp
    unit/test_profile_manager.cpp
    unit/test_send.cpp
    unit/test_button.cpp
    test_config_loader.cpp
    ${PROD_SOURCES}
)

target_include_directories(pedal_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/include
    ${CMAKE_SOURCE_DIR}/lib/hardware/esp32/include
    ${CMAKE_SOURCE_DIR}/test/mocks
    ${CMAKE_SOURCE_DIR}/test/fakes
    ${CMAKE_SOURCE_DIR}/.pio/libdeps/nodemcu-32s/ArduinoJson/src
)

target_compile_definitions(pedal_tests PRIVATE HOST_TEST_BUILD)

target_link_libraries(pedal_tests GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(pedal_tests
    XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/test-results
)
