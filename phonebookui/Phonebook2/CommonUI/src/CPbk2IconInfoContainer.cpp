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
* Description:  Phonebook 2 icon info container.
*
*/


// INCLUDE FILES
#include <CPbk2IconInfoContainer.h>

// Phonebook 2
#include "CPbk2IconInfo.h"

// System includes
#include <barsread.h>
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>

// Unnamed namespace for local definitions
namespace {

/**
 * Creates a resource reader.
 *
 * @param aReader       Will contain a filled reader.
 * @param aResourceId   Id of the resource to be loaded.
 * @param aCoeEnv       Coe environment to be used for resource reading.
 */
void CreateReaderLC( 
        TResourceReader& aReader,  
        TInt aResourceId,
        CCoeEnv* aCoeEnv = NULL )
    {
    CCoeEnv* coeEnv = aCoeEnv;

    if ( !coeEnv )
        {
        coeEnv = CCoeEnv::Static();
        }

    coeEnv->CreateResourceReaderLC( aReader, aResourceId );
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::CPbk2IconInfoContainer
// --------------------------------------------------------------------------
//
inline CPbk2IconInfoContainer::CPbk2IconInfoContainer()
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::~CPbk2IconInfoContainer
// --------------------------------------------------------------------------
//
CPbk2IconInfoContainer::~CPbk2IconInfoContainer()
    {
    iInfoArray.ResetAndDestroy();
    delete iMbmFileNames;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconInfoContainer* CPbk2IconInfoContainer::NewL
        ( TResourceReader& aReader )
    {
    CPbk2IconInfoContainer* self = new( ELeave ) CPbk2IconInfoContainer;
    CleanupStack::PushL(self);
    self->ConstructL( aReader );
    CleanupStack::Pop(self);
    return self;
    }


// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconInfoContainer* CPbk2IconInfoContainer::NewL
        ( TInt aResourceId, CCoeEnv* aCoeEnv /*=NULL*/ )
    {
    CPbk2IconInfoContainer* self = new( ELeave ) CPbk2IconInfoContainer;
    CleanupStack::PushL(self);
    TResourceReader reader;
    CreateReaderLC(reader, aResourceId, aCoeEnv);
    self->ConstructL( reader );
    CleanupStack::PopAndDestroy(); // reader (buffer)
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconInfoContainer* CPbk2IconInfoContainer::NewL()
    {
    CPbk2IconInfoContainer* self = new( ELeave ) CPbk2IconInfoContainer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconInfoContainer::ConstructL( TResourceReader& aReader )
    {
    // Not so many MBM files
    const TInt granularity = 4;
    iMbmFileNames = new( ELeave ) CDesCArrayFlat( granularity );
    AppendIconsFromResourceL( aReader );

    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconInfoContainer::ConstructL()
    {
    // Not so many MBM files
    const TInt granularity = 4;
    iMbmFileNames = new( ELeave ) CDesCArrayFlat( granularity );
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::Find
// --------------------------------------------------------------------------
//
EXPORT_C const CPbk2IconInfo* CPbk2IconInfoContainer::Find
        ( const TPbk2IconId& aIconId ) const
    {
    TInt index = FindInfo( aIconId );
    if ( index == KErrNotFound )
        {
        return NULL;
        }
    return iInfoArray[index];
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::AppendIconsFromResourceL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconInfoContainer::AppendIconsFromResourceL
        ( TInt aResourceId )
    {
    TResourceReader reader;
    CreateReaderLC(reader, aResourceId);
    AppendIconsFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::AppendIconsFromResourceL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconInfoContainer::AppendIconsFromResourceL
        ( TResourceReader& aReader )
    {
    // Read from resource
    TInt count = aReader.ReadInt16();
    while (count-- > 0)
        {
        CPbk2IconInfo* iconInfo =
                CPbk2IconInfo::NewLC( aReader, *iMbmFileNames );
        AppendIconL( iconInfo );
        CleanupStack::Pop( iconInfo );
        }
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::AppendIconL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconInfoContainer::AppendIconL(
        CPbk2IconInfo* aIconInfo )
    {
    const TInt index = FindInfo( aIconInfo->Pbk2IconId() );
    if ( index != KErrNotFound )
        {
        // Delete existing entry in info array
        delete iInfoArray[index];
        iInfoArray.Remove( index );
        }
    iInfoArray.AppendL( aIconInfo );
    }

// --------------------------------------------------------------------------
// CPbk2IconInfoContainer::FindInfo
// --------------------------------------------------------------------------
//
TInt CPbk2IconInfoContainer::FindInfo( const TPbk2IconId& aIconId ) const
    {
    TInt ret = KErrNotFound;

    const TInt count = iInfoArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iInfoArray[i]->Pbk2IconId() == aIconId )
            {
            ret = i;
            break;
            }
        }

    return ret;
    }

// End of File
