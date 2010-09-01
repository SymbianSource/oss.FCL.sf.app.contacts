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
* Description:  Implements the Sort order acquirer inteface to provide
*                the All contacts view sort.
*
*/


#ifndef CPBK2SORTORDERPROVIDER_H
#define CPBK2SORTORDERPROVIDER_H

// INCLUDES
#include <CVPbkSortOrderAcquirer.h>


// FORWARD DECLARATIONS
class CPbk2SortOrderManager;
class CPbk2StoreConfiguration;
class MVPbkFieldTypeList;
class TVPbkContactStoreUriPtr;

/**
 * Phonebook 2 All contacts view sort order provider. 
 */
class CPbk2SortOrderProvider :
        public CVPbkSortOrderAcquirer
    {
    public: // constructor and destructor
        /**
         * ECom constructor.
         * @param aParam    Construction parameters.
         */
        static CPbk2SortOrderProvider* NewL(TAny* aParam);

        /**
         * Destructor.
         */
        ~CPbk2SortOrderProvider();

    public: // from CVPbkSortOrderAcquirer
        TBool ApplySortOrderToStoreL( 
                const TVPbkContactStoreUriPtr& aStoreUri );
        const MVPbkFieldTypeList& SortOrder() const;
        
    private: // implementation
        CPbk2SortOrderProvider();
        void ConstructL(const MVPbkFieldTypeList& aMasterFieldTypeList);

    private: // data members
        //Own: sort order manager
        CPbk2SortOrderManager* iAllContactsViewSortOrder;
        //Own: stores used by phonebook application
        CPbk2StoreConfiguration* iStoreConfiguration;

    };

#endif // CPBK2SORTORDERPROVIDER_H

// End of file
