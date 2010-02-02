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
* Description:  Phonebook 2 contact UI control composite UI extension.
*
*/


#ifndef CPBK2CONTACTUICONTROLEXTENSION_H
#define CPBK2CONTACTUICONTROLEXTENSION_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2ContactUiControlExtension.h>
#include <MPbk2ContactUiControlExtension2.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;
class CVPbkContactManager;
class CPbk2IconArray;


// CLASS DECLARATION

/**
 * Phonebook 2 contact UI control composite UI extension.
 * Responsible for managing real contact UI control extensions and
 * delegating calls from core Phonebook 2 to them.
 */
NONSHARABLE_CLASS(CPbk2ContactUiControlExtension) : 
        public CBase,
        public MPbk2ContactUiControlExtension,
        public MPbk2ContactUiControlExtension2
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
		 *
         * @param aContactManager       Virtual Phonebook contact manager.
         * @param aExtensionLoader      UI extension loader.
		 * @return  A new instance of this class.
         */
        static CPbk2ContactUiControlExtension* NewL(
                CVPbkContactManager& aContactManager,
                CPbk2UIExtensionLoader& aExtensionLoader );
        
        /**
         * Destructor.
         */
        ~CPbk2ContactUiControlExtension();

    public: // From MPbk2ContactUiControlExtension
        void DoRelease();
        const TArray<TPbk2IconId> GetDynamicIconsL(
                const MVPbkViewContact* aContactHandle );
        void SetContactUiControlUpdate(
                MPbk2ContactUiControlUpdate* aContactUpdator );
        TAny* ContactUiControlExtensionExtension( TUid aUid );
    
    public: // From MPbk2ContactUiControlExtension2
        void SetIconArray(CPbk2IconArray& aIconArray); 

    private: // Implementation
        CPbk2ContactUiControlExtension(
                CVPbkContactManager& aContactManager,
                CPbk2UIExtensionLoader& aExtensionLoader );
        void ConstructL();
        void LoadExtensionsL();

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: UI extension loader
        CPbk2UIExtensionLoader& iExtensionLoader;       
        /// Own: Array of UI extension implementations
        RPointerArray<MPbk2ContactUiControlExtension> iUiControlExtensions;
        /// Own: Dummy array
        RArray<TPbk2IconId> iDummyArray;
    };

#endif // CPBK2CONTACTUICONTROLEXTENSION_H
           
// End of File
