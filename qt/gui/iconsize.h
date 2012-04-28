#ifndef ICONSIZE_H
#define ICONSIZE_H
#include <QObject>
#include <QStyle>
#include <QWindowsStyle>
class IconSize : public QWindowsStyle
 {
   Q_OBJECT
public:
    explicit IconSize(QWindowsStyle *parent = 0);
    int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;
signals:
public slots:
};
#endif // ICONSIZE_H
