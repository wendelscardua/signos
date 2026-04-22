find_program(
  GENERATE_METASPRITES
  generate-metasprites
  PATHS "${CMAKE_SOURCE_DIR}/tools"
)

if (NOT GENERATE_METASPRITES)
  message(FATAL_ERROR "The generate-metasprites tool is required!")
endif()

add_custom_command(
  OUTPUT metasprites.s ${CMAKE_CURRENT_BINARY_DIR}/metasprites.hpp
  COMMAND ${GENERATE_METASPRITES} generate ${CMAKE_CURRENT_BINARY_DIR}/metasprites.s ${CMAKE_CURRENT_BINARY_DIR}/metasprites.hpp ${CMAKE_SOURCE_DIR}/assets/metasprites.nss --bank .prg_rom_1 --label-prefix metasprite
  DEPENDS ${GENERATE_METASPRITES} ${CMAKE_SOURCE_DIR}/assets/metasprites.nss
)

find_program(
  GENERATE_METATILES
  generate-metatiles
  PATHS "${CMAKE_SOURCE_DIR}/tools"
)

if (NOT GENERATE_METATILES)
  message(FATAL_ERROR "The generate-metatiles tool is required!")
endif()

add_custom_command(
  OUTPUT metatiles.s
  COMMAND ${GENERATE_METATILES} generate ${CMAKE_CURRENT_BINARY_DIR}/metatiles.s ${CMAKE_SOURCE_DIR}/assets/metatiles.map --bank .prg_rom_fixed
  DEPENDS ${GENERATE_METATILES} ${CMAKE_SOURCE_DIR}/assets/metatiles.map
)

find_program(
  LDTK_TOOL
  ldtk-tool
  PATHS "${CMAKE_SOURCE_DIR}/tools"
)

if (NOT LDTK_TOOL)
  message(FATAL_ERROR "The ldtk-tool tool is required!")
endif()

add_custom_command(
  OUTPUT levels.s
  COMMAND ${LDTK_TOOL} convert ${CMAKE_CURRENT_BINARY_DIR}/levels.s ${CMAKE_SOURCE_DIR}/assets/signos.ldtk --bank .prg_rom_fixed
  DEPENDS ${CMAKE_SOURCE_DIR}/assets/signos.ldtk ${LDTK_TOOL}
)

add_library(SourceObj
  OBJECT

  config.s

  main.cpp
  title-screen.cpp
  level-screen.cpp

  attributes.cpp
  robot.cpp
  signal.cpp
  levels.cpp

  banked-asset-helpers.s
  banked-asset-helpers.cpp
  ggsound.cpp

  assets.s
  soundtrack.hpp

  ${CMAKE_CURRENT_BINARY_DIR}/levels.s
  ${CMAKE_CURRENT_BINARY_DIR}/metatiles.s
  ${CMAKE_CURRENT_BINARY_DIR}/metasprites.s
)

set_property(
  SOURCE
  assets.s
  PROPERTY
  OBJECT_DEPENDS

  main-bg.chr
  main-spr.chr
  
  main-bg.pal
  main-spr.pal
  
  title.nam
)

add_raw_asset(SOURCE "main-bg.chr")
add_raw_asset(SOURCE "main-spr.chr")

add_raw_asset(SOURCE "main-bg.pal")
add_raw_asset(SOURCE "main-spr.pal")

add_raw_asset(SOURCE "title.nam")

set_property(
  SOURCE
  soundtrack.s
  PROPERTY
  OBJECT_DEPENDS
  ${CMAKE_CURRENT_BINARY_DIR}/soundtrack.asm
  # ${CMAKE_CURRENT_BINARY_DIR}/soundtrack_dpcm.asm
  ${CMAKE_CURRENT_SOURCE_DIR}/ggsound.inc
)

set_target_properties(SourceObj PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(
  SourceObj
  PRIVATE
  ${CMAKE_BINARY_DIR}/assets
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)


include(FetchContent)

FetchContent_Declare(
  mesen-integration
  URL https://github.com/wendelscardua/mesen-integration/releases/download/v0.1.4/mesen-integration-src.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(mesen-integration)

target_include_directories(SourceObj PRIVATE "${mesen-integration_SOURCE_DIR}/include")

target_link_libraries(SourceObj
  PRIVATE
  mesen-integration
)