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
* Description:  Phonebook 2 store property content store name.
*
*/


#ifndef CPBK2STOREPROPERTYCONTENTSTORENAME_H
#define CPBK2STOREPROPERTYCONTENTSTORENAME_H

// INCLUDES
#include <CPbk2Content.h>
#include <Pbk2ContentIdentifiers.hrh>

// FORWARD DECLARATIONS
class CPbk2LocalizedText;

// CLASS DECLARATION

/**
 * Phonebook 2 store property content store name.
 */
class CPbk2StorePropertyContentStoreName : public CPbk2Content
    {
    public: // Constructor and destructor

        /** 
         * Constructor.
         *
         * @param aTexts        Array of localized text resources.
         * @param aContentId    A content id for this property.
         */                    
        CPbk2StorePropertyContentStoreName( 
                const TArray<const CPbk2LocalizedText*> aTexts,
                TPbk2ContentId aContentId );
            
        /** 
         * Destructor
         */                            
        ~CPbk2StorePropertyContentStoreName();
    
    public: // Interface

        /** 
         * Returns content UID which tells what content this class
         * represents.
         *
         * @return  Content uid.
         */
        virtual TInt Uid() const;

        /** 
         * Returns name of the store.
         *
         * @return  Store name.
         */
        virtual const TDesC& StoreName() const;

    private: // Data
        /// Own: Localized text resources
        const TArray<const CPbk2LocalizedText*> iTexts;
        /// Own: Content id for this object
        TPbk2ContentId iContentId;        
    };

#endif // CPBK2STOREPROPERTYCONTENTSTORENAME_H

// End of File
