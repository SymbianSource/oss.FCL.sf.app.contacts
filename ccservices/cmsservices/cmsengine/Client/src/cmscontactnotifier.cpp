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
* Description:  
*
*/


#include <e32std.h>
#include "cmscontact.h"
#include "cmssession.h"
#include "cmscontactfield.h"
#include "cmscontactfielditem.h"
#include "cmsnotificationhandlerapi.h"
#include "cmscontactnotifier.h"
#include "cmscommondefines.h"


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCmsContactNotifier::CCmsContactNotifier
// ----------------------------------------------------------
//
CCmsContactNotifier::CCmsContactNotifier( 
    RCmsContact& aContact, 
    MCmsNotificationHandlerAPI& aObserver,
    CCmsContactFieldItem::TCmsContactNotification aType )
    : CActive( CActive::EPriorityStandard), 
    iContact( aContact ),
    iStreamDesc( ( TText* )"", 0, 0 ), 
    iObserver( aObserver ),
    iPresenceStarted( EFalse ),
    iPhonebookStarted( EFalse ),
    iDestroyedPtr( NULL ),
    iType( aType ),
    iContactField( NULL )
    {
    }

// ----------------------------------------------------------
// CCmsContactNotifier::~CCmsContactNotifier
// ----------------------------------------------------------
//
CCmsContactNotifier::~CCmsContactNotifier()
    {        
    Cancel();
    delete iStreamBuffer;
    delete iContactField;     
    if ( iDestroyedPtr )
        {
        // We are called inside callback
        *iDestroyedPtr = ETrue;
        iDestroyedPtr = NULL;
        }
    }
    
// ----------------------------------------------------------
// CCmsContactNotifier::NewL
// ----------------------------------------------------------
//
CCmsContactNotifier* CCmsContactNotifier::NewL(
    RCmsContact& aContact, 
    MCmsNotificationHandlerAPI& aObs,
    CCmsContactFieldItem::TCmsContactNotification aType )
    {
    CCmsContactNotifier* self = new (ELeave) CCmsContactNotifier( aContact, aObs, aType );
    CleanupStack::PushL( self );
    self->ConstructL(   );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CCmsContactNotifier::ConstructL
// ---------------------------------------------------------------------------
//
void CCmsContactNotifier::ConstructL( )
    {
    CActiveScheduler::Add(this);        
    }
    
// ---------------------------------------------------------------------------
// CCmsContactNotifier::StartOrderL
// ---------------------------------------------------------------------------
//
void CCmsContactNotifier::StartOrderL( TBool aAskMore )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactNotifier::StartOrderL() - Ask more: %d" ), aAskMore );
    #endif 

    if ( IsActive() )
    	{
    	User::Leave( KErrInUse );
    	}
    else
        {        
        }
    
    if ( iType & CCmsContactFieldItem::ECmsPresenceAllNotification )
        {
        #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "  Presence notification ordered type=%d" ), iType );
        #endif
        StartPresenceNotifyL( aAskMore );             
        } 
    else if ( iType & CCmsContactFieldItem::ECmsPhonebookNotification )
        {
        #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "  Phonebook notification ordered" ) );
        #endif
        StartPhoneboookNotifyL( aAskMore );            
        }
    else
        {
        User::Leave( KErrNotSupported );
        }                      	    
    }

// ---------------------------------------------------------------------------
// CCmsContactNotifier::StartPhoneboookNotifyL
//
// ---------------------------------------------------------------------------
//
void CCmsContactNotifier::StartPhoneboookNotifyL( TBool aAskMore )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactNotifier::StartPhoneboookNotifyL()" ) );
    #endif
    if( !iPhonebookStarted )
        {
        delete iStreamBuffer; 
        iStreamBuffer = NULL;
        iStreamBuffer = HBufC::NewL( 5 );
        iStreamDesc.Set( iStreamBuffer->Des() );
        TIpcArgs arguments( iType, &iStreamDesc );
        if( !aAskMore )
            {
            iContact.SendMessage( ECmsOrderNotification, arguments, iStatus );         
            }
        else
            {
            iContact.SendMessage( ECmsOrderNextNotification, arguments, iStatus );      
            }
        iPhonebookStarted = ETrue;
        SetActive();
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( "  Notification started" ) );
        #endif
        }
    else
        {
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( "  Notification already started => ignore" ) );
        #endif
        }
    }

