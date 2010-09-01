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
*       Methods for Phonebook contact view listbox model.
*
*/


// INCLUDE FILES
#include "PbkContactViewListModelFactory.h"  // This class
#include "CPbkContactViewListCacheModel.h"
#include "CPbkContactViewListFixedIconModel.h"


// ================= MEMBER FUNCTIONS =======================

PbkContactViewListModelFactory::TParams::TParams() : 
    iEngine(NULL), 
    iView(NULL), 
    iCacheSize(0),
    iIconArray(NULL), 
    iEmptyId(EPbkNullIconId),
    iDefaultId(EPbkNullIconId)
    {
    }

/**
 * PbkContactViewListModelFactory::CreateL implementation that creates and returns
 * MPbkContactViewListModel implementation.
 */
MPbkContactViewListModel* PbkContactViewListModelFactory::CreateL
    (PbkContactViewListModelFactory::TParams& aParams)
    {
    if (!(aParams.iView->ContactViewPreferences() & EGroupsOnly) &&
        aParams.iDefaultId == EPbkNullIconId)
        {
        // View contains contacts and contact icons must be provided
        return CPbkContactViewListCacheModel::NewL(aParams);
        }
    else
        {
        return CPbkContactViewListFixedIconModel::NewL(aParams);
        }
    }

//  End of File
