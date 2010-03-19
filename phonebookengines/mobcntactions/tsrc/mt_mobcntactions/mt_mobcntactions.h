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
* Description: 
*
*/

#include <QObject>

#include <qtcontacts.h>

QTM_USE_NAMESPACE

class TestMobCntActions : public QObject
{
    Q_OBJECT

private slots:
    //Init/cleanup the test class
	void initTestCase();
	void cleanupTestCase();
	
	//Init/cleanup each test method
	void init();
	void cleanup();
	
	void emptyContactNoActionSupport();
	
	//call
	void phonenumberCallSupport();
	void phonenumberNoCallSupport();
	
	//message
	void phonenumberMessageSupport();
	void phonenumberNoMessageSupport();

	//video call
	void phonenumberVideoCallSupport();
	void phonenumberNoVideoCallSupport();
	
	//email
	void phonenumberEmailSupport();
	void phonenumberNoEmailSupport();

private:
    QContactManager *m_manager; 
};
