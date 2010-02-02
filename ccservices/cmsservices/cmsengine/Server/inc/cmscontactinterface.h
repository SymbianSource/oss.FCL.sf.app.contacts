/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __CMSCONTACTINTERFACE__
#define __CMSCONTACTINTERFACE__

// INCLUDES
#include <e32base.h>
#include "cmsnotificationhandlerapi.h"

class MVPbkContactLink;
class MVPbkStoreContact;

class MCmsContactInterface
    {
    public:
    
      /*  enum TPhonebookEvent
    {
    EContactModified = 0,
    EContactDeleted        
    };*/
        
        virtual TBool OfferContactEventL( TCmsPhonebookEvent aEventType,
                                          const MVPbkContactLink* aContactLink ) = 0;

        virtual void ContactReadyL( TInt error, MVPbkStoreContact* aContact ) = 0;

        virtual const MVPbkStoreContact& Contact() const = 0;
        
        virtual TBool HandleField( HBufC* aFieldData ) = 0;

        virtual TBool HandleField( HBufC8* aFieldData ) = 0;

        virtual TBool HandleEnabledFields( HBufC* aFieldData ) = 0;
        
        virtual void HandleError( TInt aError ) = 0;
        
    };

#endif  //__CMSCONTACTINTERFACE__


// End of File
