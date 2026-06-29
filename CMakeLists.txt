cmake_minimum_required(VERSION 3.16)
project(clean-my-keyboard
    VERSION 1.0.0
    DESCRIPTION "Disable keyboard input for cleaning"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(UDEV REQUIRED IMPORTED_TARGET libudev)

set(GUI_SOURCES
    src/main.cpp
    src/logger.cpp
    src/keyboard_service.cpp
    src/app.cpp
    src/window.cpp
    src/polkit_backend.cpp
    src/signals.cpp
    src/resources.cpp
)

# Helper sources
set(HELPER_SOURCES
    src/helper_main.cpp
    src/logger.cpp
    src/keyboard_device.cpp
    src/device_validator.cpp
    src/device_manager.cpp
)

# Build GUI
add_executable(${PROJECT_NAME} ${GUI_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE
    PkgConfig::GTK4
)

# Build Helper
add_executable(${PROJECT_NAME}-helper ${HELPER_SOURCES})
target_include_directories(${PROJECT_NAME}-helper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}-helper PRIVATE
    PkgConfig::GLIB
    PkgConfig::UDEV
)

# Must come before compile definitions that use CMAKE_INSTALL_DATADIR
include(GNUInstallDirs)

# Define helper path and configure policy file
set(HELPER_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}/clean-my-keyboard-helper")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/packaging/clean-my-keyboard.policy.in"
    "${CMAKE_CURRENT_BINARY_DIR}/clean-my-keyboard.policy"
    @ONLY
)

# Resource paths: try installed first, fall back to source tree
target_compile_definitions(${PROJECT_NAME} PRIVATE
    RESOURCE_DIR="${CMAKE_SOURCE_DIR}/resources"
    INSTALLED_RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/clean-my-keyboard"
    HELPER_PATH="${HELPER_PATH}"
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS ${PROJECT_NAME}-helper
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}
)

install(FILES packaging/clean-my-keyboard.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/clean-my-keyboard.policy"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/polkit-1/actions
)

install(FILES
    resources/style.css
    DESTINATION ${CMAKE_INSTALL_DATADIR}/clean-my-keyboard
)

install(DIRECTORY resources/icons/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/clean-my-keyboard/icons
)

install(FILES resources/keyboard-enabled.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    RENAME clean-my-keyboard.svg
)
