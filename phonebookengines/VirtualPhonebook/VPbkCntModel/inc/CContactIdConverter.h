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
* Description:  The virtual phonebook contact id converter
*
*/


#ifndef CCONTACTIDCONVERTER_H
#define CCONTACTIDCONVERTER_H

#include <CVPbkContactIdConverter.h>

class CPbkContactEngine;

namespace VPbkCntModel {

class CContactStore;

/**
 *  Contact ID converter
 *
 *  Provides ID conversion services between contact model contact
 *  and virtual phonebook contact
 *
 */
NONSHARABLE_CLASS( CContactIdConverter ): public CVPbkContactIdConverter
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aParam CContactStore of symbian contact model store
         *
         * @return a new instance of this class             
         */
        static CContactIdConverter* NewL(TAny* aParam);
        ~CContactIdConverter();
        
    public: // From CVPbkContactIdConverter
        TInt32 LinkToIdentifier(const MVPbkContactLink& aLink) const;
        MVPbkContactLink* IdentifierToLinkLC(TInt32 aIdentifier) const;
        TInt PbkEngFieldIndexL(const MVPbkStoreContactField& aField) const;
        CPbkContactItem* LinkToPbkContactItemLC(const MVPbkContactLink& aLink) const;
        CPbkContactItem* LinkToOpenPbkContactItemLCX(const MVPbkContactLink& aLink) const;
        void CommitOpenContactItemL(CPbkContactItem& aContactItem);
        
    private: // Implementation
        CContactIdConverter(MVPbkContactStore& aContactStore);        
        CPbkContactEngine* PbkEngineL() const;
        
    private: // Data
        /// Not Own: Symbian contact model store 
        CContactStore& iContactStore;
        /// Own: Phonebook engine
        mutable CPbkContactEngine* iPbkEngine;
    };

} // namespace VPbkCntModel

#endif // CCONTACTIDCONVERTER_H

// End of File
