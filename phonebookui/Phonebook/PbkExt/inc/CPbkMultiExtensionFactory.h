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
*       Composite extension factory.
*
*/


#ifndef __CPbkMultiExtensionFactory_H__
#define __CPbkMultiExtensionFactory_H__

// INCLUDES
#include <e32base.h>
#include <MPbkExtensionFactory.h>
#include "CPbkDummyControlExtension.h"

/**
 * Composite factory.
 * Combines behaviour of multiple factories.
 */
class CPbkMultiExtensionFactory : public CBase,
                                  public MPbkExtensionFactory
    {
    public:
        /**
         * Creates new composite factory object with empty
         * factory set.
         */
        static CPbkMultiExtensionFactory* NewL();

        /**
         * Appends an extension factory to this multifactory.
         * Ownership is not transferred.
         *
         * @param aFactory Factory to append. 
         */
        void AppendL(MPbkExtensionFactory* aFactory);

        /**
         * Destructor.
         */
        ~CPbkMultiExtensionFactory();

    public: // From MPbkExtensionFactory
        MPbkViewExtension* CreatePbkViewExtensionL
                (TUid aId, CPbkContactEngine& aEngine);
        MPbkViewExtension* CreatePbkViewExtensionL
                (TUid aId, CPbkContactEngine& aEngine,
                CPbkAppViewBase& aAppView);
        MPbkContactEditorExtension* CreatePbkContactEditorExtensionL
            (CPbkContactItem& aContact, 
             CPbkContactEngine& aEngine,
             MPbkContactEditorControl& aEditorControl);
        MPbkContactUiControlExtension* 
            CreatePbkUiControlExtensionL(CPbkContactEngine& aEngine);
        MPbkContactUiControlExtension* 
            CreateDummyPbkUiControlExtensionL();
        CAknView* CreateViewL(TUid aId);
        MPbkAppUiExtension* CreatePbkAppUiExtensionL(
                CPbkContactEngine& aEngine);    
        void AddPbkFieldIconsL
            (CPbkIconInfoContainer* aIconInfoContainer = NULL, 
            CPbkIconArray* aIconArray = NULL);
        void AddPbkTabIconsL
            (CPbkIconInfoContainer* aIconInfoContainer = NULL, 
            CPbkIconArray* aIconArray = NULL);

    private: // Implementation
        CPbkMultiExtensionFactory();
    
    private:
		/**
         * Ref: Extension factories.
         * This class does not own these factories.
		 * CPbkExtGlobals owns the extensions and
		 * extensions own their own factories.
		 */
        CArrayPtrFlat<MPbkExtensionFactory> iFactories;

        /// Dummy contact ui control extension
        CPbkDummyControlExtension iDummyControlExtension;
    };

#endif // __CPbkMultiExtensionFactory_H__

// End of File
