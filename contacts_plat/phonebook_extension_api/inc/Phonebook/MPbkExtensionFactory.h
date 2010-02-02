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
*    Abstract interface for creating phone book extensions.
*
*/


#ifndef __MPbkExtensionFactory_H__
#define __MPbkExtensionFactory_H__


//  INCLUDES
#include <e32std.h>  // for TUid


//  FORWARD DECLARATIONS
class MPbkViewExtension;
class MPbkContactUiControlExtension;
class TVwsViewId;
class CAknView;
class CPbkContactEngine;
class MPbkKeyEventHandler;
class MPbkAppUiExtension;
class CPbkContactItem;
class MPbkContactEditorExtension;
class MPbkContactEditorControl;
class CPbkIconInfoContainer;
class CPbkIconArray;
class CPbkAppViewBase;


// CLASS DECLARATION

/**
 * Abstract interface for creating phone book extensions. Instance of 
 * this factory should be kept as long as extensions are used, if resources
 * are needed.
 */
class MPbkExtensionFactory
    {
    public:  // Destructor
        /**
         * Destructor.
         */ 
        virtual ~MPbkExtensionFactory() {}

    public: // Interface

        /**
         * Creates a Phonebook view extension.
         *
         * @param aId   id of the Phonebook view to create the extension for
         * @param aEngine pbk engine to be provided to extensions.
         * @return Phonebook view extension object.
         */
        virtual MPbkViewExtension* CreatePbkViewExtensionL
                (TUid aId, CPbkContactEngine& aEngine) =0;

        /**
         * Creates a Phonebook view extension.
         * Default implementation is empty.
         *
         * @param aId   id of the Phonebook view to create the extension for
         * @param aEngine pbk engine to be provided to extensions
         * @param aAppView app view base to be provided to extensions
         * @return Phonebook view extension object.
         */
        virtual MPbkViewExtension* CreatePbkViewExtensionL
                (TUid aId, CPbkContactEngine& aEngine,
                CPbkAppViewBase& aAppView);

        /**
         * Creates a Phonebook contact editor extension.
         *
         * @param aContact currently edited contact
         * @param aEngine for creating temporary contact
         * @param aEditorControl for requesting information
         * @return Phonebook editor extension object.
         */
        virtual MPbkContactEditorExtension* CreatePbkContactEditorExtensionL
            (CPbkContactItem& aContact,
            CPbkContactEngine& aEngine,
            MPbkContactEditorControl& aEditorControl) =0;

        /**
         * Creates a Phonebook UI control extension.
         *
         * @param aEngine pbk engine to be provided to extensions.
         * @return Phonebook UI control extension object.
         */
        virtual MPbkContactUiControlExtension* 
            CreatePbkUiControlExtensionL(CPbkContactEngine& aEngine) =0;

        /**
         * Creates a new extension-implemented Phonebook view.
         *
         * @param aId   id of the extension view to create.
         * @return the extension view object.
         */
        virtual CAknView* CreateViewL(TUid aId) =0;

        /**
         * Creates a phonebook appui extension.
         */
        virtual MPbkAppUiExtension* CreatePbkAppUiExtensionL
            (CPbkContactEngine& aEngine) =0;

        /**
         * Adds new field icons to Phonebook. This should match with fields 
         * and icons added to Phonebook in engine extension, 
         * @see CPbkEngineExtensionInterface. Parameters are pointers
         * because it is not necessary to give either of those.
         * Default implementation is empty.
         *
         * @param aIconInfoContainer icons infos to be added to field icons
         * @param aIconArray subset of field icons
         */
        virtual void AddPbkFieldIconsL
            (CPbkIconInfoContainer* aIconInfoContainer = NULL, 
            CPbkIconArray* aIconArray = NULL);
        
        /**
         * Adds new tab icons to Phonebook. This should match with the 
         * tabs and icons added to Phonebook's view graph.
         * Parameters are pointers because it is not necessary to give 
         * either of those.
         * Default implementation is empty.
         *
         * @param aIconInfoContainer icons infos to be added to field icons
         * @param aIconArray subset of field icons
         */
        virtual void AddPbkTabIconsL
            (CPbkIconInfoContainer* aIconInfoContainer = NULL, 
            CPbkIconArray* aIconArray = NULL);
    };

// TYPES

/**
 * Entry point function definition for a Phonebook UI extension DLL. The 
 * function should be exported by the extension DLL with ordinal number 1 (one)
 * and it should return an extension factory instance. If the factory instance 
 * cannot be created the function should leave.
 */
typedef MPbkExtensionFactory* (*TPbkUiExtensionEntryPointL)();

/**
 * Inline implementations to get rid of warnings.
 */
inline void MPbkExtensionFactory::AddPbkFieldIconsL
            (CPbkIconInfoContainer* /*aIconInfoContainer*/, 
            CPbkIconArray* /*aIconArray*/)
    {
    }

inline void MPbkExtensionFactory::AddPbkTabIconsL
            (CPbkIconInfoContainer* /*aIconInfoContainer*/, 
            CPbkIconArray* /*aIconArray*/)
    {
    }

inline MPbkViewExtension* MPbkExtensionFactory::CreatePbkViewExtensionL
    (TUid /*aId*/, CPbkContactEngine& /*aEngine*/,
    CPbkAppViewBase& /*aAppView*/)
    {
    return NULL;
    }

#endif // __MPbkExtensionFactory_H__

// End of File
