QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    approximateEntropy.c \
    assess.c \
    binaryDerivate.c \
    blockFrequency.c \
    bytesToBitSequence.c \
    cephes.c \
    cusum.c \
    detect_thread.cpp \
    detect_thread_2.cpp \
    dfft.c \
    discreteFourierTransform.c \
    erf.c \
    frequency.c \
    generators.c \
    genutils.c \
    global.cpp \
    linearComplexity.c \
    longestRunOfOnes.c \
    main.cpp \
    mainwindow.cpp \
    matrix.c \
    nonOverlappingTemplateMatchings.c \
    overlappingTemplateMatchings.c \
    pokerDetect.c \
    randomExcursions.c \
    randomExcursionsVariant.c \
    rank.c \
    runs.c \
    runsDistribution.c \
    selfCorrelation.c \
    serial.c \
    universal.c \
    utilities.c

HEADERS += \
    detect_thread.h \
    detect_thread_2.h \
    global.h \
    include/cephes.h \
    include/config.h \
    include/decls.h \
    include/defs.h \
    include/erf.h \
    include/externs.h \
    include/generators.h \
    include/genutils.h \
    include/matrix.h \
    include/stat_fncs.h \
    include/utilities.h \
    mainwindow.h

VERSION =2.0.8

QMAKE_TARGET_COPYRIGHT = "Copyright(C) 2019 dds2333"

QMAKE_TARGET_DESCRIPTION = "Random Detecter"

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc

RC_ICONS = "binary_code.ico"
