/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      A singleton wrapper for Logs views CAknNavigationDecorator objects (Tab groups). 
*      Each unique Tab group is only created if it doesn't already exist. This fixes 
*      error MVHA-6TJDMJ.
*
*
*/


// INCLUDE FILES
#include "CLogsNaviDecoratorWrapper.h"
#include <AknTabObserver.h>
#include <aknnavide.h>
#include <aknnavi.h>

#include <barsread.h>   // resource reader

#include "LogsUID.h"




// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsNaviDecoratorWrapper::CLogsNaviDecmioratorWrapper
// ----------------------------------------------------------------------------
//
CLogsNaviDecoratorWrapper::CLogsNaviDecoratorWrapper() : CCoeStatic( TUid::Uid( KLogsAppUID3 )), iNaviDecorator( NULL ), iCurrentNaviResource (0)
    {
    }
    
// ----------------------------------------------------------------------------
// CLogsNaviDecoratorWrapper::NewLC
// ----------------------------------------------------------------------------
//
CLogsNaviDecoratorWrapper* CLogsNaviDecoratorWrapper::NewLC()
    {  
    CLogsNaviDecoratorWrapper* self = new (ELeave) CLogsNaviDecoratorWrapper;
    CleanupStack::PushL(self);
    return self; 
    }


// ----------------------------------------------------------------------------
// CLogsNaviDecoratorWrapper::GetNaviDecorator
// 
// Getter for the iNaviDecorator member. 
// ----------------------------------------------------------------------------
//  
CAknNavigationDecorator* CLogsNaviDecoratorWrapper::GetNaviDecoratorL(CAknNavigationControlContainer* aNaviPane,
                                                                     MAknTabObserver* aTabObserver,
                                                                     TInt aResource)
    {   
    // If the iNaviDecorator is not yet created, or it is of wrong type
    // we need to create a new.
    if (iNaviDecorator == NULL || iCurrentNaviResource != aResource)
        {     
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC( reader, aResource );
        
        if (iNaviDecorator != NULL )
            {
            // deleting the old navidecorator also Pops it from the Navi Panes's control stack
            delete iNaviDecorator;
            iNaviDecorator = NULL;
            }
        
        iNaviDecorator = aNaviPane->CreateTabGroupL( reader, aTabObserver );
        
        CleanupStack::PopAndDestroy(); // reader
        
        // Store the current iNaviDecorator's resource id 
        iCurrentNaviResource = aResource;
        }
        
    // Push the naviDecorator to the Navi Pane's control stack 
    aNaviPane->PushL( *( iNaviDecorator ) ); 
    return iNaviDecorator;
    }
    
    
// ----------------------------------------------------------------------------
// CLogsNaviDecoratorWrapper::GetNaviDecorator
// 
// Static Getter for the singleton instance of CLogsNaviDecoratorWrapper class. 
// ----------------------------------------------------------------------------
//       
CLogsNaviDecoratorWrapper* CLogsNaviDecoratorWrapper::InstanceL()
    {
    CLogsNaviDecoratorWrapper* singleton = static_cast<CLogsNaviDecoratorWrapper*>
                    ( CCoeEnv::Static( TUid::Uid(KLogsAppUID3 ) ));
    if ( !singleton )
        {
        singleton = CLogsNaviDecoratorWrapper::NewLC();
        CleanupStack::Pop(singleton);
        }
    return singleton;
    
    }
   

// ----------------------------------------------------------------------------
// CLogsNaviDecoratorWrapper::~CLogsNaviDecoratorWrapper
//
// CCoeEnv handles the cleanup. Destructor called from CCoeEnv::DestroyEnvironmentStatic
// ----------------------------------------------------------------------------
//   
CLogsNaviDecoratorWrapper::~CLogsNaviDecoratorWrapper()
    {
    delete iNaviDecorator;
    }


// End of File
