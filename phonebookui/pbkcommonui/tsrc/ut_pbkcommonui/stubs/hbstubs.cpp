/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  HbMenu2 implementation.
*
*/


#include <hbmenu.h>
#include <hbdialog.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbnotificationdialog.h>
#include "hbstubs_helper.h"
#include <mobhistorymodel.h>

int actionCount = 0;
bool dialogOpened = false;
Qt::Orientation windowOrientation = Qt::Vertical;

HbMainWindow* testWindow = 0;
HbView* testView = 0;
int testViewCount = 0;
HbAction* testSoftkeyAction = 0;

TestViewManager::TestViewManager()
{
    mManager = new QContactManager();
}

TestViewManager::~TestViewManager()
{
    delete mManager;
}

void TestViewManager::changeView( const CntViewParameters& aArgs )
{
    Q_UNUSED( aArgs );
}

void TestViewManager::back( const CntViewParameters& aArgs )
{
    Q_UNUSED( aArgs );
}

QContactManager* TestViewManager::contactManager(const QString& aType)
{
    Q_UNUSED(aType);
    return mManager;
}
    
void HbStubHelper::reset()
{
    actionCount = 0;
    dialogOpened = false;
}

int HbStubHelper::widgetActionsCount()
{
    return actionCount;
}

bool HbStubHelper::notificationDialogOpened()
{
    return dialogOpened;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//

void HbNotificationDialog::launchDialog(const QString &text, QGraphicsScene *scene)
{
    Q_UNUSED(text)
    Q_UNUSED(scene)
    dialogOpened = true;
}

HbAction *HbMenu::exec(const QPointF &pos, HbAction *action )
{
   Q_UNUSED(action)
   Q_UNUSED(pos)
   return 0;
}

HbAction *HbMenu::addAction(const QString &text)
{
    Q_UNUSED(text)
    actionCount++;
    return new HbAction();
}

HbAction *HbDialog::exec()
{
   return 0;
}

void QGraphicsWidget::addAction(QAction *action)
{
    Q_UNUSED(action)
    actionCount++;
}

void HbWidget::clearActions()
{
    actionCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//


class MobHistoryModelData : public QSharedData
{
public:
    MobHistoryModelData(): QSharedData() {}
    ~MobHistoryModelData() {}
};

MobHistoryModel::MobHistoryModel(QContactLocalId contactId,
                                 QContactManager* manager,
                                 QObject *parent)
: QAbstractListModel(parent)                                 
{
    Q_UNUSED(contactId);
    Q_UNUSED(manager);
}

MobHistoryModel::~MobHistoryModel()
{
}
    
QVariant MobHistoryModel::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QStringList list;
    list << "title" << "message" << "time";
    return QVariant(list);
}

int MobHistoryModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 3;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbMainWindow::HbMainWindow(QWidget *parent, Hb::WindowFlags windowFlags) : d_ptr(0)
{
    Q_UNUSED(parent)
    Q_UNUSED(windowFlags)
    testViewCount = 0;
    testView = 0;
    testWindow = this;
}

HbMainWindow::~HbMainWindow()
{
   // testView = 0;
    testWindow = 0;
}
    
void HbMainWindow::setOrientation(Qt::Orientation orientation, bool animate)
{
		Q_UNUSED( animate )
		
    windowOrientation = orientation; 
}

Qt::Orientation HbMainWindow::orientation() const
{
    return windowOrientation;
}


QRectF HbMainWindow::layoutRect() const
{
    return QRectF(0, 0, 100, 100);
}

HbAction* HbMainWindow::softKeyAction(Hb::SoftKeyId key) const
{
    Q_UNUSED(key)
    return testSoftkeyAction;
}

void HbMainWindow::addSoftKeyAction(Hb::SoftKeyId key, HbAction *action)
{
    Q_UNUSED(key)
    Q_UNUSED(action)
    testSoftkeyAction = action;
}

void HbMainWindow::removeSoftKeyAction(Hb::SoftKeyId key, HbAction *action)
{
    Q_UNUSED(key)
    Q_UNUSED(action)
    testSoftkeyAction = 0;
}

HbView *HbMainWindow::addView(QGraphicsWidget *widget)
{
    Q_UNUSED(widget)
    testViewCount++;
}

void HbMainWindow::setCurrentView(HbView *view, bool animate,Hb::ViewSwitchFlags flags)
{
    Q_UNUSED(animate)
    Q_UNUSED(flags);
    testView = view;
}

void HbMainWindow::removeView(QGraphicsWidget *widget)
{
    Q_UNUSED(widget)
    testViewCount--;
}

int HbMainWindow::viewCount() const
{
    return testViewCount;
}
HbView *HbMainWindow::currentView() const
{
    return testView;
}

QList<HbView *> HbMainWindow::views() const
{
    QList<HbView *> result;
    for ( int i=0; i<testViewCount; ++i )
    {
       result.append(testView);
    }
    return result;
}