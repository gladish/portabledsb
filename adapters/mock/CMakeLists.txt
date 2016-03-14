set(MOCK_ADAPTER_SRCS
  MockAdapter.cpp
  ZigBeeClusterDatabase.cpp
)

add_library(mockadapter SHARED ${MOCK_ADAPTER_SRCS})
target_include_directories(mockadapter PUBLIC "${PROJECT_SOURCE_DIR}")
target_include_directories(mockadapter PUBLIC "${AllJoyn_INCLUDE_DIR}")
target_link_libraries(mockadapter alljoyndsb ${AllJoyn_LIBRARY})
install(TARGETS mockadapter DESTINATION ${CMAKE_INSTALL_LIBDIR})

