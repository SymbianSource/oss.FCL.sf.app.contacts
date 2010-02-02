/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "cmscontact.h"
#include "cmssession.h"
#include "cmscontactfield.h"
#include "cmscontactfieldinfo.h"
#include "cmscommondefines.h"
#include "cmscontactnotifier.h"

// ----------------------------------------------------
// RCmsContact::RCmsContact
// 
// ----------------------------------------------------
//
EXPORT_C RCmsContact::RCmsContact() : iContactIdentifierPtr( NULL, 0 )
    {  
    }

// ----------------------------------------------------
// RCmsContact::~RCmsContact
// 
// ----------------------------------------------------
//
EXPORT_C RCmsContact::~RCmsContact()
    {
    iNotifiers.ResetAndDestroy();
    }

// ----------------------------------------------------
// RCmsContact::Open
// 
// ----------------------------------------------------
//
EXPORT_C TInt RCmsContact::Open( RCmsSession& aSession,
                                 TInt32 aContactId )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::Open( ID )" ) );
        RCmsSession::WriteToLog( _L8( "  Contact ID: %d" ), aContactId );
    #endif
    return CreateSubSession( aSession, ECmsOpenID, TIpcArgs( aContactId ) );
    }
        
// ----------------------------------------------------
// RCmsContact::Open
// 
// ----------------------------------------------------
//
EXPORT_C TInt RCmsContact::Open( RCmsSession& aSession,
                                 const TDesC8& aPackedLink )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::Open( LINK )" ) );
    #endif
    return CreateSubSession( aSession, ECmsOpenLink, TIpcArgs( &aPackedLink ) );
    }

// ----------------------------------------------------
// RCmsContact::Open
// 
// ----------------------------------------------------
//
EXPORT_C TInt RCmsContact::Open( RCmsSession& aSession,
                                 const TDesC& aPhoneNbr )
    {
    #ifdef _DEBUG
        HBufC8* phoneBuf = HBufC8::NewLC( aPhoneNbr.Length() );
        phoneBuf->Des().Copy( aPhoneNbr );
        TPtrC8 phoneDesc( phoneBuf->Des() );
        RCmsSession::WriteToLog( _L8( "RCmsContact::Open( MSISDN )" ) );
        RCmsSession::WriteToLog( _L8( "  Phone number: %S" ), &phoneDesc );
        CleanupStack::PopAndDestroy();  //phoneBuf
    #endif
    return CreateSubSession( aSession, ECmsOpenPhoneNbr, TIpcArgs( &aPhoneNbr ) );
    }

// ----------------------------------------------------
// RCmsContact::FieldDataL
// 
// ----------------------------------------------------
//
EXPORT_C CCmsContactField* RCmsContact::FieldDataL( TRequestStatus& aStatus,
                                                    CCmsContactFieldItem::TCmsContactField aFieldType )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::FieldDataL( SINGLE ): %d" ), aFieldType );
    #endif
    CCmsContactField* field = CCmsContactField::NewL( *this, aStatus, aFieldType );
    SendMessage( ECmsFetchContactField, *field, aFieldType );
    return field;
    }

// ----------------------------------------------------
// RCmsContact::EnabledFields
// 
// ----------------------------------------------------
//
EXPORT_C CCmsContactFieldInfo* RCmsContact::EnabledFieldsL( TRequestStatus& aStatus )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::EnabledFieldsL()" ) );
    #endif
    CCmsContactFieldInfo* info = CCmsContactFieldInfo::NewL( *this, aStatus );
    TIpcArgs arguments( info->StreamDesc8() );
    SendMessage( ECmsFetchEnabledFields, arguments, info->Activate() );
    return info;
    }

// ----------------------------------------------------
// RCmsContact::IsServiceAvailable
// 
// ----------------------------------------------------
//
EXPORT_C TInt RCmsContact::IsServiceAvailable( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aServiceType ) const
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::IsServiceAvailable() - Field: %d" ), aServiceType );
    #endif
    return SendReceive( ECmsFetchServiceAvailability, TIpcArgs( aServiceType ) );
    }

