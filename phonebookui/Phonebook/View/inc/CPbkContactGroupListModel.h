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


#ifndef __CPbkContactGroupListModel_H__
#define __CPbkContactGroupListModel_H__


//  INCLUDES
#include "CPbkContactViewListModelBase.h"
#include <cntdef.h>         // TContactItemId
#include "PbkIconId.hrh"    // TPbkIconId


//  FORWARD DECLARATIONS
class CContactViewBase;


// CLASS DECLARATION

/**
 * Listbox model for CPbkContactViewListControl.
 */
NONSHARABLE_CLASS(CPbkContactGroupListModel) : 
        public CPbkContactViewListModelBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aGroupView reference to contact model view
         */
        static CPbkContactGroupListModel* NewL(CContactViewBase& aGroupView);

        /**
         * Destructor.
         */ 
        ~CPbkContactGroupListModel();

    private:  // from CPbkContactGroupListModelBase
        void FormatBufferForContactL(const CViewContact& aViewContact) const;
        void FormatBufferForGroupL(const CViewContact& aViewContact) const;
        void FormatEmptyBuffer() const;

    private:  // Implementation
        CPbkContactGroupListModel(CContactViewBase& aView);
        void ConstructL();
        void AppendName(const TDesC& aName) const;

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
    };


#endif // __CPbkContactGroupListModel_H__

// End of File
