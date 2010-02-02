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
*     Implements AIW interest array for views.
*
*/


// INCLUDE FILES
#include <CPbkAiwInterestArray.h>

#include <CPbkAppGlobalsBase.h>
#include "MPbkAiwInterestItemFactory.h"
#include <MPbkAiwCommandObserver.h>

#include <AiwServiceHandler.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkAiwInterestArray::CPbkAiwInterestArray
        (CPbkContactEngine& aEngine):
            iEngine(aEngine)
    {
    }

CPbkAiwInterestArray::~CPbkAiwInterestArray()
    {
    iInterestItems.Close();

    delete iServiceHandler;
    }

inline void CPbkAiwInterestArray::ConstructL()
    {
    // Create AIW handler
    iServiceHandler = CAiwServiceHandler::NewL();  
    }

EXPORT_C CPbkAiwInterestArray* CPbkAiwInterestArray::NewL
        (CPbkContactEngine& aEngine)
    {
    CPbkAiwInterestArray* self =
        new (ELeave) CPbkAiwInterestArray(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


void CPbkAiwInterestArray::CreateInterestItemAndAttachL(
            TInt aInterestId,
            TInt aMenuResourceId,
            TInt aInterestResourceId,
            TBool aAttachBaseServiceInterest)
    {
    MPbkAiwInterestItem* aiwInterest = CPbkAppGlobalsBase::InstanceL()->
		AiwInterestItemFactory().CreateInterestItemL(
            aInterestId, *iServiceHandler, iEngine);

    if (aiwInterest)
        {
        aiwInterest->AttachL(
            aMenuResourceId, 
            aInterestResourceId, 
            aAttachBaseServiceInterest);

        // Append the interest item to the array, if it is not
        // already there
        TBool found(EFalse);
        TInt count = iInterestItems.Count();
        for (TInt i=0; i<count; ++i)
            {
            if (iInterestItems[i] == aiwInterest)
                {
                found = ETrue;
                break;
                }
            }
        if (!found)
            {
            User::LeaveIfError(iInterestItems.Append(aiwInterest));
            }
        }
    }


void CPbkAiwInterestArray::AttachL(
        TInt aMenuResourceId,
        TInt aInterestResourceId,
        TBool aAttachBaseServiceInterest)
    {
    // Attach criteria items to menu
    iServiceHandler->AttachMenuL(aMenuResourceId,
        aInterestResourceId);

    // If wanted, attach also base interest
    if (aAttachBaseServiceInterest)
        {
        iServiceHandler->AttachL(aInterestResourceId);
        }
    }

TBool CPbkAiwInterestArray::InitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane& aMenuPane,
        TUint aFlags)
    {
    // Offer the menu for AIW framework first
    TBool ret = iServiceHandler->HandleSubmenuL(aMenuPane);

	if (!ret)
		{
        // The submenu was not handled by AIW framework,
        // offer it to interest items
        for (TInt j = 0; j < iInterestItems.Count(); ++j)
            {
            MPbkAiwInterestItem* interestItem = iInterestItems[j];
            ret = interestItem->InitMenuPaneL(aResourceId, aMenuPane, aFlags);
            if (ret)
                {
                // Do not continue the loop if menu was handled
                break;
                }
            }
		}
    return ret;
    }

TBool CPbkAiwInterestArray::HandleCommandL(
        TInt aMenuCommandId,
        const CContactIdArray& aContacts,
        const TPbkContactItemField* aFocusedField /* =NULL */,
        TInt aServiceCommandId /* =KNullHandle */,
        MPbkAiwCommandObserver* aObserver /*= NULL*/)
    {
    TBool ret = EFalse;

    // If service command parameter was not given,
    // ask it from AIW framework
    if (!aServiceCommandId)
        {
        aServiceCommandId =
            iServiceHandler->ServiceCmdByMenuCmd(aMenuCommandId);
        }

    // There was a provider
    if (aServiceCommandId)
        {
        // Offer the command to interest items
        for (TInt j = 0; j < iInterestItems.Count(); ++j)
            {
            MPbkAiwInterestItem* interestItem = iInterestItems[j];
            ret = interestItem->HandleCommandL(
                aMenuCommandId,
                aContacts,
                aFocusedField,
                aServiceCommandId,
                aObserver);
            if (ret)
                {
                // Do not continue the loop if command was handled
                break;
                }
            }
		}
    return ret;
    }

//  End of File
