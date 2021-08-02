#include "graphview.h"

GraphView::GraphView(HoseTest *ht){
    m_ht = ht; // assign the pointer, since this is a duplicate for data manipulation.
  //  qWarning() << "create Graphview in class";
}

GraphView::~GraphView(){
  //  qWarning() << "delete graphview in class";
}

void GraphView::paintEvent(QPaintEvent *event){
    // essential to make this work. called internally by the constructor

    Q_UNUSED(event);
    if(m_ht == NULL){
     //   qWarning() << "ERROR: no hose test loaded in graphview";
        return;
    }
    if(m_ht->getTestLength() > 0){
     //   qWarning() << "paint graphview";
        QPainter paint(this);
        drawLines(&paint);
    } else {
      //  qWarning() << "paint graphview blank";
    }
}
void GraphView::drawLines(QPainter *paint){
    // generate the graph, this will run in components.

    Q_UNUSED(paint);

    // define pens for useage on drawing chart.
    QPen graphLineBlack(Qt::black, 2, Qt::SolidLine);
    QPen graphLineGrey(Qt::gray, 1, Qt::SolidLine);
    QPen pressureLine(Qt::blue, 2, Qt::SolidLine);
    QPen temperatureLine(Qt::green, 1, Qt::SolidLine);

    int textHeight = 12; // single int for determining all hgith values.
    int textWidth = 40;

    paint->setPen(graphLineBlack);
    paint->setRenderHint(QPainter::Antialiasing);
    /*
    QP->setPen(pen1);
    QP->drawLine(0, 0, width, height);

    pen1.setColor(Qt::black);
    QP->setPen(pen1);
    QP->setFont(QFont("Arial", 8));
    QP->drawText(20, 20, "639 PSI");

     recolor = true;

*/
    // determine min and max for pressure and temperature, as well as vector of all current test data.
    double minTemp = 1000.0;
    double minPres = 1000000.0;
    double maxTemp = -100.0;
    double maxPres = -100.0;

    std::vector<std::pair<double, double>> TestData; // set in a pair for each access.
    for(int i{0}; i < m_ht->getTestLength(); ++i){
        // create iterations of the data we need as well as the maximums for each.
        TestData.push_back({m_ht->getPressureAtID(i), m_ht->getTemperatureAtID(i)});

        if(TestData.at(i).first > maxPres){ maxPres = TestData.at(i).first; }
        if(TestData.at(i).first < minPres){ minPres = TestData.at(i).first; }

        if(TestData.at(i).second > maxTemp){ maxTemp = TestData.at(i).second; }
        if(TestData.at(i).second < minTemp){ minTemp = TestData.at(i).second; }
    }

    // determine chart width and height for maximum area
    int chartWidth = this->width();
    int chartHeight = this->height();

    minTemp *= 0.9;
    minPres *= 0.9;
    maxTemp *= 1.1;
    maxPres *= 1.1;

    // draw pressure guide rectangle, white color to highlight where we should be.
        int minpresline = (1 - ((m_ht->getTargetLoad() - minPres) / (maxPres - minPres))) * (chartHeight - textHeight * 4) + textHeight;
        int maxpresline = (1 - ((m_ht->getMaxPressure() - minPres) / (maxPres - minPres))) * (chartHeight - textHeight * 4) + textHeight;
        // qWarning() << QString::number(minpresline);
         paint->fillRect(textWidth + 1, minpresline, chartWidth - textWidth * 2 - 12, maxpresline - minpresline, Qt::white);

    // place fixed column names.
    paint->drawText(0, textHeight, "PSI");
    paint->drawText(chartWidth - textWidth, textHeight, "° C");
    paint->drawText(chartWidth / 2 - 50, chartHeight - textHeight, "Time (Seconds)");

    //paint->setPen(pressureLine);
    paint->fillRect(chartWidth - 145, 3 * textHeight + 3, 10, 10, Qt::blue);
    paint->drawText(chartWidth - 130, 4 * textHeight, "Pressure");

    //paint->setPen(temperatureLine);
    paint->fillRect(chartWidth - 145, 5 * textHeight + 3, 10, 10, Qt::green);
    paint->drawText(chartWidth - 130, 6 * textHeight, "Temperature");

    //paint->setPen(graphLineBlack);


    // generate pressure scale (left)
    // generate temperature scale (right)
    // each number will be assumbed to be no larger than 12px high and 40px wide, for the sake of positioning.
    int maxVertScaleItems = (chartHeight - textHeight * 3) / (textHeight * 2); // double the value, we only want one every 2 positions, for readability.

        // given the values that we have, place scaled values  from min to max on the LEFT side.
    for(int i{0}; i < maxVertScaleItems; ++i){
        // place each numbered position, starting from MAX at 0 to MIN at the bottom.
        double shownVal = ((maxPres - minPres) / maxVertScaleItems) * (maxVertScaleItems - i) + minPres; // the real value of the side number.
        int tval = shownVal * 10;  // convert number so only 1 decimal place valid.
        double trimval = tval / 10.0;
        // paint as x(0), y (i * 24);
        paint->drawText(0, (i * textHeight * 2) + textHeight * 2, QString::number(trimval));

        // now do the temperature values on the RIGHT side, same deal.
        shownVal = ((maxTemp - minTemp) / maxVertScaleItems) * (maxVertScaleItems - i) + minTemp;
        tval = shownVal * 10;
        trimval = tval / 10.0;
        paint->drawText(chartWidth - textWidth, (i * textHeight * 2) + textHeight * 2, QString::number(trimval));
    }

    // generate time scale (bottom)
    int maxHorzScaleItems = (chartWidth - (textWidth * 2)) / (textWidth * 2); // one every 2 positions is OK.

    if(maxHorzScaleItems > (int)TestData.size()){
        // if there are more items across then there is time items, show the time items.
        maxHorzScaleItems = (int)TestData.size();
    }
        // while we output the items across, be sure to manually calculate the width of each, despite the given width.
    for(int i{0}; i < maxHorzScaleItems; ++i){
        // output each time item on bottom axis.
        double shownVal = (double)TestData.size() / maxHorzScaleItems * i;
        int tval = shownVal * 10;
        double trimval = tval / 10.0; // give us 1 decimal place.
        paint->drawText(i * ((chartWidth - (textWidth * 2)) / maxHorzScaleItems) + textWidth, chartHeight - textHeight * 2, QString::number(trimval));
    }



        // preset last values of test, will ignore 0 while going through.
        int totalTestTime = (int)TestData.size();
        int lastTime = textWidth; // since time is 0
        int lastPres = (1 - (TestData.at(0).first - minPres) / (maxPres - minPres)) * (chartHeight - textHeight * 4) + textHeight;
        int lastTemp = (1 - (TestData.at(0).second - minTemp) / (maxTemp - minTemp)) * (chartHeight - textHeight * 4) + textHeight;

        if(lastPres > chartHeight - textHeight * 3){
            lastPres = chartHeight - textHeight * 3;
        }
        if( lastPres <  textHeight ) {
            lastPres = textHeight;
        }
        if(lastTemp > chartHeight - textHeight * 3){
            lastTemp = chartHeight - textHeight * 3;
        }
        if( lastTemp <  textHeight ) {
            lastTemp = textHeight;
        }

    // draw pressure and temperature line (concurrent)
        for(int i{1}; i < totalTestTime; ++i){
            // for each point, draw the line from the last point to the next.
            int time = ((double)i / totalTestTime) * ((double)chartWidth - textWidth * 2) + textWidth;

            if(time > chartWidth - textWidth - 10 || i == totalTestTime - 1){
                time = chartWidth - textWidth - 10; // force max limit.
            }


            int pres = (1 - (TestData.at(i).first - minPres) / (maxPres - minPres)) * (chartHeight - textHeight * 4) + textHeight;
            int temp = (1 - (TestData.at(i).second - minTemp) / (maxTemp - minTemp)) * (chartHeight - textHeight * 4) + textHeight;

            if(pres > chartHeight - textHeight * 3){
                pres = chartHeight - textHeight * 3;
            }
            if( pres <  textHeight ) {
                pres = textHeight;
            }
            if(temp > chartHeight - textHeight * 3){
                temp = chartHeight - textHeight * 3;
            }
            if( temp <  textHeight ) {
                temp = textHeight;
            }

            //qWarning() << "Time: " << QString::number(time) << "Pres" << QString::number(pres) << "Temp" << QString::number(temp);

            // draw the line

                // temperature
                paint->setPen(temperatureLine);
                paint->drawLine(lastTime, lastTemp, time, temp);

                // pressure
                paint->setPen(pressureLine);
                paint->drawLine(lastTime, lastPres, time, pres);

            // reset last values.
            lastTime = time;
            lastPres = pres;
            lastTemp = temp;
        }

        // draw grid lines
            // start with the vertical and horizontal main lines
            paint->setPen(graphLineBlack);
            paint->drawLine(textWidth, textHeight, textWidth, chartHeight - textHeight * 3);
            paint->drawLine(chartWidth - textWidth - 10, textHeight, chartWidth - textWidth - 10, chartHeight - textHeight * 3);
            paint->drawLine(textWidth, chartHeight - textHeight * 3, chartWidth - textWidth - 10, chartHeight - textHeight * 3);



}

void GraphView::UpdateView(){
    // force view update refresh. Will not need any inputs since we'll pull direct from hose test object.
    this->repaint();

}
