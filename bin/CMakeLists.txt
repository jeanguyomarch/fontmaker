link_directories(${FREETYPE_LIBRARY_DIRS})
include_directories(
   ${FREETYPE_INCLUDE_DIRS}
   ${CMAKE_CURRENT_SOURCE_DIR}
)

set(FLEX_IN ${CMAKE_CURRENT_SOURCE_DIR}/charmap.l)
set(FLEX_OUT ${CMAKE_CURRENT_BINARY_DIR}/charmap.c)

add_custom_command(
   OUTPUT ${FLEX_OUT}
   COMMAND ${FLEX} -o ${FLEX_OUT} ${FLEX_IN}
   DEPENDS ${FLEX_IN}
)

add_executable(fontmaker
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
   ${CMAKE_CURRENT_SOURCE_DIR}/opts.c
   ${CMAKE_CURRENT_SOURCE_DIR}/freetype.c
   ${CMAKE_CURRENT_SOURCE_DIR}/output.c
   ${CMAKE_CURRENT_SOURCE_DIR}/pgm.c
   ${CMAKE_CURRENT_SOURCE_DIR}/utf8.c
   ${FLEX_OUT}
)

target_link_libraries(fontmaker ${FREETYPE_LIBRARIES} -ll)
install(TARGETS fontmaker RUNTIME DESTINATION bin)

