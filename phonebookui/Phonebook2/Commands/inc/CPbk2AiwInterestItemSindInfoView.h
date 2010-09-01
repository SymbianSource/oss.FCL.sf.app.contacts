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
* Description:  Phonebook 2 SIND info view AIW interest item.
*
*/


#ifndef CPBK2AIWINTERESTITEMSINDINFOVIEW_H
#define CPBK2AIWINTERESTITEMSINDINFOVIEW_H

//  INCLUDES
#include <e32base.h>
#include "CPbk2AiwInterestItemBase.h"

// FORWARD DECLARATIONS
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * Phonebook 2 SIND info view AIW interest item.
 */
NONSHARABLE_CLASS(CPbk2AiwInterestItemSindInfoView) :
        public CPbk2AiwInterestItemBase

    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aInterestId       Interest id.
         * @param aServiceHandler   AIW service handler.
         * @return  A new instance of this class.
         */
        static CPbk2AiwInterestItemSindInfoView* NewL(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler );

        /**
         * Destructor.
         */
        ~CPbk2AiwInterestItemSindInfoView();

    public: // From MPbk2AiwInterestItem
        TBool DynInitMenuPaneL(
                const TInt aResourceId,
                CEikMenuPane& aMenuPane,
                const MPbk2ContactUiControl& aControl );
        TBool HandleCommandL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                TInt aServiceCommandId );

    private: // From CActive
        void DoCancel();
        void RunL();

    private: // Implementation
        CPbk2AiwInterestItemSindInfoView(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler );
    };

#endif // CPBK2AIWINTERESTITEMSINDINFOVIEW_H

// End of File
