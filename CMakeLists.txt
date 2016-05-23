cmake_minimum_required(VERSION 3.0)
project(vorbis)

include(cmake/HunterGate.cmake)
HunterGate(
	URL "https://github.com/ruslo/hunter/archive/v0.14.23.tar.gz"
	SHA1 "999F07D004649F5F549953F74F21E46B48D53931"
)

# Required modules
include(GNUInstallDirs)
include(CheckIncludeFiles)

# Build options
# option(BUILD_SHARED_LIBS "Build shared library" OFF)
if(APPLE)
    option(BUILD_FRAMEWORK "Build Framework bundle for OSX" OFF)
endif()

if(BUILD_FRAMEWORK)
	set(BUILD_SHARED_LIBS TRUE)
endif()

# Extract project version from configure.ac
file(READ configure.ac CONFIGURE_AC_CONTENTS)
string(REGEX MATCH "AC_INIT\\(\\[libvorbis\\],\\[([0-9]*).([0-9]*).([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# Helper function to get version-info
function(get_version_info result current_var_name age_var_name revision_var_name)
    string(REGEX MATCH "${current_var_name}=([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
    set(VERSION_INFO_CURRENT ${CMAKE_MATCH_1})

    string(REGEX MATCH "${age_var_name}=([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
    set(VERSION_INFO_AGE ${CMAKE_MATCH_1})

    string(REGEX MATCH "${revision_var_name}=([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
    set(VERSION_INFO_REVISION ${CMAKE_MATCH_1})

    math(EXPR VERSION_INFO_CURRENT_MINUS_AGE "${VERSION_INFO_CURRENT} - ${VERSION_INFO_AGE}")

    set(${result} "${VERSION_INFO_CURRENT_MINUS_AGE}.${VERSION_INFO_AGE}.${VERSION_INFO_REVISION}" PARENT_SCOPE)
endfunction()

# Helper function to configure pkg-config files
function(configure_pkg_config_file pkg_config_file_in)
    set(prefix ${CMAKE_INSTALL_PREFIX})
    set(exec_prefix ${CMAKE_INSTALL_FULL_BINDIR})
    set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
    set(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
    set(VERSION ${PROJECT_VERSION})
    string(REPLACE ".in" "" pkg_config_file ${pkg_config_file_in})
    configure_file(${pkg_config_file_in} ${pkg_config_file} @ONLY)
endfunction()

message(STATUS "Configuring ${PROJECT_NAME} ${PROJECT_VERSION}")

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(target_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Find ogg dependency
if(DEFINED HUNTER_ENABLED AND HUNTER_ENABLED)
	hunter_add_package(libogg)
	find_package(libogg CONFIG REQUIRED)
else
	if(NOT OGG_ROOT)
		find_package(PkgConfig QUIET)
		pkg_check_modules(PC_OGG QUIET ogg)
		find_path(OGG_INCLUDE_DIRS NAMES ogg/ogg.h HINTS ${PC_OGG_INCLUDE_DIRS} PATH_SUFFIXES ogg)
		find_library(OGG_LIBRARIES NAMES ogg HINTS ${PC_OGG_LIBRARY_DIRS})
	else()
		find_path(OGG_INCLUDE_DIRS NAMES ogg/ogg.h HINTS ${OGG_ROOT}/include PATH_SUFFIXES ogg)
		find_library(OGG_LIBRARIES NAMES ogg HINTS ${OGG_ROOT}/lib ${OGG_ROOT}/lib64)
	endif()
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(OGG DEFAULT_MSG OGG_INCLUDE_DIRS OGG_LIBRARIES)
endif()

add_subdirectory(lib)

# install
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${version_config}" COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"cmake/Config.cmake.in"
	"${project_config}"
	INSTALL_DESTINATION "${config_install_dir}"
)

configure_pkg_config_file(vorbis.pc.in)
configure_pkg_config_file(vorbisenc.pc.in)
configure_pkg_config_file(vorbisfile.pc.in)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vorbis.pc
    ${CMAKE_CURRENT_BINARY_DIR}/vorbisenc.pc
    ${CMAKE_CURRENT_BINARY_DIR}/vorbisfile.pc
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
	FILES
	"${project_config}"
	"${version_config}"
	DESTINATION
	"${config_install_dir}"
)

install(
	EXPORT
	"${target_export_name}"
	NAMESPACE
	"${namespace}"
	DESTINATION
	"${config_install_dir}"
)

