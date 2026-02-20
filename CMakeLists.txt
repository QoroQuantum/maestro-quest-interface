cmake_minimum_required(VERSION 3.20)
project(maestro-quest-interface LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Fetch and build the QuEST simulator from source ---
include(FetchContent)
FetchContent_Declare(
    QuEST
    GIT_REPOSITORY https://github.com/QuEST-Kit/QuEST.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(QuEST)

# --- maestroquest shared library ---
set(MAESTROQUEST_SOURCES maestroquest/Impl.cpp)
if(WIN32)
    list(APPEND MAESTROQUEST_SOURCES maestroquest/dllmain.cpp)
endif()

add_library(maestroquest SHARED ${MAESTROQUEST_SOURCES})

target_include_directories(maestroquest
    PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/maestroquest>
    PRIVATE ${quest_SOURCE_DIR}
)

target_include_directories(maestroquest PRIVATE ${quest_SOURCE_DIR} ${quest_SOURCE_DIR}/include ${quest_SOURCE_DIR}/quest/include ${quest_BINARY_DIR})

target_link_libraries(maestroquest PRIVATE QuEST)

# --- tests executable ---
add_executable(tests tests/tests.cpp)

target_link_libraries(tests PRIVATE maestroquest)

enable_testing()
add_test(NAME tests COMMAND tests)
