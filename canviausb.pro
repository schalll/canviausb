QT       += core gui widgets printsupport

TARGET = CANviaUSB
TEMPLATE = app

DEPENDPATH += . \
              qss \
              report \
              res \
              src \
              src\can \
              src\identifiers \
              src\utilities \
              src\can\res

INCLUDEPATH += .

# Input
HEADERS += src/appSettings.h \
           src/CommunicationData.h \
           src/CommunicationDelegate.h \
           src/CommunicationModel.h \
           src/CommunicationProxyModel.h \
           src/CommunicationView.h \
           src/DriverView.h \
           src/filterActivationModel.h \
           src/filterActivationView.h \
           src/filterDialog.h \
           src/filterListWidget.h \
           src/IdentifiersSimpleStatsView.h \
           src/IdentifiersStatsModel.h \
           src/IdentifiersStatsView.h \
           src/infoWidget.h \
           src/mainWindow.h \
           src/pluginInterface.h \
           src/pluginModel.h \
           src/PreferencesDialog.h \
           src/PrintableTableView.h \
           src/QuickSend.h \
           src/SideBarDelegate.h \
           src/SideBarItem.h \
           src/SideBarModel.h \
           src/SideBarView.h \
           src/textprinter.h \
           src/TitleFrame.h \
           src/ToolFrame.h \
           src/WidgetBase.h \
           src/WidgetCommunication.h \
           src/WidgetDrivers.h \
           src/WidgetFilteredLog.h \
           src/WidgetIdentifiers.h \
           src/WidgetIdentifiersStats.h \
           src/WidgetMonitor.h \
           src/WidgetPlugin.h \
           src/can/can_demo.h \
           src/can/can_driver.h \
           src/can/can_driverfactory.h \
           src/can/can_frame.h \
           src/can/can_lawicel.h \
           src/can/can_mhs.h \
           src/can/can_selectdriverdialog.h \
           src/can/can_zcan4usbfx.h \
           src/can/ftd2xx.h \
           src/can/settings_demo.h \
           src/can/settings_driver.h \
           src/can/settings_lawicel.h \
           src/can/settings_mhs.h \
           src/can/settings_zcan4usbfx.h \
           src/identifiers/ColorListEditor.h \
           src/identifiers/DataFormatEditor.h \
           src/identifiers/IdentifiersDelegate.h \
           src/identifiers/IdentifiersModel.h \
           src/identifiers/IdentifiersView.h \
           src/utilities/clearbutton.h \
           src/utilities/lineedit.h \
           src/utilities/lineedit_p.h \
           src/utilities/ringbuffer.h \
           src/utilities/searchbutton.h \
           src/utilities/searchlineedit.h \
    src/can/CANviaUSB_DriverInterface.h

FORMS += src/filterDialog.ui \
         src/infoWidget.ui \
         src/can/res/settings_cpcusb_arm7.ui \
         src/can/res/settings_demo.ui \
         src/can/res/settings_lawicel.ui \
         src/can/res/settings_mhs.ui \
         src/can/res/settings_zcan4usbfx.ui \
         src/can/res/settings_zcan4usbfx_complete.ui

SOURCES += src/appSettings.cpp \
           src/CommunicationData.cpp \
           src/CommunicationDelegate.cpp \
           src/CommunicationModel.cpp \
           src/CommunicationProxyModel.cpp \
           src/CommunicationView.cpp \
           src/DriverView.cpp \
           src/filterActivationModel.cpp \
           src/filterActivationView.cpp \
           src/filterDialog.cpp \
           src/filterListWidget.cpp \
           src/IdentifiersSimpleStatsView.cpp \
           src/IdentifiersStatsModel.cpp \
           src/IdentifiersStatsView.cpp \
           src/infoWidget.cpp \
           src/main.cpp \
           src/mainWindow.cpp \
           src/PreferencesDialog.cpp \
           src/PrintableTableView.cpp \
           src/QuickSend.cpp \
           src/SideBarDelegate.cpp \
           src/SideBarItem.cpp \
           src/SideBarModel.cpp \
           src/SideBarView.cpp \
           src/textprinter.cpp \
           src/TitleFrame.cpp \
           src/ToolFrame.cpp \
           src/WidgetBase.cpp \
           src/WidgetCommunication.cpp \
           src/WidgetDrivers.cpp \
           src/WidgetFilteredLog.cpp \
           src/WidgetIdentifiers.cpp \
           src/WidgetIdentifiersStats.cpp \
           src/WidgetMonitor.cpp \
           src/WidgetPlugin.cpp \
           src/can/can_demo.cpp \
           src/can/can_driver.cpp \
           src/can/can_driverfactory.cpp \
           src/can/can_frame.cpp \
           src/can/can_lawicel.cpp \
           src/can/can_mhs.cpp \
           src/can/can_selectdriverdialog.cpp \
           src/can/can_zcan4usbfx.cpp \
           src/can/settings_demo.cpp \
           src/can/settings_driver.cpp \
           src/can/settings_lawicel.cpp \
           src/can/settings_mhs.cpp \
           src/can/settings_zcan4usbfx.cpp \
           src/identifiers/ColorListEditor.cpp \
           src/identifiers/DataFormatEditor.cpp \
           src/identifiers/IdentifiersDelegate.cpp \
           src/identifiers/IdentifiersModel.cpp \
           src/identifiers/IdentifiersView.cpp \
           src/utilities/clearbutton.cpp \
           src/utilities/lineedit.cpp \
           src/utilities/ringbuffer.cpp \
           src/utilities/searchbutton.cpp \
           src/utilities/searchlineedit.cpp \
           src/pluginInterface.cpp \
           src/pluginModel.cpp

RESOURCES += qss/stylesheet.qrc report/report.qrc res/files.qrc res/icons.qrc

RC_FILE = res/icon.rc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/ftd2xx/ -lftd2xx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/ftd2xx/ -lftd2xx

INCLUDEPATH += $$PWD/libs/ftd2xx
DEPENDPATH += $$PWD/libs/ftd2xx

