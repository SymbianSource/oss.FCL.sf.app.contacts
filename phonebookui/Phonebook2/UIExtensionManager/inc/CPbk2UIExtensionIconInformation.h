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
* Description:  A class that reads the icon information of the extension
*
*/



#ifndef CPBK2UIEXTENSIONICONINFORMATION_H
#define CPBK2UIEXTENSIONICONINFORMATION_H

//  INCLUDES
#include <e32base.h>
#include <Pbk2IconArrayId.hrh>

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATION

/**
 * Reads the icon information of the UI extension. 
 * This class is responsible for reading the UI Extension
 * icon information resource structure during UI extension
 * initialization.
 */
NONSHARABLE_CLASS(CPbk2UIExtensionIconInformation) : 
        public CBase
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         * @param aResourceId A resource id of 
         *                    PHONEBOOK2_EXTENSION_ICON_INFORMATION.
         * @return A new instance of this class.
         */
        static CPbk2UIExtensionIconInformation* NewL(
                TInt aResourceId);
        
        /**
         * Destructor.
         */
        ~CPbk2UIExtensionIconInformation();

    public: // New functions
        /**
         * Returns the resource id of the PBK2_ICON_INFO array of
         * the extension.
         * @return The resource id of the PBK2_ICON_INFO array.
         */
        TInt IconInfoArrayResourceId();
        
        /**
         * Finds the icon array extension.
         *
         * @param aParentArrayId The phonebook2 icon array id.
         * @param aResId A reference to the resource id of 
         *        the icon array of the extension if found.
         * @return ETrue if the resource was found.
         */
        TBool FindIconArrayExtension(TPbk2IconArrayId aParentArrayId,
                TInt& aResId);

    private: // implementation
        CPbk2UIExtensionIconInformation();
        void ConstructL(TInt aResourceId);

    private: // implementation structure
        class TIconArrayInfo
            {
            public:
                TIconArrayInfo(TResourceReader& aReader);
            
            public: // Data
                /// Own: Parent array id
                TPbk2IconArrayId iParentArrayId;
                /// Own: Extension Icon array resource id
                TInt iExtensionIconArrayResId;
            };

    private:    // Data
        /// Own: icon info array resource id
        TInt iIconInfoArrayResId;
        /// Own: icon array extensions
        RArray<TIconArrayInfo> iIconArrayExtensions;
    };

#endif      // CPBK2UIEXTENSIONICONINFORMATION_H
            
// End of File
