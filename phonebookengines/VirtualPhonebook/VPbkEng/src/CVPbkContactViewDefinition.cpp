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
* Description:  A view defintion for view creation.
*                see structure VPBK_CONTACT_VIEW in VPbkContactView.rh
*
*/


#include "CVPbkContactViewDefinition.h"

// Virtual Phonebook
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactSelector.h>

// System includes
#include <barsread.h>


// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::CVPbkContactViewDefinition
// ---------------------------------------------------------------------------
//
CVPbkContactViewDefinition::CVPbkContactViewDefinition()
        :   iSharing( EVPbkViewSharingUndefined )
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::~CVPbkContactViewDefinition
// ---------------------------------------------------------------------------
//
CVPbkContactViewDefinition::~CVPbkContactViewDefinition()
    {
    iSubViews.ResetAndDestroy();
    delete iUri;
    delete iName;
    delete iFilter;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactViewDefinition* CVPbkContactViewDefinition::NewL(
        TResourceReader& aReader)
    {
    CVPbkContactViewDefinition* self = NewLC( aReader );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactViewDefinition* CVPbkContactViewDefinition::NewL()
    {
    CVPbkContactViewDefinition* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactViewDefinition* CVPbkContactViewDefinition::NewL(
        const CVPbkContactViewDefinition& aViewDef )
    {
    CVPbkContactViewDefinition* self = NewLC( aViewDef );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactViewDefinition* CVPbkContactViewDefinition::NewLC(
        TResourceReader& aReader)
    {
    CVPbkContactViewDefinition* self = new(ELeave) CVPbkContactViewDefinition;
    CleanupStack::PushL(self);
    self->ConstructFromResourceL(aReader);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactViewDefinition* CVPbkContactViewDefinition::NewLC()
    {
    CVPbkContactViewDefinition* self = new(ELeave) CVPbkContactViewDefinition;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactViewDefinition* CVPbkContactViewDefinition::NewLC(
        const CVPbkContactViewDefinition& aViewDef )
    {
    CVPbkContactViewDefinition* self = new(ELeave) CVPbkContactViewDefinition;
    CleanupStack::PushL(self);
    self->ConstructL(aViewDef);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkContactViewDefinition::ConstructL()
    {
    InitializeSharing();
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkContactViewDefinition::ConstructL
        ( const CVPbkContactViewDefinition& aViewDef )
    {
    if (aViewDef.iUri)
        {
        iUri = aViewDef.iUri->AllocL();
        }
    if (aViewDef.iName)
        {
        iName = aViewDef.iName->AllocL();
        }
    iType = aViewDef.iType;
    iId = aViewDef.iId;
    iFlags = aViewDef.iFlags;
    iSortPolicy = aViewDef.iSortPolicy;

    const TInt count = aViewDef.SubViewCount();
    for (TInt i = 0; i < count; ++i)
        {
        CVPbkContactViewDefinition* subView =
            CVPbkContactViewDefinition::NewL( aViewDef.SubViewAt( i ) );
        CleanupStack::PushL( subView );
        iSubViews.AppendL( subView );
        CleanupStack::Pop( subView );
        }

    if ( aViewDef.iFilter )
        {
        iFilter = CVPbkFieldTypeSelector::NewL
            ( aViewDef.iFilter->FieldTypeList() );
        // No copy constructor currently available,
        // so lets initialize by internalizing
        HBufC8* filterBuf = aViewDef.iFilter->ExternalizeLC();
        iFilter->InternalizeL( *filterBuf );
        CleanupStack::PopAndDestroy(); // filterBuf
        }

    iContactSelector = aViewDef.iContactSelector; // not owned

    iSharing = aViewDef.iSharing;

    InitializeSharing();
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::ConstructFromResourceL
// ---------------------------------------------------------------------------
//
void CVPbkContactViewDefinition::ConstructFromResourceL
        ( TResourceReader& aReader )
    {
    TUint8 version = aReader.ReadUint8();

    iId = KVPbkDefaultViewId;
    if( version >= EVPbkContactViewV3 )
	    {
	    iId = aReader.ReadInt16();
		}

    iUri = aReader.ReadHBufC16L();
    iName = aReader.ReadHBufC16L();
    iType = TVPbkContactViewType(aReader.ReadUint16());
    iFlags = aReader.ReadUint32();
    iSortPolicy = TVPbkContactViewSortPolicy(aReader.ReadInt16());

    const TInt count = aReader.ReadInt16();
    for (TInt i = 0; i < count; ++i)
        {
        CVPbkContactViewDefinition* subView =
            CVPbkContactViewDefinition::NewL( aReader );
        CleanupStack::PushL( subView );
        User::LeaveIfError( iSubViews.Append( subView ) );
        CleanupStack::Pop( subView );
        }

    if ( version >= EVPbkContactViewV2 )
        {
        iSharing = TVPbkContactViewSharing( aReader.ReadUint16() );
        }

    InitializeSharing();
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SubViewCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVPbkContactViewDefinition::SubViewCount() const
    {
    return iSubViews.Count();
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SubViewAt
// ---------------------------------------------------------------------------
//
EXPORT_C const CVPbkContactViewDefinition&
        CVPbkContactViewDefinition::SubViewAt( TInt aIndex ) const
    {
    return *iSubViews[aIndex];
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SubViewAt
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactViewDefinition&
        CVPbkContactViewDefinition::SubViewAt( TInt aIndex )
    {
    return *iSubViews[aIndex];
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::Type
// ---------------------------------------------------------------------------
//
EXPORT_C TVPbkContactViewType CVPbkContactViewDefinition::Type() const
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CVPbkContactViewDefinition::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::FlagIsOn
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkContactViewDefinition::FlagIsOn
        ( TVPbkContactViewFlag aFlag ) const
    {
    return (iFlags & aFlag);
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::Uri
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVPbkContactViewDefinition::Uri() const
    {
    if (iUri)
        {
        return *iUri;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SortPolicy
// ---------------------------------------------------------------------------
//
EXPORT_C TVPbkContactViewSortPolicy
        CVPbkContactViewDefinition::SortPolicy() const
    {
    return iSortPolicy;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::Name
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVPbkContactViewDefinition::Name() const
    {
    if (iName)
        {
        return *iName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::Sharing
// ---------------------------------------------------------------------------
//
EXPORT_C TVPbkContactViewSharing CVPbkContactViewDefinition::Sharing() const
    {
    return iSharing;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::FieldTypeFilter
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeSelector*
        CVPbkContactViewDefinition::FieldTypeFilter() const
    {
    return iFilter;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::AddSubViewL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::AddSubViewL(
        CVPbkContactViewDefinition* aSubView)
    {
    iSubViews.AppendL(aSubView);
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetType
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetType(TVPbkContactViewType aType)
    {
    iType = aType;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetId
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetId(TInt aId)
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetFlag
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetFlag(TVPbkContactViewFlag aFlag,
        TBool aState)
    {
    if (aState)
        {
        iFlags |= aFlag;
        }
    else
        {
        iFlags &= ~aFlag;
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetUriL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetUriL(const TDesC& aUri)
    {
    HBufC* uri = aUri.AllocL();
    delete iUri;
    iUri = uri;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetSortPolicy
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetSortPolicy(
        TVPbkContactViewSortPolicy aSortPolicy)
    {
    iSortPolicy = aSortPolicy;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetNameL(const TDesC& aName)
    {
    HBufC* name = aName.AllocL();
    delete iName;
    iName = name;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetSharing
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetSharing(
        TVPbkContactViewSharing aViewSharing )
    {
    iSharing = aViewSharing;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::SetFieldTypeFilterL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactViewDefinition::SetFieldTypeFilterL(
        CVPbkFieldTypeSelector* aFilter )
    {
    delete iFilter;
    iFilter = NULL;

    if ( aFilter )
        {
        iFilter = CVPbkFieldTypeSelector::NewL( *aFilter );
        }
    }

EXPORT_C void CVPbkContactViewDefinition::SetContactSelector( MVPbkContactSelector* aContactSelector )
    {
    iContactSelector = aContactSelector;
    }

EXPORT_C MVPbkContactSelector* CVPbkContactViewDefinition::ContactSelector() const
    {
    return iContactSelector;
    }

// ---------------------------------------------------------------------------
// CVPbkContactViewDefinition::InitializeSharing
// ---------------------------------------------------------------------------
//
void CVPbkContactViewDefinition::InitializeSharing()
    {
    if ( iSharing == EVPbkViewSharingUndefined )
        {
        iSharing = EVPbkLocalView;
        // Default contacts or groups view is a shared view.
        if ( iType == EVPbkContactsView ||
             iType == EVPbkGroupsView )
            {
            iSharing = EVPbkSharedView;
            }
        }
    }
// End of File
