/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#include <QObject>


class CntMainWindow;
class CntViewManager;

#include "cntbaselistview.h"
#include "cntviewparameters.h"

class CntBaseListTestView : public CntBaseListView
{
    Q_OBJECT

public:
    CntBaseListTestView(CntViewManager *viewManager, QGraphicsItem *parent = 0):
        CntBaseListView(viewManager, parent) { }

    CntViewParameters::ViewId viewId() const { return CntViewParameters::noView; }

};

class TestCntBaseListView: public QObject
{
     Q_OBJECT

private slots:

    void initTestCase();
    void createClasses();

    void setupView();
    void activateView();
    void onLongPress();
    void onListViewActivated();

    void addItemsToLayout();

    void setBannerName();
    void bannerName();
    void banner();
    void isBannerVisible();

    void listLayout();
    void listView();

    void setDataModel();
    

    void clearDataModelFilter();

    void cleanupTestCase();

private:

    CntMainWindow       *mWindow;
    CntViewManager      *mViewManager;
    CntBaseListTestView *mBaseListView;

 };