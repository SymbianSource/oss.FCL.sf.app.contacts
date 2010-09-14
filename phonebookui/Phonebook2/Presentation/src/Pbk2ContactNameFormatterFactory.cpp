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
* Description: 
*     Name formatter factory.
*
*/


#include "Pbk2ContactNameFormatterFactory.h"

// From Phonebook2
#include "Pbk2DataCaging.hrh"
#include "CPbk2ContactNameFormatter.h"
#include "CPbk2JapaneseContactNameFormatter.h"
#include "CPbk2ChineseContactNameFormatter.h"
#include "Pbk2PresentationUtils.h"
#include <RPbk2LocalizedResourceFile.h>
#include <pbk2presentation.rsg>

// From Virtual Phonebook
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <barsread.h>
#include <featmgr.h>

namespace {

MPbk2ContactNameFormatter* CreateFormatterL(
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        const CPbk2SortOrderManager& aSortOrderManager,
        CVPbkFieldTypeSelector* aTitleFieldSelector,
        const TDesC& aUnnamedText )
    {
    MPbk2ContactNameFormatter* result = NULL;
    
    FeatureManager::InitializeLibL();
    TBool chinese = FeatureManager::FeatureSupported( KFeatureIdChinese );
    FeatureManager::UnInitializeLib();
    
    if ( chinese )
        {                
        result = CPbk2ChineseContactNameFormatter::NewL( aUnnamedText, 
            aMasterFieldTypeList, aSortOrderManager, aTitleFieldSelector );
        }
    else
        {
        result = CPbk2ContactNameFormatter::NewL( aUnnamedText, 
            aMasterFieldTypeList, aSortOrderManager, aTitleFieldSelector );
        }
    
    
    return result;
    }

MPbk2ContactNameFormatter* CreateFormatterL(
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        const CPbk2SortOrderManager& aSortOrderManager,
        TResourceReader* aTitleFieldSelectorReader,
        const TDesC* aUnnamedText,
        RFs* aRFs )
    {
    RFs ownFs;
    RFs fs;
    
    if ( !aRFs )
        {
        User::LeaveIfError( ownFs.Connect() );
        CleanupClosePushL( ownFs );
        fs = ownFs;
        }
    else
        {
        fs = *aRFs;
        }
        
    RPbk2LocalizedResourceFile resFile( &fs );
    resFile.OpenLC( KPbk2RomFileDrive, 
        KDC_RESOURCE_FILES_DIR, 
        Pbk2PresentationUtils::PresentationResourceFile() );
        
    CVPbkFieldTypeSelector* titleFieldSelector = NULL;
    
    HBufC* unnamed = NULL;
    if ( !aUnnamedText )
        {
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC( R_QTN_PHOB_UNNAMED ) );
        unnamed = reader.ReadHBufCL();
        CleanupStack::PopAndDestroy(); // reader
        CleanupStack::PushL( unnamed );
        }
    else
        {
        unnamed = aUnnamedText->AllocLC();
        }
    
    if ( !aTitleFieldSelectorReader )
        {
        // Use default title fields
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC( R_TITLE_FIELD_SELECTOR ) );
        titleFieldSelector =
            CVPbkFieldTypeSelector::NewL( reader, aMasterFieldTypeList );
        CleanupStack::PopAndDestroy(); // reader
        }
    else
        {
        titleFieldSelector = CVPbkFieldTypeSelector::NewL(
            *aTitleFieldSelectorReader, aMasterFieldTypeList );
        }
    CleanupStack::PushL( titleFieldSelector );
    
    // Ownership of titleFieldSelector changes
    MPbk2ContactNameFormatter* result = CreateFormatterL( aMasterFieldTypeList,
        aSortOrderManager, titleFieldSelector, *unnamed );
        
    CleanupStack::Pop( titleFieldSelector );
    CleanupStack::PopAndDestroy(2); // resFile, unnamed
    if ( ownFs.Handle() )
        {
        CleanupStack::PopAndDestroy(); // ownFs
        }
    
    return result;    
    }
}

                
EXPORT_C MPbk2ContactNameFormatter* Pbk2ContactNameFormatterFactory::CreateL
        ( const MVPbkFieldTypeList& aMasterFieldTypeList, 
        const CPbk2SortOrderManager& aSortOrderManager, 
        RFs* aRFs )
    {
    return CreateFormatterL( aMasterFieldTypeList, aSortOrderManager, 
        NULL, NULL, aRFs );
	}

EXPORT_C MPbk2ContactNameFormatter* Pbk2ContactNameFormatterFactory::CreateL
        ( const TDesC& aUnnamedText,
        const MVPbkFieldTypeList& aMasterFieldTypeList, 
        const CPbk2SortOrderManager& aSortOrderManager, 
        RFs* aRFs )
    {
    return CreateFormatterL( aMasterFieldTypeList, aSortOrderManager, 
        NULL, &aUnnamedText, aRFs );
    }

EXPORT_C MPbk2ContactNameFormatter* Pbk2ContactNameFormatterFactory::CreateL
        ( const MVPbkFieldTypeList& aMasterFieldTypeList, 
        const CPbk2SortOrderManager& aSortOrderManager,
        TResourceReader& aTitleFieldSelectorReader,
        const TDesC* aUnnamedText, 
        RFs* aRFs )
    {
    return CreateFormatterL( aMasterFieldTypeList, aSortOrderManager, 
        &aTitleFieldSelectorReader, aUnnamedText, aRFs );
	}
