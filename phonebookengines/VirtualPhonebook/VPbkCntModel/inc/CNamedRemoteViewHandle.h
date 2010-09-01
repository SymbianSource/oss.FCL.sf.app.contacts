/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook named remote view .
*
*/


#ifndef CNAMEDREMOTEVIEWHANDLE_H
#define CNAMEDREMOTEVIEWHANDLE_H

// INCLUDES
#include <e32base.h>
#include <cntviewbase.h>

namespace VPbkCntModel {

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * Named remote view entry.
 */
NONSHARABLE_CLASS( CNamedRemoteViewHandle ) : 
        public CBase, 
        private MContactViewObserver
    {
    public: // constructor and destructor
        /**
         * Constructor.
         */
        static CNamedRemoteViewHandle* NewLC();

        /**
         * Destructor.
         */
        ~CNamedRemoteViewHandle();

    public: // interface
        /**
         * Create remote view.
         */
        void CreateRemoteViewL(
                const TDesC& aViewName,
                CContactDatabase& aDatabase,
                RContactViewSortOrder aSortOrder,
                TContactViewPreferences aViewPreferences);

    private: // from MContactViewObserver
        void HandleContactViewEvent(
             const CContactViewBase& aView, 
             const TContactViewEvent& aEvent);

    private: // implementation
        CNamedRemoteViewHandle();
        void ConstructL();

    private: // data members
        /// Own: remote view handle
        CContactNamedRemoteView* iRemoteViewHandle;
    };

} // namespace VPbkCntModel

#endif // CNAMEDREMOTEVIEWHANDLE_H

// End of File
