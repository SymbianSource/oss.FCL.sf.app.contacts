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
* Description:  Phonebook 2 AIW interest item array.
*
*/


// INCLUDE FILES
#include "CPbk2AiwInterestArray.h"

// Phonebook 2
#include "CPbk2AiwInterestItemFactory.h"
#include <CPbk2CommandHandler.h>
#include "Pbk2InternalCommands.hrh"

// Virtual Phonebook
#include <MVPbkBaseContactField.h>
#include <MVPbkContactLink.h>

// System includes
#include <AiwServiceHandler.h>


// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::CPbk2AiwInterestArray
// --------------------------------------------------------------------------
//
inline CPbk2AiwInterestArray::CPbk2AiwInterestArray()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::~CPbk2AiwInterestArray
// --------------------------------------------------------------------------
//
CPbk2AiwInterestArray::~CPbk2AiwInterestArray()
    {
    iAiwQueue.Close();
    delete iInterestItemFactory;
    iInterestItems.ResetAndDestroy();
    delete iServiceHandler;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AiwInterestArray::ConstructL()
    {
    iServiceHandler = CAiwServiceHandler::NewL();
    iInterestItemFactory = CPbk2AiwInterestItemFactory::NewL
        ( *iServiceHandler );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwInterestArray* CPbk2AiwInterestArray::NewL()
    {
    CPbk2AiwInterestArray* self = new ( ELeave ) CPbk2AiwInterestArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::QueueInterestL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestArray::QueueInterestL
        ( const TInt aInterestId, const TInt aMenuResourceId,
          const TInt aInterestResourceId,
          const TBool aAttachBaseServiceInterest)
    {
    // Queue interest item
    iAiwQueue.Append( TAiwQueueStruct( aInterestId, 
        aMenuResourceId, aInterestResourceId, aAttachBaseServiceInterest ) );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::ServiceCmdByMenuCmd
// --------------------------------------------------------------------------
//
TInt CPbk2AiwInterestArray::ServiceCmdByMenuCmd( TInt aMenuCmdId ) const
    {
    return iServiceHandler->ServiceCmdByMenuCmd( aMenuCmdId );
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::InterestId
// --------------------------------------------------------------------------
//
TInt CPbk2AiwInterestArray::InterestId() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestArray::DynInitMenuPaneL
        ( TInt aResourceId,  CEikMenuPane& aMenuPane,
          const MPbk2ContactUiControl& aControl )
    {
    // purge aiw interest queue
    PurgeAiwQueueL();

    // Offer the menu for AIW framework first
    TBool ret = iServiceHandler->HandleSubmenuL( aMenuPane );

    if ( !ret )
        {
        // The submenu was not handled by AIW framework,
        // offer it to interest items
        for (TInt j = 0; j < iInterestItems.Count(); ++j)
            {
            MPbk2AiwInterestItem* interestItem = iInterestItems[j];
            ret = interestItem->DynInitMenuPaneL
                ( aResourceId, aMenuPane, aControl );
            if ( ret )
                {
                // Do not continue the loop if menu was handled
                break;
                }
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::HandleCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestArray::HandleCommandL
        ( TInt aMenuCommandId, MPbk2ContactUiControl& aControl,
          TInt aServiceCommandId /*= KNullHandle*/ )
    {
    TBool ret = EFalse;

    // purge aiw interest queue
    PurgeAiwQueueL();

    // Send key is bound to call command or prepend command
    if ( !aControl.ContactsMarked() && aMenuCommandId == EPbk2CmdCall || 
         aMenuCommandId == EPbk2CmdPrepend )
        {
        aServiceCommandId = KAiwCmdCall;
        }
    else if ( aMenuCommandId == EPbk2CmdPoC )
        {
        aServiceCommandId = KAiwCmdPoC;
        }
    else if ( aMenuCommandId == KOtasAiwCmdSynchronize || 
            aMenuCommandId == KOtasAiwCmdSettings )
        {
        aServiceCommandId = aMenuCommandId;
        }

    // If service command parameter was not given, ask it from AIW framework
    if ( !aServiceCommandId )
        {
        aServiceCommandId =
            iServiceHandler->ServiceCmdByMenuCmd( aMenuCommandId );
        }

    // There was a provider
    if ( aServiceCommandId )
        {
        // Offer the command to interest items
        for ( TInt j = 0; j < iInterestItems.Count(); ++j )
            {
            MPbk2AiwInterestItem* interestItem = iInterestItems[j];
            ret = interestItem->HandleCommandL(
                aMenuCommandId, aControl, aServiceCommandId );
            if ( ret )
                {
                // Do not continue the loop if command was handled
                break;
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::AttachL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestArray::AttachL
        ( const TInt aMenuResourceId, const TInt aInterestResourceId,
          const TBool aAttachBaseServiceInterest )
    {
    // Attach criteria items to menu
    iServiceHandler->AttachMenuL( aMenuResourceId, aInterestResourceId );

    // If wanted, attach also base interest
    if ( aAttachBaseServiceInterest )
        {
        iServiceHandler->AttachL( aInterestResourceId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::PurgeAiwQueueL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestArray::PurgeAiwQueueL()
    {
    for ( TInt i = 0; i < iAiwQueue.Count(); ++i )
        {
        TAiwQueueStruct aiwInterest = iAiwQueue[i];

        // Create the item
        MPbk2AiwInterestItem* item = CreateInterestItemL
            ( aiwInterest.iInterestId );
        // And attach it
        AttachL( item, aiwInterest.iMenuResourceId, 
              aiwInterest.iInterestResourceId, 
              aiwInterest.iAttachBaseServiceInterest );
        }

    iAiwQueue.Reset();
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::CreateInterestItemL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPbk2AiwInterestArray::CreateInterestItemL
        ( const TInt aInterestId )
    {
    MPbk2AiwInterestItem* interestItem = NULL;

    TInt count = iInterestItems.Count();
    TBool found = EFalse;
    for ( TInt i=0; i<count; ++i )
        {
        interestItem = iInterestItems[i];
        if ( interestItem->InterestId() == aInterestId )
            {
            found = ETrue;
            break;
            }
        }

    if ( !found )
        {
        // There is not yet an interest item for the interest, create new
        interestItem =
            iInterestItemFactory->CreateInterestItemL( aInterestId );
        }

    return interestItem;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestArray::AttachL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestArray::AttachL
        ( MPbk2AiwInterestItem* aInterest, const TInt aMenuResourceId,
          const TInt aInterestResourceId,
          const TBool aAttachBaseServiceInterest )
    {
    aInterest->AttachL( aMenuResourceId, aInterestResourceId,
        aAttachBaseServiceInterest );

    // Append the interest item to the array, if it is not already there
    TBool found = EFalse;
    TInt count = iInterestItems.Count();
    for ( TInt i=0; i<count; ++i )
        {
        if ( iInterestItems[i] == aInterest )
            {
            found = ETrue;
            break;
            }

        }
    if ( !found )
        {
        User::LeaveIfError( iInterestItems.Append( aInterest ) );
        }
    }

// End of File
