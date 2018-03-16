#.rst:
# FindNodeJS
# -------
# Find Node.js and its native APIs
#
# The module supports the following components:
#
#    NODE_RUNTIME     - Node.js runtime (default)
#    ELECTRON_RUNTIME - Electron runtime
#    NW_RUNTIME       - Webpack runtime
#    NAN              - nan module: Native Abstractions for Node.js 
#    NAPI             - node-addon-api module: N-API
#
# Only 1 Runtime Component (NODE_RUNTIME, ELECTRON_RUNTIME, or NW_RUNTIME) may be specified.
#
# When first configured, a set of files will be downloaded for the specified runtime library. The downloaded
# files are stored in the build directory by default. `NodeJS_RUNTIME_ROOT_DIR` may be specified in order 
# to give the path to store the downloaded runtime.
#
# Module Input Variables
# ^^^^^^^^^^^^^^^^^^^^^^
#
# Users or projects may set the following variables to configure the module
# behaviour:
#
# :variable:`NodeJS_RUNTIME_ROOT_DIR`
#   the root of the runtime installations.
#
# Variables defined by the module
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# Result variables
# """"""""""""""""
# ``NodeJS_FOUND``
#   ``TRUE`` if the NodeJS installation and runtime files are found, ``FALSE``
#   otherwise. All variable below are defined if NodeJS is found.
# ``NodeJS_EXECUTABLE`` (Cached)
#   Path to the Node.js binary program.
# ``NodeJS_INCLUDE_DIRS`` (Cached)
#  the pathes of the Node.js libraries' headers
# ``NodeJS_LIBRARIES`` (Cached)
#   the whole set of libraries of Node.js
# ``NodeJS_RUNTIME_LIBRARY`` (Cached)
#   library for the selected runtime
# ``NodeJS_RUNTIME_INCLUDE_DIR`` (Cached)
#   include path for the selected runtime headers
# ``NodeJS_UV_INCLUDE_DIR`` (Cached)
#   include path for the libuv library headers (Available only if the component 
#   ``ELECTRON_RUNTIME`` or ``NW_RUNTIME`` is requested)
# ``NodeJS_V8_INCLUDE_DIR`` (Cached)
#   include path for the V8 library headers (Available only if the component 
#   ``ELECTRON_RUNTIME`` or ``NW_RUNTIME`` is requested)
# ``NodeJS_Electron_DIR`` (Cached)
#   path to Node.js electron module (Available only if the component 
#   ``ELECTRON_RUNTIME`` is requested)
# ``NodeJS_Electron_VERSION`` (Cached)
#   installed electron version (Available only if the component 
#   ``ELECTRON_RUNTIME`` is requested)
# ``NodeJS_NW_DIR`` (Cached)
#   path to Node.js webpack module (Available only if the component 
#   ``NW_RUNTIME`` is requested)
# ``NodeJS_NW_VERSION`` (Cached)
#   installed webpack version
# ``NodeJS_NW_LIBRARY`` (Cached)
#   NW library
# ``NodeJS_NAN_FOUND``
#   ``TRUE`` if the NodeJS NAN package is found, ``FALSE``
#   otherwise. Variables NodeJS_NAN_XXX below are defined if NAN is found.
# ``NodeJS_NAN_INCLUDE_DIR`` (Cached)
#   include path for NAN module headers (Available only if the component NAN is requested)
# ``NodeJS_NAN_VERSION`` (Cached)
#   version of NAN module (Available only if the component NAN is requested)
# ``NodeJS_NAPI_FOUND``
#   ``TRUE`` if the NodeJS node-addon-api package is found, ``FALSE``
#   otherwise. All variable NodeJS_NAPI_XXX below are defined if NAPI is found.
# ``NodeJS_NAPI_INCLUDE_DIRS`` (Cached)
#   include path for N-API module headers (Available only if the component NAPI is requested)
# ``NodeJS_NAPI_VERSION`` (Cached)
#   version of node-addon-api module headers (Available only if the component NAPI is requested)
#
# Provided functions
# ^^^^^^^^^^^^^^^^^^
#
# :command:`FindNodePackage`
#   run npm to find an installed package. If success, returns directory and version info.
# :command:`FindNodeJSRuntime`
#   look for runtime files in `NodeJS_RUNTIME_ROOT_DIR`; if not found, download the files.

