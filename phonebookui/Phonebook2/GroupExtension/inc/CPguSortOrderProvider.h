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
* Description:  Phonebook 2 all groups view sort order provider.
*
*/


#ifndef CPGUSORTORDERPROVIDER_H
#define CPGUSORTORDERPROVIDER_H

// INCLUDES
#include <CVPbkSortOrderAcquirer.h>

// FORWARD DECLARATIONS
class CPguSortOrder;
class CPbk2StoreConfiguration;
class MVPbkFieldTypeList;
class TVPbkContactStoreUriPtr;

// CLASS DECLARATION

/**
 * Phonebook 2 all groups view sort order provider.
 * Responsible implementing the sort order acquirer interface
 * for providing all groups view.
 */
class CPguSortOrderProvider : public CVPbkSortOrderAcquirer
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParam    Construction parameters.
         * @return  A new instance of this class.
         */
        static CPguSortOrderProvider* NewL(
                TAny* aParam );

        /**
         * Destructor.
         */
        ~CPguSortOrderProvider();

    public: // From CVPbkSortOrderAcquirer
        TBool ApplySortOrderToStoreL(
                const TVPbkContactStoreUriPtr& aStoreUri );
        const MVPbkFieldTypeList& SortOrder() const;

    private: // Implementation
        CPguSortOrderProvider();
        void ConstructL(
                const MVPbkFieldTypeList& aMasterFieldTypeList );

    private: // Data
        /// Own: Sort order manager
        CPguSortOrder* iAllGroupsViewSortOrder;
        /// Own: Stores used by Phonebook application
        CPbk2StoreConfiguration* iStoreConfiguration;
    };

#endif // CPGUSORTORDERPROVIDER_H

// End of File
