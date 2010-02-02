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
* Description:  An observer API for contact events
*
*/



#ifndef MVPBKSIMCONTACTOBSERVER_H
#define MVPBKSIMCONTACTOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CVPbkSimContact;

// CLASS DECLARATION

/**
*  An observer API for contact events
*
*/
class MVPbkSimContactObserver
    {
    public: // Types

        /**
        * SIM contact events
        */
        enum TEvent
            {
            /// Unknown operation
            EUnknown = 0,
            /// Reading of the contact complete
            ERead,
            /// Deleting of the contact complete
            EDelete,
            /// Saving of the contact complete
            ESave,
            };

    public: // New functions
        
        /**
        * Called after successful contact event
        * @param aEvent the contact event
        * @param aContact a pointer to contact if the event is ERead.
        *        Otherwise NULL. The client must take the ownership immediately.
        */
        virtual void ContactEventComplete( TEvent aEvent, 
            CVPbkSimContact* aContact ) = 0;

        /**
        * Called after failed contact event.
        * @param aEvent the contact event
        * @param aContact 
        * @param aError the error code.
        */
        virtual void ContactEventError( TEvent aEvent, 
            CVPbkSimContact* aContact, TInt aError ) = 0;
        
    protected:  // destructor
        
        /**
        * Destructor.
        */
        virtual ~MVPbkSimContactObserver() {}
    };

#endif      // MVPBKSIMCONTACTOBSERVER_H
            
// End of File
