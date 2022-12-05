file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

include_directories(${CMAKE_CURRENT_LIST_DIR} ${SC_MEMORY_SRC} ${SC_KPM_SRC})

add_library (courseWorkModule SHARED ${SOURCES})
target_link_libraries(courseWorkModule sc-memory sc-agents-common)

sc_codegen_ex(courseWorkModule ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)
