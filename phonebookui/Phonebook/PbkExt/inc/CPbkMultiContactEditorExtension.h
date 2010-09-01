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
*       Composite contact editor extension.
*
*/


#ifndef __CPbkMultiContactEditorExtension_H__
#define __CPbkMultiContactEditorExtension_H__

// INCLUDES
#include <e32base.h>
#include <MPbkContactEditorExtension.h>

/**
 * Composite of contact editor extensions.
 */
class CPbkMultiContactEditorExtension : public CBase,
										public MPbkContactEditorExtension
    {
    public:
        /**
         * Creates new composite AppUI extension object with empty
         * set of contained MultiContactEditors.
         */
        static CPbkMultiContactEditorExtension* NewL();
        
        /**
         * Appends an extension ContactEditor to this composite ContactEditor.
         * Ownership is transferred to this object.
         *
         * @param aContactEditor ContactEditor to append. 
         */
        void AppendL(MPbkContactEditorExtension* aContactEditor);

        /**
         * Destructor.	
         */
        ~CPbkMultiContactEditorExtension();
    public: // From MPbkContactEditorExtension
        void DynInitMenuPaneL
            (TInt aResourceId, CEikMenuPane* aMenuPane);
        TBool ProcessCommandL(TInt aCommandId);
        void PreOkToExitL(TInt aButtonId);
        void PostOkToExitL(TInt aButtonId);
        
    private: // From MPbkContactEditorExtension
        void DoRelease();

    private: // Implementation
        CPbkMultiContactEditorExtension();

    private: // Data
        /// Array of contact editor extensions.
        CArrayPtrFlat<MPbkContactEditorExtension> iContactEditors;
    };



#endif // __CPbkMultiMultiContactEditorExtension_H__

// End of File
