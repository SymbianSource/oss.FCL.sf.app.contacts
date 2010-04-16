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
#include "qmobilityglobal.h"

class CntMainWindow;
class CntViewManager;
class CntGroupMemberView;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class TestCntGroupMemberView: public QObject
{
     Q_OBJECT

private slots:

    void initTestCase();
    void createClasses();

    void aboutToCloseView();
    
    void handleExecutedCommand();

    void addActionsToToolBar();
    void deleteGroup();
    void removeFromGroup();
    void groupActions();
    void addMenuItems();
    void editContact();
    void onLongPressed();
    void onListViewActivated();
    void editGroup();
    void activateView();

    void cleanupTestCase();

private:

    CntMainWindow *mWindow;
    CntViewManager *mViewManager;
    CntGroupMemberView *mGroupMemberView;
    QContact *mContact;
    QContact *mGroupContact;

 };
