/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*    Abstract interface for phonebook contact view listbox model.
*
*/


#ifndef __MPbkContactViewListModel_H__
#define __MPbkContactViewListModel_H__

//  INCLUDES
#include <bamdesca.h>   // MDesCArray
#include <cntdef.h>     // TContactItemId
#include "PbkIconId.hrh"    // TPbkIconId

//  FORWARD DECLARATIONS
class MPbkContactEntryLoader;
class MPbkContactUiControlExtension;

// CLASS DECLARATION

/**
 * Abstract base class for listbox models.
 */
class MPbkContactViewListModel : public MDesCArray
    {
    public:  // Destructor

        /**
         * Destructor.
         */ 
        virtual ~MPbkContactViewListModel() {}

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
         * Purges an entry aContactId from this model's cache.
         */
        virtual void PurgeEntry(TContactItemId aContactId) = 0;

        /**
         * Set contact entry loader to aContactEntryLoader.
         */
        virtual void SetEntryLoader
            (MPbkContactEntryLoader& aContactEntryLoader) = 0;

        /**
        * Sets control extension to model.
        *
        * @param aControlExtension control extensions used in model
        */
        virtual void SetContactUiControlExtension
                (MPbkContactUiControlExtension& aControlExtension) = 0;

        /**
         * Flushes cache.
         */
        virtual void FlushCache() = 0;

        /**
         * Refreshes view sort order.
         */
        virtual void RefreshSortOrderL() = 0;
	};

#endif // __MPbkContactViewListModel_H__
            
// End of File
