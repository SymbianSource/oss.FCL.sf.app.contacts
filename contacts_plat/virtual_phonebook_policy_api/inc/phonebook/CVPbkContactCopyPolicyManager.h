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
* Description:  A class for finding a copy policy for different stores
*
*/



#ifndef CVPBKCONTACTCOPYPOLICYMANAGER_H
#define CVPBKCONTACTCOPYPOLICYMANAGER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class CImplementationInformation;
class CVPbkContactCopyPolicy;
class MVPbkContactCopyPolicy;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * A class for finding a copy policy for different stores.
 *
 * @lib VPbkEng.lib
 */
class CVPbkContactCopyPolicyManager : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         * @return A new instance of this class
         */
        IMPORT_C static CVPbkContactCopyPolicyManager* NewL();

        /**
         * Destructor.
         */
        virtual ~CVPbkContactCopyPolicyManager();

    public: // New functions

        /**
         * Returns the policy for the given store or NULL if there
         * is no policy for the given URI. Ownership is not given.
         *
         * @param aContactManager the contact manager that has
         *        an open store for aUri
         * @param aUri The URI of the target store
         * @return The copy policy or NULL. Ownership is not given.
         */
        IMPORT_C MVPbkContactCopyPolicy* GetPolicyL(
                CVPbkContactManager& aContactManager,
                const TVPbkContactStoreUriPtr& aUri);

    private: // Construction
        CVPbkContactCopyPolicyManager();
        void ConstructL();

    private:    // implementation
        CImplementationInformation* FindInfoL(
                const TVPbkContactStoreUriPtr& aUri);
        MVPbkContactCopyPolicy* FindPolicy(TUid aUid);
        MVPbkContactCopyPolicy* LoadPolicyL(TUid aUid,
                CVPbkContactManager& aContactManager);

    private:    // Data
        /// Own: copy policy implementation informations
        RPointerArray<CImplementationInformation> iImplementationInfoArray;
        /// Own: copy policies
        RPointerArray<CVPbkContactCopyPolicy> iPolicyArray;
    };

#endif      // CVPBKCONTACTCOPYPOLICYMANAGER_H

// End of File
