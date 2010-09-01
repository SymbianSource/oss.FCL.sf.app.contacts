/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 UI Service IPC client.
*
*/


#include "RPbk2UIService.h"

// Phonebook 2
#include <Pbk2InternalUID.h>

// --------------------------------------------------------------------------
// RPbk2UIService::RPbk2UIService
// --------------------------------------------------------------------------
//
RPbk2UIService::RPbk2UIService() :
        iConfigurationPackagePtr( NULL, 0 ),
        iDataPackagePtr( NULL, 0 ),
        iInstructionsPtr( NULL, 0 ),
        iResultArraySizePtr( NULL, 0 ),
        iArrayResultsPtr( NULL, 0 ),
        iCanceledPtr( NULL, 0 ),
        iExitCommandIdPtr( NULL, 0 ),
        iCountPtr( NULL, 0 ),
        iContactLinkPtr( NULL, 0 ),
        iResultFieldSizePtr( NULL, 0 ),
        iFieldResultsPtr( NULL, 0 )
    {
    }

// --------------------------------------------------------------------------
// RPbk2UIService::LaunchAssignL
// --------------------------------------------------------------------------
//
void RPbk2UIService::LaunchAssignL
        ( TRequestStatus& aStatus,  HBufC8* aConfigurationPackage,
        HBufC8* aDataPackage, HBufC8* aAssignInstructions, TBool& aCanceled )
    {
    iConfigurationPackagePtr.Set( NULL, 0, 0 );
    if ( aConfigurationPackage )
        {
        iConfigurationPackagePtr.Set( aConfigurationPackage->Des() );
        }

    iDataPackagePtr.Set( NULL, 0, 0 );
    if ( aDataPackage )
        {
        iDataPackagePtr.Set( aDataPackage->Des() );
        }

    iInstructionsPtr.Set( NULL, 0, 0 );
    if ( aAssignInstructions )
        {
        iInstructionsPtr.Set( aAssignInstructions->Des() );
        }

    TPckg<TBool> canceled( aCanceled );
    iCanceledPtr.Set( canceled );

    SendReceive( EPbk2LaunchAssign,
        TIpcArgs( &iConfigurationPackagePtr, &iDataPackagePtr,
            &iInstructionsPtr, &iCanceledPtr ),
                aStatus );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::LaunchAttributeAssignL
// --------------------------------------------------------------------------
//
void RPbk2UIService::LaunchAttributeAssignL
        ( TRequestStatus& aStatus, HBufC8* aConfigurationPackage,
          TPbk2AttributeAssignData& aAttributePackage,
          HBufC8* aAssignInstructions, TBool& aCanceled )
    {
    iConfigurationPackagePtr.Set( NULL, 0, 0 );
    if ( aConfigurationPackage )
        {
        iConfigurationPackagePtr.Set( aConfigurationPackage->Des() );
        }

    TPckg<TPbk2AttributeAssignData> attributePackage( aAttributePackage );
    iDataPackagePtr.Set( attributePackage );

    iInstructionsPtr.Set( NULL, 0, 0 );
    if ( aAssignInstructions )
        {
        iInstructionsPtr.Set( aAssignInstructions->Des() );
        }

    TPckg<TBool> canceled( aCanceled );
    iCanceledPtr.Set( canceled );

    SendReceive( EPbk2LaunchAttributeAssign,
        TIpcArgs( &iConfigurationPackagePtr, &iDataPackagePtr,
            &iInstructionsPtr, &iCanceledPtr ),
                aStatus );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::LaunchAttributeUnassignL
// --------------------------------------------------------------------------
//
void RPbk2UIService::LaunchAttributeUnassignL
        ( TRequestStatus& aStatus, HBufC8* aConfigurationPackage,
          TPbk2AttributeAssignData aAttributePackage,
          HBufC8* aAssignInstructions, TBool& aCanceled )
    {
    iConfigurationPackagePtr.Set( NULL, 0, 0 );
    if ( aConfigurationPackage )
        {
        iConfigurationPackagePtr.Set( aConfigurationPackage->Des() );
        }

    TPckg<TPbk2AttributeAssignData> attributePackage( aAttributePackage );
    iDataPackagePtr.Set( attributePackage );

    iInstructionsPtr.Set( NULL, 0, 0 );
    if ( aAssignInstructions )
        {
        iInstructionsPtr.Set( aAssignInstructions->Des() );
        }

    TPckg<TBool> canceled( aCanceled );
    iCanceledPtr.Set( canceled );

    SendReceive( EPbk2LaunchAttributeUnassign,
        TIpcArgs( &iConfigurationPackagePtr, &iDataPackagePtr,
            &iInstructionsPtr, &iCanceledPtr ),
                aStatus );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::LaunchFetchL
// --------------------------------------------------------------------------
//
void RPbk2UIService::LaunchFetchL
        ( TRequestStatus& aStatus,  HBufC8* aConfigurationPackage,
          HBufC8* aFetchInstructions, TBool& aCanceled )
    {
    iConfigurationPackagePtr.Set( NULL, 0, 0 );
    if ( aConfigurationPackage )
        {
        iConfigurationPackagePtr.Set( aConfigurationPackage->Des() );
        }

    iInstructionsPtr.Set( NULL, 0, 0 );
    if ( aFetchInstructions )
        {
        iInstructionsPtr.Set( aFetchInstructions->Des() );
        }

    TPckg<TBool> canceled( aCanceled );
    iCanceledPtr.Set( canceled );

    SendReceive( EPbk2LaunchFetch,
        TIpcArgs( &iConfigurationPackagePtr, &iInstructionsPtr,
            &iCanceledPtr ), aStatus );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::CancelAssign
// --------------------------------------------------------------------------
//
void RPbk2UIService::CancelAssign
        ( TBool& aCanceled )
    {
    TPckg<TBool> canceled( aCanceled );
    iCanceledPtr.Set( canceled );

    SendReceive( EPbk2CancelAssign, TIpcArgs( &iCanceledPtr ) );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::CancelFetch
// --------------------------------------------------------------------------
//
void RPbk2UIService::CancelFetch
        ( TBool& aCanceled )
    {
    TPckg<TBool> canceled( aCanceled );
    iCanceledPtr.Set( canceled );

    SendReceive( EPbk2CancelFetch, TIpcArgs( &iCanceledPtr ) );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::GetResultsL
// --------------------------------------------------------------------------
//
void RPbk2UIService::GetResultsL( HBufC8& aArrayResults, TInt& aExtraData,
        HBufC& aFieldResults )
    {
    iArrayResultsPtr.Set( aArrayResults.Des() );
    TPckg<TInt> extraData( aExtraData );
    iFieldResultsPtr.Set( aFieldResults.Des() );
    
    User::LeaveIfError( SendReceive( EPbk2GetResults,
        TIpcArgs( &iArrayResultsPtr, &extraData, &iFieldResultsPtr ) ) );
    }

// --------------------------------------------------------------------------
// RPbk2UIService:GetResultSizeL
// --------------------------------------------------------------------------
//
void RPbk2UIService::GetResultSizeL( TInt& aArrayResultSize,
        TInt& aFieldResultSize )
    {
    TPckg<TInt> resultArraySize( aArrayResultSize );
    iResultArraySizePtr.Set( resultArraySize );
    
    TPckg<TInt> resultFieldSize( aFieldResultSize );
    iResultFieldSizePtr.Set( resultFieldSize );
    
    User::LeaveIfError( SendReceive( EPbk2GetResultSize,
        TIpcArgs( &iResultArraySizePtr, &iResultFieldSizePtr ) ) );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::ExitRequestL
// --------------------------------------------------------------------------
//
void RPbk2UIService::ExitRequestL
        ( TRequestStatus& aStatus, TInt& aExitCommandId )
    {
    TPckg<TInt> exitId( aExitCommandId );
    iExitCommandIdPtr.Set( exitId );
    SendReceive( EPbk2ExitRequest, TIpcArgs( &iExitCommandIdPtr ),
        aStatus );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::CancelExitRequest
// --------------------------------------------------------------------------
//
void RPbk2UIService::CancelExitRequest()
    {
    SendReceive( EPbk2CancelExitRequest );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::ExitServiceL
// --------------------------------------------------------------------------
//
void RPbk2UIService::ExitServiceL
        ( TBool aAcceptOutput, TInt aExitCommandId )
    {
    TPckg<TBool> acceptOutput( aAcceptOutput );
    TPckg<TInt> exitId( aExitCommandId );
    User::LeaveIfError( SendReceive( EPbk2ExitService,
        TIpcArgs( &acceptOutput, &exitId ) ) );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::AcceptRequestL
// --------------------------------------------------------------------------
//
void RPbk2UIService::AcceptRequestL
        ( TRequestStatus& aStatus, TInt& aNumContacts,
          HBufC8& aContactLink )
    {
    TPckg<TInt> numContacts( aNumContacts );
    iCountPtr.Set( numContacts );
    iContactLinkPtr.Set( aContactLink.Des() );
    SendReceive( EPbk2AcceptRequest,
        TIpcArgs( &iCountPtr, &iContactLinkPtr ), aStatus );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::CancelAcceptRequest
// --------------------------------------------------------------------------
//
void RPbk2UIService::CancelAcceptRequest()
    {
    SendReceive( EPbk2CancelAcceptRequest );
    }

// --------------------------------------------------------------------------
// RPbk2UIService::AcceptServiceL
// --------------------------------------------------------------------------
//
void RPbk2UIService::AcceptServiceL
        ( TBool aAcceptOutput, HBufC8* aContactLink )
    {
    TPckg<TBool> acceptOutput( aAcceptOutput );
    if ( aContactLink )
        {
        iContactLinkPtr.Set( aContactLink->Des() );
        User::LeaveIfError(
            SendReceive( EPbk2AcceptService,
            TIpcArgs( &acceptOutput, &iContactLinkPtr ) ) );
        }
    }

// --------------------------------------------------------------------------
// RPbk2UIService::ServiceUid
// --------------------------------------------------------------------------
//
TUid RPbk2UIService::ServiceUid() const
    {
    return TUid::Uid( KPbk2UIService );
    }

// End of File
