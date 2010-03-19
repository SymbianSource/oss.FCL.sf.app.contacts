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

class QContactManager;
class MobCntModel;

class TestMobCntModel : public QObject
{
    Q_OBJECT

private slots:
	void initTestCase();
	void create();
	
	void data();
	void rowCount();
	
	void contact();
	void indexOfContact();
	void contactManager();
	void setFilterAndSortOrder();
	void myCard();

	void rowId();
	void dataForDisplayRole();

	void updateContactIcon();
    void handleAdded();
    void handleChanged();
    void handleRemoved();
	void handleMyCardChanged();
	
	void cleanupTestCase();
	
private:
    QContactManager *mManager;
    MobCntModel     *mCntModel;
};
