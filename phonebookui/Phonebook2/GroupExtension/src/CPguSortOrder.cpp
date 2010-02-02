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
* Description:  Phonebook 2 group view sort order.
*
*/


#include "CPguSortOrder.h"

// Phonebook 2
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2GroupUIRes.rsg>
#include <Pbk2DataCaging.hrh>

// Virtual Phonebook
#include <CVPbkSortOrder.h>
#include <CVPbkContactManager.h>

// System includes
#include <barsread.h>

/// Unnamed namespace for local definitons
namespace {

_LIT(KPbk2GroupResFile, "Pbk2GroupUIRes.rsc");

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPreCond_NullPointer
        };

    void Panic(TInt aReason)
        {
        _LIT(KPanicText, "CPguSortOrder");
        User::Panic(KPanicText, aReason);
        }
#endif  // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPguSortOrder::CPguSortOrder
// --------------------------------------------------------------------------
//
CPguSortOrder::CPguSortOrder
        ( const MVPbkFieldTypeList& aMasterFieldTypeList ) :
            iMasterFieldTypeList ( aMasterFieldTypeList )
    {
    }

// --------------------------------------------------------------------------
// CPguSortOrder::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguSortOrder::ConstructL()
    {
    iSortOrder = CreateSortOrderL();
    }

// --------------------------------------------------------------------------
// CPguSortOrder::NewL
// --------------------------------------------------------------------------
//
CPguSortOrder* CPguSortOrder::NewL
        ( const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    CPguSortOrder* self =
        new ( ELeave ) CPguSortOrder ( aMasterFieldTypeList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguSortOrder::~CPguSortOrder
// --------------------------------------------------------------------------
//
CPguSortOrder::~CPguSortOrder()
    {
    delete iSortOrder;
    }

// --------------------------------------------------------------------------
// CPguSortOrder::FieldTypeCount
// --------------------------------------------------------------------------
//
TInt CPguSortOrder::FieldTypeCount() const
    {
    return iSortOrder->FieldTypeCount();
    }

// --------------------------------------------------------------------------
// CPguSortOrder::FieldTypeAt
// --------------------------------------------------------------------------
//
const MVPbkFieldType& CPguSortOrder::FieldTypeAt( TInt aIndex ) const
    {
    return iSortOrder->FieldTypeAt( aIndex );
    }

// --------------------------------------------------------------------------
// CPguSortOrder::ContainsSame
// --------------------------------------------------------------------------
//
TBool CPguSortOrder::ContainsSame( const MVPbkFieldType& aFieldType ) const
    {
    return iSortOrder->ContainsSame( aFieldType );
    }

// --------------------------------------------------------------------------
// CPguSortOrder::MaxMatchPriority
// --------------------------------------------------------------------------
//
TInt CPguSortOrder::MaxMatchPriority() const
    {
    return iSortOrder->MaxMatchPriority();
    }

// --------------------------------------------------------------------------
// CPguSortOrder::FindMatch
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CPguSortOrder::FindMatch
        ( const TVPbkFieldVersitProperty& aMatchProperty,
          TInt aMatchPriority ) const
    {
    return iSortOrder->FindMatch( aMatchProperty, aMatchPriority );
    }

// --------------------------------------------------------------------------
// CPguSortOrder::FindMatch
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CPguSortOrder::FindMatch
        ( TVPbkNonVersitFieldType aNonVersitType ) const
    {
    return iSortOrder->FindMatch( aNonVersitType );
    }

// --------------------------------------------------------------------------
// CPguSortOrder::Find
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CPguSortOrder::Find( TInt aFieldTypeResId ) const
    {
    return iSortOrder->Find( aFieldTypeResId );
    }

// --------------------------------------------------------------------------
// CPguSortOrder::CreateSortOrderL
// --------------------------------------------------------------------------
//
CVPbkSortOrder* CPguSortOrder::CreateSortOrderL()
    {
    TResourceReader reader;
    RPbk2LocalizedResourceFile resFile;
    resFile.OpenLC( KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR,
        KPbk2GroupResFile );
    HBufC8* buffer = resFile.AllocReadL( R_GROUP_DISPLAY_ORDER );
    CleanupStack::PopAndDestroy(); // resFile
    CleanupStack::PushL( buffer );
    reader.SetBuffer( buffer );

    CVPbkSortOrder* result = CVPbkSortOrder::NewL
        ( reader, iMasterFieldTypeList );

    CleanupStack::PopAndDestroy(); // AllocReadLC

    return result;
    }

// End of File
