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

#ifndef T_CNTNAVIGATOR_H_
#define T_CNTNAVIGATOR_H_

#include <QtTest/QtTest>

class CntViewNavigator;
class T_NavigatorTest : public QObject
{
    Q_OBJECT
    
public:
    T_NavigatorTest();
    ~T_NavigatorTest();
    
private slots:
    void init();
    void cleanup();
    
    void testWithoutExceptions();
    void testWithExecptions();
    
    
private:
    CntViewNavigator* mNavigator;
};
#endif /* T_CNTNAVIGATOR_H_ */