# detect NodeJS (which should be on environment path)
if (NOT (NodeJS_EXECUTABLE AND NodeJS_VERSION))
  find_program(NodeJS_EXECUTABLE node)
  get_filename_component(NodeJS_BIN_DIR ${NodeJS_EXECUTABLE} DIRECTORY)

  # parse the version
  execute_process(COMMAND "${NodeJS_EXECUTABLE}" --version OUTPUT_VARIABLE NodeJS_VERSION)
  string(STRIP ${NodeJS_VERSION} NodeJS_VERSION)
  string(SUBSTRING ${NodeJS_VERSION} 1 -1 NodeJS_VERSION)
  set(NodeJS_VERSION ${NodeJS_VERSION} CACHE INTERNAL "version of Node.js binary")
endif (NOT (NodeJS_EXECUTABLE AND NodeJS_VERSION))

# Get native target architecture
include(CheckSymbolExists)
if(WIN32)
  check_symbol_exists("_M_AMD64" "" RTC_ARCH_X64)
  if(NOT RTC_ARCH_X64)
    check_symbol_exists("_M_IX86" "" RTC_ARCH_X86)
    if (NOT RTC_ARCH_X86)
      check_symbol_exists("_M_ARM" "" RTC_ARCH_ARM)
    endif()
  endif(NOT RTC_ARCH_X64)
  # add check for arm here
  # see http://msdn.microsoft.com/en-us/library/b0084kay.aspx
else(WIN32)
  check_symbol_exists("__x86_64__" "" RTC_ARCH_X64)
  if(NOT RTC_ARCH_X64)
    check_symbol_exists("__i386__" "" RTC_ARCH_X86)
    if (NOT RTC_ARCH_X86)
      check_symbol_exists("__arm__" "" RTC_ARCH_ARM)
    endif()
  endif(NOT RTC_ARCH_X64)
endif(WIN32)

if(RTC_ARCH_X64)
  set(ARCH_STR x64)
elseif(RTC_ARCH_X86)
  set(ARCH_STR x86)
elseif(RTC_ARCH_ARM)
  set(ARCH_STR ARM)
else()
  message(FATAL_ERROR "Unknown architecture")
endif()

# set command options to run npm
if (WIN32)
  set(NPM "cmd")
  list(APPEND NPM_ARGS "/c" npm)
else(WIN32)
  set(NPM "npm")
  list(LENGTH APPEND)
endif(WIN32)

