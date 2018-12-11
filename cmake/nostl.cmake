find_package(PythonInterp)
if (NOT PYTHONINTERP_FOUND)
    message(FATAL "Did not find python interpreter")
endif()

set(NOSTL_DIR ${CMAKE_CURRENT_BINARY_DIR}/nostl)
file(MAKE_DIRECTORY ${NOSTL_DIR})
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_nostl.py
    WORKING_DIRECTORY ${NOSTL_DIR}
    RESULT_VARIABLE env_rv
)
if(NOT env_rv STREQUAL "0")
    message(FATAL_ERROR "generate_nostl.py failed: ${env_rv}\n")
endif()

add_library(nostl INTERFACE)
target_include_directories(nostl SYSTEM BEFORE INTERFACE ${NOSTL_DIR})
