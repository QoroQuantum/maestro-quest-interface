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
    GIT_TAG        v4.2.0
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

# --- C++ example (requires Maestro) ---
# Build with: cmake -B build -DMAESTRO_DIR=/path/to/maestro
if(MAESTRO_DIR)
    add_executable(quest_example examples/quest_example.cpp)

    target_include_directories(quest_example PRIVATE ${MAESTRO_DIR})

    # Maestro headers transitively include Eigen, QCSim, Boost, etc.
    # Look for them inside the Maestro scikit-build output.
    file(GLOB MAESTRO_BUILD_DIRS "${MAESTRO_DIR}/build/cp*")
    list(GET MAESTRO_BUILD_DIRS 0 MAESTRO_BUILD_DIR)
    if(EXISTS "${MAESTRO_BUILD_DIR}/_deps/eigen-src")
        target_include_directories(quest_example PRIVATE
            ${MAESTRO_BUILD_DIR}/_deps/eigen-src
            ${MAESTRO_BUILD_DIR}/_deps/qcsim-src
        )
    endif()

    find_library(MAESTRO_LIB maestro PATHS ${MAESTRO_DIR}/build
                 ${MAESTRO_DIR}/build/cp312-cp312-macosx_26_0_arm64
                 NO_DEFAULT_PATH)
    if(MAESTRO_LIB)
        target_link_libraries(quest_example PRIVATE ${MAESTRO_LIB})
        message(STATUS "Maestro library found: ${MAESTRO_LIB}")
    else()
        message(WARNING "Maestro library not found in ${MAESTRO_DIR}/build — quest_example will not link")
    endif()
else()
    message(STATUS "MAESTRO_DIR not set — skipping quest_example (requires Maestro)")
endif()

enable_testing()
add_test(NAME tests COMMAND tests)
