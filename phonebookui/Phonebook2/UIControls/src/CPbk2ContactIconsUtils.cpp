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
* Description:  Phonebook 2 contact view list box model.
*
*/


// INCLUDE FILES

#include <CPbk2ContactIconsUtils.h>

// Phonebook 2
#include <CPbk2IconArray.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StoreViewDefinition.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2ContactUiControlExtension.h>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionIconSupport.h>

#include <pbk2uicontrols.rsg>
#include <Pbk2PresentationUtils.h>
#include "Pbk2NamesListExUID.h"

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkViewContact.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <MVPbkBaseContactField.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkContactFieldData.h>

// System includes
#include <eikenv.h>
#include <barsread.h>


EXPORT_C CPbk2ContactIconsUtils* CPbk2ContactIconsUtils::NewL(
        CPbk2StorePropertyArray& aStoreProperties,
        MPbk2ContactUiControlExtension* aUiExtension )
    {
    CPbk2ContactIconsUtils* self = new(ELeave) CPbk2ContactIconsUtils(
            aStoreProperties,
            aUiExtension);
    return self;
    }

CPbk2ContactIconsUtils::~CPbk2ContactIconsUtils()
    {
    }

CPbk2ContactIconsUtils::CPbk2ContactIconsUtils(
        CPbk2StorePropertyArray& aStoreProperties,
        MPbk2ContactUiControlExtension* aUiExtension ):
        iStoreProperties(aStoreProperties),
        iUiExtension(aUiExtension)
    {
    }

EXPORT_C CPbk2IconArray* CPbk2ContactIconsUtils::CreateListboxIconArrayL(
        TInt aResourceId)
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );
    CPbk2IconArray* iconArray = CPbk2IconArray::NewL( reader );
    CleanupStack::PushL( iconArray );

    // Add icons from UI extensions
    CPbk2UIExtensionManager* extManager = CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();
    extManager->IconSupportL().AppendExtensionIconsL( *iconArray );
    CleanupStack::PopAndDestroy(); // extManager
    CleanupStack::Pop( iconArray );
    CleanupStack::PopAndDestroy(); // reader

    return iconArray;    
    }

EXPORT_C void CPbk2ContactIconsUtils::GetIconIdsForContactL(
        const MVPbkViewContact& aViewContact,
        RArray<TPbk2IconId>& aIconArray ) const
    {
    CleanupClosePushL( aIconArray );
    aIconArray.Reset();
    
    // get dynamic icons from ui extension
    if ( iUiExtension )
        {
        const TArray<TPbk2IconId>& ids =
            iUiExtension->GetDynamicIconsL( &aViewContact );
        for (TInt i = 0; i < ids.Count(); ++i)
            {
            aIconArray.AppendL( ids[i] );
            }
        }
    
    // find icon from view definition
    TVPbkContactViewType viewType = aViewContact.ParentView().Type();
    const TInt propCount = iStoreProperties.Count();
    TBool propertyFound = EFalse;
    TBool viewIconFound = EFalse;
    for (TInt i = 0; i < propCount && !propertyFound; ++i)
        {
        // Find store property for the view contact
        const CPbk2StoreProperty& prop = iStoreProperties.At(i);
        if (aViewContact.MatchContactStore(prop.StoreUri().UriDes()))
            {
            propertyFound = ETrue;
            TArray<const CPbk2StoreViewDefinition*> pbk2Defs = prop.Views();
            const TInt defCount = pbk2Defs.Count();
            for (TInt j = 0; j < defCount && !viewIconFound; ++j)
                {
                // if types are same use item icon
                if (viewType == pbk2Defs[j]->ViewDefinition().Type())
                    {
                    aIconArray.AppendL( pbk2Defs[j]->ViewItemIconId() );
                    viewIconFound = ETrue;
                    }
                }
            }
        }
    CleanupStack::Pop();
    }

//  End of File
