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
*     Implements interface for Logs reader configuration
*
*/


// INCLUDE FILES
#include "CLogsReaderConfig.h"

// CONSTANTS


// ----------------------------------------------------------------------------
// CLogsReaderConfig::NewLC
// ----------------------------------------------------------------------------
//
CLogsReaderConfig* CLogsReaderConfig::NewLC( 
    const TLogsFilter aFilter,
    const TDesC* aNumber,
    const TDesC* aRemoteParty )
    {
    CLogsReaderConfig* self = new (ELeave) CLogsReaderConfig( aFilter );
    CleanupStack::PushL( self );
    self->ConstructL( aNumber, aRemoteParty, NULL );
    return self;
    }


// ----------------------------------------------------------------------------
// CLogsReaderConfig::NewLC
// ----------------------------------------------------------------------------
//
CLogsReaderConfig* CLogsReaderConfig::NewLC( const TDesC* aNumber ) 
    {
    CLogsReaderConfig* self = new (ELeave) CLogsReaderConfig;
    CleanupStack::PushL( self );
    self->ConstructL( aNumber, NULL, NULL );
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsReaderConfig::NewLC
// ----------------------------------------------------------------------------
//
CLogsReaderConfig* CLogsReaderConfig::NewLC( 
    const TDesC* aNumber,
    const TDesC* aRemoteParty, 
	const TDesC8* aContactLink )
    {
    CLogsReaderConfig* self = new (ELeave) CLogsReaderConfig;
    CleanupStack::PushL( self );
    self->ConstructL( aNumber, aRemoteParty, aContactLink );
    return self;
    }
// ----------------------------------------------------------------------------
// CLogsReaderConfig::~CLogsReaderConfig
// ----------------------------------------------------------------------------
//
CLogsReaderConfig::~CLogsReaderConfig()
    {
    delete iNumber;
    delete iRemoteParty;
    delete iContactLink;
    }

// ----------------------------------------------------------------------------
// CLogsReaderConfig::CLogsReaderConfig
// ----------------------------------------------------------------------------
//
CLogsReaderConfig::CLogsReaderConfig()
    {
    }

// ----------------------------------------------------------------------------
// CLogsReaderConfig::CLogsReaderConfig
// ----------------------------------------------------------------------------
//
CLogsReaderConfig::CLogsReaderConfig(
    const TLogsFilter aFilter ) :
        iFilter( aFilter )
    {
    }

// ----------------------------------------------------------------------------
// CLogsReaderConfig::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsReaderConfig::ConstructL(
    const TDesC* aNumber,
    const TDesC* aRemoteParty,
    const TDesC8* aContactLink )
    {
    if( aNumber )
        {
        iNumber = aNumber->AllocL();
        }
    else
        {
        iNumber = TPtrC( KNullDesC16 ).AllocL();
        }
    if( aRemoteParty )
        {
        iRemoteParty = aRemoteParty->AllocL();
        }
    else
        {
        iRemoteParty = TPtrC( KNullDesC16 ).AllocL();
        }
    if( aContactLink )
        {
        iContactLink = aContactLink->AllocL();
        }
    else
        {
        iContactLink = TPtrC8( KNullDesC8 ).AllocL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsReaderConfig::Filter
// ----------------------------------------------------------------------------
//
TLogsFilter CLogsReaderConfig::Filter() const
    {
    return iFilter;
    }
      
// ----------------------------------------------------------------------------
// CLogsReaderConfig::Number
// ----------------------------------------------------------------------------
//
TDesC& CLogsReaderConfig::Number() const
    {
    return *iNumber;
    }
       
// ----------------------------------------------------------------------------
// CLogsReaderConfig::RemoteParty
// ----------------------------------------------------------------------------
//
TDesC& CLogsReaderConfig::RemoteParty() const
    {
    return *iRemoteParty;
    }

// ----------------------------------------------------------------------------
// CLogsReaderConfig::ContactLink
// ----------------------------------------------------------------------------
//
TDesC8& CLogsReaderConfig::ContactLink() const
    {
    return *iContactLink;
    }