// ---------------------------------------------------------------------------
// CCmsContactNotifier::StartPresenceNotifyL
//
// ---------------------------------------------------------------------------
//
void CCmsContactNotifier::StartPresenceNotifyL( TBool aAskMore )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactNotifier::StartPresenceNotifyL()" ) );
    #endif
    delete iContactField;
    iContactField = NULL;    
    iContactField = CCmsContactField::NewL( iContact, iStatus, 
                    CCmsContactFieldItem::ECmsPresenceData ); 
    TIpcArgs arguments( iType, iContactField->StreamDesc8() );
    if( !aAskMore )
        {
        iContact.SendMessage( ECmsOrderNotification, arguments, iContactField->Activate() );         
        }
    else
        {
        iContact.SendMessage( ECmsOrderNextNotification, arguments, iContactField->Activate() );         
        }  
    SetActive();    
    }
        
// ----------------------------------------------------------
// CCmsContactNotifier::RunL
// ----------------------------------------------------------
//
void CCmsContactNotifier::RunL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactNotifier::RunL() - Status: %d" ), iStatus.Int() );
    #endif 
    TInt pbEvent = KErrGeneral;
    TInt status = iStatus.Int();
    if( status )
        {
        RunError( status );
        }
    else
        {
        // Set the member to point to stack variable
        TBool destroyed( EFalse );
        iDestroyedPtr = &destroyed;
        if ( CCmsContactFieldItem::ECmsPresenceAllNotification & Type() )
            {
            TRAP_IGNORE( iObserver.HandlePresenceNotificationL( iContactField ) );
            }
        else if( CCmsContactFieldItem::ECmsPhonebookNotification == Type() )
            {
            TLex lex( iStreamDesc );
            TInt event = KErrNotFound;
            lex.Val( event );
            iPhonebookStarted = EFalse;            
            TRAP_IGNORE( iObserver.HandlePhonebookNotificationL( ( TCmsPhonebookEvent  )event ) );
            pbEvent = event;
            }
        else
            {
            RunError( KErrNotSupported );
            }
        if ( !destroyed )
            {
            iDestroyedPtr = NULL;
            }
        CheckRestartL( destroyed, ( TCmsPhonebookEvent  )pbEvent );
        }
    }

// ----------------------------------------------------------
// CCmsContactNotifier::CheckRestartL
//
// ----------------------------------------------------------
//
void CCmsContactNotifier::CheckRestartL( TBool aDestroyed, TCmsPhonebookEvent aEvent )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactNotifier::CheckRestartL()" ) );
    #endif
    //If the object has been deleted inside the callback or
    //if the contact handle has been deleted in the server,
    //notifications must not be re-ordered
    if( !aDestroyed && ECmsContactDeleted != aEvent )
        {
        // Ownership is transferred
        iContactField = NULL;                 
        StartOrderL( ETrue );
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( "  Notification renewed" ) );
        #endif
        }         
    } 
   
// ----------------------------------------------------------
// CCmsContactNotifier::RunError
// ----------------------------------------------------------
//
TInt CCmsContactNotifier::RunError( TInt aError )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactNotifier::RunError() - Error: %d" ), aError );
    #endif       
    // Save this since we ask to delete ourself and members cannot be used anymore.
    MCmsNotificationHandlerAPI& observer = iObserver; 
    CCmsContactFieldItem::TCmsContactNotification myType = Type();    
    iContact.DeleteNotifier( myType );
    observer.CmsNotificationTerminatedL( aError, myType );
    return KErrNone;          
    }          

// ----------------------------------------------------------
// CCmsContactNotifier::DoCancel
// ----------------------------------------------------------
//
void CCmsContactNotifier::DoCancel()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactNotifier::DoCancel()" ) );
    #endif
    TIpcArgs arguments( iType );
    iContact.SendMessage( ECmsCancelNotification, arguments );
    if ( CCmsContactFieldItem::ECmsPresenceAllNotification & Type() )
        {
        iContactField->Cancel();
        }
    }

// ----------------------------------------------------------
//
CCmsContactFieldItem::TCmsContactNotification CCmsContactNotifier::Type()
    {
    return iType;
    }
        
        

