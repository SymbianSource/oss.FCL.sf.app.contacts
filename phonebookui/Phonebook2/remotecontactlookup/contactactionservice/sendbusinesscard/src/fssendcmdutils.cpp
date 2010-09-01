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
* Description:  Send command's utility functions
*
*/


#include "fssendcmdutils.h"

#include <barsread.h> // TResourceReader
#include <coemain.h> // CCoeEnv
#include <MVPbkBaseContact.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkFieldType.h>

TBool FsSendCmdUtils::IsContactEmpty(const MVPbkBaseContact* aContact)
	{
	TInt fieldCount = aContact->Fields().FieldCount();
	const MVPbkBaseContactFieldCollection& fieldSet = aContact->Fields();
	for (TInt i=0; i < fieldCount; ++i)
		{
		const MVPbkBaseContactField& field = fieldSet.FieldAt(i);
		if (!field.FieldData().IsEmpty())
			{
			// There was a field, which was not empty (or all spaces),
			// that's enough for us, the contact gets sent
			return EFalse;
			}
		}
		
	return ETrue;			
	}

const MVPbkBaseContactField* FsSendCmdUtils::FindFieldL(
	const MVPbkBaseContact& aContact,
	TInt aResIdForFieldType,
	const MVPbkFieldTypeList& aMasterFieldTypeList)
	{
	const MVPbkFieldType& fieldtype = 
		ReadFieldTypeFromResL(aResIdForFieldType, aMasterFieldTypeList);
	return FindField(aContact, fieldtype, aMasterFieldTypeList);
	}
	
const MVPbkBaseContactField* FsSendCmdUtils::FindField(
	const MVPbkBaseContact& aContact,
	const MVPbkFieldType& aFieldType,
	const MVPbkFieldTypeList& aMasterFieldTypeList)
	{
	const MVPbkBaseContactFieldCollection& fields = aContact.Fields();
	TInt fieldCount(fields.FieldCount());
	for (TInt i = 0; i < fieldCount; ++i)
		{
		const MVPbkBaseContactField& field = fields.FieldAt(i);
		if (IsFieldMatching(field, aFieldType, aMasterFieldTypeList))
			{
			return &field;
			}
		}
	return NULL;		
	}
	
const MVPbkFieldType& FsSendCmdUtils::ReadFieldTypeFromResL(
	TInt aResId,
	const MVPbkFieldTypeList& aMasterFieldTypeList)
	{
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, aResId);
    const TInt count = reader.ReadInt16();
    
    TVPbkFieldTypeMapping mapping(reader);
    const MVPbkFieldType* fieldType = mapping.FindMatch(aMasterFieldTypeList);
    if (!fieldType)
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PopAndDestroy(); // CreateResourceReaderLC
    return *fieldType;
	}	
	
const MVPbkFieldType* FsSendCmdUtils::FieldType(
	const MVPbkBaseContactField& aField,
	const MVPbkFieldTypeList& aMasterFieldTypeList)
	{
    for (TInt matchPriority = 0; 
        matchPriority <= aMasterFieldTypeList.MaxMatchPriority();
        ++matchPriority)
        {
        return aField.MatchFieldType(matchPriority);
        }	
	return NULL;
	}
		
TBool FsSendCmdUtils::IsFieldMatching(
	const MVPbkBaseContactField& aField,
	const MVPbkFieldType& aFieldType,
	const MVPbkFieldTypeList& aMasterFieldTypeList)
	{
    const MVPbkFieldType* result = NULL;

    for (TInt matchPriority = 0; 
        matchPriority <= aMasterFieldTypeList.MaxMatchPriority();
        ++matchPriority)
        {
        result = aField.MatchFieldType(matchPriority);
        if (result && result->IsSame(aFieldType))
        	{
        	return ETrue;
        	}
        }

    return EFalse;	
	}  		
		
// End of file
