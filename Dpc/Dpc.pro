include(../lib.pri)

QT += core network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

DEFINES += DPC_LIB

win32 {
    RC_FILE += Dpc_mingw.rc
}

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    dpc/dpc_resource.qrc

HEADERS += \
    dpc/dep_about_box.h \
    dpc/dep_settings.h \
    dpc/dep_user_roles.h \
    dpc/dpc_global.h \
    dpc/gui/delegates/ComboBoxDelegate.h \
    dpc/gui/delegates/LineEditDelegate.h \
    dpc/gui/delegates/ProgressBarDelegate.h \
    dpc/gui/delegates/PushButtonDelegate.h \
    dpc/gui/delegates/SpinBoxDelegate.h \
    dpc/gui/dialogs/channel/ChannelsDialog.h \
    dpc/gui/dialogs/channel/ISettingsWidget.h \
    dpc/gui/dialogs/channel/Rs485SettingsWidget.h \
    dpc/gui/dialogs/channel/TcpSettingsWidget.h \
    dpc/gui/dialogs/channel/VComSettingsWidget.h \
    dpc/gui/dialogs/msg_box/MsgBox.h \
    dpc/gui/gui.h \
    dpc/gui/widgets/CheckableComboBox.h \
    dpc/gui/widgets/CheckableHeaderView.h \
    dpc/gui/widgets/ColorSelectButton.h \
    dpc/gui/widgets/ComboBox.h \
    dpc/gui/widgets/IpAddressWidget.h \
    dpc/gui/widgets/LineEdit.h \
    dpc/gui/widgets/SpinBox.h \
    dpc/gui/widgets/TableView.h \
    dpc/gui/widgets/journal/JournalTableModel.h \
    dpc/gui/widgets/journal/JournalWidget.h \
    dpc/gui/widgets/qxd_tableview/QxdFilter.h \
    dpc/gui/widgets/qxd_tableview/QxdFrame.h \
    dpc/gui/widgets/qxd_tableview/QxdHeaderView.h \
    dpc/gui/widgets/qxd_tableview/QxdHeaderViewMenu.h \
    dpc/gui/widgets/qxd_tableview/QxdProxyModel.h \
    dpc/gui/widgets/qxd_tableview/QxdTableView.h \
    dpc/gui/widgets/qxd_tableview/filters/CompareFilter.h \
    dpc/gui/widgets/qxd_tableview/filters/CompareFilterEditor.h \
    dpc/gui/widgets/qxd_tableview/filters/FilterEditor.h \
    dpc/gui/widgets/qxd_tableview/filters/IValueEditor.h \
    dpc/gui/widgets/qxd_tableview/filters/ListFilter.h \
    dpc/gui/widgets/qxd_tableview/filters/ListFilterEditor.h \
    dpc/gui/widgets/qxd_tableview/filters/ListFilterModel.h \
    dpc/gui/widgets/qxd_tableview/filters/TextFilter.h \
    dpc/gui/widgets/qxd_tableview/filters/TextFilterEditor.h \
    dpc/helper.h \
    dpc/journal/Journal.h \
    dpc/journal/Source.h \
    dpc/json_helper.h \
    dpc/log_settings_dialog.h \
    dpc/login_dialog.h \
    dpc/return_codes.h \
    dpc/sybus/ParamAttribute.h \
    dpc/sybus/ParamDimension.h \
    dpc/sybus/ParamPack.h \
    dpc/sybus/channel/Channel.h \
    dpc/sybus/channel/ChannelsFactory.h \
    dpc/sybus/channel/Common.h \
    dpc/sybus/channel/async/AsyncChannel.h \
    dpc/sybus/channel/async/ChannelReply.h \
    dpc/sybus/channel/async/ChannelRequest.h \
    dpc/sybus/channel/async/DeviceDefinition.h \
    dpc/sybus/channel/async/channel_types/AsyncRs485Channel.h \
    dpc/sybus/channel/async/channel_types/AsyncTcpChannel.h \
    dpc/sybus/channel/async/channel_types/AsyncVComChannel.h \
    dpc/sybus/channel/async/manager/AsyncChannelManager.h \
    dpc/sybus/channel/async/manager/IRequest.h \
    dpc/sybus/channel/async/manager/requests/MultipleRequest.h \
    dpc/sybus/channel/async/manager/requests/ReadParamRequest.h \
    dpc/sybus/channel/async/manager/requests/WriteParamRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/AttrPathRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/DeletePathRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/DownloadFileRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/FileSystemResponseRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/IFileSystemRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/ListPathRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/MakeDirRequest.h \
    dpc/sybus/channel/async/manager/requests/filesystem/UploadFileRequest.h \
    dpc/sybus/channel/async/manager/requests/metadata/AttributeRequest.h \
    dpc/sybus/channel/async/manager/requests/metadata/DimensionRequest.h \
    dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.h \
    dpc/sybus/channel/async/manager/requests/metadata/NameRequest.h \
    dpc/sybus/channel/async/manager/requests/metadata/ParamListRequest.h \
    dpc/sybus/channel/async/manager/requests/metadata/SizeRequest.h \
    dpc/sybus/channel/async/manager/requests/metadata/TypeRequest.h \
    dpc/sybus/channel/channel_types/Rs485Channel.h \
    dpc/sybus/channel/channel_types/TcpChannel.h \
    dpc/sybus/channel/channel_types/VComChannel.h \
    dpc/sybus/smparlist.h \
    dpc/sybus/utils.h \
    dpc/utilities.h \
    version.h

