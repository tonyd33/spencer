#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bddisasm::bddisasm" for configuration "Release"
set_property(TARGET bddisasm::bddisasm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bddisasm::bddisasm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/bddisasm.lib"
  )

list(APPEND _cmake_import_check_targets bddisasm::bddisasm )
list(APPEND _cmake_import_check_files_for_bddisasm::bddisasm "${_IMPORT_PREFIX}/lib/bddisasm.lib" )

# Import target "bddisasm::bdshemu" for configuration "Release"
set_property(TARGET bddisasm::bdshemu APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bddisasm::bdshemu PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/bdshemu.lib"
  )

list(APPEND _cmake_import_check_targets bddisasm::bdshemu )
list(APPEND _cmake_import_check_files_for_bddisasm::bdshemu "${_IMPORT_PREFIX}/lib/bdshemu.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
