find_package(PkgConfig)

pkg_check_modules(PC_AllJoyn QUIET liballjoyn)
set(AllJoyn_DEFINITIONS ${PC_AllJoyn_CFLAGS_OTHER})

find_path(AllJoyn_INCLUDE_DIR alljoyn/AllJoynStd.h
          HINTS ${PC_AllJoyn_INCLUDEDIR} ${PC_AllJoyn_INCLUDE_DIRS}
          PATH_SUFFIXES cpp/inc )

find_library(AllJoyn_LIBRARY NAMES liballjoyn${CMAKE_SHARED_LIBRARY_SUFFIX}
          HINTS ${PC_AllJoyn_LIBDIR} ${PC_AllJoyn_LIBRARY_DIRS} 
          PATH_SUFFIXES cpp/lib )

set(AllJoyn_LIBRARIES ${AllJoyn_LIBRARY} )
set(AllJoyn_INCLUDE_DIRS ${AllJoyn_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set AllJoyn_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(AllJoyn  DEFAULT_MSG
                                  AllJoyn_LIBRARY AllJoyn_INCLUDE_DIR)
                                  
mark_as_advanced(AllJoyn_INCLUDE_DIR AllJoyn_LIBRARY )

