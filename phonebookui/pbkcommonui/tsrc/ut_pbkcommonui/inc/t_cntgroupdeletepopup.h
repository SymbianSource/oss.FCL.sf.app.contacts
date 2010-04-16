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
#include "cntgroupdeletepopup.h"

QTM_BEGIN_NAMESPACE
class QContactManager;
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class TestCntGroupDeletePopup: public QObject
{
     Q_OBJECT

private slots:

    void init();
   
    void populateListOfGroup();
    void deleteGroup();

    void cleanup();

private:    
    
    QContactManager         *mManager;
    CntGroupDeletePopup  *mPopup;
 };