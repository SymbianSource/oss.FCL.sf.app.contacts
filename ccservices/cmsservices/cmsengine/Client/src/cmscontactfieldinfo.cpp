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
#include "cmscontactfieldinfo.h"
#include "cmscommondefines.h"

// ----------------------------------------------------
// CCmsContactFieldInfo::CCmsContactFieldInfo
// 
// ----------------------------------------------------
//
CCmsContactFieldInfo::CCmsContactFieldInfo( RCmsContact& aContact ) :
                                            CCmsContactBase( aContact )
    {
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::CCmsContactFieldInfo
// 
// ----------------------------------------------------
//
CCmsContactFieldInfo::CCmsContactFieldInfo( RCmsContact& aContact,
                                            TRequestStatus& aClientStatus ) :
                                            CCmsContactBase( aContact, aClientStatus )
    {
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::NewL
// 
// ----------------------------------------------------
//
CCmsContactFieldInfo* CCmsContactFieldInfo::NewL( RCmsContact& aContact )
    {
    CCmsContactFieldInfo* self = new ( ELeave ) CCmsContactFieldInfo( aContact );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;       
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::NewL
// 
// ----------------------------------------------------
//
CCmsContactFieldInfo* CCmsContactFieldInfo::NewL( RCmsContact& aContact, TRequestStatus& aClientStatus )
    {
    CCmsContactFieldInfo* self = new ( ELeave ) CCmsContactFieldInfo( aContact, aClientStatus );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::Fields
// 
// ----------------------------------------------------
//
EXPORT_C const RArray<CCmsContactFieldItem::TCmsContactField>& CCmsContactFieldInfo::Fields() const
    {
    return iEnabledFields;
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::NewL
// 
// ----------------------------------------------------
//    
void CCmsContactFieldInfo::ConstructL()
    {
    iBinary = EFalse;
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::NewL
// 
// ----------------------------------------------------
//
EXPORT_C CCmsContactFieldInfo::~CCmsContactFieldInfo()
    {
    iEnabledFields.Close();
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::RunL
// 
// ----------------------------------------------------
//
void CCmsContactFieldInfo::RunL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactFieldInfo::RunL() - Error: %d" ), iStatus.Int() );
    #endif
    if( KErrNone == iStatus.Int() )
        {
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( " Enabled fields fetched OK." ) );
        #endif
        const TChar delimiter = 32;
        for( TInt i = 0;iStreamDesc16.Length() > 0;i++ )
            {
            TUint fieldId = 0;
            TPtrC idDesc( _L( "" ) );
            TInt index = iStreamDesc16.Locate( delimiter );
            index < 0 ? idDesc.Set( iStreamDesc16 ) :
                        idDesc.Set( iStreamDesc16.Left( index ) );
            TLex luthor( idDesc );
            User::LeaveIfError( luthor.Val( fieldId ) );
            CCmsContactFieldItem::TCmsContactField field = 
                ( CCmsContactFieldItem::TCmsContactField )fieldId;
            User::LeaveIfError( iEnabledFields.Append( field ) );
            index > 0 ? iStreamDesc16.Delete( 0, index + 1 ) :
                        iStreamDesc16.Delete( 0, iStreamDesc16.Length() );
            }
        User::RequestComplete( iClientStatus, KErrNone );   
        }
    else if( iStatus > 0 )
        {
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( " Not enough memory allocated => Reallocate" ) );
        #endif
        ReAllocBufferL( iStatus.Int() );
        TIpcArgs args( StreamDesc16() );
        TInt operation( ( TInt )ECmsFetchEnabledFields );
        iContact.ResendReceive( operation, Activate(), args );
        }
    else
        {   
        //Handle errors in the base class
        HandleError();
        }
    }

// End of File
