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

#ifndef CNTSERVICEVIEWFACTORY_H_
#define CNTSERVICEVIEWFACTORY_H_

#include "cntabstractviewfactory.h"

class CntAbstractView;
class CntServiceHandler;

class CntServiceViewFactory : public CntDefaultViewFactory
{
public:
    CntServiceViewFactory( CntServiceHandler* aService ); 
    ~CntServiceViewFactory();
    
public:
    CntAbstractView* createView( int aViewId );
    
private:
    CntServiceHandler* mService;
};
#endif /* CNTSERVICEVIEWFACTORY_H_ */
