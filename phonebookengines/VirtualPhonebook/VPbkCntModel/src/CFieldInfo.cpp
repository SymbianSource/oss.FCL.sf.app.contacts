/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook field info
*
*/


#include "CFieldInfo.h"

#include <barsread.h>
#include <cntfield.h>

namespace VPbkCntModel {

const TInt KFieldInfoGranularity = 8;
    
CFieldInfo::CFieldInfo()
    {
	}

inline void CFieldInfo::ConstructL(TResourceReader& aReader)
    {    
    iFieldStorageType = aReader.ReadInt32();
    iContentType = CContentType::NewL();
	const TUid contactFieldUid = TUid::Uid(aReader.ReadInt32());
	iContentType->SetMapping(TUid::Uid(aReader.ReadInt32()));
	if (contactFieldUid != KUidContactFieldNone)
        {
	    iContentType->AddFieldTypeL(contactFieldUid);
        }
    TInt count = aReader.ReadInt16();
	while (--count >= 0)
        {
		iContentType->AddFieldTypeL(TUid::Uid(aReader.ReadInt32()));
        }
    iCategory = aReader.ReadInt32();
    iFieldName = aReader.ReadHBufCL();
    iFlags = aReader.ReadInt32();
	}

CFieldInfo* CFieldInfo::NewL(TResourceReader& aReader)
    {
	CFieldInfo* self = new(ELeave) CFieldInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aReader);
	CleanupStack::Pop(self);
	return self;
	}

CFieldInfo::~CFieldInfo()
    {
    delete iFieldName;
    delete iContentType;
    }

inline TBool CFieldInfo::IsEqualFlags(const CContactItemField& aField) const
    {
    TUint flags = 0;
    if (aField.IsHidden())
		{
		flags |= EContactFieldFlagHidden;
		}
    if (aField.IsReadOnly())
		{
		flags |= EContactFieldFlagReadOnly;
		}
    // Do not check synchronize flag as ctnmodel modifies it
    if (aField.IsDisabled())
		{
		flags |= EContactFieldFlagDisabled;
		}
    return ((flags & iFlags) == flags);
    }
        
TBool CFieldInfo::IsEqualType(const CContactItemField& aField) const
    {
    return
        (iFieldStorageType == aField.StorageType() &&
        iContentType->IsEqual( aField.ContentType() ) &&
        TUint(iCategory) == aField.UserFlags() &&
        IsEqualFlags(aField));
    }  

TBool CFieldInfo::IsEqualLabel(const CContactItemField& aField) const
    {
    return
        ((!iFieldName && aField.Label().Length()==0) || 
        (iFieldName && aField.Label()==*iFieldName));
    }

CContactItemField* CFieldInfo::CreateFieldL() const
    {
    CContactItemField* field = CContactItemField::NewLC(iFieldStorageType, 
        *iContentType);
    
	field->SetUserFlags( iCategory );
    if ( iFieldName )
        {
	    field->SetLabelL( *iFieldName );
        }
	if ( iFlags & EContactFieldFlagHidden )
        {
		field->SetHidden( ETrue );
        }
	if ( iFlags & EContactFieldFlagReadOnly )
        {
		field->SetReadOnly( ETrue );
        }
	if ( iFlags & EContactFieldFlagSynchronize )
        {
		field->SetSynchronize( ETrue );
        }
	if ( iFlags & EContactFieldFlagDisabled )
        {
		field->SetDisabled( ETrue );
        }
    CleanupStack::Pop(field);
    return field;
    }
    
CFieldsInfo::CFieldsInfo() :
    CArrayPtrFlat<CFieldInfo>(KFieldInfoGranularity)
    {
    }

inline void CFieldsInfo::ConstructL(TResourceReader& aReader)
    {
    AppendFieldsL(aReader);
    }

CFieldsInfo* CFieldsInfo::NewL(TResourceReader& aReader)
    {
	CFieldsInfo* self = new(ELeave) CFieldsInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aReader);
	CleanupStack::Pop(self);
	return self;
    }

void CFieldsInfo::AppendFieldsL(TResourceReader& aReader)
    {
    TInt count = aReader.ReadInt16();
    while (--count >= 0)
        {
        CFieldInfo* fieldInfo = CFieldInfo::NewL(aReader);
        CleanupStack::PushL(fieldInfo);
        AppendL(fieldInfo);
        CleanupStack::Pop(fieldInfo);
        }
    }
        
CFieldsInfo::~CFieldsInfo()
    {
    ResetAndDestroy();
    }

} // VPbkCntModel
