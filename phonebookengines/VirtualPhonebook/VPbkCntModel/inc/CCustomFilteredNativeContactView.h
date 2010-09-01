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
* Description:  Contacts Model store custom filtered native contact
*                view implementation. Derives from CContactViewBase.
*
*/


#ifndef VPBKCNTMODEL_CCUSTOMFILTEREDNATIVECONTACTVIEW_H
#define VPBKCNTMODEL_CCUSTOMFILTEREDNATIVECONTACTVIEW_H

// INCLUDES
#include <cntviewbase.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;

NONSHARABLE_CLASS( CCustomFilteredNativeContactView ): 
        public CContactViewBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aDb   Contact store.
         * @return  A new instance of this class.
         */
        static CCustomFilteredNativeContactView* NewL(
                CContactStore& aContactStore );
        /**
         * Destructor.
         */
        ~CCustomFilteredNativeContactView();

    public: // Interface

        /**
         * Sets custom filter view.
         *
         * @param aFilterView       View to set.
         */
        void SetCustomFilterView(
                MVPbkContactViewBase& aFilterView );

        /**
         * Sets native base view.
         *
         * @param aNativeBaseView   View to set.
         */
        void SetNativeBaseView(
                CContactViewBase& aNativeBaseView );

    public: // From CContactViewBase
        TContactItemId AtL(
                TInt aIndex ) const;
        const ::CViewContact& ContactAtL(
                TInt aIndex ) const;
        TInt CountL() const;
        TInt FindL(
                TContactItemId aId ) const;
        HBufC* AllFieldsLC(
                TInt aIndex,
                const TDesC& aSeparator ) const;
        TContactViewPreferences ContactViewPreferences();
        const RContactViewSortOrder& SortOrderL() const;

    private: // Implementation
        CCustomFilteredNativeContactView(
                CContactStore& aContactStore );
        void ConstructL();

    private: // Data
        /// Ref: Custom filter
        MVPbkContactViewBase* iFilteredView;
        /// Ref: Native base view
        CContactViewBase* iNativeBaseView;
        /// Ref: Contact store
        CContactStore& iContactStore;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CCUSTOMFILTEREDNATIVECONTACTVIEW_H

// End of File
