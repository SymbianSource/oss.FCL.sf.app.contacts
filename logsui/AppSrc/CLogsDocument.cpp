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
*     Logs document class
*
*/


// INCLUDE FILES
#include <aknapp.h>

#include "CLogsDocument.h"
#include "CLogsEngine.h"
#include "CLogsAppUi.h"
#include "LogsUID.h"
#include <featmgr.h>    //Feature manager

#include "LogsDebug.h"
#include "LogsTraces.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsDocument::CLogsDocument
// ----------------------------------------------------------------------------
//
CLogsDocument::CLogsDocument( CAknApplication& aApp )
    : CAknDocument( aApp )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsDocument::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsDocument::ConstructL()
    {
    TRACE_ENTRY_POINT;
    CCoeEnv::Static()->AddForegroundObserverL( *this ); 
    iEngine = CLogsEngine::NewL();

    //Set up  FeatureManager
    //This must be called in the scope of the thread before calling
    //any other FeatureManager methods. 
    FeatureManager::InitializeLibL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsDocument::NewL
// ----------------------------------------------------------------------------
//
CLogsDocument* CLogsDocument::NewL( CAknApplication& aApp )
    {
    TRACE_ENTRY_POINT;
    CLogsDocument* self = new( ELeave ) CLogsDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  // self
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsDocument::~CLogsDocument
// ----------------------------------------------------------------------------
//
CLogsDocument::~CLogsDocument()
    {
    TRACE_ENTRY_POINT;
    CCoeEnv::Static()->RemoveForegroundObserver( *this );
    delete iEngine;

    //Free allocated FeatureManager
    FeatureManager::UnInitializeLib();

    //ECom clear up (done here  as in CLogsAppUi dtor is too early as view framework 
    //destroys the view only after appui has been destroyed).

    //Signal the destruction of interface implementation to ECOM.
    REComSession::DestroyedImplementation( TUid::Uid(ELogSettingsViewId) ); 

    //Direct users of ECOM plugins must call REComSession::FinalClose() method when all 
    //implementations they have created have been destroyed.
    REComSession::FinalClose();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsDocument::CreateAppUiL
// ----------------------------------------------------------------------------
//
CEikAppUi* CLogsDocument::CreateAppUiL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( new( ELeave ) CLogsAppUi( iEngine ) ); //Ownership  transferred to framework.
    }


// ----------------------------------------------------------------------------
// CLogsDocument::Engine
// ----------------------------------------------------------------------------
//
CLogsEngine* CLogsDocument::Engine()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEngine; 
    }

// ----------------------------------------------------------------------------
// CLogsDocument::HandleGainingForeground
// ----------------------------------------------------------------------------
//
void CLogsDocument::HandleGainingForeground()   //Non-leaving wrapper 
    {
    TRACE_ENTRY_POINT;
    TRAPD( err, HandleGainingForegroundL() );
    if( err ) 
        {
        CCoeEnv::Static()->HandleError( err );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsDocument::HandleGainingForegroundL
// ----------------------------------------------------------------------------
//
void CLogsDocument::HandleGainingForegroundL()  //Leaving 
    {
    TRACE_ENTRY_POINT;
    CLogsAppUi* ptr = STATIC_CAST(CLogsAppUi*, iAppUi);
   

    //Reset execution mode back to foreground
    ptr->SetExecutionMode( ELogsInForeground );
        
    //Update process name from the caption (calls eventually CLogsDocument::UpdateTaskNameL)        
    CEikonEnv::Static()->UpdateTaskNameL();     
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CLogsDocument::HandleLosingForeground
// ----------------------------------------------------------------------------
//
void CLogsDocument::HandleLosingForeground()    //Non-leaving wrapper
    {
    TRACE_ENTRY_POINT;
   
    TRAPD( err, HandleLosingForegroundL() );
    if( err ) 
        {
        CCoeEnv::Static()->HandleError( err );
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsDocument::HandleLosingForegroundL
// ----------------------------------------------------------------------------
//
void CLogsDocument::HandleLosingForegroundL()   //Leaving version
    {
    TRACE_ENTRY_POINT;
    //Update process name from the caption (calls eventually CLogsDocument::UpdateTaskNameL)        
    CEikonEnv::Static()->UpdateTaskNameL(); 
    
    CLogsAppUi* ptr = STATIC_CAST( CLogsAppUi*, iAppUi );
    
    if ( ptr->ExecutionMode() == ELogsInBackground_ExitOrEndPressed )
        {
        iEngine->FreeResourcesForBGModeL();  //Deletes objects to free some memory
        }
   
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsDocument::UpdateTaskNameL
// ----------------------------------------------------------------------------
//
void CLogsDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    TRACE_ENTRY_POINT;
    CEikDocument::UpdateTaskNameL( aWgName );   //Updates the task name from application's caption
    TRACE_EXIT_POINT;
    }

//  End of File

