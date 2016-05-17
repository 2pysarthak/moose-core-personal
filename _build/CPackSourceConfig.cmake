# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


SET(CPACK_ARCHIVE_COMPONENT_INSTALL "ON")
SET(CPACK_BINARY_BUNDLE "")
SET(CPACK_BINARY_CYGWIN "")
SET(CPACK_BINARY_DEB "")
SET(CPACK_BINARY_DRAGNDROP "")
SET(CPACK_BINARY_NSIS "")
SET(CPACK_BINARY_OSXX11 "")
SET(CPACK_BINARY_PACKAGEMAKER "")
SET(CPACK_BINARY_RPM "")
SET(CPACK_BINARY_STGZ "")
SET(CPACK_BINARY_TBZ2 "")
SET(CPACK_BINARY_TGZ "")
SET(CPACK_BINARY_TZ "")
SET(CPACK_BINARY_WIX "")
SET(CPACK_BINARY_ZIP "")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE "ON")
SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
SET(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "")
SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ";;")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "openmpi-bin
    , python-matplotlib
    , python-qt4, openscenegraph
    , python-tornado, python-suds, python-nose, python-setuptools
    , libxml2")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dilawar Singh <dilawars@ncbs.res.in>")
SET(CPACK_GENERATOR "TGZ;TBZ2;TZ")
SET(CPACK_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp$;\\.#;/#")
SET(CPACK_INSTALLED_DIRECTORIES "/home/sarthak/Work/moose-core;/")
SET(CPACK_INSTALL_CMAKE_PROJECTS "")
SET(CPACK_INSTALL_PREFIX "/usr/local")
SET(CPACK_MODULE_PATH "/home/sarthak/Work/moose-core/cmake_modules")
SET(CPACK_NSIS_DISPLAY_NAME "moose 3.0.1")
SET(CPACK_NSIS_INSTALLER_ICON_CODE "")
SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
SET(CPACK_NSIS_PACKAGE_NAME "moose 3.0.1")
SET(CPACK_OUTPUT_CONFIG_FILE "/home/sarthak/Work/moose-core/_build/CPackConfig.cmake")
SET(CPACK_PACKAGE_CONTACT "dilawars@ncbs.res.in")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "/home/sarthak/Work/moose-core/README.md")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MOOSE, The Neural Simulator")
SET(CPACK_PACKAGE_FILE_NAME "moose-3.0.1-Source")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "moose 3.0.1")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "moose 3.0.1")
SET(CPACK_PACKAGE_NAME "moose")
SET(CPACK_PACKAGE_RELOCATABLE "true")
SET(CPACK_PACKAGE_VENDOR "NCBS Bangalore")
SET(CPACK_PACKAGE_VERSION "3.0.1")
SET(CPACK_PACKAGE_VERSION_MAJOR "3")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(CPACK_RESOURCE_FILE_LICENSE "/usr/share/cmake-2.8/Templates/CPack.GenericLicense.txt")
SET(CPACK_RESOURCE_FILE_README "/usr/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "/usr/share/cmake-2.8/Templates/CPack.GenericWelcome.txt")
SET(CPACK_RPM_PACKAGE_ARCHITECTURE "")
SET(CPACK_RPM_PACKAGE_REQUIRES "openmpi-devel python-matplotlib-qt4 python-qt4  python-tornado python-suds python-nose python-setuptools libxml2")
SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "")
SET(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "")
SET(CPACK_RPM_SPEC_INSTALL_POST "/bin/true")
SET(CPACK_RPM_SPEC_MORE_DEFINE "%define ignore #")
SET(CPACK_RPM_USER_FILELIST "%ignore /;%ignore /usr;%ignore /usr/share;%ignore /usr/share/applications;%ignore /tmp")
SET(CPACK_SET_DESTDIR "OFF")
SET(CPACK_SOURCE_CYGWIN "")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;TZ")
SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp$;\\.#;/#")
SET(CPACK_SOURCE_INSTALLED_DIRECTORIES "/home/sarthak/Work/moose-core;/")
SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/home/sarthak/Work/moose-core/_build/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "moose-3.0.1-Source")
SET(CPACK_SOURCE_TBZ2 "ON")
SET(CPACK_SOURCE_TGZ "ON")
SET(CPACK_SOURCE_TOPLEVEL_TAG "Linux-Source")
SET(CPACK_SOURCE_TZ "ON")
SET(CPACK_SOURCE_ZIP "OFF")
SET(CPACK_STRIP_FILES "")
SET(CPACK_SYSTEM_NAME "Linux")
SET(CPACK_TOPLEVEL_TAG "Linux-Source")
SET(CPACK_WIX_SIZEOF_VOID_P "8")
