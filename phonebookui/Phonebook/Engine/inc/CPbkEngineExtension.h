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
*      CPbkContactEngine extension implementation class.
*
*/


#ifndef __CPBKENGINEEXTENSION_H__
#define __CPBKENGINEEXTENSION_H__

// INCLUDES
#include <e32base.h>        // CBase

// FORWARD DECLARATIONS
class CPbkFieldsInfo;
class CPbkEngineExtensionInterface;
class RFs;


// CLASS DECLARATION

/**
 * Phonebook engine internal interface to engine extensions.
 */
NONSHARABLE_CLASS(CPbkEngineExtension) : 
        public CBase
    {
    public: // Interface
        /**
         * Performs 2-phased construction.
         * @param[in] aFs Open file server session.
         * @return newly created instance of this class.
         */
        static CPbkEngineExtension* NewL(RFs& aFs);

        /**
         * Standard c++ destructor.
         */
        ~CPbkEngineExtension();

        /**
         * Calls extension plugin(s) to add new field types to aFieldsInfo.
         * @param aFieldsInfo fields info object to add extension field infos to.
         */
        void AddFieldTypesL(CPbkFieldsInfo& aFieldsInfo);

        /**
         * Calls extension plugin(s) to modify field types in aFieldsInfo.
         * @param aFieldsInfo fields info object that contains all field types.
         */
        TBool ModifyFieldTypesL(CPbkFieldsInfo& aFieldsInfo);

    private: // Implementation
        CPbkEngineExtension();
        void ConstructL(RFs& aFs);

    private:  // Data
        /// Own: engine extensions.
        RPointerArray<CPbkEngineExtensionInterface> iExtensions;
    };


#endif // __CPBKENGINEEXTENSION_H__

// End of File
