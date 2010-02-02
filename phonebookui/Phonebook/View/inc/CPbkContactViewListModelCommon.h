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
*    Phonebook contact view listbox model.
*
*/


#ifndef __CPbkContactViewListModelCommon_H__
#define __CPbkContactViewListModelCommon_H__


//  INCLUDES
#include "CPbkContactViewListModelBase.h"
#include <cntviewbase.h>    // RContactViewSortOrder
#include "PbkIconId.hrh"    // TPbkIconId
#include "PbkContactViewListModelFactory.h"


//  FORWARD DECLARATIONS
class CPbkIconArray;
class MPbkContactUiControlExtension;
class CPbkExtGlobals;
/// Use for testing only!!!
class CPbkContactViewListModel_TestAccess;


// CLASS DECLARATION

/**
 * Offers some basic functionality for listbox models 
 * used with CPbkContactViewListControl.
 */
NONSHARABLE_CLASS(CPbkContactViewListModelCommon) : 
        public CPbkContactViewListModelBase,
        public MPbkContactViewListModel
    {   
    public:  // from MDesCArray
        TInt MdcaCount() const;
        TPtrC MdcaPoint(TInt aIndex) const;

    public: // From MPbkContactViewListModel
        void SetUnnamedText(const TDesC* aText);
        void SetContactUiControlExtension
                (MPbkContactUiControlExtension& aControlExtension);
        void RefreshSortOrderL();

    protected: // Constructor and destructor.
        /**
         * Constructor.
		 * @param aParams @see PbkContactViewListModelFactory
         */ 
        CPbkContactViewListModelCommon(
            PbkContactViewListModelFactory::TParams& aParams);

        /**
         * Second phase constructor.
         */ 
        void ConstructL();

        /**
         * Destructor.
         */ 
        ~CPbkContactViewListModelCommon();

    protected:  // from CPbkContactViewListModelBase
        void AddDynamicIconL(const CViewContact& aViewContact) const;
        void FormatBufferForGroupL(const CViewContact& aViewContact) const;
        void FormatEmptyBuffer() const;

    protected:  // Implementation
        void AppendIconIndex(TPbkIconId aIconId) const;
        void AppendName(const TDesC& aName) const;
        const RContactViewSortOrder& ViewSortOrder() const;
        void AddIconL(TPbkIconId aIconId) const;
        
    private: // Constructor
        /**
        * Constructor.
        */
        CPbkContactViewListModelCommon();

        /// Use for testing only!!!
        friend class CPbkContactViewListModel_TestAccess;

    private:  // Data
        enum 
            {
            /// Maximum length of a formatted list box item text 
            KMaxListBoxText = 256
            };
        /// Own: buffer for formatted list box item text
        TBuf<KMaxListBoxText> iFormattingBuffer;
        /// Own: index of the group label field
        TInt iGroupLabelFieldIndex;
        /// Ref: text to use for unnamed items
        const TDesC* iUnnamedText;
        /// Ref: icon index mapping array
        CPbkIconArray* iIconArray;
        /// Own: empty icon id
        TPbkIconId iEmptyIconId;
        /// Own: default icon id to use for all rows if != EPbkNullIconId.
        TPbkIconId iDefaultIconId;
        /// Own: view's sort order
        RContactViewSortOrder iViewSortOrder;
        /// Ref: for updating dynamic icon
        MPbkContactUiControlExtension* iControlExtension;
    };


#endif // __CPbkContactViewListModelCommon_H__

// End of File
