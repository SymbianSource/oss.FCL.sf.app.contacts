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

#ifndef T_CNTDEFAULTVIEWMANAGER_H_
#define T_CNTDEFAULTVIEWMANAGER_H_

#include <QObject>

class CntDefaultViewManager;
class CntMainWindow;

class T_CntDefaultViewManager : public QObject
{
    Q_OBJECT
    
public:
    T_CntDefaultViewManager();
    ~T_CntDefaultViewManager();
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    void testChangeView();
    void testBack();
    void testGetContactManager();
    void testIsDepracatedView();
    
private:
    CntDefaultViewManager* mManager;
    CntMainWindow* mWindow;
};
#endif /* T_CNTDEFAULTVIEWMANAGER_H_ */
