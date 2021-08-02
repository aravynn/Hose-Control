#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QLabel>
#include <QPainter>

#include <vector>
#include "hosetest.h" // for HT object.

/**
 * @brief The GraphView class
 *
 * Generate the graph as per information sent by the generating thread for displaying information.
 *
 *
 *
 */




class GraphView : public QLabel
{
public:
    HoseTest *m_ht;

    //GraphView(); // we need a hose test to use this bit.
    GraphView(HoseTest *ht);
    ~GraphView();

    void paintEvent(QPaintEvent *event); // essential to make this work. called internally by the constructor
    void drawLines(QPainter *QP); // our own function for the code generation.

    void UpdateView();
    bool exists(){ return true; } // if this exists, return true;
};

#endif // GRAPHVIEW_H
