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

#include "cntserviceviewfactory.h"
#include "cntabstractview.h"


#include "cntserviceeditview.h"
#include "cntservicecontactselectionview.h"

#include "cntservicecontactcardview.h"
#include "cntservicecontactfetchview.h"
#include "cntserviceassigncontactcardview.h"


CntServiceViewFactory::CntServiceViewFactory( CntAbstractServiceProvider& aServiceProvider ) : 
CntDefaultViewFactory(),
mProvider( aServiceProvider )
{
}

CntServiceViewFactory::~CntServiceViewFactory()
{
}
    
CntAbstractView* CntServiceViewFactory::createView( int aViewId )
{
    switch ( aViewId )
    {
    // contact fetch service view (for eg. requested by messaging)
    case serviceContactFetchView:
        return new CntServiceContactFetchView( mProvider );

    // contact selection service view (selecting contact to edit when updating existing contact)
    case serviceContactSelectionView:
        return new CntServiceContactSelectionView( mProvider );
            
    case serviceEditView:
        return new CntServiceEditView( mProvider );

    case serviceContactCardView:
        return new CntServiceContactCardView( mProvider );
            
    case serviceAssignContactCardView:
        return new CntServiceAssignContactCardView( mProvider );

    default:
        return CntDefaultViewFactory::createView( aViewId );
    }
}
