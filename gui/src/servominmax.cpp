
#include <QToolTip>
#include "servominmax.h"
#include "trackersettings.h"

ServoMinMax::ServoMinMax(QWidget *parent) : QWidget(parent)
{    
    min_travel = TrackerSettings::MIN_PWM; // In trackersettings.h
    max_travel = TrackerSettings::MAX_PWM;

    c_max = TrackerSettings::MAX_CNT;
    c_min = TrackerSettings::MIN_CNT;

    // Minimum value the maximum slider is allowed
    min_max = min_travel + 250;

    // Maximum value the minimum slider is allowed
    max_min = max_travel - 250;

    padding = 10;
    hpad = padding/2;
    sliderw  = 8;
    roundx = 1;
    roundy = 1;
    minSliderSelected = false;
    maxSliderSelected = false;
    centerSliderSelected = false;
    actValue = c_value; // Default Centered
    showPos = false;

    setDefaults();
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMouseTracking(true);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(showContextMenu(const QPoint &)));
}

void ServoMinMax::showContextMenu(const QPoint &pos)
{
       QMenu contextMenu(tr("Context menu"), this);

       QAction action1("Default Values", this);
       QAction action2("Re-Center", this);

       connect(&action1, SIGNAL(triggered()), this, SLOT(setDefaults()));
       connect(&action2, SIGNAL(triggered()), this, SLOT(reCenter()));
       contextMenu.addAction(&action1);
       contextMenu.addAction(&action2);
       contextMenu.exec(mapToGlobal(pos));
}

void ServoMinMax::reCenter()
{
    c_value = (max_val - min_val) /2 + min_val;
    emit(centerChanged(c_value));
    update();
}

void ServoMinMax::setDefaults()
{
    c_value = (TrackerSettings::MAX_PWM-TrackerSettings::MIN_PWM)/2 + TrackerSettings::MIN_PWM;
    min_val = TrackerSettings::DEF_MIN_PWM;
    max_val = TrackerSettings::DEF_MAX_PWM;

    update();
    emit minimumChanged(min_val);
    emit maximumChanged(max_val);
    emit centerChanged(c_value);
}


void ServoMinMax::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush brush;
    QPen pen;


    travel = max_travel - min_travel; // Overall range
    minSliderPos = (min_val - min_travel) / travel;
    maxSliderPos = (max_val - min_travel) / travel;
    centerSliderPos = (c_value - min_travel) / travel;
    minmaxp = (min_max - min_travel) / travel;
    maxminp = (max_min - min_travel) / travel;

    minmaxp = ((width()-padding) * minmaxp) + hpad;
    maxminp = ((width()-padding) * maxminp) + hpad;

    // Setup Slider Rectangles
    minSlider.setTop(hpad);
    minSlider.setBottom(height() - hpad);
    minSlider.setX((width()-padding) * minSliderPos + hpad);
    minSlider.setWidth(sliderw);
    minSlider.translate(-sliderw/2,0);

    maxSlider.setTop(hpad);
    maxSlider.setBottom(height() - hpad);
    maxSlider.setX((width()-padding) * maxSliderPos + hpad);
    maxSlider.setWidth(sliderw);
    maxSlider.translate(-sliderw/2,0);

    centerSlider.setTop(hpad);
    centerSlider.setBottom(height() - hpad);
    centerSlider.setX((width()-padding) * centerSliderPos + hpad);
    centerSlider.setWidth(sliderw);
    centerSlider.translate(-sliderw/2,0);

    double linetop = height() * 4/12;
    double linebot = height() * 8/12;


    // Max Travel Settings
    painter.drawLine(hpad,height()/2,width()-hpad,height()/2);
    // Left Line
    painter.drawLine(hpad,linetop,hpad,linebot);
    // Right Line
    painter.drawLine(width()-hpad,linetop,width()-hpad,linebot);


    // Draw MinMaxBar
    QRectF minmaxrect(minSlider.center().x(),linetop,maxSlider.center().x()-minSlider.center().x(),linebot-linetop);
    QLinearGradient minmaxbar(minmaxrect.x(),0,minmaxrect.right(),0);
    QColor minmaxcolor = qRgb(182, 255, 153);
    minmaxbar.setColorAt(0,minmaxcolor.darker(130));
    minmaxbar.setColorAt(0.5,minmaxcolor);
    minmaxbar.setColorAt(1,minmaxcolor.darker(130));
    painter.setBrush(minmaxbar);
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawRect(minmaxrect);

    // Draw Left Slider
    painter.setPen(Qt::NoPen);
    QLinearGradient lg(0,0,0,minSlider.height()-5);
    QColor color(Qt::blue);
    lg.setColorAt(0,color.lighter(130));
    lg.setColorAt(0.5,color);
    lg.setColorAt(1,color.lighter(130));
    painter.setBrush(lg);
    painter.drawRoundedRect(minSlider,roundx,roundy);

    // Draw Right Slider
    painter.drawRoundedRect(maxSlider,roundx,roundy);

    // Draw Center Slider
    color = Qt::red;
    lg.setColorAt(0,color.darker());
    lg.setColorAt(0.5,color);
    lg.setColorAt(1,color.darker());
    painter.setBrush(lg);
    painter.drawRoundedRect(centerSlider,roundx,roundy);

    // Draw the current output
    if(showPos) {
        painter.save();
        painter.setBrush(Qt::yellow);
        painter.setPen(QPen(Qt::black,1,Qt::SolidLine));
        double triheight = (linebot - linetop)*0.75;
        double ap = (actValue - min_travel) / travel;
        ap = ((width()-padding) * ap) + hpad;
        painter.translate(ap,height()/2-1);
        painter.rotate(45);
        painter.drawRect(-triheight/2,-triheight/2,triheight,triheight);
        painter.restore();
    }
}

