/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A sim contact sorting utility
*
*/



#ifndef VPBKSIMSTOREIMPL_CSIMCNTSORTUTIL_H
#define VPBKSIMSTOREIMPL_CSIMCNTSORTUTIL_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RVPbkSimFieldTypeArray;
class MVPbkSimContact;
class CVPbkSimCntField;
class MVPbkContactViewSortPolicy;

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  A sim contact sorting utility
*
*/
NONSHARABLE_CLASS(CSimCntSortUtil) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aSortOrder the sort order for the view that uses this class
        * @return a new instance of this class
        */
        static CSimCntSortUtil* NewL( const RVPbkSimFieldTypeArray& aSortOrder );
        
        /**
        * Destructor.
        */
        virtual ~CSimCntSortUtil();

    public: // New functions
        
        /**
        * Compares two contacts.
        * @param aLeft the left contact
        * @param aRight the right contact
        * @return 0  if aLeft and aRight have equal sorting order.
        *         >0  if aLeft is sorted after aRight
        *         <0  if aLeft is sorted before aRight.
        */
        TInt Compare( const MVPbkSimContact& aLeft, 
            const MVPbkSimContact& aRight );

    private:    // Construction

        /**
        * C++ constructor.
        */
        CSimCntSortUtil( const RVPbkSimFieldTypeArray& aSortOrder );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private:    // New functions
        class CSortKeyArray;
        /// Insert a new key to the array
        void InsertKey( CSortKeyArray& aKeyArray, 
            const CVPbkSimCntField& aField );

    private:    // Data
        /// The sort order
        const RVPbkSimFieldTypeArray& iSortOrder;
        /// Own: Virtual phonebook client provided sort utility or
        /// own simple sort policy if client provided is not found
        MVPbkContactViewSortPolicy* iSortPolicy;
        /// Own: an array for keys used by sortutil component
        CSortKeyArray* iLeftKeyArray;
        /// Own: an array for keys used by sortutil component
        CSortKeyArray* iRightKeyArray;
  
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CSIMCNTSORTUTIL_H
            
// End of File
