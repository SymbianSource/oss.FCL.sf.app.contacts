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
* Description:  Field Versit Property
*
*/


// INCLUDES
#include "TVPbkFieldVersitProperty.h"
#include <barsread.h>

EXPORT_C TVPbkFieldVersitProperty::TVPbkFieldVersitProperty() :
    iName(EVPbkVersitNameNone),
    iSubField(EVPbkVersitSubFieldNone),
    iExtension(NULL)
    {
    }

EXPORT_C TVPbkFieldVersitProperty::TVPbkFieldVersitProperty
        (TResourceReader& aResReader) :
    iName(static_cast<TVPbkFieldTypeName>(aResReader.ReadUint8())),
    iSubField(static_cast<TVPbkSubFieldType>(aResReader.ReadUint8())),
    iParameters(aResReader),
    iExtensionName(aResReader.ReadTPtrC8())
    {
    }

EXPORT_C void TVPbkFieldVersitProperty::ReadFromResource
        (TResourceReader& aResReader)
    {
    iName = static_cast<TVPbkFieldTypeName>(aResReader.ReadUint8());
    iSubField = static_cast<TVPbkSubFieldType>(aResReader.ReadUint8());
    iParameters.ReadFromResource(aResReader);
    iExtensionName.Set(aResReader.ReadTPtrC8());
    }

EXPORT_C TBool TVPbkFieldVersitProperty::Matches(const TVPbkFieldVersitProperty& aOther) const
    {
    return (
        iName == aOther.iName &&
        iSubField == aOther.iSubField &&
        aOther.iParameters.ContainsAll(iParameters) &&
        (iName != EVPbkVersitNameX || 
            iExtensionName.CompareF(aOther.iExtensionName) == 0) );
    }