# define the function to look for an installed node package (runs npm)
function(FindNodePackage dir ver pkg)
  
  message ("Looking for Node.js package: ${pkg}")

  # check the local node_modules first
  execute_process(COMMAND ${NPM} ${NPM_ARGS} list ${pkg} OUTPUT_VARIABLE list_output 
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}) # local install
  string(STRIP ${list_output} list_output)
  string(FIND ${list_output} "\n" pos REVERSE)
  if (pos EQUAL -1)
    message(FATAL_ERROR "expects 2 output lines from \"npm list pkg\"")
  endif()
  math(EXPR pos "${pos}+1")
  string(SUBSTRING ${list_output} ${pos} -1 pkg_info)
  string(FIND ${pkg_info} "@" at_pos)
  if (at_pos EQUAL -1) # not found, try global
    execute_process(COMMAND ${NPM} ${NPM_ARGS} list ${pkg} -g OUTPUT_VARIABLE list_output 
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}) # global install
    string(STRIP ${list_output} list_output)
    string(FIND ${list_output} "\n" pos REVERSE)
    if (pos EQUAL -1)
      message(FATAL_ERROR "expects 2 output lines from \"npm list pkg -g\"")
    endif()
    math(EXPR pos "${pos}+1")
    string(SUBSTRING ${list_output} ${pos} -1 pkg_info)
    string(FIND ${pkg_info} "@" at_pos)

    if (at_pos EQUAL -1) # not found, try current directory
      execute_process(COMMAND ${NPM} ${NPM_ARGS} list OUTPUT_VARIABLE list_output 
                      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}) # global install
      string(STRIP ${list_output} list_output)
      string(FIND ${list_output} "\n" pos REVERSE)
      if (pos EQUAL -1)
        message(FATAL_ERROR "expects 2 output lines from \"npm list\"")
      endif()
      math(EXPR pos "${pos}+1")
      string(SUBSTRING ${list_output} ${pos} -1 pkg_info)
      string(FIND ${pkg_info} "@" at_pos)
      set(not_pkg TRUE)
    endif()
  endif()

  if (NOT (at_pos EQUAL -1))
    # retrieve the version string
    math(EXPR at_pos "${at_pos}+1")
    string(SUBSTRING ${pkg_info} ${at_pos} -1 ver_)
    set(${ver} ${ver_} PARENT_SCOPE)
  
    # first line contains the path
    string(FIND ${list_output} " " pos)
    math(EXPR pos "${pos}+1")
    string(FIND ${list_output} "\n" len)
    math(EXPR len ${len}-${pos})
    string(SUBSTRING ${list_output} ${pos} ${len} dir_) # path to the parent of node_modules
    file(TO_CMAKE_PATH ${dir_} dir_)
    if (NOT not_pkg)
      string (CONCAT dir_ ${dir_} "/node_modules/${pkg}")
    endif (NOT not_pkg)
    set(${dir} ${dir_} PARENT_SCOPE)
  endif()
endfunction(FindNodePackage)

# separate requested components into runtime & library
foreach(component IN LISTS NodeJS_FIND_COMPONENTS)
  if (component MATCHES .+_RUNTIME)
    list(APPEND runtime_component ${component})
  else ()
    list(APPEND library_components ${component})
  endif()
endforeach(component IN LISTS NodeJS_FIND_COMPONENTS)

# only 1 RUNTIME component maybe requested
if (runtime_component)
  list(LENGTH runtime_component num_runtime)
  if (${num_runtime} GREATER 1)
    message(FATAL_ERROR "Cannot set multiple RUNTIME components.")
  endif()
  list(GET runtime_component 0 runtime_component)
else()
  set(runtime_component "NODE_RUNTIME") # default runtime
endif()

# If not specified by user, create root directory to store runtime headers and .lib files 
set(NodeJS_RUNTIME_ROOT_DIR "${CMAKE_BINARY_DIR}" CACHE PATH "Root directory of where to load/save node runtime")
if (NOT IS_DIRECTORY ${NodeJS_RUNTIME_ROOT_DIR})
  message(FATAL_ERROR "NodeJS_RUNTIME_ROOT_DIR is not a valid directory (${NodeJS_RUNTIME_ROOT_DIR}).")
endif()

