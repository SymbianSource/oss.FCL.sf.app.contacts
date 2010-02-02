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
* Description:  Phonebook 2 copy contacts result diplay utility.
*
*/


// INCLUDE FILES
#include "TPbk2CopyContactsUtil.h"

// Phonebook 2
#include <CPbk2StoreConfiguration.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactLink.h>
#include <MVPbkBaseContact.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KOneStore = 1;
const TInt KNoContacts = 0;

} /// namespace

// --------------------------------------------------------------------------
// TPbk2CopyContactsUtil::CopyContactTargetStoresLC
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray* TPbk2CopyContactsUtil::
        CopyContactTargetStoresLC(
            MPbk2ContactUiControl& aControl,
            CVPbkContactStoreUriArray* aDiscardedUris)
    {
    CVPbkContactStoreUriArray* targets = CVPbkContactStoreUriArray::NewLC();

    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();

    // Add the number of writable stores
    CPbk2StoreConfiguration& config = appServices->StoreConfiguration();
    CVPbkContactStoreUriArray* stores =
        config.SupportedStoreConfigurationL();
    CleanupStack::PushL(stores);
    TInt count = stores->Count();
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkContactStore* store =
            appServices->ContactManager().
                ContactStoresL().Find((*stores)[i]);
        if ( store && !store->StoreProperties().ReadOnly() )
            {
            TVPbkContactStoreUriPtr uri = store->StoreProperties().Uri();
            if ( aDiscardedUris && aDiscardedUris->IsIncluded( uri ) )
                {
                // Do not append, the URI needs to be discarded
                }
            else
                {
                targets->AppendL( uri );
                }
            }
        }
    CleanupStack::PopAndDestroy(); // stores

    CleanupStack::PopAndDestroy(); // appServices

    // Get the number of selected contacts
    MVPbkContactLinkArray* selected = 
        aControl.SelectedContactsOrFocusedContactL();
    CleanupDeletePushL( selected );
    if ( selected ) // selected is NULL if the names list is empty
        {
        count = selected->Count();
        }
    else
        {
        count = KNoContacts;
        }

    // Construct a second URI array for all the writable stores
    // of selected contacs
    CVPbkContactStoreUriArray* writablesInSelected =
        CVPbkContactStoreUriArray::NewLC();
    for ( TInt i = 0; i < count; ++i )
        {
        const MVPbkContactStoreProperties& props =
            selected->At(i).ContactStore().StoreProperties();
        // If the store is not readonly and it is not allready in the array,
        // add it there
        if ( !props.ReadOnly() && !writablesInSelected->IsIncluded
                ( props.Uri() ) )
            {
            writablesInSelected->AppendL( props.Uri() );
            }
        }

    if ( writablesInSelected->Count() == KOneStore )
        {
        // The contact selection contains contacts only from one writable
        // store. Remove that store from the target count.
        targets->Remove( ( *writablesInSelected )[0] );
        }

    CleanupStack::PopAndDestroy( 2 ); // selected, writablesInSelected
    return targets;
    }

//  End of File
