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
*		Phonebook field info group.
*
*/


// INCLUDE FILES
#include "CPbkFieldInfoGroup.h"
#include <barsread.h>
#include "CPbkFieldsInfo.h"
#include "CPbkFieldInfo.h"


// ==================== MEMBER FUNCTIONS ====================

EXPORT_C TPbkFieldGroupId CPbkFieldInfoGroup::Id() const
    {
    return iId;
    }

EXPORT_C const TDesC& CPbkFieldInfoGroup::Label() const
    {
    if (iLabel)
        {
        return *iLabel;
        }
    else
        {
        return KNullDesC;
        }
    }

EXPORT_C TPbkIconId CPbkFieldInfoGroup::IconId() const
    {
    return iIconId;
    }

EXPORT_C TInt CPbkFieldInfoGroup::Count() const
    {
    return iFieldInfos.Count();
    }

EXPORT_C const CPbkFieldInfo& CPbkFieldInfoGroup::At(TInt aIndex) const
    {
    return *iFieldInfos[aIndex];
    }

EXPORT_C TPbkAddItemOrdering CPbkFieldInfoGroup::AddItemOrdering() const
    {
    return iAddItemOrdering;
    }

void CPbkFieldInfoGroup::AddL(const CPbkFieldInfo& aFieldInfo)
    {
    User::LeaveIfError(iFieldInfos.Append(&aFieldInfo));
    }

inline CPbkFieldInfoGroup::CPbkFieldInfoGroup()
    {
    }

inline void CPbkFieldInfoGroup::ConstructL(TResourceReader& aReader)
    {
    iId = aReader.ReadInt8();
    iLabel = aReader.ReadHBufCL();
    iIconId = static_cast<TPbkIconId>(aReader.ReadInt8());
    iAddItemOrdering = static_cast<TPbkAddItemOrdering>(aReader.ReadInt8());
    }

CPbkFieldInfoGroup* CPbkFieldInfoGroup::NewLC(TResourceReader& aReader)
    {
    CPbkFieldInfoGroup* self = new(ELeave) CPbkFieldInfoGroup;
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    return self;
    }

CPbkFieldInfoGroup::~CPbkFieldInfoGroup()
    {
    iFieldInfos.Close();
    delete iLabel;
    }


// End of File
