/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An interface for loding contact store plug-ins.
*
*/


#ifndef M_MVPBKCONTACTSTORELOADER_H
#define M_MVPBKCONTACTSTORELOADER_H

#include <e32def.h>

class TVPbkContactStoreUriPtr;
class MVPbkContactStoreList;

/**
 *  An interface for loding contact store plug-ins.
 *
 *  A user of this interface needs to load an ECom store plug-in for
 *  the specified contact store.
 *
 */
NONSHARABLE_CLASS( MVPbkContactStoreLoader )
    {
protected:
    /// No to be destroyed by the client
    virtual ~MVPbkContactStoreLoader() {}

public:

    /**
     * A list of contact stores. If  a call LoadContatStoreL succeeded then
     * the new store is loaded to this list. If the store was already loaded
     * then the list doesn't change.
     *
     * @return A list of contact stores
     */
    virtual MVPbkContactStoreList& ContactStoreListL() const = 0;
    
    /**
     * Loads an store plugin for given URI
     * If there is no store plug-in for the aURI then nothing changes.
     *
     * @param aURI the store URI to be loaded.
     */
    virtual void LoadContactStoreL( const TVPbkContactStoreUriPtr& aURI ) = 0;
    };


#endif // M_MVPBKCONTACTSTORELOADER_H
