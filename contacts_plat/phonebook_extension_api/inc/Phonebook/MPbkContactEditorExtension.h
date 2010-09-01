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
*    Abstract interface for contact editor extensions.
*
*/


#ifndef __MPbkContactEditorExtension_H__
#define __MPbkContactEditorExtension_H__


//  INCLUDES
#include <MPbkUiExtensionBase.h>


//  FORWARD DECLARATIONS
class CEikMenuPane;
class MPbkContactEditorControl;
class MPbkContactEditorField;


// CLASS DECLARATION

/**
 * Abstract interface for contact editor extensions.
 */
class MPbkContactEditorExtension : public MPbkUiExtensionBase
    {
    public: // Interface
        /**
         * Filters the menu pane of the phonebook contact editor.
         *
         * @param aResourceId menu resource id.
         * @param aMenuPane menu pane which will be filtered.
         */
        virtual void DynInitMenuPaneL
            (TInt aResourceId, CEikMenuPane* aMenuPane) =0;

        /**
         * Process extension commands.
         * 
         * @param aCommandId command id.
         * @return ETrue if command was handled.
         */
        virtual TBool ProcessCommandL(TInt aCommandId) =0;

        /**
         * For adding additional functionality when exiting an editor
         * before saving contact to contact database. 
         *
         * @see PostOkToExitL
         * @param aButtonId button which has been pressed
         */
        virtual void PreOkToExitL(TInt aButtonId);

        /**
         * For adding additional functionality when exiting an editor
         * after the contact has been saved to contact database.
         *
         * @see PreOkToExitL
         * @param aButtonId button which has been pressed
         */
        virtual void PostOkToExitL(TInt aButtonId);

    protected:
        /**
         * Destructor.
         */
        virtual ~MPbkContactEditorExtension();
    };


// INLINE FUNCTIONS

inline void MPbkContactEditorExtension::PreOkToExitL(TInt /*aButtonId*/)
    {
    }

inline void MPbkContactEditorExtension::PostOkToExitL(TInt /*aButtonId*/)
    {
    }

inline MPbkContactEditorExtension::~MPbkContactEditorExtension()
    {
    }


#endif // __MPbkContactEditorExtension_H__

// End of File

