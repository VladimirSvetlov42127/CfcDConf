include(common.pri)

TEMPLATE = app

DESTDIR = $${BIN_PATH}

unix: QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/$${LIBS_PATH}/
