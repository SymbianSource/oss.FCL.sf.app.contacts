/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef PRESENCECACHEBUDDYIDCREATOR_H
#define PRESENCECACHEBUDDYIDCREATOR_H

#include <e32base.h>

class CVPbkContactManager;
class CPresenceCacheBuddyIdCreatorPrivate;

/**
 *
 *  @lib presencecacheutils.lib
 *  @since 9.2
 */
NONSHARABLE_CLASS( CPresenceCacheBuddyIdCreator ) : public CBase
    {
public: // constructor and destructor

    /**
     * Constructor.
     *
     * @param aContactManager pointer to contact manager. Ownership is not taken.
     * @return New instance of the CPresenceCacheBuddyIdCreator.
     */
    IMPORT_C static CPresenceCacheBuddyIdCreator* NewL(
            CVPbkContactManager* aContactManager = NULL );

    virtual ~CPresenceCacheBuddyIdCreator();

public: // New functions

    /**
     * Creates a buddy ID which is understood by Phonebook UI and is used for social data.
     * The buffer string representation is internal to phonebook.
     *
     * @param aContactLink A packed contact link (in the format used by Contact Service API,
     *                     which happens to be the same as the format used by Virtual Phonebook)
     * @return The buddy ID, according to the format of XSP ID field of VPbk.
     */
    IMPORT_C HBufC* CreateBuddyIdLC( const TDesC8& aContactLink );


private:
    inline CPresenceCacheBuddyIdCreator();
    inline void ConstructL( CVPbkContactManager* aContactManager );

private: // data
    CPresenceCacheBuddyIdCreatorPrivate* iImpl;

    };

#endif // PRESENCECACHEBUDDYIDCREATOR_H
