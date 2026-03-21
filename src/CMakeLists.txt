add_subdirectory(core)
add_subdirectory(ui)

qt_add_executable(AstroFind
    main.cpp
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

# i18n — requires qt6-linguist (dnf install qt6-linguist)
if(Qt6LinguistTools_FOUND)
    qt_add_translations(AstroFind
        TS_FILES
            ${CMAKE_SOURCE_DIR}/i18n/astrofind_en.ts
            ${CMAKE_SOURCE_DIR}/i18n/astrofind_pt_BR.ts
        QM_FILES_OUTPUT_VARIABLE QM_FILES
    )
    message(STATUS "Qt6 LinguistTools found — translations will be compiled")
else()
    message(WARNING "Qt6 LinguistTools not found — translations disabled. Install: dnf install qt6-linguist")
endif()

target_link_libraries(AstroFind PRIVATE
    astrofind_ui
    astrofind_core
    Qt6::Core
    Qt6::Widgets
)

# Apply linker hardening flags to the executable
target_link_options(AstroFind PRIVATE ${ASTROFIND_HARDEN_LINK})

set_target_properties(AstroFind PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE    TRUE
)

# ── Linux install: binary + .desktop + icons ──────────────────────────────────
if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)

    install(TARGETS AstroFind
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(FILES ${CMAKE_SOURCE_DIR}/astrofind.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )

    foreach(SIZE 16 32 48 64 128 256 512)
        install(
            FILES ${CMAKE_SOURCE_DIR}/resources/icons/astrofind-${SIZE}.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${SIZE}x${SIZE}/apps
            RENAME astrofind.png
        )
    endforeach()
endif()
