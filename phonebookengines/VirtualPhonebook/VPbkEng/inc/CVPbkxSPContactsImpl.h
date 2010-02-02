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
* Description:  xSP Contact manager implementation.
 *
*/


#ifndef VPBKXSPCONTACTSIMPL_H
#define VPBKXSPCONTACTSIMPL_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class CVPbkContactManager;
class MVPbkContactViewBase;
class MVPbkBaseContact;
class MVPbkStoreContact;
class MVPbkContactFieldTextData;
class TVPbkContactStoreUriPtr;
class MVPbkOperationObserver;
template <typename T> class MVPbkOperationResultObserver;
class MVPbkOperationErrorObserver;

// CLASS DECLARATION

/**
 * Implementation class for fetching xSP Contacts.
 */
NONSHARABLE_CLASS( CVPbkxSPContactsImpl ):
public CBase
    {
public:
    /**
     * Creates the xSP contact manager.
     * @param aContactManager Contact manager which already has its stores opened.
     * 
     * @return Top contact manager instance
     */
    static CVPbkxSPContactsImpl* NewL(
            CVPbkContactManager& aContactManager );

    /**
     * Creates the xSP contact manager impl.
     * Holds its own CVPbkContactManager instance.
     * 
     * @return xSP contact manager instance 
     */
    static CVPbkxSPContactsImpl* NewL();

    ~CVPbkxSPContactsImpl();

public:
    /// @see CVPbkTopContactManager
    MVPbkContactOperationBase* GetxSPContactLinksL(
            const MVPbkStoreContact& aContact,
            MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aResultObserver,
            MVPbkOperationErrorObserver& aErrorObserver );

private: // construction
    void ConstructL();
    CVPbkxSPContactsImpl();

private: // data
    CVPbkContactManager* iContactManager; // not owned
    };

#endif //VPBKXSPCONTACTSIMPL_H
//End of file