// ----------------------------------------------------
// RCmsContact::ContactStore
// 
// ----------------------------------------------------
//
EXPORT_C TCmsContactStore RCmsContact::ContactStore() const
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::ContactStore()" ) );
    #endif
    TInt result = SendReceive( ECmsFindParentStore, TIpcArgs() );
    return ( TCmsContactStore )result;
    }

// ----------------------------------------------------
// RCmsContact::FindExternalContact
// 
// ----------------------------------------------------
//
EXPORT_C void RCmsContact::FindExternalContact( TRequestStatus& aStatus ) const
    {   
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::FindExternalStore()" ) );
    #endif
    SendReceive( ECmsFindExternalContact, TIpcArgs(), aStatus );
    }

// ----------------------------------------------------
// RCmsContact::GetContactIdentifierL
// 
// ----------------------------------------------------
//
EXPORT_C HBufC8* RCmsContact::GetContactIdentifierL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::GetContactIdentifierL()" ) );
    #endif
            
    HBufC8* identifierbuffer = HBufC8::NewL( KStreamBufferDefaultSizeText );    
    iContactIdentifierPtr.Set( identifierbuffer->Des() );     
    
    //Currently only MVPbkContactLinkArray is supported.
    SendReceive(ECmsFetchContactIdentifier, TIpcArgs(&iContactIdentifierPtr));
           
    return identifierbuffer;
    }


// ----------------------------------------------------
// RCmsContact::Cancel
// 
// ----------------------------------------------------
//
EXPORT_C void RCmsContact::Cancel( CCmsContactBase& aContactBase )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::Cancel()" ) );
    #endif
    aContactBase.Cancel();
    Send(ECmsCancelOperation , TIpcArgs() );
    }

// ----------------------------------------------------
// RCmsContact::CancelExternalContactFind
// 
// ----------------------------------------------------
//
EXPORT_C void RCmsContact::CancelExternalContactFind() const
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::CancelExternalContactFind()" ) );
    #endif
    Send( ECmsCancelExternalContactFind, TIpcArgs() );
    }

// ----------------------------------------------------
// RCmsContact::Close
// 
// ----------------------------------------------------
//
EXPORT_C void RCmsContact::Close()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::Close()" ) );
    #endif
        
    iNotifiers.ResetAndDestroy();
        
    RSubSessionBase::CloseSubSession( ECmsCloseSubsession );
    }

// ----------------------------------------------------
// RCmsContact::Subscribe
// 
// ----------------------------------------------------
//
EXPORT_C void RCmsContact::OrderNotificationsL( 
        MCmsNotificationHandlerAPI* aHandler,
        CCmsContactFieldItem::TCmsContactNotification aNotificationType )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::OrderNotificationsL()" ) );
    #endif
    // We have to create separate notifiers for Presence and Phonebook events.
    // There are two separate queues in the CSM server, that's why.
    if ( aNotificationType & CCmsContactFieldItem::ECmsPresenceAllNotification )    
        {
        // mask phonebook bits off.
        TUint myMask = aNotificationType & CCmsContactFieldItem::ECmsPresenceAllNotification;
        CCmsContactNotifier* notifier = CreateNotifierL( aHandler, (CCmsContactFieldItem::TCmsContactNotification)myMask );  
        notifier->StartOrderL();          
        }
    
    if ( aNotificationType & CCmsContactFieldItem::ECmsPhonebookNotification )    
        {
        CCmsContactNotifier* notifier = CreateNotifierL( aHandler, CCmsContactFieldItem::ECmsPhonebookNotification );  
        notifier->StartOrderL();          
        }         
    }

// ----------------------------------------------------
// RCmsContact::UnSubscribe
// 
// ----------------------------------------------------
//

EXPORT_C void RCmsContact::CancelNotifications( 
        CCmsContactFieldItem::TCmsContactNotification aNotificationType )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::CancelNotifications()" ) );
    #endif
    DeleteNotifier( aNotificationType );
    }

// ----------------------------------------------------
// RCmsContact::UnSubscribe
// 
// ----------------------------------------------------
//
EXPORT_C TInt RCmsContact::GetContactActionFieldCount(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction)
    {
    return SendReceive( ECmsGetContactActionFieldCount, TIpcArgs( aContactAction ) );
    }

