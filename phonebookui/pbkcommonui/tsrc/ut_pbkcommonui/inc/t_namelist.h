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

#ifndef T_NAMELIST_H_
#define T_NAMELIST_H_

#include <QObject>

class TestViewManager;
class CntNamesView;
class CntNamesViewPrivate;

class T_NameListTest : public QObject
{
    Q_OBJECT
    
public:
    T_NameListTest();
    ~T_NameListTest();
    
private slots:
    void init();
    void cleanup();
    void testPublicNamesList();
    
    void testBanner();
    void testFinder();
    void testKeyboard();
    
private:
    TestViewManager* mViewManager;
    CntNamesViewPrivate* mNamesPrivate;
};
#endif /* T_NAMELIST_H_ */