SOURCES += \
    dpc/dep_about_box.cpp \
    dpc/gui/delegates/ComboBoxDelegate.cpp \
    dpc/gui/delegates/LineEditDelegate.cpp \
    dpc/gui/delegates/ProgressBarDelegate.cpp \
    dpc/gui/delegates/PushButtonDelegate.cpp \
    dpc/gui/delegates/SpinBoxDelegate.cpp \
    dpc/gui/dialogs/channel/ChannelsDialog.cpp \
    dpc/gui/dialogs/channel/Rs485SettingsWidget.cpp \
    dpc/gui/dialogs/channel/TcpSettingsWidget.cpp \
    dpc/gui/dialogs/channel/VComSettingsWidget.cpp \
    dpc/gui/dialogs/msg_box/MsgBox.cpp \
    dpc/gui/gui.cpp \
    dpc/gui/widgets/CheckableComboBox.cpp \
    dpc/gui/widgets/CheckableHeaderView.cpp \
    dpc/gui/widgets/ColorSelectButton.cpp \
    dpc/gui/widgets/ComboBox.cpp \
    dpc/gui/widgets/IpAddressWidget.cpp \
    dpc/gui/widgets/LineEdit.cpp \
    dpc/gui/widgets/SpinBox.cpp \
    dpc/gui/widgets/TableView.cpp \
    dpc/gui/widgets/journal/JournalTableModel.cpp \
    dpc/gui/widgets/journal/JournalWidget.cpp \
    dpc/gui/widgets/qxd_tableview/QxdFilter.cpp \
    dpc/gui/widgets/qxd_tableview/QxdFrame.cpp \
    dpc/gui/widgets/qxd_tableview/QxdHeaderView.cpp \
    dpc/gui/widgets/qxd_tableview/QxdHeaderViewMenu.cpp \
    dpc/gui/widgets/qxd_tableview/QxdProxyModel.cpp \
    dpc/gui/widgets/qxd_tableview/QxdTableView.cpp \
    dpc/gui/widgets/qxd_tableview/filters/CompareFilter.cpp \
    dpc/gui/widgets/qxd_tableview/filters/CompareFilterEditor.cpp \
    dpc/gui/widgets/qxd_tableview/filters/FilterEditor.cpp \
    dpc/gui/widgets/qxd_tableview/filters/IValueEditor.cpp \
    dpc/gui/widgets/qxd_tableview/filters/ListFilter.cpp \
    dpc/gui/widgets/qxd_tableview/filters/ListFilterEditor.cpp \
    dpc/gui/widgets/qxd_tableview/filters/ListFilterModel.cpp \
    dpc/gui/widgets/qxd_tableview/filters/TextFilter.cpp \
    dpc/gui/widgets/qxd_tableview/filters/TextFilterEditor.cpp \
    dpc/journal/Journal.cpp \
    dpc/json_helper.cpp \
    dpc/log_settings_dialog.cpp \
    dpc/login_dialog.cpp \
    dpc/sybus/ParamAttribute.cpp \
    dpc/sybus/ParamDimension.cpp \
    dpc/sybus/ParamPack.cpp \
    dpc/sybus/channel/Channel.cpp \
    dpc/sybus/channel/ChannelsFactory.cpp \
    dpc/sybus/channel/async/AsyncChannel.cpp \
    dpc/sybus/channel/async/ChannelReply.cpp \
    dpc/sybus/channel/async/channel_types/AsyncRs485Channel.cpp \
    dpc/sybus/channel/async/channel_types/AsyncTcpChannel.cpp \
    dpc/sybus/channel/async/channel_types/AsyncVComChannel.cpp \
    dpc/sybus/channel/async/manager/AsyncChannelManager.cpp \
    dpc/sybus/channel/async/manager/IRequest.cpp \
    dpc/sybus/channel/async/manager/requests/MultipleRequest.cpp \
    dpc/sybus/channel/async/manager/requests/ReadParamRequest.cpp \
    dpc/sybus/channel/async/manager/requests/WriteParamRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/AttrPathRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/DeletePathRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/DownloadFileRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/FileSystemResponseRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/IFileSystemRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/ListPathRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/MakeDirRequest.cpp \
    dpc/sybus/channel/async/manager/requests/filesystem/UploadFileRequest.cpp \
    dpc/sybus/channel/async/manager/requests/metadata/AttributeRequest.cpp \
    dpc/sybus/channel/async/manager/requests/metadata/DimensionRequest.cpp \
    dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.cpp \
    dpc/sybus/channel/async/manager/requests/metadata/NameRequest.cpp \
    dpc/sybus/channel/async/manager/requests/metadata/ParamListRequest.cpp \
    dpc/sybus/channel/async/manager/requests/metadata/SizeRequest.cpp \
    dpc/sybus/channel/async/manager/requests/metadata/TypeRequest.cpp \
    dpc/sybus/channel/channel_types/Rs485Channel.cpp \
    dpc/sybus/channel/channel_types/TcpChannel.cpp \
    dpc/sybus/channel/channel_types/VComChannel.cpp \
    dpc/sybus/utils.cpp \
    dpc/utilities.cpp
