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
#ifndef CNTABSTRACTVIEWFACTORY_H_
#define CNTABSTRACTVIEWFACTORY_H_

#include <QObject>
class CntAbstractView;
class HbMainWindow;

class CntAbstractViewFactory : public QObject
    {
    Q_OBJECT
    
public:
    virtual ~CntAbstractViewFactory() {}
    virtual CntAbstractView* createView( int aViewId ) = 0;
    };

/*!
 * Default implementation of the view factory
 * 
 */
class CntDefaultViewFactory : public CntAbstractViewFactory
    {
public:
    CntDefaultViewFactory();
    ~CntDefaultViewFactory();
    
public:
    CntAbstractView* createView( int aId );
    };
#endif /* CNTABSTRACTVIEWFACTORY_H_ */
