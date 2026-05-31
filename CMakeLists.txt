cmake_minimum_required(VERSION 3.19)
project(SortPhoto2 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(SortPhoto2
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    sortingfiles.h sortingfiles.cpp
)

target_include_directories(SortPhoto2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/exiv2-mingw/include
)

target_link_directories(SortPhoto2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/exiv2-mingw/lib
)

target_link_libraries(SortPhoto2
    PRIVATE
        Qt::Core
        Qt::Widgets
        exiv2
)

add_custom_command(TARGET SortPhoto2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/exiv2-mingw/lib"
    $<TARGET_FILE_DIR:SortPhoto2>
)

include(GNUInstallDirs)

install(TARGETS SortPhoto2
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SortPhoto2
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
