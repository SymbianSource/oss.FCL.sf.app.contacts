/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A list for sort order aquirer implementations
*
*/


#ifndef VPBKSIMSTORE_CSORTORDERACQUIRERLIST_H
#define VPBKSIMSTORE_CSORTORDERACQUIRERLIST_H

#include <CVPbkSortOrderAcquirer.h>
#include <CVPbkEComImplementationsList.h>
#include <VPbkSortOrderAcquirerUid.h>

namespace VPbkSimStore {

NONSHARABLE_CLASS( CSortOrderAcquirerList ) 
    :   public CVPbkEComImplementationsList<CVPbkSortOrderAcquirer,
                CVPbkSortOrderAcquirer::TSortOrderAcquirerParam>
    {
    public: // Construction and destruction
        static CSortOrderAcquirerList* NewLC(
                CVPbkSortOrderAcquirer::TSortOrderAcquirerParam& aParam );
    };

inline CSortOrderAcquirerList* CSortOrderAcquirerList::NewLC(
        CVPbkSortOrderAcquirer::TSortOrderAcquirerParam& aParam )
    {
    CSortOrderAcquirerList* self = new(ELeave) CSortOrderAcquirerList;
    CleanupStack::PushL(self);
    self->LoadImplementationsL( 
            TUid::Uid( KVPbkSortOrderAcquirerInterfaceUID ),
            aParam );
    return self;
    }
    
} // namespace VPbkSimStore

#endif // VPBKSIMSTORE_CSORTORDERACQUIRERLIST_H
