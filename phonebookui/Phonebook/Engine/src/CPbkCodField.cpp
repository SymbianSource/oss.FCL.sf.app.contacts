/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides object for call field information
*
*/



// INCLUDE FILES
#include "CPbkCodField.h"
#include <e32def.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbkCodField::CPbkCodField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbkCodField::CPbkCodField(CPbkContactEngine& aPbkContactEngine):
    iPbkContactEngine(aPbkContactEngine)
    {
    }

// -----------------------------------------------------------------------------
// CPbkCodField::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPbkCodField* CPbkCodField::NewL(CPbkContactEngine& aPbkContactEngine)
    {
    return new( ELeave ) CPbkCodField(aPbkContactEngine);
    }

// Destructor
CPbkCodField::~CPbkCodField()
    {
    }

// -----------------------------------------------------------------------------
// CPbkCodField::CodFieldId
// -----------------------------------------------------------------------------
//
EXPORT_C TPbkFieldId CPbkCodField::CodFieldIdL
        (TContactItemId aContactItemId) const
    {
    // Search for COD text field
    CPbkContactItem* ci = iPbkContactEngine.ReadContactL(aContactItemId);
    TPbkFieldId ret = CodFieldId(*ci);
    delete ci;

    return ret;
    }

// -----------------------------------------------------------------------------
// CPbkCodField::CodFieldId
// -----------------------------------------------------------------------------
//
EXPORT_C TPbkFieldId CPbkCodField::CodFieldId
        (CPbkContactItem& aPbkContactItem) const
    {
    TPbkFieldId ret(KErrNotFound);

    // Search for COD text field
    TPbkContactItemField* field = aPbkContactItem.FindField
        (EPbkFieldIdCodImageID);
    if (!field)
        {
        // Search for COD image if text not found
        field = aPbkContactItem.FindField(EPbkFieldIdCodTextID);
        if (field)
            {
            ret = EPbkFieldIdCodTextID;
            }
        }
    else
        {
        ret = EPbkFieldIdCodImageID;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CPbkCodField::NameL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CPbkCodField::GetTextL(TContactItemId aContactItemId) const
    {
    // Search for COD text field
    CPbkContactItem* ci = iPbkContactEngine.ReadContactLC(aContactItemId);
    TPbkContactItemField* field = ci->FindField(EPbkFieldIdCodTextID);    
    if (!field)
        {
        CleanupStack::PopAndDestroy(ci);
        User::Leave(KErrNotFound);
        }
    if (field->StorageType() != KStorageTypeText)
        {
        CleanupStack::PopAndDestroy(ci);
        User::Leave(KErrNotSupported);
        }

    HBufC* result = field->Text().AllocL();
    CleanupStack::PopAndDestroy(ci);    
    return result;
    }

//  End of File
