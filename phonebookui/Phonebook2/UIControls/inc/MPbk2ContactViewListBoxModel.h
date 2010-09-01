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
* Description: 
*    Abstract interface for Phonebook 2 contact view list box model.
*
*/


#ifndef MPBK2CONTACTVIEWLISTBOXMODEL_H
#define MPBK2CONTACTVIEWLISTBOXMODEL_H

//  INCLUDES
#include <bamdesca.h>   // MDesCArray

// CLASS DECLARATION

/**
 * Abstract base class for listbox models.
 */
class MPbk2ContactViewListBoxModel : public MDesCArray
    {
    public:  // Destructor

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactViewListBoxModel() {}

    public: // Interface

        /**
         * Sets the text to use for unnamed list items.
         *
         * @param aText reference to the text to use for unnamed list items.
         *              The referenced descriptor must exist for the lifetime
         *              of this object. If aText==NULL unnamed list items are
         *              presented with an empty text.
         */
        virtual void SetUnnamedText(const TDesC* aText) = 0;

        /**
         * Used to get formatted contact title from list box model
         *
         * @param aIndex is indef of a contact.
         * @return HBufC is descriptor that contains formatted contact name.
         *         Ownership of the buffer is passed to the caller.
         */
        virtual HBufC* GetContactTitleLC( TInt aIndex ) const = 0;
	};

#endif // MPBK2CONTACTVIEWLISTBOXMODEL_H

// End of File
