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
* Description:  Phonebook 2 icon factory.
*
*/


#ifndef CPBK2ICONFACTORY_H
#define CPBK2ICONFACTORY_H

#include <e32base.h>

class CPbk2IconInfoContainer;
class CGulIcon;
class CFbsBitmap;
class CEikImage;
class TPbk2IconId;
class MAknsSkinInstance;
class CPbk2IconInfo;
class CPbk2ApplicationServices; 
/**
 * Factory class for creating Phonebook 2 icons.
 * Responsible for:
 *  - creating phonebook2 icons.
 */
class CPbk2IconFactory : public CBase
    {
    public: // Construction and destruction
        /**
         * Creates an icon factory for all core Phonebook 2 icons.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconFactory* NewL();

        /**
         * Creates an icon factory for all core Phonebook 2 icons.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconFactory* NewLC();

        /**
         * Creates an icon factory for specified icons.
         *
         * @param aIconContainer    A collection of Phonebook 2 
         *                          icon info objects
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconFactory* NewL(
                const CPbk2IconInfoContainer& aIconContainer );

        /**
         * Creates an icon factory for specified icons.
         *
         * @param aIconContainer    A collection of Phonebook 2 
         *                          icon info objects
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconFactory* NewLC(
                const CPbk2IconInfoContainer& aIconContainer );

        /**
         * Destructor.
         */
        virtual ~CPbk2IconFactory();

    public: // Interface

        /**
         * Creates a Phonebook 2 icon identified by the given id.
         *
         * @param aIconId   The id of the Phonebook 2 icon to create.
         * @return  A new icon instance or NULL no icon is found for the id.
         */
        IMPORT_C CGulIcon* CreateIconL(
                const TPbk2IconId& aIconId ) const;

        /**
         * Creates a Phonebook 2 icon identified by the given id.
         *
         * @param aIconId   The id of the Phonebook 2 icon to create.
         * @return  A new icon instance or NULL if no icon is found
         *          for the id. NULL icon is not pushed to the CleanupStack.
         */
        IMPORT_C CGulIcon* CreateIconLC(
                const TPbk2IconId& aIconId ) const;

        /**
         * Creates icon bitmaps.
         *
         * @param aIconId   The phonebook2 icon id.
         * @param aSkin     Skin instance for bitmap creation.
         * @param aBitmap   Created bitmap.
         * @param aMask     Created mask.
         */
        IMPORT_C void CreateIconL(
                const TPbk2IconId& aIconId,
                MAknsSkinInstance& aSkin,
                CFbsBitmap*& aBitmap,
                CFbsBitmap*& aMask );

        /**
         * Creates icon bitmaps.
         *
         * @param aIconId   The phonebook2 icon id.
         * @param aSkin     Skin instance for bitmap creation.
         * @param aBitmap   Created bitmap.
         * @param aMask     Created mask.
         */
        IMPORT_C void CreateIconLC(
                const TPbk2IconId& aIconId,
                MAknsSkinInstance& aSkin,
                CFbsBitmap*& aBitmap,
                CFbsBitmap*& aMask );

        /**
         * Creates an image identified by the given id.
         *
         * @param aIconId   The id of the Phonebook 2 icon to create.
         * @return  A new icon instance or NULL if no icon is found
         *          for the id. NULL icon is not pushed to the CleanupStack.
         */
        IMPORT_C CEikImage* CreateImageL(
                const TPbk2IconId& aIconId ) const;

        /**
         * Creates an image identified by the given id.
         *
         * @param aIconId   The id of the Phonebook 2 icon to create.
         * @return  A new icon instance or NULL if no icon is found
         *          for the id. NULL icon is not pushed to the CleanupStack.
         */
        IMPORT_C CEikImage* CreateImageLC(
                const TPbk2IconId& aIconId ) const;
                
        /**
         * Appends a new icon to this factory. 
         *
         * @param aIconInfo             icon to be appended. Ownership of the 
         *                              iconInfo is taken.
         */
        IMPORT_C void AppendIconL(
                CPbk2IconInfo* aIconInfo );
                

    private: // Implementation
        CPbk2IconFactory(
                const CPbk2IconInfoContainer* aIconContainer );
        void ConstructL();

    private: // Data
        /// Own: A container for all core Phonebook 2 icons
        CPbk2IconInfoContainer* iAllIconsContainer;
        /// Ref: A container from the client
        const CPbk2IconInfoContainer* iIconContainer;
        /// Own: Application Services
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2ICONFACTORY_H

// End of File
