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
pkg_check_modules(UDEV REQUIRED IMPORTED_TARGET libudev)

set(SOURCES
    src/main.cpp
    src/logger.cpp
    src/keyboard_service.cpp
    src/app.cpp
    src/window.cpp
    src/keyboard_device.cpp
    src/device_validator.cpp
    src/device_manager.cpp
    src/evdev_backend.cpp
    src/udev_monitor.cpp
    src/signals.cpp
    src/resources.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    PkgConfig::GTK4
    PkgConfig::UDEV
)

# Must come before compile definitions that use CMAKE_INSTALL_DATADIR
include(GNUInstallDirs)

# Resource paths: try installed first, fall back to source tree
target_compile_definitions(${PROJECT_NAME} PRIVATE
    RESOURCE_DIR="${CMAKE_SOURCE_DIR}/resources"
    INSTALLED_RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/clean-my-keyboard"
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES packaging/clean-my-keyboard.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES
    resources/keyboard-enabled.svg
    resources/keyboard-disabled.svg
    resources/style.css
    DESTINATION ${CMAKE_INSTALL_DATADIR}/clean-my-keyboard
)

install(FILES resources/keyboard-enabled.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    RENAME clean-my-keyboard.svg
)

