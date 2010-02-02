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
*       Dummy view extension, which does nothing.
*
*/


#ifndef __CPbkDummyControlExtension_H__
#define __CPbkDummyControlExtension_H__

//  INCLUDES
#include <MPbkContactUiControlExtension.h>
#include <PbkIconId.hrh>    // TPbkIconId

// CLASS DECLARATION

/**
 * Dummy control extension, which does nothing. 
 * Used as default implementation.
 */
class CPbkDummyControlExtension : public CBase,
                                  public MPbkContactUiControlExtension
    {
    public: // Constructor and destructor
        /**
         * Constructor.
         */
        CPbkDummyControlExtension();

        /**
         * Destructor.
         */
        ~CPbkDummyControlExtension();

    public: // From MPbkContactUiControlExtension
        void DoRelease();
        void IconArrayResourceId(TInt& aArrayIconInfoId, TInt& aArrayIconId);
        const TArray<TPbkIconId> GetDynamicIconsL(TContactItemId aContactId);
        const TArray<TPbkIconId> GetDynamicIconsL
                (const CPbkContactItem& aContact);
        void SetContactUiControlUpdate
                (MPbkContactUiControlUpdate* aContactUpdator);

    private: // Data
        /// Own: to return an empty array from GetDynamicIconsL.
        RArray<TPbkIconId> iIcons;
    };

#endif // __CPbkDummyControlExtension_H__

// End of File
