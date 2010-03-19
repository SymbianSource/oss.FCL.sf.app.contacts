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
class CntEditView;

class TestCntEditView: public QObject
{
    Q_OBJECT
    
private slots:

    void initTestCase();
    void createView();


    void addSoftkeyAction();
    void signalMapper();
    void addActionsToToolBar();
    void addMenuItems();
    void activateView();
    void viewId();
    void prepareToEditContact();   
    void aboutToCloseView();
    void onListViewActivated();
    void deleteDetail();
    void onLongPressed();
    void addField();
    void openNameEditor();
    void handleExecutedCommand();
    void cleanupTestCase();

private:
    void cleanupClasses();
    void resetView();
    
private:

    CntMainWindow *mWindow;
    CntViewManager *mViewManager;
    CntEditView *mEditView;

};
