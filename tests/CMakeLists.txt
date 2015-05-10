if (WITH_EXAMPLES)

set(FONTMAKER ${CMAKE_CURRENT_BINARY_DIR}/../bin/fontmaker)
set(OUT_C     ${CMAKE_CURRENT_BINARY_DIR}/font.c)
set(OUT_H     ${CMAKE_CURRENT_BINARY_DIR}/font.h)
set(OUT_HXX   ${CMAKE_CURRENT_BINARY_DIR}/font.hxx)
set(FONT      ${CMAKE_CURRENT_SOURCE_DIR}/Consolas.ttf)
set(CHARMAP   ${CMAKE_CURRENT_SOURCE_DIR}/charmap.fm)

add_custom_command(
   OUTPUT ${OUT_C}
   COMMAND ${FONTMAKER} --gc ${OUT_C} --font-file=${FONT} --map-file=${CHARMAP} --font-size=12
   DEPENDS ${CHARMAP}
)

add_custom_command(
   OUTPUT ${OUT_H}
   COMMAND ${FONTMAKER} --gh ${OUT_H} --font-file=${FONT} --map-file=${CHARMAP} --font-size=12
   DEPENDS ${CHARMAP}
)

add_custom_command(
   OUTPUT ${OUT_HXX}
   COMMAND ${FONTMAKER} --ghpp ${OUT_HXX} --font-file=${FONT} --map-file=${CHARMAP} --font-size=12
   DEPENDS ${CHARMAP}
)


include_directories(fontmaker_test ${CMAKE_CURRENT_BINARY_DIR})
add_executable(fontmaker_test
   ${OUT_C}
   ${OUT_H}
   test.c
)

add_executable(fontmaker_cpp_test
   ${OUT_HXX}
   test.cc
)

endif ()

