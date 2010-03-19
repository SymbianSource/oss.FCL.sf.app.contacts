/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

class CntModelProvider;

class TestCntModelProvider: public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void instance();
    void twoInstances();
    void release();
    void contactModel();
    void contactManager();
    void contactSimManager();

    void cleanupTestCase();

private:

    CntModelProvider *mModelProvider;
};

