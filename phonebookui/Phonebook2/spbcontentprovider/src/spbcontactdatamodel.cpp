/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Contact model class for MyCard
 *
 */

#include "spbcontactdatamodel.h"
#include "spbcontactdatamodelprivate.h"


// ---------------------------------------------------------------------------
// CSpbContactDataModel::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSpbContactDataModel* CSpbContactDataModel::NewL( 
    CVPbkContactManager& aCntManager,
    CCoeEnv& aCoeEnv,
    TInt aFieldTypeSelectorRes,
    MPbk2ClipListBoxText* aClip )
    {
    CSpbContactDataModel* self = new (ELeave) CSpbContactDataModel;
    CleanupStack::PushL(self);
    self->iImpl = CSpbContactDataModelPrivate::NewL( 
        aCntManager, aCoeEnv, aFieldTypeSelectorRes, aClip );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModel::~CSpbContactDataModel
// ---------------------------------------------------------------------------
//
CSpbContactDataModel::~CSpbContactDataModel()
    {
    delete iImpl;
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModel::PresentationFieldIndex
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSpbContactDataModel::PresentationFieldIndex( TInt aListIndex )
    {
    return iImpl->PresentationFieldIndex( aListIndex );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::ExternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CSpbContactDataModel::ExternalizeL( RWriteStream& aStream ) const
    {
    iImpl->ExternalizeL( aStream );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CSpbContactDataModel::InternalizeL( RReadStream& aStream )
    {
    iImpl->InternalizeL( aStream );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::SetDataL
// --------------------------------------------------------------------------
//
EXPORT_C void CSpbContactDataModel::SetDataL( 
    const CPbk2PresentationContact& aContact,
    const CPbk2IconArray* aIconArray )
    {
    iImpl->SetDataL( aContact, aIconArray );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::ContactLink
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLink* CSpbContactDataModel::ContactLink()
    {
    return iImpl->ContactLink();
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::IsEmpty
// --------------------------------------------------------------------------
//
EXPORT_C TBool CSpbContactDataModel::IsEmpty()
    {
    return iImpl->IsEmpty();
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::Reset
// --------------------------------------------------------------------------
//
EXPORT_C void CSpbContactDataModel::Reset()
    {
    iImpl->Reset();
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::Text
// --------------------------------------------------------------------------
//
EXPORT_C TPtrC CSpbContactDataModel::Text( TTextTypes aType ) const
    {
    return iImpl->Text( aType );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::Data
// --------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CSpbContactDataModel::Data( TBinaryTypes aType ) const
    {
    return iImpl->Data( aType );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::SetIconArrayL
// --------------------------------------------------------------------------
//
EXPORT_C void CSpbContactDataModel::UpdateIconsL( const CPbk2IconArray& aIconArray )
    {
    iImpl->UpdateIconsL( aIconArray );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::SetClipListBoxText
// --------------------------------------------------------------------------
//
EXPORT_C void CSpbContactDataModel::SetClipListBoxText( MPbk2ClipListBoxText* aClip )
    {
    iImpl->SetClipListBoxText( aClip );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModel::ListBoxModel
// --------------------------------------------------------------------------
//
EXPORT_C MDesCArray& CSpbContactDataModel::ListBoxModel()
    {
    return *iImpl;
    }

// End of File
