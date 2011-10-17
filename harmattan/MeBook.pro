# Add more folders to ship with the application, here

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT+= declarative network webkit sql
symbian:TARGET.UID3 = 0xE4950ADC

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=
DEPENDPATH += . .. ../core ../generators ../catalogs ../ui
INCLUDEPATH += . .. ../core ../generators ../catalogs ../ui
include(../epub/epub.pri)

HEADERS += \
    ../core/shared.h \
    ../generators/epubgenerator.h \
    ../core/settings.h \
    ../core/roleitemmodel.h \
    ../core/rendersettings.h \
    ../core/renderer.h \
    ../core/library.h \
    ../core/documentgenerator.h \
    ../core/catalog.h \
    ../core/bookmark.h \
    ../core/book.h \
    ../core/annotation.h \
    ../ui/bookview.h \
    ../catalogs/feedbooks.h

SOURCES += main.cpp \
    ../generators/epubgenerator.cpp \
    ../core/settings.cpp \
    ../core/renderer.cpp \
    ../core/library.cpp \
    ../core/documentgenerator.cpp \
    ../core/catalog.cpp \
    ../core/bookmark.cpp \
    ../core/book.cpp \
    ../core/annotation.cpp \
    ../ui/bookview.cpp \
    ../catalogs/feedbooks.cpp


OTHER_FILES += \
    qml/MainPage.qml \
    qml/main.qml \
    MeBook.desktop \
    MeBook.svg \
    MeBook.png \
    qml/components/MBookListItem.qml \
    qml/SettingsPage.qml \
    qml/components/SelectorButton.qml \
    qml/ReadingPage.qml \
    qml/components/SearchBar.qml \
    qml/DownloadBooksPage.qml \
    qml/components/UIConstants.js \
    qml/components/MySelectionDialog.qml \
    qml/components/MyCommonDialog.qml \
    qml/BookInfoPage.qml \
    splash.png

RESOURCES += \
    res.qrc

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

# enable booster
CONFIG += qdeclarative-boostable
QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_LFLAGS += -pie -rdynamic







