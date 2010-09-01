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
* Description:  Top Contact manager implementation.
*
*/


#ifndef VPBKTOPCONTACTMANAGERIMPL_H
#define VPBKTOPCONTACTMANAGERIMPL_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class CVPbkContactManager;
class MVPbkContactViewBase;
class MVPbkBaseContact;
class MVPbkContactFieldTextData;
class TVPbkContactStoreUriPtr;
class MVPbkOperationObserver;
template <typename T>
class MVPbkOperationResultObserver;
class MVPbkOperationErrorObserver;

// CLASS DECLARATION

/**
 * Implementation class for managing Top Contact properties of contacts.
 */
NONSHARABLE_CLASS( CVPbkTopContactManagerImpl ):
    public CBase
    {
    public:
    	/**
         * Creates the top contact manager.
    	 * @param aContactManager Contact manager which already has its stores opened.
    	 * 
    	 * @return Top contact manager instance
    	 */
        static CVPbkTopContactManagerImpl* NewL(
                CVPbkContactManager& aContactManager );
        
        /**
         * Creates the top contact manager passing an already constructed top
         * contacts view as a parameter. This greatly improves performance of adding 
         * new favorites as the top contact view contsrution can be a heavy operation.
         * 
         * Note, passing the view doesn't affect the view and links fetching 
         * (e.g. GetTopContactsViewL, GetTopContactLinksL, etc.)
         * 
         * @param aContactManager Contact manager which already has its stores opened.
         * @param aTopView top contact view which already has been constructed.
         * 
         * @return Top contact manager instance
         */
        static CVPbkTopContactManagerImpl* NewL(
               CVPbkContactManager& aContactManager,
               MVPbkContactViewBase& aTopView);

        /**
         * Creates the top contact manager impl.
         * Holds its own CVPbkContactManager instance.
         * 
         * @return Top contact manager instance 
         */
        static CVPbkTopContactManagerImpl* NewL();

        ~CVPbkTopContactManagerImpl();

    public:
        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* GetTopContactsViewL(
                MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkNonTopContactManager
        MVPbkContactOperationBase* GetNonTopContactsViewL(
                MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* GetTopContactLinksL(
                MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver );
        
        MVPbkContactOperationBase* GetNonTopContactLinksL(
                MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver );
        
        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* AddToTopL(
                const MVPbkContactLink& aContactLink,
                MVPbkOperationObserver& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* AddToTopL(
                const MVPbkContactLinkArray& aContactLinks,
                MVPbkOperationObserver& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* AddToTopL(
                const TDesC8& aPackedLinks,
                MVPbkOperationObserver& aObserver,
                MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* RemoveFromTopL(
        	const MVPbkContactLink& aContactLink,
        	MVPbkOperationObserver& aObserver,
        	MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* RemoveFromTopL(
        	const MVPbkContactLinkArray& aContactLinks,
        	MVPbkOperationObserver& aObserver,
        	MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* RemoveFromTopL(
        	const TDesC8& aPackedLinks,
        	MVPbkOperationObserver& aObserver,
        	MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        MVPbkContactOperationBase* SetTopOrderL(
	    	const MVPbkContactLinkArray& aContactLinks,
	    	MVPbkOperationObserver& aObserver,
	    	MVPbkOperationErrorObserver& aErrorObserver );

        /// @see CVPbkTopContactManager
        static TBool IsTopContact ( const MVPbkBaseContact& aContact );
        
    private: // construction
        void ConstructL();
        CVPbkTopContactManagerImpl();

    private: // data
        // Client's contact manager
        // or reference to own contact manager iOurContactManager
        CVPbkContactManager* iContactManager; // not owned
        CVPbkContactManager* iOurContactManager; // owned
        
        // Reference to a clients top contact view 
        MVPbkContactViewBase* iTopView; // not owned
    };

#endif //VPBKTOPCONTACTMANAGERIMPL_H
//End of file
