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
* Description:  A list query for selecting a contact store.
*
*/



// INCLUDE FILES
#include "CPbk2StoreListQuery.h"

#include <aknlistquerydialog.h>

#include <pbk2uicontrols.rsg>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StorePropertyContentStoreName.h>
#include <CPbk2ApplicationServices.h>

#include <CVPbkContactStoreUriArray.h>
#include <badesca.h>

// -----------------------------------------------------------------------------
// CPbk2StoreListQuery::CPbk2StoreListQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2StoreListQuery::CPbk2StoreListQuery()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2StoreListQuery::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreListQuery* CPbk2StoreListQuery::NewL()
    {
    CPbk2StoreListQuery* self = new( ELeave ) CPbk2StoreListQuery();
    return self;
    }

// Destructor
CPbk2StoreListQuery::~CPbk2StoreListQuery()
    {
    delete iAllItemText;
    }

// -----------------------------------------------------------------------------
// CPbk2StoreListQuery::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2StoreListQuery::ExecuteLD(const TDesC& aTitle,
        CVPbkContactStoreUriArray& aUriArray)
    {
    CleanupStack::PushL(this);

    TInt selection = KErrNotFound;
    CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog(&selection);
    CleanupStack::PushL( dlg );
    dlg->PrepareLC(R_PBK2_STORE_LIST_QUERY);
    dlg->QueryHeading()->SetTextL(aTitle);
    dlg->SetItemTextArray(CreateListItemsL(aUriArray));
    dlg->SetOwnershipType(ELbmOwnsItemArray);
    CleanupStack::Pop( dlg );
    TInt ret = dlg->RunLD();

    if (ret == EAknSoftkeyOk &&
        (selection != AllItemIndex(aUriArray)))
        {
        SetSelection(aUriArray, selection);
        }
    CleanupStack::PopAndDestroy(this);
    return ret;
    }

// -----------------------------------------------------------------------------
// CPbk2StoreListQuery::SetAllItemTextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreListQuery::SetAllItemTextL(const TDesC& aAllItemText,
            TBool aUseTopPosition)
    {
    delete iAllItemText;
    iAllItemText = NULL;
    iAllItemText = aAllItemText.AllocL();
    iAllItemUsesTopPosition = aUseTopPosition;
    }

// -----------------------------------------------------------------------------
// CPbk2StoreListQuery::CreateListItemsL
// -----------------------------------------------------------------------------
//
MDesCArray* CPbk2StoreListQuery::CreateListItemsL(
        CVPbkContactStoreUriArray& aUriArray)
    {
    const TInt granularity = 4;
    const TInt firstPosition = 0;
    CDesCArrayFlat* itemArray = new(ELeave) CDesCArrayFlat(granularity);
    CleanupStack::PushL(itemArray);

    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();
    CPbk2StorePropertyArray& props = appServices->StoreProperties();
    CleanupStack::PopAndDestroy(); // appServices

    const TInt count = aUriArray.Count();
    TPbk2ContentId contentId = EPbk2CopyQueryItem;
    if ( iAllItemText )
        {
        // All item text is used only in Memory info query.
        // If this class is used different way this contentId should be
        // determined more accurated way.
        contentId = EPbk2MemInfoPhone;
        }

    for (TInt i = count - 1; i >= 0; --i)
        {
        const CPbk2StoreProperty* prop = props.FindProperty(aUriArray[i]);
        if (prop)
            {
            CPbk2Content* content = prop->RetrieveContentLC( contentId );

            CPbk2StorePropertyContentStoreName* propContent =
                dynamic_cast<CPbk2StorePropertyContentStoreName*>( content );

            if (propContent && propContent->StoreName() != KNullDesC())
                {
                // Keep the same order as in the URI array
                itemArray->InsertL(firstPosition, propContent->StoreName());
                }
            else
                {
                // Backup plan -> use store name
                itemArray->InsertL(firstPosition, prop->StoreName());
                }

            CleanupStack::PopAndDestroy( content );
            }
        else
            {
            aUriArray.Remove(aUriArray[i]);
            }
        }

    if (iAllItemText)
        {
        if (iAllItemUsesTopPosition)
            {
            itemArray->InsertL(firstPosition, *iAllItemText);
            }
        else
            {
            itemArray->AppendL(*iAllItemText);
            }
        }

    CleanupStack::Pop(itemArray);
    return itemArray;
    }

// -----------------------------------------------------------------------------
// CPbk2StoreListQuery::SetSelection
// -----------------------------------------------------------------------------
//
void CPbk2StoreListQuery::SetSelection(CVPbkContactStoreUriArray& aUriArray,
        TInt aSelection)
    {
    TInt selection = aSelection;
    if (iAllItemText && iAllItemUsesTopPosition)
        {
        --selection;
        }

    const TInt count = aUriArray.Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        if (i != selection)
            {
            aUriArray.Remove(aUriArray[i]);
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2StoreListQuery::AllItemIndex
// -----------------------------------------------------------------------------
//
TInt CPbk2StoreListQuery::AllItemIndex(CVPbkContactStoreUriArray& aUriArray)
    {
    if (iAllItemText)
        {
        TInt itemIndex = aUriArray.Count() - 1;
        if (iAllItemUsesTopPosition)
            {
            itemIndex = 0;
            }
        return itemIndex;
        }
    else
        {
        return KErrNotFound;
        }
    }
//  End of File