# FUNCTION to detect & download a runtime library
#   Accesses: NodeJS_RUNTIME_ROOT_DIR
function (FindNodeJSRuntime base incl_suffix headerurl winliburl)
  # check if the runtime header files have been downloaded already
  set(incl_hint "${NodeJS_RUNTIME_ROOT_DIR}/${base}/${incl_suffix}")
  find_path (NodeJS_RUNTIME_INCLUDE_DIR node.h
             HINTS ${incl_hint}
             DOC "Include directory of Node runtime library"
             NO_DEFAULT_PATH)

  # if not available, download and unpack the header files
  if (NOT NodeJS_RUNTIME_INCLUDE_DIR)
    get_filename_component(DL_DST ${headerurl} NAME)
    set(DL_DST "${CMAKE_BINARY_DIR}/${DL_DST}")
    
    message("Downloading runtime header files from\n\t${headerurl}")

    file(DOWNLOAD ${headerurl} ${DL_DST})
    execute_process(COMMAND "cmake" -E tar xzf "${DL_DST}" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}") # unpack root
    find_path (NodeJS_RUNTIME_INCLUDE_DIR node.h
              HINTS ${NodeJS_RUNTIME_ROOT_DIR}
              PATH_SUFFIXES "${base}/${incl_suffix}"
              DOC "Include directory of Node runtime library"
              NO_DEFAULT_PATH)
  endif()

  # find the runtime library file
  set(lib_hint "${NodeJS_RUNTIME_ROOT_DIR}/${base}/lib/win-${ARCH_STR}")
  find_library (NodeJS_RUNTIME_LIBRARY node HINTS ${lib_hint} DOC "Node runtime library")

  # if not available additionally download .lib file for windows build
  if (WIN32 AND NOT NodeJS_RUNTIME_LIBRARY)
    message("Downloading runtime library file from \n\t${winliburl}")
    file(DOWNLOAD ${winliburl} "${lib_hint}/node.lib")
    find_library (NodeJS_RUNTIME_LIBRARY node HINTS ${lib_hint} DOC "Node runtime library")
  endif ()
endfunction(FindNodeJSRuntime)

# Find the include & library for the specified runtime
if (runtime_component STREQUAL NODE_RUNTIME)

  if (NodeJS_VERSION VERSION_LESS "4.0.0")
    message(FATAL_ERROR "Node.js version earlier than v4.0.0 is not supported.")
  endif(NodeJS_VERSION VERSION_LESS "4.0.0")

  # Create root directory in build directory (name=packed directory name)
  FindNodeJSRuntime("node-v${NodeJS_VERSION}" # expected top directory name of headers.tar.gz
                    "include/node" 
                    "https://nodejs.org/dist/v${NodeJS_VERSION}/node-v${NodeJS_VERSION}-headers.tar.gz" 
                    "https://nodejs.org/dist/v${NodeJS_VERSION}/win-${ARCH_STR}/node.lib")

elseif (runtime_component STREQUAL ELECTRON_RUNTIME)

  if (NOT NodeJS_Electron_DIR) 
    # electron package must be installed (only search once)
    FindNodePackage(dir_ ver_ electron)
    set(NodeJS_Electron_DIR ${dir_} CACHE PATH "Electron Node Module Directory")
    set(NodeJS_Electron_VERSION ${ver_} CACHE INTERNAL "Electron Node Module Version")
  endif(NOT NodeJS_Electron_DIR)

  if (RTC_ARCH_X86)
    set(winliburl "https://atom.io/download/atom-shell/v${NodeJS_Electron_VERSION}/node.lib")
  else()
    set(winliburl "https://atom.io/download/atom-shell/v${NodeJS_Electron_VERSION}/${ARCH_STR}/node.lib")
  endif(RTC_ARCH_X86)
  set(base_ "node-v${NodeJS_Electron_VERSION}")
  FindNodeJSRuntime(${base_} 
                    "src" 
                    "https://atom.io/download/atom-shell/v${NodeJS_Electron_VERSION}/node-v${NodeJS_Electron_VERSION}.tar.gz" 
                    ${winliburl})

  if (NodeJS_RUNTIME_INCLUDE_DIR)
    # v8.h and uv.h files are given in deps directory
    set(dir_ "${NodeJS_RUNTIME_ROOT_DIR}/${base_}/deps")
    find_path (NodeJS_V8_INCLUDE_DIR v8.h
              HINTS "${dir_}/v8/include"
              DOC "Include directory of NodeJS v8 headers"
              NO_DEFAULT_PATH)
    find_path (NodeJS_UV_INCLUDE_DIR uv.h
              HINTS "${dir_}/uv/include"
              PATH_SUFFIXES "${base}/${incl_suffix}"
              DOC "Include directory of NodeJS uv headers"
              NO_DEFAULT_PATH)
  endif (NodeJS_RUNTIME_INCLUDE_DIR)
  