void ServoMinMax::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(min_val != min_start && minSliderSelected) {
        //qDebug() << "Min Val" << min_val;
        emit minimumChanged(min_val);
    }
    if(max_val != max_start && maxSliderSelected) {
        //qDebug() << "Max Val" << max_val;
        emit maximumChanged(max_val);
    }
    if(c_value != cnt_start && centerSliderSelected) {
        //qDebug() << "Cnt Val" << c_value;
        emit centerChanged(c_value);
    }

    minSliderSelected = false;
    maxSliderSelected = false;
    centerSliderSelected = false;
}

void ServoMinMax::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton) {
        if(minSlider.contains(event->pos())) {
            mouseDownPoint = event->globalPos();
         //   qDebug() << "Clicked on Min Slider";
            minSliderSelected = true;
            min_start = min_val;
        } else if(maxSlider.contains(event->pos())) {
            mouseDownPoint = event->globalPos();
         //   qDebug() << "Clicked on Max Slider";
            maxSliderSelected = true;
            max_start = max_val;
        } else if(centerSlider.contains(event->pos())) {
            mouseDownPoint = event->globalPos();
        //    qDebug() << "Clicked on Center Slider";
            centerSliderSelected = true;
            cnt_start = c_value;
        }
    }
}

void ServoMinMax::mouseMoveEvent(QMouseEvent *event)
{
    // Movement from mouse down point in X axis
    int xtrans = mouseDownPoint.x() - event->globalPos().x();
    double xtravelperpixel = travel/(width()-padding);
    double xtr = xtrans*xtravelperpixel;

    if(minSliderSelected) {
        min_val = min_start - xtr;
        if(min_val < min_travel)
            min_val = min_travel;
        if(min_val > min_max)
            min_val = min_max;
        update();
    } else if (maxSliderSelected) {
        max_val = max_start - xtr;
        if(max_val > max_travel)
            max_val = max_travel;
        if(max_val < max_min)
            max_val = max_min;
        update();
    } else if (centerSliderSelected) {
        c_value = cnt_start - xtr;
        if(c_value < c_min)
            c_value = c_min;
        if(c_value > c_max)
            c_value = c_max;
        update();
    }
}

void ServoMinMax::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton) {
        if(centerSlider.contains(event->pos())) {
            reCenter();
        }
    }
}

bool ServoMinMax::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
            QPoint glbpoint = helpEvent->globalPos();
            glbpoint.setY(glbpoint.y()-40);
            if(minSlider.contains(helpEvent->pos())) {
                QToolTip::showText(glbpoint, QString::number(min_val) + "uS");
            } else if(maxSlider.contains(helpEvent->pos())) {
                QToolTip::showText(glbpoint, QString::number(max_val) + "uS");
            } else if(centerSlider.contains(helpEvent->pos())) {
                QToolTip::showText(glbpoint, QString::number(c_value) + "uS");
            } else {
                QToolTip::hideText();
                event->ignore();
            }
            return true;
        }
        return QWidget::event(event);
}
