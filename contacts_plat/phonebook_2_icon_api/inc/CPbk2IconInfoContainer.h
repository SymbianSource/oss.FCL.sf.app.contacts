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
* Description:  Phonebook 2 icon info container.
*
*/


#ifndef CPBK2ICONINFOCONTAINER_H
#define CPBK2ICONINFOCONTAINER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CCoeEnv;
class CPbk2IconInfo;
class TPbk2IconId;
class CPbk2ServiceManager;

// CLASS DECLARATION

/**
 * Phonebook 2 icon info container.
 * A collection of Phonebook 2 icon info objects.
 */
class CPbk2IconInfoContainer : public CBase
    {
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aReader   Resource reader pointed to an array
         *                  of PBK2_ICON_INFO resources.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconInfoContainer* NewL(
                TResourceReader& aReader );

        /**
         * Creates a new instance of this class.
         *
         * @param aResourceId   Resource id of an array of
         *                      PBK2_ICON_INFO resources. The resource
         *                      file must be open before usage.
         * @param aCoeEnv       Optional CCoeEnv instance. This
         *                      function performs faster if this
         *                      parameter is provided.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconInfoContainer* NewL(
                TInt aResourceId,
                CCoeEnv* aCoeEnv = NULL );

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconInfoContainer* NewL();

        /**
         * Destructor.
         */
        ~CPbk2IconInfoContainer();

    public: // Interface
        /**
         * Finds a particular icon info from the container.
         *
         * @param aIconId   The id of the icon to search for.
         * @return  The matched icon info or NULL if not found.
         */
        IMPORT_C const CPbk2IconInfo* Find(
                const TPbk2IconId& aIconId ) const;

        /**
         * Appends new icon infos from resource. The resource file must
         * be opened before usage.
         *
         * @param aResourceId   Resource id for icon infos to be added.
         */
        IMPORT_C void AppendIconsFromResourceL(
                TInt aResourceId );

        /**
         * Appends new icon infos to the container.
         *
         * @param aReader   A resource reader pointed to an array of
         *                  PBK2_ICON_INFO resource structures.
         */
        IMPORT_C void AppendIconsFromResourceL(
                TResourceReader& aReader );

        /**
         * Appends new icon info. The ownership of the icon info is 
         * transferred.
         *
         * @param aIconInfo   Icon info to be added to this container.
         */
        IMPORT_C void AppendIconL(
                CPbk2IconInfo* aIconInfo );

    private: // Implementation
        CPbk2IconInfoContainer();
        void ConstructL( TResourceReader& aReader );
        void ConstructL();
        TInt FindInfo( const TPbk2IconId& aIconId ) const;

    private: // Data
        /// Own: An array of icon infos
        RPointerArray<CPbk2IconInfo> iInfoArray;
        /// Own: An array of bitmap file names
        CDesCArrayFlat* iMbmFileNames;
    };

#endif // CPBK2ICONINFOCONTAINER_H

// End of File
