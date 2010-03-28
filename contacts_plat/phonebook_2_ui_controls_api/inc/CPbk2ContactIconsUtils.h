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
* Description:  Phonebook 2 icon utils.
*
*/


#ifndef CPBK2ICONSUTILS_H
#define CPBK2ICONSUTILS_H

// INCLUDE FILES
#include <e32base.h>
#include <tpbk2iconid.h>
// FORWARD DECLARATIONS
class CPbk2StorePropertyArray;
class MPbk2ContactUiControlExtension;
class MVPbkViewContact;
class CPbk2IconArray;

/**
 * Phonebook 2 utils to retrieve icons attributed to a contact.
 */
class CPbk2ContactIconsUtils : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aStoreProperties  Store properties
         * @param aUiExtension      UI contact control extension
         * 
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ContactIconsUtils* NewL(
                CPbk2StorePropertyArray& aStoreProperties,
                MPbk2ContactUiControlExtension* aUiExtension );

        /**
         * Destructor.
         */
        ~CPbk2ContactIconsUtils();

    public: // Interface
        /**
         * Return icons attributed to a contact based on
         * store properties and ui control extension plug-ins. 
         *
         * @param aViewContact  the view contact to find icons for.
         * @param aIconArray  result icon array with contact's icons
         */
        IMPORT_C void GetIconIdsForContactL(
                const MVPbkViewContact& aViewContact,
                RArray<TPbk2IconId>& aIconArray ) const;

        /**
         * Creates an icon array based on the resource id and appends to  it 
         * icons from UI extension plug-ins.
         *
         * @param aResourceId PBK2_ICON_ARRAY type resource id
         */        
        IMPORT_C static CPbk2IconArray* CreateListboxIconArrayL(TInt aResourceId);
        
    private:
        CPbk2ContactIconsUtils(
                CPbk2StorePropertyArray& aStoreProperties,
                MPbk2ContactUiControlExtension* aUiExtension );        

    private: // Data
        /// Ref: Store UI properties
        CPbk2StorePropertyArray& iStoreProperties;
        /// Ref: Ui extension point
        MPbk2ContactUiControlExtension* iUiExtension;
    };

#endif // CPBK2ICONSUTILS_H

// End of File
