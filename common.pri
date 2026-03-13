DTOOLS_VERSION = 0.1.1

win32: OS = win
unix: OS = linux

CONFIG(debug, debug|release) {
    BUILD = debug
} else {
    BUILD = release
}

BUILD_DIR = $${OS}/$${QT_ARCH}/qt$${QT_MAJOR_VERSION}/$${BUILD}

RCC_DIR =     $${OUT_PWD}/$${BUILD_DIR}/rcc/
UI_DIR =      $${OUT_PWD}/$${BUILD_DIR}/ui/
MOC_DIR =     $${OUT_PWD}/$${BUILD_DIR}/moc/
OBJECTS_DIR = $${OUT_PWD}/$${BUILD_DIR}/obj/

INCLUDEPATH += $${OUT_PWD}/$${BUILD_DIR}/ui/
DEPENDPATH += $${OUT_PWD}/$${BUILD_DIR}/ui/

BIN_PATH =  $${PWD}/$${BUILD_DIR}/bin/
LIBS_PATH = $${PWD}/$${BUILD_DIR}/lib/

LIBS += -L$${LIBS_PATH}/

CONFIG += c++17
