
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

find_package( Qt4 COMPONENTS REQUIRED )

find_package( KDE4 REQUIRED )

add_definitions( -I${KDE4_INCLUDE_DIR} )
add_definitions( -I${Qt4_INCLUDE_DIR} )

INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

QT4_WRAP_UI( UI logwindow.ui configuredialog.ui )

QT4_WRAP_CPP( MOC check_updates.h qtUpdateNotifier.h logwindow.h instance.h startsynaptic.h configuredialog.h checkoldpackages.h )

QT4_ADD_RESOURCES( ICONS icons.qrc )

add_executable( qt-update-notifier check_updates.cpp main.cpp qtUpdateNotifier.cpp 
                logwindow.cpp instance.cpp startsynaptic.cpp configuredialog.cpp checkoldpackages.cpp utility.cpp
                ${MOC} ${UI} ${ICONS} )

TARGET_LINK_LIBRARIES( qt-update-notifier -L${KDE4_LIB_DIR} -lkdeui ) 

set_target_properties( qt-update-notifier PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
set_target_properties( qt-update-notifier PROPERTIES LINK_FLAGS "-pie" )

add_executable( qt-update-notifier-start-synaptic qt-update-notifier-start-synaptic.c process.c )

set_target_properties( qt-update-notifier-start-synaptic PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic -std=c99" )
set_target_properties( qt-update-notifier-start-synaptic PROPERTIES LINK_FLAGS "-pie" )

TARGET_LINK_LIBRARIES( qt-update-notifier-start-synaptic -pthread ) 

install ( FILES qt-update-notifier.png DESTINATION share/icons )
install ( FILES qt-update-notifier-updating.png DESTINATION share/icons )
install ( FILES qt-update-notifier-updates-are-available.png DESTINATION share/icons )
install ( FILES qt-update-notifier-need-attention.png DESTINATION share/icons )

file( WRITE "${PROJECT_BINARY_DIR}/qt-update-install-path.h"
"#define QT_UPDATE_NOTIFIER_TRANSLATION_PATH \"${CMAKE_INSTALL_PREFIX}/share/qt-update-notifier-translations/translations.qm\""
)

file( WRITE "${PROJECT_BINARY_DIR}/qt-update-synaptic-helper.h"
"#define QT_UPDATE_NOTIFIER_HELPER_PATH \"${CMAKE_INSTALL_PREFIX}/bin/qt-update-notifier-start-synaptic\""
)

SET( TS_PATH share/qt-update-notifier-translations/translations.ts )
SET( QM_PATH share/qt-update-notifier-translations/translations.qm )

install ( FILES translations.ts/translations.ts DESTINATION ${TS_PATH} )
install ( FILES translations.ts/greek_GR.ts DESTINATION     ${TS_PATH} )
install ( FILES translations.ts/dutch_NL.ts DESTINATION     ${TS_PATH} )
install ( FILES translations.ts/german_DE.ts DESTINATION    ${TS_PATH} )

install ( FILES translations.qm/greek_GR.qm DESTINATION  ${QM_PATH} )
install ( FILES translations.qm/dutch_NL.qm DESTINATION  ${QM_PATH} )
install ( FILES translations.qm/german_DE.qm DESTINATION ${QM_PATH} )

install( TARGETS qt-update-notifier RUNTIME DESTINATION bin )

install( TARGETS qt-update-notifier-start-synaptic RUNTIME DESTINATION bin 
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

install ( SCRIPT "${CMAKE_SOURCE_DIR}/PostInstall.cmake" )

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
Path=
StartupNotify=true
Terminal=false
TerminalOptions=
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=Unique
X-KDE-SubstituteUID=false
X-KDE-Username=
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
Path=
StartupNotify=true
Terminal=false
TerminalOptions=
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=Unique
X-KDE-SubstituteUID=false
X-KDE-Username=
Categories=PackageManager;System;Application;X-MandrivaLinux-System-Configuration;X-MandrivaLinux-System-Configuration-Packaging;\n")
