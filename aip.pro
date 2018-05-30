#-------------------------------------------------
#
# Project created by QtCreator 2015-12-04T14:13:19
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += xml


TARGET = aip
TEMPLATE = app
INCLUDEPATH += External-Interface/
INCLUDEPATH += Function-Module/
INCLUDEPATH += Display-Interface/


SOURCES += \
    External-Interface/linux_can.cpp \
    External-Interface/Histogram.cpp \
    External-Interface/Linux_Usart.cpp \
    External-Interface/linux_sql.cpp \
    External-Interface/linux_rtc.cpp \
    External-Interface/linux_color.cpp \
    External-Interface/ipmethod.cpp \
    External-Interface/mytcpthread.cpp \
    External-Interface/pwm_api.cpp \
    External-Interface/SqlClient.cpp \
    External-Interface/UdpClient.cpp \
    Function-Module/Widget_ACW.cpp \
    Function-Module/Widget_DLR.cpp \
    Function-Module/Widget_BLOCK.cpp \
    Function-Module/Widget_IMP.cpp \
    Function-Module/Widget_DCW.cpp \
    Function-Module/Widget_LVS.cpp \
    Function-Module/Widget_IR.cpp \
    Function-Module/Widget_PWR.cpp \
    Function-Module/Widget_MAG.cpp \
    Display-Interface/password.cpp \
    Display-Interface/w_conf.cpp \
    Display-Interface/w_colr.cpp \
    Display-Interface/w_help.cpp \
    Display-Interface/w_debug.cpp \
    Display-Interface/w_main.cpp \
    Display-Interface/w_keyb.cpp \
    Display-Interface/w_test.cpp \
    Display-Interface/w_syst.cpp \
    External-Interface/wt_wave.cpp \
    Function-Module/Widget_INDL.cpp \
    Display-Interface/pwr_picture.cpp \
    External-Interface/main.cpp \
    Function-Module/widget_bldc_pg.cpp \
    Function-Module/widget_load.cpp \
    Function-Module/widget_noload.cpp \
    Function-Module/widget_bemf.cpp \
    External-Interface/pic.cpp \
    Display-Interface/w_data.cpp \
    External-Interface/SQL_DB.cpp \
    External-Interface/linux_Thread.cpp


HEADERS  += \
    External-Interface/linux_can.h \
    External-Interface/Histogram.h \
    External-Interface/Linux_Usart.h \
    External-Interface/linux_sql.h \
    External-Interface/linux_rtc.h \
    External-Interface/linux_color.h \
    External-Interface/em335x_drivers.h \
    External-Interface/ipmethod.h \
    External-Interface/mytcpthread.h \
    External-Interface/define.h \
    External-Interface/pwm_api.h \
    External-Interface/SqlClient.h \
    External-Interface/UdpClient.h \
    Function-Module/Widget_ACW.h \
    Function-Module/Widget_DLR.h \
    Function-Module/Widget_BLOCK.h \
    Function-Module/Widget_IMP.h \
    Function-Module/Widget_DCW.h \
    Function-Module/Widget_LVS.h \
    Function-Module/Widget_IR.h \
    Function-Module/Widget_PWR.h \
    Function-Module/Widget_MAG.h \
    Display-Interface/password.h \
    Display-Interface/w_conf.h \
    Display-Interface/w_colr.h \
    Display-Interface/w_help.h \
    Display-Interface/w_debug.h \
    Display-Interface/w_main.h \
    Display-Interface/w_keyb.h \
    Display-Interface/w_test.h \
    Display-Interface/w_syst.h \
    External-Interface/wt_wave.h \
    Function-Module/Widget_INDL.h \
    Display-Interface/pwr_picture.h \
    Function-Module/widget_bldc_pg.h \
    Function-Module/widget_load.h \
    Function-Module/widget_noload.h \
    Function-Module/widget_bemf.h \
    External-Interface/pic.h \
    Display-Interface/w_data.h \
    External-Interface/SQL_DB.h \
    External-Interface/linux_Thread.h


FORMS    += \
    Function-Module/Widget_ACW.ui \
    Function-Module/Widget_DLR.ui \
    Function-Module/Widget_BLOCK.ui \
    Function-Module/Widget_IMP.ui \
    Function-Module/Widget_DCW.ui \
    Function-Module/Widget_LVS.ui \
    Function-Module/Widget_IR.ui \
    Function-Module/Widget_PWR.ui \
    Function-Module/Widget_MAG.ui \
    Display-Interface/password.ui \
    Display-Interface/w_conf.ui \
    Display-Interface/w_colr.ui \
    Display-Interface/w_help.ui \
    Display-Interface/w_debug.ui \
    Display-Interface/w_main.ui \
    Display-Interface/w_keyb.ui \
    Display-Interface/w_test.ui \
    Display-Interface/w_syst.ui \
    Function-Module/widget_bemf.ui \
    Function-Module/widget_noload.ui \
    Function-Module/widget_load.ui \
    Function-Module/widget_bldc_pg.ui \
    Display-Interface/pwr_picture.ui \
    Function-Module/Widget_INDL.ui \
    Display-Interface/pwr_picture.ui \
    Display-Interface/w_data.ui


RESOURCES += \
    Display-Interface/image.qrc

OTHER_FILES += Other-File/
CONFIG      += thread
OBJECTS_DIR += Out-O/
UI_DIR      += Out-UI/
MOC_DIR     += Out-Moc/

TRANSLATIONS += user_zh.ts
