link_directories(${FREETYPE_LIBRARY_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})

add_executable(fontmaker
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
   ${CMAKE_CURRENT_SOURCE_DIR}/opts.c
   ${CMAKE_CURRENT_SOURCE_DIR}/freetype.c
   ${CMAKE_CURRENT_SOURCE_DIR}/output.c
   ${CMAKE_CURRENT_SOURCE_DIR}/charmap.c
   ${CMAKE_CURRENT_SOURCE_DIR}/pgm.c
   ${CMAKE_CURRENT_SOURCE_DIR}/utf8.c
)

target_link_libraries(fontmaker ${FREETYPE_LIBRARIES})
install(TARGETS fontmaker       RUNTIME DESTINATION bin)

