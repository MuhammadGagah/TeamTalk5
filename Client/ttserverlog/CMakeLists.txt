cmake_minimum_required(VERSION 3.1)
project (ttserverlog)

include_directories (../../Library/TeamTalk_DLL)
link_directories (../../Library/TeamTalk_DLL)

option (BUILD_TEAMTALK_CLIENT_LOG_EXAMPLE "Build ttserverlog C++ client example" ON)

if (MSVC)
  add_definitions (-DUNICODE -D_UNICODE)
endif()

if (BUILD_TEAMTALK_CLIENT_LOG_EXAMPLE)
  add_executable (ttserverlog Main.cpp)
  set_property(TARGET ttserverlog PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../Library/TeamTalk_DLL")
  if (BUILD_TEAMTALK_DLL)
    target_link_libraries (ttserverlog TeamTalk5DLL)
  else()
    find_library(TEAMTALK5_LIBRARY TeamTalk5 PATHS ../../Library/TeamTalk_DLL)
    if (NOT TEAMTALK5_LIBRARY)
      message(WARNING "Cannot find TeamTalk5 shared library in ../../Library/TeamTalk_DLL")
    else()
      target_link_libraries (ttserverlog ${TEAMTALK5_LIBRARY})
    endif()
  endif()
  install (TARGETS ttserverlog DESTINATION Client/ttserverlog)
endif()
