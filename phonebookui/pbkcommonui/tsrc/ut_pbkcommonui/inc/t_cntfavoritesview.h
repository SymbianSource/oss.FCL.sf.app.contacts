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
class CntFavoritesView;

class TestCntFavoritesView: public QObject
{
     Q_OBJECT

private slots:

    void initTestCase();
    void init();
    void cleanup();

    void openNamesList();
    void aboutToCloseView();
    void activateView();

private:

    CntMainWindow *mWindow;
    CntViewManager *mViewManager;
    CntFavoritesView *mFavoritesView;

 };
