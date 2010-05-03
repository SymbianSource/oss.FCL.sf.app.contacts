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
#include "cntservicesubeditview.h"
#include "cntservicecontactfetchview.h"
#include "cntservicecontactselectionview.h"

CntServiceViewFactory::CntServiceViewFactory(CntServiceHandler* aHandler) : 
CntDefaultViewFactory(),
mService( aHandler )
{
}

CntServiceViewFactory::~CntServiceViewFactory()
{
}
    
CntAbstractView* CntServiceViewFactory::createView( int aViewId )
{
    switch ( aViewId )
    {
    // contact fetch service view (fetching contacts from for example messaging)
    case serviceContactFetchView:
        return new CntServiceContactFetchView( mService );
    
    // contact selection service view (selecting contact to edit when updating existing contact)
    case serviceContactSelectionView:
        return new CntServiceContactSelectionView( mService );
            
    case serviceEditView:
        return new CntServiceEditView( mService );
        
    case serviceSubEditView:
        return new CntServiceSubEditView( mService );
    
    case serviceContactCardView:
        return new CntServiceEditView( mService );
            
    case serviceAssignContactCardView:
        return new CntServiceSubEditView( mService );
        
    default:
        return CntDefaultViewFactory::createView( aViewId );
    }
}
