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
* Description:  An ECOM interface for acquiring the presentation sort order.
*
*                Requires that the remote view name is in the display name field.
*
*/


#ifndef CVPBKSORTORDERACQUIRER_H
#define CVPBKSORTORDERACQUIRER_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class TVPbkContactStoreUriPtr;

/**
 * Virtual Phonebook sort order acquirer plugin interface. 
 * This interface is used to acquire the sort orders for the 
 * remote views supported by the repository.
 */
class CVPbkSortOrderAcquirer :
        public CBase
    {
    public: // construction parameter structure
        class TSortOrderAcquirerParam
            {
            public: // constructor
                TSortOrderAcquirerParam(
                    const MVPbkFieldTypeList& aMasterFieldTypeList);

            public: // data members
                /// Ref: master field type list
                const MVPbkFieldTypeList& iMasterFieldTypeList;
                /// Own: Spare for future extension
                TAny* iSpare;
                /// Own: Spare for future extension
                TAny* iSpare2;
            };

    public: // constructor and destructor
        /**
         * ECom constructor.
         * @param aUid  Unique identifier of this implementation.
         * @param aParam    Construction parameters.
         */
        static CVPbkSortOrderAcquirer* NewL(TUid aUid, 
                TSortOrderAcquirerParam aParam);

        /**
         * Destructor.
         */
        ~CVPbkSortOrderAcquirer();

    public: // interface
        
        /**
         * Answers to the question should this acquirer be applied to views
         * created from store identified by aStoreUri.
         *
         * param aStoreUri A store identifier
         * @return ETrue if this acquirer can be applied to the view from 
         *               aStoreUri. EFalse otherwise.
         */
        virtual TBool ApplySortOrderToStoreL( 
                const TVPbkContactStoreUriPtr& aStoreUri ) = 0;
                
        /**
         * Sort order used for this view. View name is encoded in the
         * ECom resource structure display name field. And the view 
		 * preferences in the opague data field.
         * @return Sort ordered field type list.
         */
        virtual const MVPbkFieldTypeList& SortOrder() const = 0;
        
    private: // data members
        /// Own: destructor ID key
        TUid iDtorIDKey;

    };

// INLINE FUNCTIONS
inline CVPbkSortOrderAcquirer::TSortOrderAcquirerParam::TSortOrderAcquirerParam(
        const MVPbkFieldTypeList& aMasterFieldTypeList) :
    iMasterFieldTypeList ( aMasterFieldTypeList )
    {
    }

inline CVPbkSortOrderAcquirer* CVPbkSortOrderAcquirer::NewL(
        TUid aUid, 
        TSortOrderAcquirerParam aParam)
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
        aUid, _FOFF(CVPbkSortOrderAcquirer, iDtorIDKey),
        static_cast<TAny*>(&aParam));

    return reinterpret_cast<CVPbkSortOrderAcquirer*>(ptr);
    }

inline CVPbkSortOrderAcquirer::~CVPbkSortOrderAcquirer()
    {
    REComSession::DestroyedImplementation(iDtorIDKey);
    }

#endif // CVPBKSORTORDERACQUIRER_H

// End of file
