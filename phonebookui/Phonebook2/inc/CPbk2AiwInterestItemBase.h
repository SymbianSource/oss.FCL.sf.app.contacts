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
* Description:  Phonebook 2 AIW interest item base.
*
*/


#ifndef CPBK2AIWINTERESTITEMBASE_H
#define CPBK2AIWINTERESTITEMBASE_H

// INCLUDES
#include <e32base.h>
#include <MPbk2AiwInterestItem.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * Phonebook 2 AIW interest item base.
 */
class CPbk2AiwInterestItemBase : public CActive,
                                 public MPbk2AiwInterestItem
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aInterestId       Interest id.
         * @param aServiceHandler   AIW service handler.
         * @return  A new instance of this class.
         */
        IMPORT_C CPbk2AiwInterestItemBase(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler );

        /**
         * Destructor.
         */
        virtual ~CPbk2AiwInterestItemBase(){}

    public: // Interface

        /**
         * Check if this object has already attached into 
         * a menupane.
         *
         * @param aMenuResourceId       Menu resource id.
         * @return  ETrue if already attached to menu, 
         *          EFalse if not attached.
         */
        IMPORT_C TBool IsMenuPaneAttached(
                TInt aMenuResourceId );

    public: // From MPbk2AiwInterestItem
        IMPORT_C TInt InterestId() const;
        IMPORT_C void AttachL(
                TInt aMenuResourceId,
                TInt aInterestResourceId,
                TBool aAttachBaseServiceInterest );
  
    private: // Data structures
        struct TAttachedResources
            {
            TAttachedResources()
                {
                iMenuResourceId = 0;
                iInterestResourceId = 0;
                iBaseServiceAttached = 0;
                }
            TInt iMenuResourceId;
            TInt iInterestResourceId;
            TBool iBaseServiceAttached;
            };

    protected: // Data
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;

    private: // Data
        /// Own: Interest id
        TInt iInterestId;
        /// Own: Attached resources
        CArrayFixFlat<TAttachedResources> iAttachedResources;
    };

#endif // CPBK2AIWINTERESTITEMBASE_H
            
// End of File
