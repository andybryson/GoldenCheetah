/*
 * Copyright (c) 2010 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _GC_RideNavigator_h
#define _GC_RideNavigator_h 1
#include "GoldenCheetah.h"

#include "MainWindow.h"
#include "MetricAggregator.h"
#include "RideMetadata.h"
#include "DBAccess.h"
#include "MainWindow.h"
#include "Settings.h"
#include "Colors.h"

#include <QSqlTableModel>
#include <QTableView>

class NavigatorCellDelegate;
class GroupByModel;
class DiaryWindow;
class BUGFIXQSortFilterProxyModel;

//
// The RideNavigator
//
// A list of rides displayed using a QTreeView on top of
// a QSQLTableModel which reads from the "metrics" table
// via the DBAccess database connection
//
class RideNavigator : public GcWindow
{
    Q_OBJECT
    G_OBJECT

    Q_PROPERTY(int sortByIndex READ sortByIndex WRITE setSortByIndex USER true)
    Q_PROPERTY(int sortByOrder READ sortByOrder WRITE setSortByOrder USER true)
    Q_PROPERTY(int groupBy READ groupBy WRITE setGroupBy USER true)
    Q_PROPERTY(QString columns READ columns WRITE setColumns USER true)
    Q_PROPERTY(QString widths READ widths WRITE setWidths USER true)

    friend class ::NavigatorCellDelegate;
    friend class ::GroupByModel;
    friend class ::DiaryWindow;

    public:
        RideNavigator(MainWindow *);
        ~RideNavigator();

        void borderMenu(const QPoint &pos);

        // so the cell delegate can access
        QTreeView *tableView; // the view

    signals:

    public slots:
        void refresh();

        void showEvent(QShowEvent *event);

        void resizeEvent(QResizeEvent*);
        void showTreeContextMenuPopup(const QPoint &);

        // working with columns
        void columnsChanged();
        void removeColumn();
        void showColumnChooser();

        // user double clicked or pressed enter on ride
        void selectRide(const QModelIndex &index);

        // user selection here or on mainwindow's ride list
        void cursorRide();
        void selectRow();
        bool eventFilter(QObject *object, QEvent *e);
        void rideTreeSelectionChanged(); // watch main window

        // drop column headings from column chooser
        void dragEnterEvent(QDragEnterEvent *event);
        void dropEvent(QDropEvent *event);

        // how wide am I?
        void setWidth(int x);
        void setSortBy(int index, Qt::SortOrder);
        void setGroupByColumn();

        int sortByIndex() const { return _sortByIndex; }
        void setSortByIndex(int x) { _sortByIndex = x; }

        int sortByOrder() const { return _sortByOrder; }
        void setSortByOrder(int x) { _sortByOrder = x; }

        int groupBy() const { return _groupBy; }
        void setGroupBy(int x) { _groupBy = x; }
 
        QString columns() const { return _columns; }
        void setColumns(QString x) { _columns = x; }

        QString widths() const { return _widths; }
        void setWidths (QString x) { _widths = x; resetView(); } // only reset once widths are set

        void resetView(); // when columns/width changes

    protected:
        QSqlTableModel *sqlModel; // the sql table
        GroupByModel *groupByModel; // for group by
        BUGFIXQSortFilterProxyModel *sortModel; // for sort/filter

        // keep track of the headers
        QList<QString> logicalHeadings;
        QList<QString> visualHeadings;

        // this maps friendly names to metric names
        QMap <QString, QString> nameMap;
        QMap<QString, const RideMetric *> columnMetrics;

    private:
        MainWindow *main;
        bool active;
        bool init;
        int currentColumn;
        int pwidth;
        NavigatorCellDelegate *delegate;
        QVBoxLayout *mainLayout;

        // properties
        int _sortByIndex;
        int _sortByOrder;
        int _groupBy;
        QString _columns;
        QString _widths;
};

//
// Used to paint the cells in the ride navigator
//
class NavigatorCellDelegate : public QItemDelegate
{
    Q_OBJECT
    G_OBJECT


public:
    NavigatorCellDelegate(RideNavigator *, QObject *parent = 0);

    // These are all null since we don't allow editing
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // We increase the row height if there is a calendar text to display
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const ;

    // override stanard painter to use color config to paint background
    // and perform correct level of rounding for each column before displaying
    // it will also override the values returned from metricDB with in-core values
    // when the ride has been modified but not saved (i.e. data is dirty)
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setWidth(int x) { pwidth=x; }

private slots:

    void commitAndCloseEditor();

private:
    RideNavigator *rideNavigator;
    int pwidth;

};

//
// Column Chooser
//
// Used by the ride navigator, provides an array
// of pushbuttons representing each available field
// and lets the user drag them onto the RideNavigator
// to include into the column list. Similarly, the
// available columns can be dragged off the ride
// navigator and dropped back in the array
//
class ColumnChooser : public QWidget
{
    Q_OBJECT
    G_OBJECT


public:
    ColumnChooser(QList<QString>&columnHeadings);

public slots:
    void buttonClicked(QString name);

private:
    QGridLayout *buttons;
    QSignalMapper *clicked;
};
#endif