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
*		Factory for Phonebook contact name formatting
*
*/


// INCLUDE FILES
#include    "PbkNameFormatterFactory.h"
#include    <MPbkContactNameFormat.h>
#include    <featmgr.h>
#include    <bldvariant.hrh>
#include    "CPbkContactNameFormatter.h"
#include    "CPbkChinaContactNameFormatter.h"
#include    "CPbkJapanContactNameFormatter.h"
#include    "CPbkSortOrderManager.h"


// ==================== MEMBER FUNCTIONS ====================

MPbkContactNameFormat* PbkNameFormatterFactory::CreateL
        (const TDesC& aUnnamedText,
		const CPbkSortOrderManager& aSortOrderManager,
		TBool aSettingsVisible)
    {
    // create variants depending on situation
    MPbkContactNameFormat* result = NULL;

    // if the settings menu item is defined as visible in localization file,
    // default name formatter should be used instead of china version
    // in chinese variant builds.

    if (FeatureManager::FeatureSupported(KFeatureIdChinese) &&
		!aSettingsVisible)
        {
        // Chinese name formatting
        result = CPbkChinaContactNameFormatter::NewL(aUnnamedText, aSortOrderManager.SortOrder());
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        // japanese name formatting
        result = CPbkJapanContactNameFormatter::NewL(aUnnamedText, aSortOrderManager);
        }
    else
        {
        // default name formatting
        result = CPbkContactNameFormatter::NewL(aUnnamedText, aSortOrderManager);
        }

    return result;
    }

//  End of File 
