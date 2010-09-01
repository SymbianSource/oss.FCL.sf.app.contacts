/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CFscContactActionMenuImpl.
*
*/


// INCUDES
#include "emailtrace.h"
#include <implementationproxy.h>
#include "cfsccontactactionservice.h"
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <avkon.hrh>
#include <AknsUtils.h>
#include <aknnotewrappers.h> 
#include <textresolver.h> 
#include <aknstyluspopupmenu.h>

#include "cfsccontactactionmenuimpl.h"
#include "fsccontactactionmenuuids.hrh"
#include "cfsccontactactionmenumodelimpl.h"
#include "tfsccontactactionmenuconstructparameters.h"
#include "cfsccontactactionmenuitemimpl.h"
#include "cfsccontactactionmenucasitemimpl.h"


const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KFscContactActionMenuImplImpUid, 
                                CFscContactActionMenuImpl::NewL )
    };
      
    
// ======== LOCAL FUNCTIONS ========

// ================= MEMBER FUNCTIONS =======================

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuImpl* CFscContactActionMenuImpl::NewL(
    TAny* aParams )
    {
    FUNC_LOG;

    TFscContactActionMenuConstructParameters* params = 
        reinterpret_cast< TFscContactActionMenuConstructParameters* >( aParams );

    CFscContactActionMenuImpl* self = 
        new (ELeave) CFscContactActionMenuImpl( *params );
    CleanupStack::PushL( self );
    self->ConstructL( *params );
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::~CFscContactActionMenuImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuImpl::~CFscContactActionMenuImpl()
    {
    FUNC_LOG;    
    StopWait();
    delete iWait;
    delete iDummyMenuItem;
    delete iModel;    
    delete iTextResolver;    
    CloseStylusPopup();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::ExecuteL
// ---------------------------------------------------------------------------
//
TFscActionMenuResult CFscContactActionMenuImpl::ExecuteL( 
    TFscContactActionMenuPosition /*aPosition*/, TInt /*aIndex*/, MFsActionMenuPositionGiver* aPositionGiver)
    {
    FUNC_LOG;
          
    // Initialise some member variables
    iExit = EFalse;
    iMenuResult = EFscMenuDismissed;
    iError = KErrNone;
    
    // Check if menu is empty
    if ( iModel->VisibleItemCount() > 0 )
        {
        // Execute menu        
        TRAP( iError, DoExecuteL( aPositionGiver ) );
        User::LeaveIfError( iError );
        }

    return iMenuResult;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::Dismiss
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::Dismiss( TBool aSlide )
    {
    FUNC_LOG;
    iExit = !aSlide;
    iMenuResult = EFscMenuDismissed;
    // cancel action execution if running
    // needs to be canceled to close select popup dialog
    if ( iIsActionExecuting )
        {
        iModel->Service()->CancelExecute();
        iIsActionExecuting = EFalse;
        }
     
    DoDismiss();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::Model
// ---------------------------------------------------------------------------
//
MFscContactActionMenuModel& CFscContactActionMenuImpl::Model()
    {
    FUNC_LOG;
    return *iModel; 
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FocusedItemIndex
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuImpl::FocusedItemIndex()
    {
    FUNC_LOG;
    TInt result = iSelectedItemIndex;
    return result;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FocusedItem
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItem& CFscContactActionMenuImpl::FocusedItem()
    {
    FUNC_LOG;
    TInt i = FocusedItemIndex();
    TInt err = KErrNone;
    
    CFscContactActionMenuItem* focusedItem = NULL;
    
    TRAP( err, focusedItem = &(iModel->ItemL( i )) );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        focusedItem = iDummyMenuItem;
        }
    
    return *focusedItem;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FadedComponent
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HandleGainingForeground()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FadedComponent
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HandleLosingForeground()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ProcessCommandL(TInt aCommandId)
    {
    FUNC_LOG;
    
    switch ( aCommandId )
        {
        case KErrCancel:
            {
            
            iMenuResult = EFscMenuDismissed;
            iSelectedItemIndex = KErrNotFound;
            break;
            }
            
        default:
            {
            iSelectedItemIndex = aCommandId;
            HandleItemClick(iSelectedItemIndex);
            //iMenuResult = EFscCustomItemSelected;
            break;
            }
        }
    
    StopWait();
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::CFscContactActionMenuImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuImpl::CFscContactActionMenuImpl( 
    const TFscContactActionMenuConstructParameters& /*aParams*/ ) 
    : CFscContactActionMenu(), 
      iSelectedItemIndex( 0 ), isExecuteCompleted( ETrue )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ConstructL(
    const TFscContactActionMenuConstructParameters& aParams )
    {
    FUNC_LOG;
    iWait = new ( ELeave ) CActiveSchedulerWait();
    iModel = CFscContactActionMenuModelImpl::NewL( aParams.iService );
    iTextResolver = CTextResolver::NewL( *CCoeEnv::Static() );
    iDummyMenuItem = CFscContactActionMenuItemImpl::NewL();
    iDummyMenuItem->SetImplementationUid( KNullUid );   
    }   


void CFscContactActionMenuImpl::DoExecuteL( MFsActionMenuPositionGiver* aPositionGiver )
    {
    FUNC_LOG;

    TPoint positionOfPopup( 0,0 );
    if ( aPositionGiver )
        {
        positionOfPopup = aPositionGiver->ActionMenuPosition();
        }
    
    LaunchStylusPopupMenuL( positionOfPopup );
    }


// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::DoDismiss
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::DoDismiss()
    {
    FUNC_LOG;   
    
   
    }

// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when QueryActionsL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::QueryActionsComplete()
    {
    FUNC_LOG;
    // CAS observer method implementation - not used in this component
    }

// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when QueryActionsL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::QueryActionsFailed( TInt /*aError*/ )
    {
    FUNC_LOG;
    // CAS observer method implementation - not used in this component
    }
    
// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when ExecuteL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ExecuteComplete()
    {
    FUNC_LOG;
    isExecuteCompleted = ETrue;
    iIsActionExecuting = EFalse;
    iMenuResult = EFscCasItemSelectedAndExecuted;
    DoDismiss();
    }
     
// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when ExecuteL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ExecuteFailed( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    isExecuteCompleted = ETrue;
    
    //Fix for: EJKA-7KAEVA
    if ( KErrCancel != iError )
        {
        // Show error note
        TPtrC buf;
        buf.Set( iTextResolver->ResolveErrorString( iError ) );
        if ( buf.Length() > 0 )
            {
            CAknWarningNote* note = new CAknWarningNote();
        
            if ( note )
                {
                // If error string was found and note execution is 
                // successfull iError will be KErrNone. Otherwise 
                // iError will be delivered to the client application
                TRAP( iError, note->ExecuteLD( buf ) );
                }
            else
                {
                iError = KErrNoMemory;
                }
            }
        }
    else
        {
        iError = KErrNone;
        iIsActionExecuting = EFalse;
        iMenuResult = EFscMenuDismissed;
        }
    
    DoDismiss();
    }
    

// --------------------------------------------------------------------------
// CPbk2AppViewBase::DoLaunchStylusPopupL
// --------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::DoLaunchStylusPopupL( const TPoint& aPoint )
    {
    iStylusPopup = CAknStylusPopUpMenu::NewL( this, aPoint, NULL );
    
    iStylusPopup->SetPosition( aPoint );
        
    TInt count = iModel->VisibleItemCount();    
    for ( TInt index =0; index < count; index++ )
        {
        CFscContactActionMenuItem* baseItem = NULL;
        TRAP_IGNORE( baseItem = &iModel->VisibleItemL( index ) );
        if ( baseItem )
            {
            iStylusPopup->AddMenuItemL( baseItem->MenuText(), index );
            }
        }   
    
    iStylusPopup->ShowMenu();
    }


// --------------------------------------------------------------------------
// CPbk2AppViewBase::CloseStylusPopup
// --------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::CloseStylusPopup()
    {
    if ( iStylusPopup )
        {
        delete iStylusPopup;
        iStylusPopup = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::LaunchStylusPopupMenuL
// --------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::LaunchStylusPopupMenuL( const TPoint& aPoint )
    {
    CloseStylusPopup();
    TRAPD( err, DoLaunchStylusPopupL( aPoint ) );
    if ( err )
        {
        CloseStylusPopup();
        User::Leave( err );
        }
    else
        {
        StartWait(); 
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::SetEmphasis
// --------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/)
    {
    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::HandleItemClick
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HandleItemClick( TInt selectedItemIndex )
    {
    FUNC_LOG;    
    
    TInt itemCount = iModel->VisibleItemCount();
    if ( selectedItemIndex >= 0 && selectedItemIndex < itemCount )
        {
        // Check type of selected item
        CFscContactActionMenuItem* baseItem = NULL;
        TRAP( iError, baseItem = &iModel->VisibleItemL( selectedItemIndex ) );
        
        if ( iError == KErrNone )
            {
            // Check type of selected item
            CFscContactActionMenuCasItemImpl* item =
                dynamic_cast<CFscContactActionMenuCasItemImpl*>( baseItem );
            if ( item != NULL )
                {
                iMenuResult = EFscCasItemSelectedAndExecuting;
                TUid actionUid = item->ImplementationUid();
                isExecuteCompleted = EFalse;
                iIsActionExecuting = ETrue;
                TRAP( iError,
                    iModel->Service()->ExecuteL( actionUid, this ) );
                }
            else
                {
                // Custom item is not executed by the menu
                iMenuResult = EFscCustomItemSelected;
                DoDismiss();
                }
            }

        if ( iError != KErrNone )
            {
            // Show error note
            TPtrC buf;
            buf.Set( iTextResolver->ResolveErrorString( iError ) );
            if ( buf.Length() > 0 )
                {
                CAknWarningNote* note = new CAknWarningNote();
                
                if ( note )
                    {
                    // If error string was found and note execution is 
                    // successfull iError will be KErrNone. Otherwise 
                    // iError will be delivered to the client application
                    TRAP( iError, note->ExecuteLD( buf ) );
                    }
                else
                    {
                    iError = KErrNoMemory;
                    }
                }
            }
        }
    else
        {
        iMenuResult = EFscMenuDismissed;
        }
        
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::StartWait
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::StartWait()
    {
    FUNC_LOG;
    iWait->Start();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::StopWait
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::StopWait()
    {
    FUNC_LOG;
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }
    
// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
        sizeof( TImplementationProxy );
    return ImplementationTable;
    }

