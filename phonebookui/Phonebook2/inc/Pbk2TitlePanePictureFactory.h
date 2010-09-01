/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 title pane picture factory.
*
*/


#ifndef PBK2TITLEPANEPICTUREFACTORY_H
#define PBK2TITLEPANEPICTUREFACTORY_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class CEikImage;
class CPbk2StorePropertyArray;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Phonebook 2 title pane picture factory.
 * Responsible for creating title pane picture.
 */
class Pbk2TitlePanePictureFactory
    {
    public: // Interface

        /**
         * Creates a picture to be set in title pane for the contact.
         *
         * @param aContact          The contact for which the picture
         *                          is to be created.
         * @param aStoreProperties  Store properties array.
         * @return  The picture for the title pane.
         */
        IMPORT_C static CEikImage* CreateTitlePanePictureLC(
                const MVPbkStoreContact* aContact,
                CPbk2StorePropertyArray& aStoreProperties );

    };

#endif // PBK2TITLEPANEPICTUREFACTORY_H

// End of File
