cmake_minimum_required(VERSION 3.5)

if("$ENV{TRAVIS_BUILD_NUMBER}" STREQUAL "")
  message("Warning: Could not find TRAVIS_BUILD_NUMBER environment variable")
  set(VERSION_NUMBER "0.0.42")
else()
  set(VERSION_NUMBER "0.0.$ENV{TRAVIS_BUILD_NUMBER}")
endif()

project(bulk VERSION ${VERSION_NUMBER})

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

add_library(utilities src/command_reader.cpp)
target_include_directories(utilities
    PRIVATE 
    "include"
)


add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE 
    "include"
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE utilities)

add_executable(test_writer tests/test_writer.cpp)
target_include_directories(test_writer
    PRIVATE 
    "${CMAKE_HOME_DIRECTORY}/include"
    "${GTEST_INCLUDE_DIR}"
)
target_link_libraries(test_writer
    PRIVATE
    utilities
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(test_reader tests/test_reader.cpp)
target_include_directories(test_reader
    PRIVATE 
    "${CMAKE_HOME_DIRECTORY}/include"
    "${GTEST_INCLUDE_DIR}"
)
target_link_libraries(test_reader
    PRIVATE
    utilities
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)


enable_testing()
add_test(test_writer bin/test_writer)

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT vefremov@gmail.com)

include(CPack)

configure_file(bintray-descriptor.in.json ${CMAKE_BINARY_DIR}/bintray-descriptor.json)
