# Базовые настройки Qt
QT += core gui widgets network
CONFIG += c++17

# Имя проекта и выходного файла
TARGET = packet
TEMPLATE = app

# Директории для временных файлов
OBJECTS_DIR = temp/obj
MOC_DIR = temp/moc
RCC_DIR = temp/rcc
UI_DIR = temp/ui

# Исходные файлы (указываем относительные пути)
SOURCES += \
    source/main.cpp \
    source/mainwindow.cpp \
    source/tcpclient.cpp \
    source/tcpserver.cpp \
    source/udpclient.cpp \
    source/udpserver.cpp

# Заголовочные файлы
HEADERS += \
    include/mainwindow.h \
    include/tcpclient.h \
    include/tcpserver.h \
    include/udpclient.h \
    include/udpserver.h

# UI-файлы из папки ui
FORMS += \
    ui/mainwindow.ui

# Путь к заголовкам (чтобы #include работал без полного пути)
INCLUDEPATH += include


# Настройки для Debug/Release
CONFIG(debug, debug|release) {
    DESTDIR = debug
    DEFINES += DEBUG
}

CONFIG(release, debug|release) {
    DESTDIR = release
    DEFINES += NDEBUG
}
