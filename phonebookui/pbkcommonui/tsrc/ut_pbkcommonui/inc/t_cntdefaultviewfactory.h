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

#ifndef T_CNTDEFAULTVIEWFACTORY_H_
#define T_CNTDEFAULTVIEWFACTORY_H_

#include <QObject>

class CntDefaultViewFactory;

class T_CntDefaultViewFactory : public QObject
{
    Q_OBJECT
public:
    T_CntDefaultViewFactory();
    ~T_CntDefaultViewFactory();
    
private slots:
    void init();
    void cleanup();
    
    void testViewCreation();
    
private:
    CntDefaultViewFactory* mFactory;
};
#endif /* T_CNTDEFAULTVIEWFACTORY_H_ */
