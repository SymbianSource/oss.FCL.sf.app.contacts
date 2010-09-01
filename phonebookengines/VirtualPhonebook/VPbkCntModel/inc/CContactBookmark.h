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
* Description:  A contact bookmark implementation for Contacts Model contact
*
*/


#ifndef C_CCONTACTBOOKMARK_H
#define C_CCONTACTBOOKMARK_H

#include <e32base.h>
#include <cntdef.h>
#include <MVPbkContactBookmark.h>

namespace VPbkCntModel  {

// FORWARD DECLARATIONS
class CContactStore;

/**
 *  A contact bookmark implementation for Contacts Model contact
 *
 */
NONSHARABLE_CLASS( CContactBookmark ): public CBase,
						               public MVPbkContactBookmark
    {
public:

    /**
     * @param aId the unique Contacts Model contact id
     * @param aStore the store in which the contact resides
     * @return a new instance of this class
     */
    static CContactBookmark* NewL( TContactItemId aId,
        CContactStore& aStore );

    /**
     * @param aId the unique Contacts Model contact id
     * @param aStore the store in which the contact resides
     * @return a new instance of this class
     */
    static CContactBookmark* NewLC( TContactItemId aId,
        CContactStore& aStore );

    virtual ~CContactBookmark();

    /**
     * Returns the unique contact id
     *
     * @return the unique contact id
     */
    inline TContactItemId ContactId() const;

    /**
     * Returns the contact store of the contact
     *
     * @return the contact store of the contact
     */
    inline CContactStore& ContactStore() const;

private:
    CContactBookmark( TContactItemId aId, CContactStore& aStore );

private: // data

    /**
     * Own: the unique contact id
     */
    TContactItemId iId;

    /**
     * Ref: the contact store of the contact.
     */
    CContactStore& iStore;
    };

inline TContactItemId CContactBookmark::ContactId() const
    {
    return iId;
    }

inline CContactStore& CContactBookmark::ContactStore() const
    {
    return iStore;
    }
} // namespace VPbkCntModel

#endif // C_CCONTACTBOOKMARK_H
