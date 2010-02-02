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
*		Maps a CPbkFieldInfo object's export vCard type to a MPbkVcardProperty
*
*/


// INCLUDE FILES
#include "TPbkFieldInfoExportTypeAdapter.h"
#include <cntdef.h>
#include <cntfield.h>
#include "CPbkFieldInfo.h"


// ==================== MEMBER FUNCTIONS ====================

EXPORT_C TPbkFieldInfoExportTypeAdapter::TPbkFieldInfoExportTypeAdapter
        (const CPbkFieldInfo& aFieldInfo) :
    iFieldInfo(aFieldInfo)
    {
    }

// Explicit destructor prevents compiler generating multiple destructors in
// client object files.
EXPORT_C TPbkFieldInfoExportTypeAdapter::~TPbkFieldInfoExportTypeAdapter()
    {
    }

EXPORT_C TUid TPbkFieldInfoExportTypeAdapter::PropertyName() const
    {
    const TUid vCardMapping = iFieldInfo.ContentType().Mapping();
    // "N" and "SOUND" properties are special cases which
    // need to be handled separately.
    switch (vCardMapping.iUid)
        {
        case KIntContactFieldVCardMapUnusedN:
            {
            // Instead of the vCard mapping return the contact model field 
            // type, which distinguishes the property subfields correctly
            // Same goes for the name reading fields which are considered
            // to be also name type.
            return iFieldInfo.ContentType().FieldType(0);
            }
        // default branch is missing on purpose
        }
    return vCardMapping;
    }

EXPORT_C const MPbkVcardParameters& TPbkFieldInfoExportTypeAdapter::PropertyParameters() const
    {
    return *this;
    }

TInt TPbkFieldInfoExportTypeAdapter::ParameterCount() const
    {
    return Max(0, iFieldInfo.ContentType().FieldTypeCount()-1);
    }

TUid TPbkFieldInfoExportTypeAdapter::ParameterAt(TInt aIndex) const
    {
    return iFieldInfo.ContentType().FieldType(aIndex+1);
    }


// End of File