elseif (runtime_component STREQUAL NW_RUNTIME)

  if (NOT NodeJS_NW_DIR) 
    # nw package must be installed (only search once)
    FindNodePackage(dir_ ver_ nw)
    set(NodeJS_NW_DIR ${dir_} CACHE PATH "NW Module Directory")
    set(NodeJS_NW_VERSION ${ver_} CACHE INTERNAL "NW Module Version")
  endif(NOT NodeJS_NW_DIR)

  if (NodeJS_NW_VERSION VERSION_LESS "0.13.0")
    message(FATAL_ERROR "Node.js version earlier than v0.13.0 is not supported.")
  endif(NodeJS_NW_VERSION VERSION_LESS "0.13.0")

  if (RTC_ARCH_X86)
    set(winliburl "https://node-webkit.s3.amazonaws.com/v${NodeJS_NW_VERSION}/node.lib")
  else()
    set(winliburl "https://node-webkit.s3.amazonaws.com/v${NodeJS_NW_VERSION}/${ARCH_STR}/node.lib")
  endif(RTC_ARCH_X86)
  set(base_ "node")
  FindNodeJSRuntime(${base_} 
                    "src" 
                    "https://node-webkit.s3.amazonaws.com/v${NodeJS_NW_VERSION}/nw-headers-v${NodeJS_NW_VERSION}.tar.gz" 
                    ${winliburl})

  # also need nw.lib
  if (WIN32)
    set(lib_hint "${NodeJS_RUNTIME_ROOT_DIR}/${base}/lib/win-${ARCH_STR}")
    find_library (NodeJS_NW_LIBRARY nw HINTS ${lib_hint} DOC "NW runtime library")
    if (NOT NodeJS_NW_LIBRARY)
      if (RTC_ARCH_X86)
        set(winliburl "https://node-webkit.s3.amazonaws.com/v${NodeJS_NW_VERSION}/nw.lib")
      else()
        set(winliburl "https://node-webkit.s3.amazonaws.com/v${NodeJS_NW_VERSION}/${ARCH_STR}/nw.lib")
      endif(RTC_ARCH_X86)
      file(DOWNLOAD ${winliburl} "${lib_hint}/nw.lib")
      find_library (NodeJS_NW_LIBRARY nw HINTS ${lib_hint} DOC "NW runtime library")
    endif (NOT NodeJS_NW_LIBRARY)
  endif(WIN32)

  # v8.h and uv.h files are given in deps directory
  if (NodeJS_RUNTIME_INCLUDE_DIR)
    set(dir_ "${NodeJS_RUNTIME_ROOT_DIR}/${base_}/deps")
    find_path (NodeJS_V8_INCLUDE_DIR v8.h
              HINTS "${dir_}/v8/include"
              DOC "Include directory of NodeJS v8 headers"
              NO_DEFAULT_PATH)
    find_path (NodeJS_UV_INCLUDE_DIR uv.h
              HINTS "${dir_}/uv/include"
              PATH_SUFFIXES "${base}/${incl_suffix}"
              DOC "Include directory of NodeJS uv headers"
              NO_DEFAULT_PATH)
  endif (NodeJS_RUNTIME_INCLUDE_DIR)
    
else()
  message(FATAL_ERROR "${runtime_component} is not a valid or supported RUNTIME component.")
endif (runtime_component STREQUAL NODE_RUNTIME)

# all is good if both include & library
if ((EXISTS ${NodeJS_RUNTIME_INCLUDE_DIR}) AND (EXISTS ${NodeJS_RUNTIME_LIBRARY}))
  set("NodeJS_${runtime_component}_FOUND" TRUE)
