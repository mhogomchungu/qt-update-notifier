
cmake_minimum_required( VERSION 3.1 )

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

project(qt-update-notifier)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

file( WRITE "${PROJECT_BINARY_DIR}/version.h" "#define VERSION \"1.7.1\"\n" )

find_package( Qt5Widgets REQUIRED )
find_package( Qt5Core REQUIRED )
find_package( Qt5Network REQUIRED )

set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories( ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} )

add_definitions( ${Qt5Widgets_DEFINITIONS} )
add_definitions( ${Qt5Network_DEFINITIONS} )
add_definitions( ${Qt5Core_DEFINITIONS} )

ADD_SUBDIRECTORY( src/3rdParty/tasks )
ADD_SUBDIRECTORY( src/3rdParty/NetworkAccessManager )
include_directories( src/3rdParty/tasks )
include_directories( src/3rdParty/NetworkAccessManager )

Qt5_WRAP_UI( UI src/logwindow.ui src/configuredialog.ui src/twitter.ui src/ignorepackagelist.ui )

Qt5_WRAP_CPP( MOC src/qtUpdateNotifier.h src/logwindow.h src/configuredialog.h src/statusicon.h src/twitter.h src/ignorepackagelist.h )

Qt5_ADD_RESOURCES( ICONS icons/icons.qrc )
if( KF5 )
        find_package( KF5Notifications REQUIRED )
        add_definitions( ${KF5Notifications_DEFINITIONS} )

        file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define KF5 1\n" )
else()
        file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define KF5 0\n" )
endif()

add_executable( qt-update-notifier src/main.cpp src/qtUpdateNotifier.cpp src/settings.cpp src/statusicon.cpp
                src/logwindow.cpp src/configuredialog.cpp src/utility.cpp src/twitter.cpp src/ignorepackagelist.cpp src/tablewidget.cpp
                ${MOC} ${UI} ${ICONS} )
if( KF5 )
        TARGET_LINK_LIBRARIES( qt-update-notifier ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} KF5::Notifications networkAccessManager tasks )
else()
        TARGET_LINK_LIBRARIES( qt-update-notifier ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} networkAccessManager tasks )
endif()

set_target_properties( qt-update-notifier PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIE -pthread -pedantic" )
set_target_properties( qt-update-notifier PROPERTIES LINK_FLAGS "-pie" )

add_executable( qt-update-notifier-cli src/qt-update-notifier-cli.c src/process.c )

set_target_properties( qt-update-notifier-cli PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIE -pthread -pedantic -std=c99" )
set_target_properties( qt-update-notifier-cli PROPERTIES LINK_FLAGS "-pie" )

TARGET_LINK_LIBRARIES( qt-update-notifier-cli -pthread )

install ( FILES icons/qt-update-notifier.png DESTINATION share/icons )
install ( FILES icons/ob-qt-update-notifier.png DESTINATION share/icons )
install ( FILES icons/qt-update-notifier-updating.png DESTINATION share/icons )
install ( FILES icons/qt-update-notifier-updates-are-available.png DESTINATION share/icons )
install ( FILES icons/qt-update-notifier-important-info.png DESTINATION share/icons )
install ( FILES icons/qt-update-notifier-inconsistent-state.png DESTINATION share/icons )

file( WRITE "${PROJECT_BINARY_DIR}/qt-update-install-path.h"
"#define QT_UPDATE_NOTIFIER_TRANSLATION_PATH \"${CMAKE_INSTALL_PREFIX}/share/qt-update-notifier-translations/translations.qm\""
)

file( WRITE "${PROJECT_BINARY_DIR}/qt-update-synaptic-helper.h"
"#define QT_UPDATE_NOTIFIER_HELPER_PATH \"${CMAKE_INSTALL_PREFIX}/bin/qt-update-notifier-cli\""
)

SET( TS_PATH share/qt-update-notifier-translations/translations.ts )
SET( QM_PATH share/qt-update-notifier-translations/translations.qm )

#install ( FILES translations.ts/translations.ts DESTINATION ${TS_PATH} )
#install ( FILES translations.ts/greek_GR.ts DESTINATION     ${TS_PATH} )
#install ( FILES translations.ts/dutch_NL.ts DESTINATION     ${TS_PATH} )
#install ( FILES translations.ts/german_DE.ts DESTINATION    ${TS_PATH} )
#install ( FILES translations.ts/czech_CS.ts DESTINATION     ${TS_PATH} )
#install ( FILES translations.ts/slovak_SK.ts DESTINATION    ${TS_PATH} )

install ( FILES translations.qm/greek_GR.qm DESTINATION  ${QM_PATH} )
install ( FILES translations.qm/dutch_NL.qm DESTINATION  ${QM_PATH} )
install ( FILES translations.qm/german_DE.qm DESTINATION ${QM_PATH} )
install ( FILES translations.qm/czech_CS.qm DESTINATION  ${QM_PATH} )
install ( FILES translations.qm/slovak_SK.qm DESTINATION ${QM_PATH} )
install ( FILES translations.qm/polish_PL.qm DESTINATION ${QM_PATH} )

install( TARGETS qt-update-notifier RUNTIME DESTINATION bin )

install( TARGETS qt-update-notifier-cli RUNTIME DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
)

file( WRITE  ${PROJECT_BINARY_DIR}/desktop_file.h "\n#define DESKTOP_FILE_PATH      \"${CMAKE_INSTALL_PREFIX}/share/applications/qt-update-notifier.desktop\"\n")

file( WRITE  ${PROJECT_BINARY_DIR}/kdesu_path.h "\n#define KDESU_PATH      \"${KDE4_LIB_DIR}/kde4/libexec/kdesu\"\n")

install ( FILES ${PROJECT_BINARY_DIR}/qt-update-notifier.desktop DESTINATION share/applications
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install ( FILES ${PROJECT_BINARY_DIR}/qt-update-notifier-autostart.desktop DESTINATION /etc/xdg/autostart/
PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY )

add_custom_target( uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/qt-update-notifier.desktop"
"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/qt-update-notifier
GenericName[en_US]=PCLinuxOS Qt based updates notifier
GenericName=PCLinuxOS Qt based update notifier
Icon=${CMAKE_INSTALL_PREFIX}/share/icons/qt-update-notifier.png
MimeType=
Name[en_US]=Qt-update-notifier
Name=Qt-update-notifier
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
Categories=PackageManager;System;Application;X-MandrivaLinux-System-Configuration;X-MandrivaLinux-System-Configuration-Packaging;\n")

file( WRITE "${PROJECT_BINARY_DIR}/qt-update-notifier-autostart.desktop"
"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/qt-update-notifier -a
GenericName[en_US]=PCLinuxOS Qt based updates notifier
GenericName=PCLinuxOS Qt based update notifier
Icon=${CMAKE_INSTALL_PREFIX}/share/icons/qt-update-notifier.png
MimeType=
Name[en_US]=Qt-update-notifier-autostart
Name=Qt-update-notifier-autostart
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
Categories=PackageManager;System;Application;X-MandrivaLinux-System-Configuration;X-MandrivaLinux-System-Configuration-Packaging;\n")
