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
class CntMyCardSelectionView;

class TestCntMyCardSelectionView: public QObject
{
     Q_OBJECT

private slots:

    void initTestCase();
    void createClasses();

    void aboutToCloseView();
    void onListViewActivated();
   
    void cleanupTestCase();

private:

    CntMainWindow          *mWindow;
    CntViewManager         *mViewManager;
    CntMyCardSelectionView *mMyCardSelectionView;

 };