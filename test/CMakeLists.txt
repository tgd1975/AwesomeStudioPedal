set(PROD_SOURCES
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/bank_manager.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/event_dispatcher.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/send.cpp
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/src/pedal_config.cpp
    ${CMAKE_SOURCE_DIR}/lib/hardware/esp32/src/button.cpp
    ${CMAKE_SOURCE_DIR}/test/fakes/arduino_shim.cpp
)

add_executable(pedal_tests
    unit/test_event_dispatcher.cpp
    unit/test_bank_manager.cpp
    unit/test_send.cpp
    unit/test_button.cpp
    ${PROD_SOURCES}
)

target_include_directories(pedal_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/PedalLogic/include
    ${CMAKE_SOURCE_DIR}/lib/hardware/esp32/include
    ${CMAKE_SOURCE_DIR}/test/mocks
    ${CMAKE_SOURCE_DIR}/test/fakes
)

target_compile_definitions(pedal_tests PRIVATE HOST_TEST_BUILD)

target_link_libraries(pedal_tests GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(pedal_tests
    XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/test-results
)
