/*
    Copyright (C) 2004-2011 Martin Jolicoeur (snmpb1@gmail.com) 

    This file is part of the SnmpB project 
    (http://sourceforge.net/projects/snmpb)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "graph.h"

#include "agent.h"
#include "comboboxes.h"

class tracker: public QwtPlotZoomer
{
public:
    tracker(QWidget *canvas):
        QwtPlotZoomer(canvas)
    {
        setTrackerMode(AlwaysOn);
    }

    virtual QwtText trackerText(const QPoint &pos) const
    {
        QColor bg(Qt::white);
        bg.setAlpha(200);

        QwtText text = QwtPlotZoomer::trackerText(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }
};

GraphItem::GraphItem(Snmpb *snmpb):QwtPlot(snmpb->MainUI()->GraphName->text())
{
    s = snmpb;
    s->MainUI()->GraphTab->addTab(this, s->MainUI()->GraphName->text());
    dataCount = 0;
    timerID = 0;

    new tracker(canvas());
    
    for ( int i = 0; i < PLOT_HISTORY; i++ )
        timeData[i] = i;

    // Zero all curve structures
    for( int j = 0; j < NUM_PLOT_PER_GRAPH; j++)
    {
        curves[j].object = NULL;
        memset(curves[j].data, 0, sizeof(double)*PLOT_HISTORY);
    }
}

GraphItem::~GraphItem()
{
    // Free curve objects
    for( int j = 0; j < NUM_PLOT_PER_GRAPH; j++)
        if (curves[j].object) delete curves[j].object;

#if 0 // crashes the app 
    if (s->MainUI()->GraphTab && (s->MainUI()->GraphTab->indexOf(this) != -1))
        s->MainUI()->GraphTab->removeTab(s->MainUI()->GraphTab->indexOf(this));
#endif
}

void GraphItem::AddCurve(QString name, QPen& pen)
{
    int i = 0;
    
    for (i = 0; i < NUM_PLOT_PER_GRAPH; i++)
    {
        if (curves[i].object && (curves[i].object->title().text() == name))
            return;
        else if (!curves[i].object)
            break;
    }
    
    if (i >= NUM_PLOT_PER_GRAPH)
        return;

    curves[i].object = new QwtPlotCurve(name);
    curves[i].object->attach(this);
    curves[i].object->setPen(pen);

    if (!timerID)
        timerID = startTimer(1000); // 1 second
    
    replot();
}

void GraphItem::RemoveCurve(QString name)
{
    /* No other curve left, kill the timer first ... */
    if (timerID && ((/*TODO*/1-1) == 0))
    {
        killTimer(timerID);
        timerID = 0;
    }

    for (int i = 0; i < NUM_PLOT_PER_GRAPH; i++)
    {
        if (curves[i].object && (curves[i].object->title().text() == name))
        {
            delete(curves[i].object);
            curves[i].object = NULL;
            return;
        }
        else if (!curves[i].object)
            return;
    }
}

void GraphItem::timerEvent(QTimerEvent *)
{
    if ( dataCount < PLOT_HISTORY )
    {
        dataCount++;
    }
    else
    {
        /* Time shift of 1 sec */
        for ( int j = 0; j < PLOT_HISTORY; j++ )
            timeData[j]++;
        
        for ( int i = 0; i < PLOT_HISTORY - 1; i++ )
        {
            for ( int c = 0; c < 1; c++ )
            {
                curves[c].data[i] = curves[c].data[i+1];
            }
        }    
    }
    
    /* Set the data */
    curves[0].data[dataCount-1] = 
        s->AgentObj()->GetSyncValue(curves[0].object->title().text());
    
    setAxisScale(QwtPlot::xBottom, timeData[0], timeData[PLOT_HISTORY - 1]);
    
    for ( int c = 0; c < 1/* TODO */; c++ )
    {
        if (curves[c].object)
            curves[c].object->setRawSamples(timeData, curves[c].data, dataCount);
    }

    replot();
}

