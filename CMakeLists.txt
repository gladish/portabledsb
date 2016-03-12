cmake_minimum_required(VERSION 2.6)

#--------------------------------------------------------------------------------------------------
project(portabledsb CXX)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(PACKAGE_NAME alljoyndsb)
set(PACKAGE_VERSION_MAJOR 1)
set(PACKAGE_VERSION_MINOR 0)
set(PACKAGE_VERSION_PATCH 0)
set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")

#--------------------------------------------------------------------------------------------------
# installation directories

# set install prefix if not given by the user
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/opt/${CMAKE_PROJECT_NAME}" CACHE PATH "Install path prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# set libdir
if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif()

#--------------------------------------------------------------------------------------------------
# build debian package:
#    make package 
#    sudo dpkg -i ./portable-<version>-Linux.deb
#    /usr/bin/moc-adapter --adapter libmockadapter.so --config /path/to/config.xml

include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "DEB")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-dev")
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CMAKE_PROJECT_NAME}-A portable C++ version of the AllJoyn DSB.")
set(CPACK_PACKAGE_VENDOR "Comcast Cable Communications Management,LLC")
set(CPACK_PACKAGE_CONTACT "info@comcast.com")
set(CPACK_PACKAGE_VERSION_MAJOR "${PACKAGE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DEFAULT_LOCATION "${CMAKE_INSTALL_PREFIX}")

#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_SET_DESTDIR On)
set(CMAKE_INSTALL_PREFIX ${CPACK_INSTALL_PREFIX})

include(CPack)

#--------------------------------------------------------------------------------------------------
# hack: disallow in-source build, because otherwize we would overwrite the Makefile in the source directory

get_filename_component(srcdir "${PROJECT_SOURCE_DIR}" REALPATH)
get_filename_component(bindir "${PROJECT_BINARY_DIR}" REALPATH)
if("${srcdir}" STREQUAL "${bindir}")
  message(FATAL_ERROR "in-source build is not allowed")
endif()

#--------------------------------------------------------------------------------------------------
# find packages

# add known alljoyn directories to the seach path
if(IS_DIRECTORY /opt/alljoyn)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /opt/alljoyn)
endif()
if(IS_DIRECTORY /Users/jgladi200/Work/alljoyn/alljoyn-15.09.00a-src/build/darwin/x86/debug/dist)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /Users/jgladi200/Work/alljoyn/alljoyn-15.09.00a-src/build/darwin/x86/debug/dist)
endif()

# find required AllJoyn:
find_package(AllJoyn REQUIRED alljoyn )

# find required LibXml2
find_package(LibXml2 REQUIRED )

# find required UUID
find_package(UUID REQUIRED )

# find OpenSSL library 
find_package(OpenSSL)

# find GTest
find_package(GTest)
if(GTEST_FOUND)
  enable_testing()
  message(STATUS "testing is enabled")
else()
  message(AUTHOR_WARNING "if you want to enable testing, please set GTEST_ROOT (as a CMake or environment variable) to the root directory of the gtest install prefix")
endif()

#--------------------------------------------------------------------------------------------------
# common compiler/linker settings

if(CMAKE_COMPILER_IS_GNUCXX)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
  check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
  if(COMPILER_SUPPORTS_CXX11)
    set(PROJECT_CXX_FLAGS "-std=c++11")
  elseif(COMPILER_SUPPORTS_CXX0X)
    set(PROJECT_CXX_FLAGS "-std=c++0x")
  else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_CXX_FLAGS}")
  set(CMAKE_SKIP_BUILD_RPATH  FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
  if(UNIX AND NOT APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-zdefs")
  endif()
endif()

add_definitions(-DQCC_OS_GROUP_POSIX)



#--------------------------------------------------------------------------------------------------
# build targets


# alljoyndsb library:
add_subdirectory(common) 

# alljoyndsb_bridge library:
add_subdirectory(bridge)

# mockadapter library:
add_subdirectory(adapters/mock)

# moc-adapter:
add_executable       (moc-adapter main.cpp)
target_include_directories(mockadapter PUBLIC "${PROJECT_SOURCE_DIR}")
target_link_libraries(moc-adapter alljoyndsb_bridge alljoyndsb ${AllJoyn_LIBRARY} ${LIBXML2_LIBRARIES} ${UUID_LIBRARIES} -lpthread)
install(TARGETS moc-adapter DESTINATION bin)

# create pkgconfig file
set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir      ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(includedir  ${CMAKE_INSTALL_PREFIX}/include)
set(PROJECT_CXXFLAGS "${PROJECT_CXX_FLAGS}")
set(PROJECT_LIBS "-lalljoyndsb_bridge -lalljoyndsb")
configure_file(${PROJECT_SOURCE_DIR}/cmake/portabledsb.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc DESTINATION share/pkgconfig)

# tests:
if(GTEST_FOUND)
  add_subdirectory(Tests)
endif()


#--------------------------------------------------------------------------------------------------
# additional install targets:

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/alljoyndsb.xml DESTINATION etc)

#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt DESTINATION ${CMAKE_INSTALL_PREFIX})

file(GLOB_RECURSE INCLUDE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
foreach ( file ${INCLUDE_FILES} )
    get_filename_component( subdir ${file} DIRECTORY )
    install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file} DESTINATION include/${CMAKE_PROJECT_NAME}/${subdir} )
endforeach()


#--------------------------------------------------------------------------------------------------
# mini-howto:

# configure:
#     builddir$ cmake /path/to/srcdir
#   if a library cannot be found, try to add the library prefix to the search path 'CMAKE_PREFIX_PATH':
#     builddir$ cmake /path/to/srcdir -DCMAKE_PREFIX_PATH=/opt/alljoyn

# build and install:
#   builddir$ make
#   builddir$ make install

# build debian package and install:
#   builddir$ cmake /path/to/srcdir -DCMAKE_PREFIX_PATH=/usr
#   builddir$ make package
#   builddir$ sudo dpkg -i portabledsb*.deb
#   builddir$ dpkg -L portabledsb

# uninstall debian package
#   $ sudo dpkg -P portabledsb


