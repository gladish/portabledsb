
set(BRIDGE_SRCS
  AllJoynAbout.cpp
  AllJoynHelper.cpp
  Bridge.cpp
  BridgeBusObject.cpp
  BridgeConfig.cpp
  ConfigManager.cpp
  BridgeError.cpp
)

add_library(alljoyndsb_bridge STATIC ${BRIDGE_SRCS})
target_include_directories(alljoyndsb_bridge PUBLIC "${PROJECT_SOURCE_DIR}")
target_include_directories(alljoyndsb_bridge PUBLIC "${AllJoyn_INCLUDE_DIR}")
target_include_directories(alljoyndsb_bridge PUBLIC "${LIBXML2_INCLUDE_DIR}")
install(TARGETS alljoyndsb_bridge DESTINATION lib)
