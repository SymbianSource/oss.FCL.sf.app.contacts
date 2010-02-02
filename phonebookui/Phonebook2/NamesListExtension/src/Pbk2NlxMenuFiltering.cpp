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
* Description:  Phonebook 2 menu filtering helper.
*
*/


// INCLUDE FILES
#include "Pbk2NlxMenuFiltering.h"

#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkTopContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkBaseContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CPbk2StoreConfiguration.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactView.h>
#include <MPbk2ContactViewSupplier.h>
#include <MPbk2ContactViewSupplier2.h>
#include <MPbk2ContactLinkIterator.h>
#include <MPbk2UiControlCmdItem.h>
#include <Pbk2Commands.hrh>
#include "pbk2nameslistex.hrh"


const TInt KMinNumberOfTopContactsForRearranging = 2;

TBool Pbk2NlxMenuFiltering::OpenReady( MPbk2ContactUiControl& aControl )
    {
    
   MPbk2ContactUiControl2* tempControl = 
       reinterpret_cast<MPbk2ContactUiControl2*>
           (aControl.ContactUiControlExtension(KMPbk2ContactUiControlExtension2Uid ));

    return (!(tempControl->FocusedCommandItem()) && (aControl.NumberOfContacts() > 0) &&
           !aControl.ContactsMarked());
    }
    
TBool Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( MPbk2ContactUiControl& aControl )
    {
    MPbk2ContactUiControl2* tempControl = 
        reinterpret_cast<MPbk2ContactUiControl2*>
            (aControl.ContactUiControlExtension(KMPbk2ContactUiControlExtension2Uid ));

    const MPbk2UiControlCmdItem* cmdItem = tempControl->FocusedCommandItem();
    if ( cmdItem )
        {
        return cmdItem->CommandId() == EPbk2CmdAddFavourites;
        }
    return EFalse;
    }

TBool Pbk2NlxMenuFiltering::MyCardCmdSelected( MPbk2ContactUiControl& aControl )
    {
    MPbk2ContactUiControl2* tempControl = 
        reinterpret_cast<MPbk2ContactUiControl2*>
            (aControl.ContactUiControlExtension(KMPbk2ContactUiControlExtension2Uid ));

    const MPbk2UiControlCmdItem* cmdItem = tempControl->FocusedCommandItem();
    if ( cmdItem )
        {
        return cmdItem->CommandId() == EPbk2CmdOpenMyCard;
        }
    return EFalse;
    }

TBool Pbk2NlxMenuFiltering::RclCmdSelectOptSelected( MPbk2ContactUiControl& aControl )
    {
    MPbk2ContactUiControl2* tempControl = 
        reinterpret_cast<MPbk2ContactUiControl2*>
            (aControl.ContactUiControlExtension(KMPbk2ContactUiControlExtension2Uid ));

    const MPbk2UiControlCmdItem* cmdItem = tempControl->FocusedCommandItem();
    if ( cmdItem )
        {
        return cmdItem->CommandId() == EPbk2CmdRcl;
        }
    return EFalse;
    }

TBool Pbk2NlxMenuFiltering::ReadOnlyContactSelectedL( MPbk2ContactUiControl& aControl )
    {
    TBool result = EFalse;
    const MVPbkBaseContact* contact = aControl.FocusedContactL();
    
    if (contact)
        {
        // create link to get store
        MVPbkContactLink* link = contact->CreateLinkLC();
        result = link->ContactStore().StoreProperties().ReadOnly();
        CleanupStack::PopAndDestroy();
        }
    
    return result;
    }

TBool Pbk2NlxMenuFiltering::TopReadyL( MPbk2ContactUiControl& aControl )
    {
    CVPbkContactStoreUriArray* currentConfig =
         Phonebook2::Pbk2AppUi()->ApplicationServices().
             StoreConfiguration().CurrentConfigurationL();
    TBool phMemOk = currentConfig->IsIncluded(
                        VPbkContactStoreUris::DefaultCntDbUri() );
    delete currentConfig;
    
    return phMemOk &&
           NonTopContactSelectedL( aControl ) &&
           !ReadOnlyContactSelectedL( aControl );
    }
    
TBool Pbk2NlxMenuFiltering::TopContactSelectedL( MPbk2ContactUiControl& aControl )
    {
    TBool result = EFalse;
    const MVPbkBaseContact* contact = aControl.FocusedContactL();
    
    if (contact)
        {
        result = CVPbkTopContactManager::IsTopContact( *contact );
        }
    return result;
    }

