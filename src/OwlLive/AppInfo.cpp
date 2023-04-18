#include "AppInfo.h"

AppInfo::AppInfo(QObject* parent)
  : QObject{parent} {
  version("1.0.0");
}
