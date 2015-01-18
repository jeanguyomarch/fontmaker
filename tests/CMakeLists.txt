if (WITH_EXAMPLES)

set(FONTMAKER ${CMAKE_CURRENT_BINARY_DIR}/../bin/fontmaker)
set(OUT_C     ${CMAKE_CURRENT_BINARY_DIR}/font.c)
set(OUT_H     ${CMAKE_CURRENT_BINARY_DIR}/font.h)
set(FONT      ${CMAKE_CURRENT_SOURCE_DIR}/Consolas.ttf)
set(CHARMAP   ${CMAKE_CURRENT_SOURCE_DIR}/charmap.fm)

add_custom_command(
   OUTPUT ${OUT_C}
   COMMAND ${FONTMAKER} --gc ${OUT_C} --font-file=${FONT} --map-file=${CHARMAP} --font-size=12
)

add_custom_command(
   OUTPUT ${OUT_H}
   COMMAND ${FONTMAKER} --gh ${OUT_H} --font-file=${FONT} --map-file=${CHARMAP} --font-size=12
)

include_directories(fontmaker_test ${CMAKE_CURRENT_BINARY_DIR})
add_executable(fontmaker_test
   ${OUT_C}
   ${OUT_H}
   test.c
)

endif ()

