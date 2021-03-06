set(OBC_BASEDIR_COMPONENTS ${CMAKE_SOURCE_DIR}/src/components)
set(OBC_BASEDIR_CORE ${CMAKE_SOURCE_DIR}/src/core)
set(OBC_BASEDIR_BASE ${CMAKE_SOURCE_DIR}/src/base)

set(OBC_BASE_SOURCES
    ${OBC_BASEDIR_CORE}/BackendSync.cpp
    ${OBC_BASEDIR_CORE}/BackendSync.hpp

    ${CMAKE_SOURCE_DIR}/src/roomservice.cpp
    ${CMAKE_SOURCE_DIR}/src/roomservice.hpp


    #
    ${CMAKE_SOURCE_DIR}/src/utils/OBCHelpers.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/OBCHelpers.hpp
    )