Graph::Graph(Snmpb *snmpb)
{
    s = snmpb;
 
    // Connect some signals
    connect( s->MainUI()->GraphAdd, SIGNAL( clicked() ), 
             this, SLOT( CreateGraph() ));
    connect( s->MainUI()->GraphDelete, SIGNAL( clicked() ), 
             this, SLOT( DeleteGraph() ));
    connect( s->MainUI()->PlotAdd, SIGNAL( clicked() ), 
             this, SLOT( CreatePlot() ));
    connect( s->MainUI()->PlotDelete, SIGNAL( clicked() ), 
             this, SLOT( DeletePlot() ));    
#if 0 //MART
    connect( s->MainUI()->PlotMIBTree, SIGNAL( SelectedOid(const QString&) ), 
             this, SLOT( SetObjectString(const QString&) ));
#endif
}

void Graph::CreateGraph(void)
{
#if 1  //MART
    s->MainUI()->GraphName->setText("NewGraph");
    s->MainUI()->Graph->setEnabled(true);
    s->MainUI()->GraphList->addItem("NewGraph");
    s->MainUI()->GraphName->setFocus(Qt::OtherFocusReason);  
#endif

    if (!s->MainUI()->GraphName->text().isEmpty())
    {        
        GraphItem *GI;
        for (int i = 0; i < Items.count(); i++)
        {
            GI = Items[i];
            if (GI->title().text() == s->MainUI()->GraphName->text())
            {
                QString err = QString("Graph \"%1\" already exist !")
                      .arg(s->MainUI()->GraphName->text());
                QMessageBox::information ( NULL, "Graph", err, 
                             QMessageBox::Ok, Qt::NoButton);
                return;
            }
        }
                
        GI = new GraphItem(s); 
        Items.append(GI);
    }
}

void Graph::DeleteGraph(void)
{
    if (!s->MainUI()->GraphName->text().isEmpty())
    {
        GraphItem *GI;
        for (int i = 0; i < Items.count(); i++)
        {
            GI = Items[i];
            if (GI->title().text() == s->MainUI()->GraphName->text())
            {
                Items.removeAll(GI);
                delete GI;
                return;
            }
        }
    }
}

void Graph::CreatePlot(void)
{
#if 1  //MART
    s->MainUI()->PlotDisplayName->setText("NewPlot");
    s->MainUI()->Plot->setEnabled(true);
    s->MainUI()->PlotList->addItem("NewPlot");
    s->MainUI()->PlotDisplayName->setFocus(Qt::OtherFocusReason);  
#endif

    if (!s->MainUI()->PlotObject->text().isEmpty())
    {
        // Create the pen with the combobox values
        QPen p(s->MainUI()->PlotColor->itemData(
               s->MainUI()->PlotColor->currentIndex(), 
               Qt::DisplayRole).value<QColor>(),
               s->MainUI()->PlotWidth->itemData(
               s->MainUI()->PlotWidth->currentIndex(), 
               Qt::DisplayRole).toUInt(),
               (enum Qt::PenStyle)(s->MainUI()->PlotShape->itemData(
               s->MainUI()->PlotShape->currentIndex(), 
               Qt::DisplayRole).toUInt()));
#ifdef NOTYET  
        printf("Creating plot %s\n", s->MainUI()->PlotObject->currentText().toLatin1().data());
#endif 
        if (!s->MainUI()->GraphName->text().isEmpty())
        {
            GraphItem *GI = NULL;
            for (int i = 0; i < Items.count(); i++)
            {
                GI = Items[i];
                if (GI->title().text() == s->MainUI()->GraphName->text())
                    break;
            }
            if (GI)
                GI->AddCurve(s->MainUI()->PlotObject->text(), p);
        }
    }
}

void Graph::DeletePlot(void)
{
    if (!s->MainUI()->PlotObject->text().isEmpty())
    {
#ifdef NOTYET
        printf("Deleting plot %s\n", s->MainUI()->PlotObject->currentText().toLatin1().data());
#endif 
        if (!s->MainUI()->GraphName->text().isEmpty())
        {
            GraphItem *GI = NULL;
            for (int i = 0; i < Items.count(); i++)
            {
                GI = Items[i];
                if (GI->title().text() == s->MainUI()->GraphName->text())
                    break;
            }
            if (GI) 
                GI->RemoveCurve(s->MainUI()->PlotObject->text());
        }
    }
}

void Graph::SetObjectString(const QString& oid)
{
    Q_UNUSED(oid)
#if 0 //MART
    s->MainUI()->PlotObject->insertItem(0, oid);
    s->MainUI()->PlotObject->setCurrentIndex(0);
#endif
}

