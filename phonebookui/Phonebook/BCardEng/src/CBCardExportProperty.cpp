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
*    BCard export property.
*
*/


// INCLUDE FILES
#include "CBCardExportProperty.h"
#include <barsread.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldsInfo.h>

// ==================== MEMBER FUNCTIONS ====================

inline CBCardExportProperty::CBCardExportProperty()
    {
    }

inline void CBCardExportProperty::ConstructL(
        TResourceReader& aReader, const CPbkFieldsInfo& aFieldsInfo)
    {
    HBufC* name16 = aReader.ReadHBufCL();    
    CleanupStack::PushL(name16);
    HBufC8* name = HBufC8::NewLC(name16->Length());
    TPtr8 namePtr = name->Des();
    namePtr.Copy(*name16);        
    iName = name;
    CleanupStack::Pop(name);
    CleanupStack::PopAndDestroy(name16);

    iStorageType  = TPbkVersitStorageType(aReader.ReadInt8());
    
    TInt count=aReader.ReadInt16();
	while (count-- > 0)
		{
        const TPbkFieldId fieldId = TPbkFieldId(aReader.ReadInt8());
        const TPbkFieldLocation location = TPbkFieldLocation(aReader.ReadInt8());
        
        CPbkFieldInfo* info = aFieldsInfo.Find(fieldId, location);
        if (info)
            {
            User::LeaveIfError(iFieldInfos.Append(info));
            }
        }
    }

CBCardExportProperty* CBCardExportProperty::NewLC(
        TResourceReader& aReader, const CPbkFieldsInfo& aFieldsInfo)
    {
    CBCardExportProperty* self = new(ELeave) CBCardExportProperty;
    CleanupStack::PushL(self);
    self->ConstructL(aReader, aFieldsInfo);
    return self;    
    }

CBCardExportProperty::~CBCardExportProperty()
    {
    delete iName;
    iFieldInfos.Close();
    }

const TDesC8& CBCardExportProperty::Name() const
    {
    return *iName;
    }

TPbkVersitStorageType CBCardExportProperty::StorageType() const
    {
    return iStorageType;
    }

TInt CBCardExportProperty::InfoCount() const
    {
    return iFieldInfos.Count();
    }

const CPbkFieldInfo* CBCardExportProperty::InfoAt(TInt aIndex) const
    {
    return iFieldInfos[aIndex];
    }

// End of File