// ----------------------------------------------------
// RCmsContact::SendMessage
// 
// ----------------------------------------------------
//
void RCmsContact::SendMessage( TInt aServerMessage,
                               CCmsContactBase& aContactField,   
                               CCmsContactFieldItem::TCmsContactField aFieldType )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::SendMessage()" ) );
    #endif
    if( aContactField.IsBinary() )
        {
        SendReceive( aServerMessage, TIpcArgs(
                     aFieldType, aContactField.StreamDesc8() ), aContactField.Activate() );
        }
    else
        {
        SendReceive( aServerMessage, TIpcArgs(
                     aFieldType, aContactField.StreamDesc16() ), aContactField.Activate() );
        }
    }

// ----------------------------------------------------
// RCmsContact::SendMessage
// 
// ----------------------------------------------------
//
void RCmsContact::SendMessage( TInt aServerMessage,
                               CCmsContactBase& aContactField,   
                               CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::SendMessage()" ) );
    #endif
    if( aContactField.IsBinary() )
        {
        SendReceive( aServerMessage, TIpcArgs(
                     aFieldGroup, aContactField.StreamDesc8() ), aContactField.Activate() );
        }
    else
        {
        SendReceive( aServerMessage, TIpcArgs(
                     aFieldGroup, aContactField.StreamDesc16() ), aContactField.Activate() );
        }
    }

// ----------------------------------------------------
// RCmsContact::SendMessage
// 
// ----------------------------------------------------
//
void RCmsContact::SendMessage( TInt aServerMessage,
                               TIpcArgs& aArguments,
                               TRequestStatus& aStatus )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::SendMessage()" ) );
    #endif
    SendReceive( aServerMessage, aArguments, aStatus );
    }

// ----------------------------------------------------
// RCmsContact::SendMessage
// 
// ----------------------------------------------------
//
void RCmsContact::SendMessage( TInt aServerMessage,
                               TIpcArgs& aArguments )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::SendMessage()" ) );
    #endif
    SendReceive( aServerMessage, aArguments );
    }

// ----------------------------------------------------
// RCmsContact::ResendReceive
// 
// ----------------------------------------------------
//
void RCmsContact::ResendReceive( TInt aOperation )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::ResendReceive()" ) );
    #endif
    SendReceive( aOperation, TIpcArgs() );
    }

// ----------------------------------------------------
// RCmsContact::ResendReceive
// 
// ----------------------------------------------------
//
void RCmsContact::ResendReceive( TInt aOperation,
                                 TRequestStatus& aStatus,
                                 const TIpcArgs& aArguments )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "RCmsContact::ResendReceive()" ) );
    #endif
    SendReceive( aOperation, aArguments, aStatus );
    }

// ----------------------------------------------------
// RCmsContact::CreateNotifierL
// 
// ----------------------------------------------------
//
CCmsContactNotifier* RCmsContact::CreateNotifierL( 
        MCmsNotificationHandlerAPI* aHandler,
        CCmsContactFieldItem::TCmsContactNotification aNotificationType )
    {
    CCmsContactNotifier* current = NULL;    
    TInt count = iNotifiers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        current = iNotifiers[i];
        if ( current->Type() == aNotificationType )
            {
            User::Leave( KErrAlreadyExists );
            }
        }
    // create a new notifier
    current = CCmsContactNotifier::NewL( *this, *aHandler, aNotificationType);
    User::LeaveIfError( iNotifiers.Append( current ) );
    return current;
    } 

// ----------------------------------------------------
// RCmsContact::DeleteNotifier
// 
// ----------------------------------------------------
//
void RCmsContact::DeleteNotifier( 
        CCmsContactFieldItem::TCmsContactNotification aNotificationType )
    {
    CCmsContactNotifier* current = NULL;    
    TInt count = iNotifiers.Count();
    for ( TInt i = count-1; i >= 0; i-- )
        {
        current = iNotifiers[i];
        if ( current->Type() & aNotificationType )
            {
            iNotifiers.Remove(i);
            iNotifiers.Compress();            
            delete current;
            }
        }
    }


