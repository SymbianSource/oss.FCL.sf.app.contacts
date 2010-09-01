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
* Description:  Extension for Phonebook engine.
*
*/


#ifndef __CFMPBKENGINEEXTENSION_H
#define __CFMPBKENGINEEXTENSION_H


// INCLUDES
#include <f32file.h>
#include <barsc.h>
#include <CPbkEngineExtensionInterface.h>

// FORWARD DECLARATIONS
class CPbkConstants;
class CPbkFieldsInfo;

// CLASS DECLARATION

/**
 * Extension for Phonebook engine.
 */
class CFmPbkEngineExtension : public CPbkEngineExtensionInterface
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of CFmPbkEngineExtension.
         * @param[in] aParam ECom construction parameter of type
         *   CPbkEngineExtensionInterface::TEngExtParams*.
         */
        static CFmPbkEngineExtension* NewL(TAny* aParam);

        /**
         * Destructor.
         */
        ~CFmPbkEngineExtension();

    public: // From CPbkEngineExtensionInterface
        TBool GetAdditionalFieldTypesResourceL(TFieldTypeResource& aResource);
        TBool ModifyFieldTypesL(CPbkFieldsInfo& aFieldsInfo);
        TInt AdditionalFieldTypesResourceCount() const;

    private: // Implementation
        CFmPbkEngineExtension();
        void ConstructL(
            const CPbkEngineExtensionInterface::TEngExtParams& aParam);
        void GetFieldTypeL(TFieldTypeResource& aResource, 
                           TInt aPbkResId, 
                           TInt aCntResId);

    private: // Data
        /// Own: will be set if there has not been set RFs with SetFs
        RFs iFs;
        /// Own: Local variation manager
        CPbkConstants* iConstants;
        /// Current index of the separate resources
        TInt iCurrentResourceIndex;
        /// Own: Field type resource file
        RResourceFile iResourceFile;
    };

#endif  // __CFMPBKENGINEEXTENSION_H

// End of file
