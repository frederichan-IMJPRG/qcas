#include "iconsize.h"
IconSize::IconSize(QWindowsStyle *parent) : QWindowsStyle()
   {
   Q_UNUSED (parent);
   }

   int IconSize::pixelMetric(PixelMetric metric, const QStyleOption * option, const QWidget * widget)const
   {
   int s = QWindowsStyle::pixelMetric(metric, option, widget);
   if (metric == QStyle::PM_SmallIconSize) {
   s = 32;
   }
   return s;
   }


