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


// INCLUDES
#include "cmscontact.h"
#include "cmssession.h"
#include "cmscontactbase.h"
#include "cmscontactfielditem.h"
#include "cmscommondefines.h"

// ----------------------------------------------------
// CCmsContactBase::CCmsContactBase
// 
// ----------------------------------------------------
//
CCmsContactBase::CCmsContactBase( RCmsContact& aContact ) :
                                  CActive( EPriorityStandard ),
                                  iStreamDesc16( ( TText* )"", 0, 0 ),
                                  iStreamDesc8( ( TText8* )"", 0, 0 ),
                                  iContact( aContact )
    {
    
    }

// ----------------------------------------------------
// CCmsContactBase::CCmsContactBase
// 
// ----------------------------------------------------
//       
CCmsContactBase::CCmsContactBase( RCmsContact& aContact,
                                  TRequestStatus& aClientStatus ) :
                                  CActive( EPriorityStandard ),
                                  iStreamDesc16( ( TText* )"", 0, 0 ),
                                  iStreamDesc8( ( TText8* )"", 0, 0 ),
                                  iContact( aContact ),
                                  iClientStatus( &aClientStatus )
    {
    
    }

// ----------------------------------------------------
// CCmsContactBase::~CCmsContactBase
// 
// ----------------------------------------------------
//
CCmsContactBase::~CCmsContactBase()
    {
    Cancel(); 
    delete iStreamBuffer8;
    delete iStreamBuffer16;
    }

// ----------------------------------------------------
// CCmsContactBase::BaseConstructL
// 
// ----------------------------------------------------
//
void CCmsContactBase::BaseConstructL()
    {
    CActiveScheduler::Add( this );
    ReAllocBufferL( iBinary ? KStreamBufferDefaultSizeBinary : KStreamBufferDefaultSizeText );
    }

// ----------------------------------------------------
// CCmsContactBase::BaseConstructL
// 
// ----------------------------------------------------
//
void CCmsContactBase::ReAllocBufferL( TInt aBufferSize )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactFieldInfo::ReAllocBufferL() - Size: %d" ), aBufferSize );
    #endif
    if( !iBinary )
        {
        delete iStreamBuffer16;
        iStreamBuffer16 = NULL;
        iStreamBuffer16 = HBufC::NewL( aBufferSize );
        iStreamDesc16.Set( iStreamBuffer16->Des() );
        }
    else
        {
        delete iStreamBuffer8;
        iStreamBuffer8 = NULL;
        iStreamBuffer8 = HBufC8::NewL( aBufferSize );
        iStreamDesc8.Set( iStreamBuffer8->Des() );
        }
    }

// ----------------------------------------------------
// CCmsContactBase::StreamDesc8
// 
// ----------------------------------------------------
//
TPtr* CCmsContactBase::StreamDesc16()
    {
    return &iStreamDesc16;
    }

// ----------------------------------------------------
// CCmsContactBase::StreamDesc
// 
// ----------------------------------------------------
//
TPtr8* CCmsContactBase::StreamDesc8()
    {
    return &iStreamDesc8;
    }

// ----------------------------------------------------
// CCmsContactField::ConstructL
// 
// ----------------------------------------------------
//
TBool CCmsContactBase::IsBinary() const    
    {
    return iBinary;  
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::DoCancel
// 
// ----------------------------------------------------
//
void CCmsContactBase::HandleError()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactFieldInfo::HandleError()" ) );
    #endif
    if( iClientStatus )
        {
        User::RequestComplete( iClientStatus, iStatus.Int() );
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( " Client request completed with %d" ), iStatus.Int() );
        #endif
        }
    else
        {
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( " Client not active => ignore" ) );
        #endif
        }
    }

// ----------------------------------------------------
// CCmsContactBase::Activate
// 
// ----------------------------------------------------
//
TRequestStatus& CCmsContactBase::Activate()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactBase::Activate()" ) );
    #endif
    SetActive();
    if( iClientStatus )
        {
        iStatus = KRequestPending;
        *iClientStatus = KRequestPending;
        }
    return iStatus;  
    }

// ----------------------------------------------------
// CCmsContactFieldInfo::DoCancel
// 
// ----------------------------------------------------
//
void CCmsContactBase::DoCancel()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactBase::DoCancel()" ) );
    #endif
    if( NULL != iClientStatus )
        {
        User::RequestComplete( iClientStatus, KErrCancel );
        }
    else
        {
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( "  No request pending" ) );
        #endif
        }
    }




// End of File
