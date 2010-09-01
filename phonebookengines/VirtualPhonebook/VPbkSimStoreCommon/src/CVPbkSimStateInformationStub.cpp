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
* Description:  A stub for different sim status information
*
*/


// INCLUDE FILES

#include "cvpbksimstateinformation.h" 

// From Virtual Phonebook
// System includes

// ================= MEMBER FUNCTIONS =======================

CVPbkSimStateInformation::CVPbkSimStateInformation()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkSimStateInformation::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkSimStateInformation::ConstructL()
    {
    // empty in stub
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimStateInformation* CVPbkSimStateInformation::NewL()
    {
    return new(ELeave) CVPbkSimStateInformation;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::~CVPbkSimStateInformation
// -----------------------------------------------------------------------------
//
CVPbkSimStateInformation::~CVPbkSimStateInformation()
    {
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::SimInserted
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::SimInsertedL()
    {
    /// SIM is always inserted
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::BTSapEnabled
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::BTSapEnabledL()
    {
    /// BT SAP always off
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::ActiveFdnBlocksAdnStoreL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::ActiveFdnBlocksAdnStoreL(
        MVPbkSimPhone& /*aSimPhone*/)
    {
    /// FDN never blocks ADN
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::NewSimCardL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::NewSimCardL()
    {
    /// Always known SIM card
    return EFalse;
    }
    
//  End of File  