else()
  set("NodeJS_${runtime_component}_FOUND" FALSE)
endif()

# Check for NAN
if (NAN IN_LIST library_components)
  if (NOT NodeJS_NAN_INCLUDE_DIR)
    # check the local modules first
    FindNodePackage(dir_ ver_ nan)
    set(NodeJS_NAN_INCLUDE_DIR ${dir_} CACHE PATH "NAN Node Module Directory")
    set(NodeJS_NAN_VERSION ${ver_} CACHE INTERNAL "NAN Node Module Version")
  endif()
  if (NodeJS_NAN_INCLUDE_DIR)
    set(NodeJS_NAN_FOUND TRUE)
  else()
    set(NodeJS_NAN_FOUND FALSE)
  endif()
endif (NAN IN_LIST library_components)

# Check for NAPI
if (NAPI IN_LIST library_components)
  if (NOT NodeJS_NAPI_INCLUDE_DIRS)
    # check the local modules first
    FindNodePackage(dir_ ver_ "node-addon-api")

    # only support Node.js newer than v8.5
    if (NodeJS_VERSION VERSION_LESS_EQUAL "8.5.0")
      message(FATAL_ERROR "Node.js version 8.5.0 or earlier is not supported.")
    endif (NodeJS_VERSION VERSION_LESS_EQUAL "8.5.0")

    set(NodeJS_NAPI_VERSION ${ver_} CACHE INTERNAL "N-API Node Module Version")
    if (dir_)
      # need to include 2 directories
      list(APPEND dir_ "${dir_}/external-napi")
      set(NodeJS_NAPI_INCLUDE_DIRS ${dir_} CACHE PATH "N-API Node Module Directory")
    endif (dir_)
  endif(NOT NodeJS_NAPI_INCLUDE_DIRS)

  if (NodeJS_NAPI_INCLUDE_DIRS)
    set(NodeJS_NAPI_FOUND TRUE)
  else()
    set(NodeJS_NAPI_FOUND FALSE)
  endif()
endif (NAPI IN_LIST library_components)

# check all the required components are set
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NodeJS
  FOUND_VAR NodeJS_FOUND
  REQUIRED_VARS
    NodeJS_RUNTIME_INCLUDE_DIR 
    NodeJS_RUNTIME_LIBRARY
  HANDLE_COMPONENTS
  VERSION_VAR NodeJS_VERSION
)

if(NodeJS_FOUND)
  list(APPEND NodeJS_INCLUDE_DIRS ${NodeJS_RUNTIME_INCLUDE_DIR} ${NodeJS_V8_INCLUDE_DIR} ${NodeJS_UV_INCLUDE_DIR}
                                  ${NodeJS_NAN_INCLUDE_DIR} ${NodeJS_NAPI_INCLUDE_DIRS})
  list(APPEND NodeJS_LIBRARIES ${NodeJS_RUNTIME_LIBRARY} ${NodeJS_NW_LIBRARY})
endif()

mark_as_advanced(
  NodeJS_EXECUTABLE
  NodeJS_VERSION
  NodeJS_Electron_DIR
  NodeJS_Electron_VERSION
  NodeJS_NW_DIR
  NodeJS_NW_VERSION
  NodeJS_NW_LIBRARY
  NodeJS_NAN_INCLUDE_DIR
  NodeJS_NAN_VERSION
  NodeJS_NAPI_INCLUDE_DIRS
  NodeJS_NAPI_VERSION
  NodeJS_RUNTIME_INCLUDE_DIR
  NodeJS_RUNTIME_LIBRARY
  NodeJS_UV_INCLUDE_DIR
  NodeJS_V8_INCLUDE_DIR
)

# compatibility variables
set(NodeJS_VERSION_STRING ${NodeJS_VERSION})
set(CMAKE_JS_INC ${NodeJS_INCLUDE_DIRS})
set(CMAKE_JS_LIB ${NodeJS_LIBRARIES})
