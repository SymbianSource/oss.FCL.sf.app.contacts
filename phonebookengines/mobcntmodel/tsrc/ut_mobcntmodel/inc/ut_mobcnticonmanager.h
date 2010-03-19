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

#include <QtTest/QtTest>
#include <QObject>

class MobCntIconManager;

class TestMobCntIconManager: public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
	void initTestCase();
    void cleanupTestCase();
    void testNonExistingIcon();
    void testOneExistingIcon();
    void testCancel();
    void testMultipleExistingIcon();
	
private:
	
    MobCntIconManager* mIconMgr; // SUT 
    QIcon mIcon;
  
};
