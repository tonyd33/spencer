vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO tonyd33/modlib
    REF 0979a1e7738bf7c8c4a279ec72f5b6191554470e
    SHA512 0764202ab9a894f1336f0bed6c72c6a3c688c77fe7b9c23e24221a18c5d0d9fcd14a273f6ba82839db32c29a680d7be1cfa74ccc9e7a0d49a36cd8643e0608a4
    HEAD_REF master
)

# this is deprecated in favor of vcpkg_msbuild_install (these names give me an aneurysm) but the new one isn't linking
# stuff correctly and i've spent too long figuring this build system out so goodbye
vcpkg_install_msbuild(
  USE_VCPKG_INTEGRATION # i'm braindead for not seeing this. it's required for installing the sub-dependency...
  SOURCE_PATH "${SOURCE_PATH}"
  PROJECT_SUBPATH "modlib.sln"
  PLATFORM x64
  LICENSE_SUBPATH "LICENSE.txt"
)

# vcpkg_msbuild_install(
#   SOURCE_PATH "${SOURCE_PATH}"
#   PROJECT_SUBPATH "modlib/modlib.vcxproj"
#   PLATFORM x64
# )

vcpkg_copy_pdbs()
file(INSTALL "${SOURCE_PATH}/modlib/" DESTINATION "${CURRENT_PACKAGES_DIR}/include/${PORT}" FILES_MATCHING PATTERN "*.h*")

# configure_file("${CMAKE_CURRENT_LIST_DIR}/usage" "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage" COPYONLY)