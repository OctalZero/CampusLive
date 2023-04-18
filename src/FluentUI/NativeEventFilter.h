﻿#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class NativeEventFilter : public QAbstractNativeEventFilter
{

public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
};

#endif // NATIVEEVENTFILTER_H
