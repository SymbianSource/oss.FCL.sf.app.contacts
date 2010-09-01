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
* Description:  The virtual phonebook commit contacts operation
*
*/



#ifndef COWNCONTACTLINKOPERATION_H
#define COWNCONTACTLINKOPERATION_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactOperationBase.h>

// FORWARD DECLARAIONS
class MVPbkContactLink;
class MVPbkSingleContactLinkOperationObserver;

namespace VPbkCntModel {

NONSHARABLE_CLASS( COwnContactLinkOperation ): 
        public CActive,
        public MVPbkContactOperationBase
    {
    public:
        /**
        * Constructor.
        * @param aOwnContactLink own contact link
        * @param aObserver an observer to pass the own contact link
        *
        */
    	COwnContactLinkOperation(
        		MVPbkContactLink* aOwnContactLink,
        		MVPbkSingleContactLinkOperationObserver& aObserver );
        ~COwnContactLinkOperation();

    private: // From CActive
        void RunL();
        void DoCancel();

    private: // Data
        /// Ref: Own contact link, passed to the observer, if not canceled
        MVPbkContactLink* iOwnContactLink;
        /// Ref: an observer to notify
        MVPbkSingleContactLinkOperationObserver& iObserver;
    };

} // namespace VPbkCntModel

#endif // COWNCONTACTLINKOPERATION_H

//End of file
