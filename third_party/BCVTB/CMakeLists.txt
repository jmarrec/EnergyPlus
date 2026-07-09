SET(SRC
BCVTB/defines.h
BCVTB/utilSocket.c
BCVTB/utilSocket.h
BCVTB/utilXml.c
BCVTB/utilXml.h
)

IF( WIN32 )
  LIST( APPEND SRC BCVTB/bcvtb.def )
ENDIF()

IF (UNIX)
  ADD_DEFINITIONS("-fPIC")
ENDIF()

ADD_LIBRARY( bcvtb STATIC ${SRC} )
target_include_directories(bcvtb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bcvtb PRIVATE epexpat)
target_compile_options(bcvtb PRIVATE $<$<COMPILE_LANG_AND_ID:C,AppleClang>:-Wno-shorten-64-to-32>)

IF( WIN32 )
  TARGET_LINK_LIBRARIES( bcvtb wsock32 )
ENDIF()

set_target_properties(
  bcvtb
  PROPERTIES
  INSTALL_NAME_DIR "@executable_path"
)

