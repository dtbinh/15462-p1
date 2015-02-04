# Install script for directory: /afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/p1

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF(EXISTS "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/../p1" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/../p1")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/../p1"
         RPATH "")
  ENDIF()
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/../p1")
FILE(INSTALL DESTINATION "/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/.." TYPE EXECUTABLE FILES "/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/build/p1/p1")
  IF(EXISTS "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/../p1" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/../p1")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr22/bofeiz/private/15462/15462-p1/src/../p1")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

