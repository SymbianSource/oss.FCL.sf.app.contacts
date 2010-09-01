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
* Description:  Default store URIs offered by the Virtual Phonebook
*
*/



// INCLUDE FILES
#include "VPbkContactStoreUris.h"
#include <VPbkStoreUriLiterals.h>

// ========================== OTHER EXPORTED FUNCTIONS =========================

namespace VPbkContactStoreUris {
// -----------------------------------------------------------------------------
// DefaultCntDbUri
// Returns: default contact data base URI
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& DefaultCntDbUri()
    {
    return KVPbkDefaultCntDbURI;
    }

// -----------------------------------------------------------------------------
// SimGlobalAdnUri
// Returns: ADN SIM store URI (global means the ADN store that can be accessed
// by both GSM and 3G phone)
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& SimGlobalAdnUri()
    {
    return KVPbkSimGlobalAdnURI;
    }

// -----------------------------------------------------------------------------
// SimGlobalFdnUri
// Returns: FDN SIM store URI (global means the FDN store that can be accessed
// by both GSM and 3G phone)
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& SimGlobalFdnUri()
    {
    return KVPbkSimGlobalFdnURI;
    }

// -----------------------------------------------------------------------------
// SimGlobalSdnUri
// Returns: SDN SIM store URI (global means the SDN store that can be accessed
// by both GSM and 3G phone)
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& SimGlobalSdnUri()
    {
    return KVPbkSimGlobalSdnURI; 
    }

// -----------------------------------------------------------------------------
// SimGlobalSdnUri
// Returns: MSISDN SIM store URI (global means the MSISDN store that
// can be accessed by both GSM and 3G phone)
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& SimGlobalOwnNumberUri()
    {
    return KVPbkSimGlobalOwnNumberURI; 
    }
}  // namespace VPbkContactStoreUris
//  End of File  
