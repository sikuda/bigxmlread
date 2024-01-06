# -------------------------
# Read big xml file project
# -------------------------
#
# method: open first level and rereading file if necessary.
#

QT           += xml \
                widgets
HEADERS       = bigxmlmainwindow.h \
                bigxmlreader.h
SOURCES       = bigxmlmain.cpp \
                bigxmlmainwindow.cpp \
                bigxmlreader.cpp

win32 {
    QMAKE_LFLAGS += -static-libgcc
}
