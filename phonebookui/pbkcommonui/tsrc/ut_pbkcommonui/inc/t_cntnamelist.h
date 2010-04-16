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

#ifndef T_CNTNAMELIST_H_
#define T_CNTNAMELIST_H_

#include <QObject>

class CntNamesView;
class TestViewManager;

class T_CntNameListTest : public QObject
{
    Q_OBJECT
    
public:
    T_CntNameListTest();
    ~T_CntNameListTest();
    
private slots:
    void init();
    void cleanup();
    
    void testNameList();
    void testCreateNewContact();
    void testShowPreviousView();
    void testShowCollectionView();
    void testShowFinder();
    void testShowContactCard();
    void testShowContextMenu();
   
private:
    CntNamesView* mView;
    TestViewManager* mManager;
};
#endif /* T_CNTNAMELIST_H_ */
