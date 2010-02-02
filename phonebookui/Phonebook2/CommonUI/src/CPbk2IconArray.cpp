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
* Description:  An icon array for Phonebook 2 icons.
*
*/


// INCLUDE FILES
#include <CPbk2IconArray.h>

// Phonebook 2
#include <CPbk2IconFactory.h>
#include <CPbk2IconInfo.h>
#include <MPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <TPbk2AppIconId.h>
#include <fbs.h>

// System includes
#include <barsread.h>       // TResourceReader
#include <gulicon.h>        // CGulIcon

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KGranularity = 10;

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicLogic_ReadArrayL
    };

void Panic(TInt aReason)
    {
    _LIT( KPanicText, "CPbk2IconArray" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace


/**
 * An array that hides TPbk2IconId.h include from header.
 */
class CPbk2IconArray::CIdArray : public CArrayFixFlat<TPbk2IconId>
    {
    public: // Constructor

        /**
         * Constructor.
         */
        CIdArray();
    };

// --------------------------------------------------------------------------
// CPbk2IconArray::CIdArray::CIdArray
// --------------------------------------------------------------------------
//
CPbk2IconArray::CIdArray::CIdArray() :
        CArrayFixFlat<TPbk2IconId>( KGranularity )
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::CPbk2IconArray
// --------------------------------------------------------------------------
//
CPbk2IconArray::CPbk2IconArray() :
        CArrayPtrFlat<CGulIcon>( KGranularity ),
        iArrayId( EPbk2NullIconArrayId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::~CPbk2IconArray
// --------------------------------------------------------------------------
//
CPbk2IconArray::~CPbk2IconArray()
    {
    // Free contents of this array for leave-safety
    ResetAndDestroy();
    delete iIconFactory;
    delete iIdMap;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconArray* CPbk2IconArray::NewL( TResourceReader& aReader )
    {
    CPbk2IconArray* self = new( ELeave ) CPbk2IconArray;
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconArray* CPbk2IconArray::NewL(
        const MPbk2FieldPropertyArray& aFieldProperties )
    {
    CPbk2IconArray* self = new( ELeave ) CPbk2IconArray;
    CleanupStack::PushL( self );
    self->ConstructL( aFieldProperties );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconArray* CPbk2IconArray::NewL( TResourceReader& aReader,
        const CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2IconArray* self = new( ELeave ) CPbk2IconArray;
    CleanupStack::PushL( self );
    self->ConstructL( aReader, aIconInfoContainer );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::BaseConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::BaseConstructL()
    {
    iIdMap = new ( ELeave ) CIdArray;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::ConstructL()
    {
    BaseConstructL();
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC();
    AppendDefaultIconsL( *factory );
    CleanupStack::PopAndDestroy( factory );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::ConstructL( TResourceReader& aReader )
    {
    BaseConstructL();
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC();
    DoAppendIconsFromResourceL( aReader, *factory, ETrue );
    CleanupStack::PopAndDestroy( factory );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::ConstructL
        ( const MPbk2FieldPropertyArray& aFieldProperties )
    {
    BaseConstructL();
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC();
    AppendDefaultIconsL( *factory );

    const TInt count = aFieldProperties.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TPbk2IconId& id = aFieldProperties.At(i).IconId();
        if ( FindIcon( id ) == KErrNotFound )
            {
            ReserveMemoryL( 1 ); // Reserve space for new icon
            AppendIconWithMappingL( *factory, id );
            }
        }
    CleanupStack::PopAndDestroy( factory );

    iArrayId = EPbk2FieldTypeIconArrayId;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::ConstructL( TResourceReader& aReader,
        const CPbk2IconInfoContainer& aIconInfoContainer )
    {
    BaseConstructL();
    AppendIconsFromResourceL( aReader, aIconInfoContainer );
    }
    
// --------------------------------------------------------------------------
// CPbk2IconArray::Id
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2IconArrayId CPbk2IconArray::Id() const
    {
    return iArrayId;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::FindIcon
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2IconArray::FindIcon( const TPbk2IconId& aIconId ) const
    {
    const TInt count = iIdMap->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( (*iIdMap)[i] == aIconId )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::FindAndCreateIconL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2IconArray::FindAndCreateIconL
        ( const TPbk2IconId& aIconId )
    {
    TInt index = FindIcon( aIconId );
    if ( index == KErrNotFound )
        {
        if ( !iIconFactory )
            {
            iIconFactory = CPbk2IconFactory::NewL();
            }        
        ReserveMemoryL( 1 ); // reserve memory for the new icon
        AppendIconWithMappingL( *iIconFactory, aIconId );
        // last index
        index = Count() - 1;
        }
    else
        {
        // Delete factory it's not needed
        delete iIconFactory;
        iIconFactory = NULL;
        }
    return index;
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::AppendIconsFromResourceL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconArray::AppendIconsFromResourceL(
        TResourceReader& aReader,
        const CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC( aIconInfoContainer );
    DoAppendIconsFromResourceL( aReader, *factory, EFalse );
    CleanupStack::PopAndDestroy( factory );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::RefreshL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconArray::RefreshL()
    {
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC();
    DoRefreshL( *factory );
    CleanupStack::PopAndDestroy( factory );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::RefreshL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconArray::RefreshL(
        const CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC( aIconInfoContainer );
    DoRefreshL( *factory );
    CleanupStack::PopAndDestroy( factory );
    }
    
// --------------------------------------------------------------------------
// CPbk2IconArray::AppendIconL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconArray::AppendIconL( 
        CGulIcon* aIcon,  const TPbk2IconId& aIconId ) 
    {
    ReserveMemoryL( 1 ); // Reserve space for new icon and iconId
    AppendL( aIcon );
    iIdMap->AppendL( aIconId );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::AppendIconL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconArray::AppendIconL( CPbk2IconInfo* aIconInfo ) 
    {
    if ( !iIconFactory )
        {
        iIconFactory = CPbk2IconFactory::NewL();
        }        

    iIconFactory->AppendIconL(aIconInfo );
    AppendIconWithMappingL(*iIconFactory, aIconInfo->Pbk2IconId());
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::RemoveIcon
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2IconArray::RemoveIcon(
		const TPbk2IconId& aIconId )
	{
	// search the icon first
	const TInt count = iIdMap->Count();
	for ( TInt i = 0; i < count; ++i )
		{
		if ( (*iIdMap)[i] == aIconId )
			{
			//remove id from the array
			iIdMap->Delete( i );
			// get object
			CGulIcon* icon = At(i);
			delete icon;
			//remove pointer from the array 
			Delete( i );
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

// --------------------------------------------------------------------------
// CPbk2IconArray::AppendDefaultIconsL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::AppendDefaultIconsL(
        const CPbk2IconFactory& aIconFactory )
    {
    TPbk2AppIconId markId( EPbk2qgn_indi_marked_add );
    TPbk2AppIconId emptyId( EPbk2qgn_prop_nrtyp_empty );
    ReserveMemoryL( 2 ); // num of added icons
    AppendIconWithMappingL( aIconFactory, markId );
    AppendIconWithMappingL( aIconFactory, emptyId );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::DoAppendIconsFromResourceL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::DoAppendIconsFromResourceL( TResourceReader& aReader,
        const CPbk2IconFactory& aIconFactory, TBool aUpdateId )
    {
    TInt8 id = aReader.ReadInt8();
    if ( aUpdateId )
        {
        iArrayId = static_cast<TPbk2IconArrayId>( id );
        }

    TInt count = aReader.ReadInt16();
    ReserveMemoryL( count );

    while ( count-- > 0 )
        {
        TPbk2IconId iconId( aReader );
        AppendIconWithMappingL( aIconFactory, iconId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::AppendIconWithMappingL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::AppendIconWithMappingL
        ( const CPbk2IconFactory& aIconFactory, const TPbk2IconId& aIconId )
    {
    CGulIcon* gulIcon = aIconFactory.CreateIconLC( aIconId );
    __ASSERT_DEBUG(gulIcon, Panic(EPanicLogic_ReadArrayL));
    AppendL( gulIcon );
    CleanupStack::Pop( gulIcon );
    iIdMap->AppendL( aIconId );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::ReserveMemoryL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::ReserveMemoryL( TInt aAmount )
    {
    SetReserveL( Count() + aAmount );
    iIdMap->SetReserveL( iIdMap->Count() + aAmount );
    }

// --------------------------------------------------------------------------
// CPbk2IconArray::DoRefreshL
// --------------------------------------------------------------------------
//
void CPbk2IconArray::DoRefreshL( const CPbk2IconFactory& aFactory )
    {
    const TInt count = iIdMap->Count();
    for (TInt i = 0; i < count; ++i)
        {
        const TPbk2IconId& iconId = iIdMap->At(i);
        CGulIcon* gulIcon = aFactory.CreateIconL( iconId );
        if ( gulIcon )
            {
            // replace the old icon only if new one was loaded
            delete At(i);
            At(i) = gulIcon;
            }
        }
    }

//  End of File