TBool Pbk2NlxMenuFiltering::NonTopContactSelectedL( MPbk2ContactUiControl& aControl )
    {
    TBool result = EFalse;
    const MVPbkBaseContact* contact = aControl.FocusedContactL();
    
    if (contact)
        {
        result = !CVPbkTopContactManager::IsTopContact( *contact );
        }
    return result;
    }

TBool Pbk2NlxMenuFiltering::TopRearrangingReadyL(MPbk2ContactUiControl& aControl )
    {
    MPbk2ContactViewSupplier2* viewSupplierExtension = 
	    reinterpret_cast<MPbk2ContactViewSupplier2*>(
	        Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().
	            MPbk2ContactViewSupplierExtension(
                    KMPbk2ContactViewSupplierExtension2Uid ));
    
    if ( viewSupplierExtension != NULL && TopContactSelectedL( aControl ) )
        {
        const MVPbkContactViewBase* topView = viewSupplierExtension->
            TopContactsViewL();
        if ( topView )
            {
            return topView->ContactCountL() >= KMinNumberOfTopContactsForRearranging;
            }
        }
    return EFalse;
    }

TBool Pbk2NlxMenuFiltering::TopContactMarkedL( MPbk2ContactUiControl& aControl )
    {
    MPbk2ContactViewSupplier2* viewSupplierExtension = 
	    reinterpret_cast<MPbk2ContactViewSupplier2*>(
	        Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().
	            MPbk2ContactViewSupplierExtension(
                    KMPbk2ContactViewSupplierExtension2Uid ));
    
    MVPbkContactViewBase* topView = NULL;
    if ( viewSupplierExtension != NULL )
        {
        topView = viewSupplierExtension->TopContactsViewL();
        }
    
    TBool topIncluded = EFalse;
    if ( topView )
        {
        MPbk2ContactLinkIterator* iterator =
            aControl.SelectedContactsIteratorL();

        if ( iterator )
            {
            CleanupDeletePushL( iterator );
        
            while( iterator->HasNext()&& !topIncluded )
                {
                MVPbkContactLink* link = iterator->NextL();
                CleanupDeletePushL( link );
                if ( topView->IndexOfLinkL( *link ) >= 0 )
                    {
                    topIncluded = ETrue;
                    }

                CleanupStack::PopAndDestroy();
                }
            CleanupStack::PopAndDestroy(); // iterator
            }
        }
    
    return topIncluded;
    }

TBool Pbk2NlxMenuFiltering::NonTopContactMarkedL( MPbk2ContactUiControl& aControl )
    {
    MPbk2ContactViewSupplier2* viewSupplierExtension = 
	    reinterpret_cast<MPbk2ContactViewSupplier2*>(
	        Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().
	            MPbk2ContactViewSupplierExtension(
                    KMPbk2ContactViewSupplierExtension2Uid ));
        
    MVPbkContactViewBase* topView = NULL;
    if ( viewSupplierExtension != NULL )
        {
        topView = viewSupplierExtension->TopContactsViewL();
        }
    
    TBool nonTopIncluded = EFalse;
    if ( topView )
        {
        MPbk2ContactLinkIterator* iterator =
            aControl.SelectedContactsIteratorL();

        if ( iterator )
            {
            CleanupDeletePushL( iterator );
        
            while( iterator->HasNext()&& !nonTopIncluded )
                {
                MVPbkContactLink* link = iterator->NextL();
                CleanupDeletePushL( link );
                if ( topView->IndexOfLinkL( *link ) == KErrNotFound )
                    {
                    nonTopIncluded = ETrue;
                    }
                CleanupStack::PopAndDestroy();
                }
            CleanupStack::PopAndDestroy(); // iterator
            }
        }
    else
        {
        nonTopIncluded = ETrue;
        }
    
    return nonTopIncluded;
    }

TBool Pbk2NlxMenuFiltering::CheckDeletableInMarkedContactsL(
        MPbk2ContactUiControl& aControl )
    {
    MPbk2ContactLinkIterator* iterator =
        aControl.SelectedContactsIteratorL();
    
    TBool result = EFalse;

    if ( iterator )
        {
        CleanupDeletePushL( iterator );
        
        while( iterator->HasNext() && !result )
            {
            MVPbkContactLink* link = iterator->NextL();
            if ( link && 
                 !link->ContactStore().StoreProperties().ReadOnly() )
                {
                result = ETrue;
                }
            delete link;
            }
        CleanupStack::PopAndDestroy(); // iterator
        }
    
    return result;
    }

//  End of File
