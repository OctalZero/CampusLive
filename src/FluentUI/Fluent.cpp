#include "Fluent.h"

#include <QFontDatabase>
#include <QQmlContext>
#include <QGuiApplication>
#include <QQuickWindow>
#include "FluColors.h"
#include "NativeEventFilter.h"
#include "FluTheme.h"
#include "WindowHelper.h"
#include "FluApp.h"
#include "Def.h"

Fluent* Fluent::m_instance = nullptr;

Fluent* Fluent::getInstance() {
  if (Fluent::m_instance == nullptr) {
    Fluent::m_instance = new Fluent;
  }
  return Fluent::m_instance;
}

QString Fluent::version() const {
  return QStringLiteral(VERSION_IN);
}

void Fluent::registerTypes(const char* uri) {
  Q_INIT_RESOURCE(res);
  int major = 1;
  int minor = 0;

  qmlRegisterType<WindowHelper>(uri, major, minor, "WindowHelper");
  qmlRegisterType<FluColorSet>(uri, major, minor, "FluColorSet");

  qmlRegisterType(QUrl("qrc:/ui/controls/FluStatusView.qml"), uri, major, minor, "FluStatusView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluPagination.qml"), uri, major, minor, "FluPagination");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluToggleButton.qml"), uri, major, minor, "FluToggleButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluTableView.qml"), uri, major, minor, "FluTableView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluPivotItem.qml"), uri, major, minor, "FluPivotItem");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluPivot.qml"), uri, major, minor, "FluPivot");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluFlipView.qml"), uri, major, minor, "FluFlipView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluPaneItemExpander.qml"), uri, major, minor, "FluPaneItemExpander");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluTabView.qml"), uri, major, minor, "FluTabView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluArea.qml"), uri, major, minor, "FluArea");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluBadge.qml"), uri, major, minor, "FluBadge");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluMediaPlayer.qml"), uri, major, minor, "FluMediaPlayer");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluContentPage.qml"), uri, major, minor, "FluContentPage");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluScrollablePage.qml"), uri, major, minor, "FluScrollablePage");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluPaneItemHeader.qml"), uri, major, minor, "FluPaneItemHeader");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluPaneItem.qml"), uri, major, minor, "FluPaneItem");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluPaneItemSeparator.qml"), uri, major, minor, "FluPaneItemSeparator");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluNavigationView.qml"), uri, major, minor, "FluNavigationView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluCalendarPicker.qml"), uri, major, minor, "FluCalendarPicker");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluCalendarView.qml"), uri, major, minor, "FluCalendarView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluDatePicker.qml"), uri, major, minor, "FluDatePicker");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluTimePicker.qml"), uri, major, minor, "FluTimePicker");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluColorView.qml"), uri, major, minor, "FluColorView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluColorPicker.qml"), uri, major, minor, "FluColorPicker");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluCarousel.qml"), uri, major, minor, "FluCarousel");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluAutoSuggestBox.qml"), uri, major, minor, "FluAutoSuggestBox");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluExpander.qml"), uri, major, minor, "FluExpander");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluTreeView.qml"), uri, major, minor, "FluTreeView");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluContentDialog.qml"), uri, major, minor, "FluContentDialog");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluMenuItem.qml"), uri, major, minor, "FluMenuItem");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluMenu.qml"), uri, major, minor, "FluMenu");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluScrollBar.qml"), uri, major, minor, "FluScrollBar");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluTextButton.qml"), uri, major, minor, "FluTextButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluMultilineTextBox.qml"), uri, major, minor, "FluMultilineTextBox");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluTooltip.qml"), uri, major, minor, "FluTooltip");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluDivider.qml"), uri, major, minor, "FluDivider");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluIcon.qml"), uri, major, minor, "FluIcon");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluObject.qml"), uri, major, minor, "FluObject");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluInfoBar.qml"), uri, major, minor, "FluInfoBar");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluWindow.qml"), uri, major, minor, "FluWindow");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluRectangle.qml"), uri, major, minor, "FluRectangle");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluAppBar.qml"), uri, major, minor, "FluAppBar");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluButton.qml"), uri, major, minor, "FluButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluCheckBox.qml"), uri, major, minor, "FluCheckBox");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluComboBox.qml"), uri, major, minor, "FluComboBox");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluDropDownButton.qml"), uri, major, minor, "FluDropDownButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluFilledButton.qml"), uri, major, minor, "FluFilledButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluIconButton.qml"), uri, major, minor, "FluIconButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluProgressBar.qml"), uri, major, minor, "FluProgressBar");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluProgressRing.qml"), uri, major, minor, "FluProgressRing");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluRadioButton.qml"), uri, major, minor, "FluRadioButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluSlider.qml"), uri, major, minor, "FluSlider");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluTextBox.qml"), uri, major, minor, "FluTextBox");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluText.qml"), uri, major, minor, "FluText");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluFilledButton.qml"), uri, major, minor, "FluFilledButton");
  qmlRegisterType(QUrl("qrc:/ui/controls/FluToggleSwitch.qml"), uri, major, minor, "FluToggleSwitch");

  qmlRegisterUncreatableMetaObject(Fluent_Awesome::staticMetaObject,  uri, major, minor, "FluentIcons", "Access to enums & flags only");
}

void Fluent::initializeEngine(QQmlEngine* engine, const char* uri) {
  nativeEvent = new NativeEventFilter();
  qApp->installNativeEventFilter(nativeEvent);
  Q_UNUSED(engine)
  Q_UNUSED(uri)
#ifdef Q_OS_WIN
  QFont font;
  font.setFamily("Microsoft YaHei");
  QGuiApplication::setFont(font);
  //    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
#endif
  QFontDatabase::addApplicationFont(":/ui/res/font/Segoe_Fluent_Icons.ttf");
  FluApp* app = FluApp::getInstance();
  engine->rootContext()->setContextProperty("FluApp", app);
  FluColors* colors = FluColors::getInstance();
  engine->rootContext()->setContextProperty("FluColors", colors);
  FluTheme* theme = FluTheme::getInstance();
  engine->rootContext()->setContextProperty("FluTheme", theme);
}
