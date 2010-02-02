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
* Description: MyCard class observes the state of MyCard, whether its created
* or not. Also responsible for linking the data to MyCard.
*
*/


#ifndef CPBK2MYCARD_H_
#define CPBK2MYCARD_H_

//  INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactLinkOperationObserver.h>
#include <MVPbkContactStoreObserver.h>
#include <MPbk2ImageOperationObservers.h>
#include <MVPbkSingleContactOperationObserver.h>
#include "mpbk2mycardobserver.h"
#include <TPbk2IconId.h>

class MPbk2CmdItemVisibilityObserver;
class MVPbkContactOperationBase;
class MVPbkContactStore;
class MPbk2MyCardObserver;
class CPbk2ImageManager;
class MPbk2ImageOperation;
class CVPbkContactManager;
class CPbk2ApplicationServices;
class MVPbkBaseContact;
class CFbsBitmap;

/**
 *
 */
NONSHARABLE_CLASS( CPbk2MyCard ) : public CBase,
                                   public MVPbkSingleContactLinkOperationObserver,
                                   public MVPbkContactStoreObserver,
                                   public MVPbkSingleContactOperationObserver,
                                   public MPbk2ImageGetObserver
    {
    public: // Construction & destruction
    	
        static CPbk2MyCard* NewL( MVPbkContactStore& aPhoneStore );
        ~CPbk2MyCard();

        void SetObserverL(MPbk2MyCardObserver& aObserver);
        void RemoveObserver(MPbk2MyCardObserver& aObserver);
        
        enum{ EUnkown = 0x0, 
			  EExisting, 
			  ENonExisting };
        inline TInt MyCardState() {return iMyCardState;}
        
    public:
    	/*
    	 * Returns my card link, NULL if it doesn't exist
    	 */
    	MVPbkContactLink* MyCardLink() const;
    	
    	/*
    	 * Returns my card thumbnail bitmap, NULL if it doesn't exist.
    	 * 
    	 */
    	CFbsBitmap* MyCardBitmap() const;
    	
    	/*
    	 * Returns MyCard icon id if created, otherwise empty TPbk2IconId
    	 */
    	const TPbk2IconId& MyCardIconId() const;
    	    	
    	/*
    	 * Returns MVPbkStoreContact if available, NULL otherwise
    	 */
    	MVPbkStoreContact* MyCardStoreContact() const;
    	
        
    private: // From MVPbkSingleContactLinkOperationObserver
        void VPbkSingleContactLinkOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkContactLink* aLink );

        void VPbkSingleContactLinkOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactStoreObserver
        void StoreReady( MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // From MVPbkSingleContactOperationObserver
       void VPbkSingleContactOperationComplete(
    		   MVPbkContactOperationBase& aOperation,
    		   MVPbkStoreContact* aContact );
       
       void VPbkSingleContactOperationFailed(
    		   MVPbkContactOperationBase& aOperation,
    		   TInt aError );
       
    private: // From MPbk2ImageGetObserver
        virtual void Pbk2ImageGetComplete(
                MPbk2ImageOperation& aOperation,
                CFbsBitmap* aBitmap );
        virtual void Pbk2ImageGetFailed(
                MPbk2ImageOperation& aOperation,
                TInt aError );
        
    private:
        CPbk2MyCard( MVPbkContactStore& aPhoneStore );
        void ConstructL();

        void NotifyObservers( MPbk2MyCardObserver::TMyCardStatusEvent aEvent );
        
        /*
         * Starts loading thumbnail for the mycard contact if there is one
         */
        void LoadThumbnailL( MVPbkBaseContact& aContact );
        
    private: // data
    	TInt iMyCardState;
    	///  Own
        MVPbkContactOperationBase* iOperation;
        /// Ref
        MVPbkContactStore* iPhoneStore;
        /// Own
        MVPbkContactLink* iMyCardLink;
        /// 
        RPointerArray<MPbk2MyCardObserver> iObserverArray;
        /// Own. Phonebook image manager
		CPbk2ImageManager*   iImageManager;
		/// Own. Image operation
		MPbk2ImageOperation* iImageOperation;
		/// Own. for the store contact fetching 
		MVPbkContactOperationBase* 	iRetrieveOperation;
		// Ref. contact manager
		CVPbkContactManager* iContactManager;
		// Own. instance
		CPbk2ApplicationServices* iAppServices;
		// Own. MycardStoreContact
		MVPbkStoreContact*	iMyCardStoreContact;
		/// Ref. Thumbnail bitmap
		CFbsBitmap*			iThumbBitmap;
		/// default ID for the mycard icon
		TPbk2IconId iIconId;
		/// Empty id
		TPbk2IconId	iEmptyId;
    };

#endif /*CPBK2MYCARD_H_*/
