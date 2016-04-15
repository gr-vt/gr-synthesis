INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SYNTHESIS synthesis)

FIND_PATH(
    SYNTHESIS_INCLUDE_DIRS
    NAMES synthesis/api.h
    HINTS $ENV{SYNTHESIS_DIR}/include
        ${PC_SYNTHESIS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SYNTHESIS_LIBRARIES
    NAMES gnuradio-synthesis
    HINTS $ENV{SYNTHESIS_DIR}/lib
        ${PC_SYNTHESIS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SYNTHESIS DEFAULT_MSG SYNTHESIS_LIBRARIES SYNTHESIS_INCLUDE_DIRS)
MARK_AS_ADVANCED(SYNTHESIS_LIBRARIES SYNTHESIS_INCLUDE_DIRS)

