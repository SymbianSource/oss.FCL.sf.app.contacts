/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Internal defintions and utilites
*
*/



// INCLUDE FILES
#include "SimServerInternal.h"

#include <s32mem.h>
#include <VPbkSimCntFieldTypes.hrh>
#include <RVPbkStreamedIntArray.h>
#include "VPbkSimServerCommon.h"
#include "SimServerDebug.h"
#include "VPbkDebug.h"

// CONSTANTS
const TInt KVPbkMaxSimFieldTypes = 10;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// VPbkSimSrvUtility::CompleteRequest
// -----------------------------------------------------------------------------
//
void VPbkSimSrvUtility::CompleteRequest( const RMessage2& aMessage, 
    TInt aResult )
    {
    TInt handle = aMessage.Handle();
    if ( handle )
        {
#ifdef VPBK_ENABLE_DEBUG_PRINT
        // Print message information
        const TDesC& text = 
            VPbkSimServer::MessageInTextFormat( aMessage.Function() );
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimServer: session %x, complete request %S handle %d result %d"),
            aMessage.Session(), &text, handle, aResult );
#endif // VPBK_ENABLE_DEBUG_PRINT
        aMessage.Complete( aResult );
        }
    }

// -----------------------------------------------------------------------------
// VPbkSimSrvUtility::ReadFieldTypesL
// -----------------------------------------------------------------------------
//
void VPbkSimSrvUtility::ReadFieldTypesL( RVPbkSimFieldTypeArray& aTypeArray,
    const RMessage2& aMessage, TInt aSlot )
    {
    // Calculate some sensible max length for the sortarray data.
    TInt maxSize = sizeof( TInt ) + 
        KVPbkMaxSimFieldTypes * sizeof( TVPbkSimCntFieldType );
    TInt dataLength = aMessage.GetDesLengthL( aSlot );
    if ( dataLength <= 0 || dataLength > maxSize )
        {
        User::Leave( KErrArgument );
        }
    // Read the sortorder from the client
    HBufC8* data = HBufC8::NewLC( dataLength );
    TPtr8 ptr( data->Des() );
    aMessage.ReadL( aSlot, ptr );

    RDesReadStream stream( ptr );
    CleanupClosePushL( stream );
    stream >> aTypeArray;
    if ( aTypeArray.Count() == 0 )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( 2 ); // stream, data
    }
//  End of File  
