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
* Description:  A factory for creating contact view filters.
*
*/



// INCLUDE FILES
#include "VPbkContactViewFilterBuilder.h"

// Virtual Phonebook
#include <VPbkDataCaging.hrh>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <vpbkfieldtypeselectors.rsg>

// Virtual Phonebook Engine Utils
#include <RLocalizedResourceFile.h>

// System includes
#include <barsread.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Reads the given filter from resources and appends it to given selector.
 *
 * @param aFieldTypeSelector    Field type selector.
 * @param aResourceId           Resource id of the filter.
 * @param aResFile              Resource file.
 */
void DoAppendFilterL( CVPbkFieldTypeSelector& aFieldTypeSelector,
        TInt aResourceId, VPbkEngUtils::RLocalizedResourceFile& aResFile )
    {
    TResourceReader resReader;
    HBufC8* selectorBuf = aResFile.AllocReadLC( aResourceId );
    resReader.SetBuffer( selectorBuf );
    aFieldTypeSelector.AppendL( resReader );
    CleanupStack::PopAndDestroy(); // selectorBuf
    }

} /// namespace

// =========================== MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// VPbkContactViewFilterBuilder::AppendViewFilterL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeSelector&
    VPbkContactViewFilterBuilder::BuildContactViewFilterL
        ( CVPbkFieldTypeSelector& aFieldTypeSelector,
        TVPbkContactViewFilter aFilter,
        CVPbkContactManager& aContactManager )
    {
    // Open resource file
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC( aContactManager.FsSession(), KVPbkRomFileDrive,
        KDC_RESOURCE_FILES_DIR, KVPbkFieldTypeSelectorsResFileName );

    TInt resourceId = KErrNotFound;

    switch ( aFilter )
        {
        case EVPbkContactViewFilterEmail:
            {
            resourceId = R_VPBK_EMAIL_ADDRESS_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterPhoneNumber:
            {
            resourceId = R_VPBK_PHONE_NUMBER_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterMobileNumber:
            {
            resourceId = R_VPBK_MOBILE_NUMBER_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterFaxNumber:
            {
            resourceId = R_VPBK_FAX_NUMBER_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterRingingTone:
            {
            resourceId = R_VPBK_RINGING_TONE_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterVideoNumber:
            {
            resourceId = R_VPBK_VIDEO_NUMBER_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterPagerNumber:
            {
            resourceId = R_VPBK_PAGER_NUMBER_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterDTMF:
            {
            resourceId = R_VPBK_DTMF_NUMBER_SELECTOR;
            break;
            }
case EVPbkContactViewFilterTopContact:  
            {
            resourceId = R_VPBK_TOPCONTACT_SELECTOR;
            break;
            }
        case EVPbkContactViewFilterVoIP:
            {
            resourceId = R_VPBK_VOIP_ADDRESS_SELECTOR;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        };

    // Append the filter resource
    DoAppendFilterL( aFieldTypeSelector, resourceId, resFile );

    CleanupStack::PopAndDestroy(); // resFile
    return aFieldTypeSelector;
    }

//  End of File
