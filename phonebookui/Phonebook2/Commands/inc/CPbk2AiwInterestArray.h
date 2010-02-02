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
* Description:  Phonebook 2 AIW interest item array.
*
*/


#ifndef CPBK2AIWINTERESTARRAY_H
#define CPBK2AIWINTERESTARRAY_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2AiwInterestItem.h>

// FORWARD DECLARATIONS
class CPbk2AiwInterestItemFactory;
class CAiwServiceHandler;

/**
 * Phonebook 2 AIW interest item array.
 */
NONSHARABLE_CLASS(CPbk2AiwInterestArray) : public CBase,
                                           public MPbk2AiwInterestItem
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2AiwInterestArray* NewL();

        /**
         * Destructor.
         */
        ~CPbk2AiwInterestArray();

    public: // Interface
 
        /**
         * Queue an AIW interest to a queue.
         * The queue will be purged and the interests created and 
         * attached when the interest is needed.
         *
         * @param aInterestId                   Id of the AIW interest.
         * @param aMenuResourceId               Resource id of the menu
         *                                      to attach the id.
         * @param aInterestResourceId           Resource id of the
         *                                      AIW interest.
         * @param aAttachBaseServiceInterest    If ETrue, attaches the AIW
         *                                      resource id to AIW base
         *                                      services also (enables
         *                                      command handling via AIW
         *                                      but without menu commands).
         */
        void QueueInterestL(
                const TInt aInterestId,
                const TInt aMenuResourceId,
                const TInt aInterestResourceId,
                const TBool aAttachBaseServiceInterest );

        /**
         * Returns the AIW service command id associated to
         * given menu command. If found, it means that there is a
         * provider which can handle the menu command.
         *
         * @see CAiwServiceHandler::ServiceCmdByMenuCmd
         * @param aMenuCmdId    Menu command id to inspect.
         * @return  Service command id or KAiwCmdNone.
         */
        TInt ServiceCmdByMenuCmd(
                TInt aMenuCmdId ) const;

    public: // From MPbk2AiwInterestItem
        TInt InterestId() const;
        TBool DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane& aMenuPane,
                const MPbk2ContactUiControl& aControl );
        TBool HandleCommandL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                TInt aServiceCommandId );

    private: // From MPbk2AiwInterestItem
        void AttachL(
                const TInt aMenuResourceId,
                const TInt aInterestResourceId,
                const TBool aAttachBaseServiceInterest );

    private: // Implementation
        CPbk2AiwInterestArray();
        void ConstructL();
        void PurgeAiwQueueL();
        MPbk2AiwInterestItem* CreateInterestItemL(
                const TInt aInterestId );
        void AttachL(
                MPbk2AiwInterestItem* aInterest,
                const TInt aMenuResourceId,
                const TInt aInterestResourceId,
                const TBool aAttachBaseServiceInterest );

    private: // Data structures
        struct TAiwQueueStruct
            {
            TAiwQueueStruct(
                const TInt aInterestId, 
                const TInt aMenuResourceId,
                const TInt aInterestResourceId,
                const TBool aAttachBaseServiceInterest ) :
                    iInterestId( aInterestId ), 
                    iMenuResourceId( aMenuResourceId ),
                    iInterestResourceId( aInterestResourceId ),
                    iAttachBaseServiceInterest( aAttachBaseServiceInterest )
                        {}

            // Data members
            const TInt iInterestId;
            const TInt iMenuResourceId;
            const TInt iInterestResourceId;
            const TBool iAttachBaseServiceInterest;
            };

    private: // Data
        /// Own: Service handler
        CAiwServiceHandler* iServiceHandler;
        /// Own: AIW interest item factory
        CPbk2AiwInterestItemFactory* iInterestItemFactory;
        /// Own: Array of interest items
        RPointerArray<MPbk2AiwInterestItem> iInterestItems;
        /// Own: Queue of AIW interests
        RArray<TAiwQueueStruct> iAiwQueue;
    };

#endif // CPBK2AIWINTERESTARRAY_H

// End of File
