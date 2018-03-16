# Name of the project (will be the name of the plugin)

list(APPEND CC_SOURCES
          "arraybuffer.cc"
          "asyncworker.cc"
          "basic_types/number.cc"
          "basic_types/value.cc"
          "binding.cc"
          "buffer.cc"
          "dataview/dataview.cc"
          "dataview/dataview_read_write.cc"
          "error.cc"
          "external.cc"
          "function.cc"
          "name.cc"
          "object/delete_property.cc"
          "object/get_property.cc"
          "object/has_own_property.cc"
          "object/has_property.cc"
          "object/object.cc"
          "object/set_property.cc"
          "promise.cc"
          "typedarray.cc"
          "objectwrap.cc")

# Build a shared library with exception enabled
set(target_name "binding")
add_library(${target_name} SHARED ${CC_SOURCES})
set_target_properties(${target_name} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(${target_name} PRIVATE ${NodeJS_INCLUDE_DIRS})
target_link_libraries(${target_name} ${NodeJS_LIBRARIES})
target_compile_definitions(${target_name} PRIVATE "NAPI_CPP_EXCEPTIONS" "NAPI_DATA_VIEW_FEATURE")
# platform dependent options
if (MSVC)
  target_compile_options(${target_name} PRIVATE /EHs /analyze:WX-)
elseif(XCODE) # (not yet tested)
  set_target_properties(${target_name} PROPERTIES
  CLANG_CXX_LIBRARY libc++
  MACOSX_DEPLOYMENT_TARGET 10.7
  GCC_ENABLE_CPP_EXCEPTIONS YES
  )
else() # gcc (not yet tested)
  target_compile_options(${target_name} PRIVATE -fexceptions)
endif(MSVC)

# add post-build command to copy the file (for the compatibility with the conventional build routine)
set(EXPECTED_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}")
add_custom_command(TARGET ${target_name} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory ${EXPECTED_BINARY_DIR}
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target_name}> ${EXPECTED_BINARY_DIR})

###########################

# Build a shared library with exception disabled
set(target_name "binding_noexcept")
add_library(${target_name} SHARED ${CC_SOURCES})
set_target_properties(${target_name} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(${target_name} PRIVATE ${NodeJS_INCLUDE_DIRS})
target_link_libraries(${target_name} ${NodeJS_LIBRARIES})
target_compile_definitions(${target_name} PRIVATE "NAPI_DISABLE_CPP_EXCEPTIONS" "NAPI_DATA_VIEW_FEATURE")
# platform dependent options
if (MSVC)
  target_compile_options(${target_name} PRIVATE /EHr /analyze:WX-)
elseif(XCODE) # (not yet tested)
  set_target_properties(${target_name} PROPERTIES
  CLANG_CXX_LIBRARY libc++
  MACOSX_DEPLOYMENT_TARGET 10.7
  GCC_ENABLE_CPP_EXCEPTIONS NO
  )
else() # gcc (not yet tested)
  target_compile_options(${target_name} PRIVATE -fno-exceptions)
endif(MSVC)

# add post-build command to copy the file (for the compatibility with the conventional build routine)
set(EXPECTED_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}")
add_custom_command(TARGET ${target_name} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E make_directory ${EXPECTED_BINARY_DIR}
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target_name}> ${EXPECTED_BINARY_DIR})
