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
* Description:  Phonebook 2 AIW interest item base.
*
*/


#include "CPbk2AiwInterestItemBase.h"

// System includes
#include <AiwServiceHandler.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KGranularity = 1;

} /// namespace


// --------------------------------------------------------------------------
// CPbk2AiwInterestItemBase::CPbk2AiwInterestItemBase
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AiwInterestItemBase::CPbk2AiwInterestItemBase
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler ) :
            CActive( EPriorityIdle ),
            iServiceHandler( aServiceHandler ),
            iInterestId( aInterestId ),
            iAttachedResources( KGranularity )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemBase::~CPbk2AiwInterestItemBase
// --------------------------------------------------------------------------
//
//CPbk2AiwInterestItemBase::~CPbk2AiwInterestItemBase()
//    {
    // Do nothing
//    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemBase::IsMenuPaneAttached
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2AiwInterestItemBase::IsMenuPaneAttached
        ( TInt aMenuResourceId )
    {
    TBool menuFound = EFalse;

    // Check through attached resources
    TInt count = iAttachedResources.Count();
    for( TInt i = 0; i < count && !menuFound; ++i )
        {
        if( iAttachedResources[i].iMenuResourceId == aMenuResourceId )
            {
            menuFound = ETrue;
            }
        }

    return menuFound;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemBase::InterestId
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2AiwInterestItemBase::InterestId() const
    {
    return iInterestId;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemBase::AttachL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AiwInterestItemBase::AttachL
        ( TInt aMenuResourceId, TInt aInterestResourceId,
          TBool aAttachBaseServiceInterest )
    {
    TBool alreadyAttached = EFalse;
    TBool baseServiceAttached = EFalse;
    TAttachedResources resource;
    TInt index = KErrNotFound;

    TInt count = iAttachedResources.Count();

    // First check has the resource-interest pair already been attached
    for (TInt i=0; i<count; ++i)
        {
        if ( ( iAttachedResources[i].iMenuResourceId == aMenuResourceId) &&
            ( iAttachedResources[i].iInterestResourceId
                == aInterestResourceId ) )
            {
            alreadyAttached = ETrue;
            if (iAttachedResources[i].iBaseServiceAttached)
                {
                baseServiceAttached = ETrue;
                }
            // Store the index and break out
            index = i;
            break;
            }
        }

    if ( !alreadyAttached )
        {
        PBK2_PROFILE_START(Pbk2Profile::EAiwMenuAttach);
        // Attach criteria items to menu
        iServiceHandler.AttachMenuL(aMenuResourceId,
            aInterestResourceId);
        PBK2_PROFILE_END(Pbk2Profile::EAiwMenuAttach);

        resource.iInterestResourceId = aInterestResourceId;
        resource.iMenuResourceId = aMenuResourceId;
        }

    // If wanted, attach also base interest...
    if (aAttachBaseServiceInterest)
        {
        // ...if not already done
        if (!baseServiceAttached)
            {
            PBK2_PROFILE_START(Pbk2Profile::EAiwBaseServiceAttach);
            iServiceHandler.AttachL(aInterestResourceId);
            PBK2_PROFILE_END(Pbk2Profile::EAiwBaseServiceAttach);
            resource.iBaseServiceAttached = ETrue;
            }
        }

    if ( !alreadyAttached )
        {
        // Append to the array
        iAttachedResources.AppendL(resource);
        }
    else if ( !baseServiceAttached && resource.iBaseServiceAttached )
        {
        // We need to modify the base service flag
        iAttachedResources[index].iBaseServiceAttached = ETrue;
        }
    }

// End of File
