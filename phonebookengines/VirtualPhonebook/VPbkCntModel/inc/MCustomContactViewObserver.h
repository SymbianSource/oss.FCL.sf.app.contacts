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
* Description:  Custom contact view observer API.
*
*/

 
#ifndef VPBKCNTMODEL_MCUSTOMCONTACTVIEWOBSERVER_H
#define VPBKCNTMODEL_MCUSTOMCONTACTVIEWOBSERVER_H

// FORWARD DECLARATIONS
class MVPbkBaseContact;

// CLASS DECLARATIONS

namespace VPbkCntModel {

/**
 * Custom contact view observer API.
 */
NONSHARABLE_CLASS( MCustomContactViewObserver )
    {
    public: // Interface

        /**
         * Handles a contact view event sent by the custom made native view.
         *
         * @param aView     The view that sent the event.
         * @param aEvent    The event to handle.
         */
        virtual void HandleCustomContactViewEvent(
                const CContactViewBase& aView,
                const TContactViewEvent& aEvent ) = 0;
    
    protected: // Protected destructor

        /**
         * Destructor.
         */
        virtual ~MCustomContactViewObserver() { }
    };

} /// namespace

#endif // VPBKCNTMODEL_MCUSTOMCONTACTVIEWOBSERVER_H

// End of File
