
set(SDK_SRCS
  AdapterInterface.cpp
  AdapterDevice.cpp
  AdapterLoader.cpp
  AdapterLog.cpp
  AdapterMethod.cpp
  AdapterNamedValue.cpp
  AdapterProperty.cpp
  AdapterSignal.cpp
  Guid.cpp
  Value.cpp
)

add_library(alljoyndsb SHARED ${SDK_SRCS})
target_include_directories(alljoyndsb PUBLIC "${PROJECT_SOURCE_DIR}")
target_include_directories(alljoyndsb PUBLIC "${UUID_INCLUDE_DIRS}")
target_link_libraries(alljoyndsb ${AllJoyn_LIBRARY} ${LIBXML2_LIBRARIES} ${UUID_LIBRARIES} -ldl)
install(TARGETS alljoyndsb DESTINATION ${CMAKE_INSTALL_LIBDIR})
