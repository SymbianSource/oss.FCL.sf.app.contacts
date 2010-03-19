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
#include <qtcontacts.h>

class CntEditViewDetailItem;

class TestCntEditViewDetailItem : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    
    void create();
    void createPrimitives();
    void recreatePrimitives();
    void updatePrimitives();
    
    void setDetail();
    
    void detail();
    void fieldType();
    
    void onLongPress();
    
    void cleanupTestCase();

private:
    CntEditViewDetailItem   *mDetailItem;

};

// EOF
