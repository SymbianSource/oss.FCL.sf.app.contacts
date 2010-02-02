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
* Description:  Phonebook 2 contact editor parameters.
*
*/


#ifndef TPBK2CONTACTEDITORPARAMS_H
#define TPBK2CONTACTEDITORPARAMS_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class TCoeHelpContext;
class MPbk2ExitCallback;
class MVPbkBaseContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor parameters.
 */
class TPbk2ContactEditorParams
    {
    public: // Data types

        /**
         * Flags for the editor
         */
        enum TFlags
            {
            /// Set this on if iStoreContact is a new one
            ENewContact     = 0x00000001,
            /// Set this on if iStoreContact has already been modified
            EModified       = 0x00000002,
            /// Set this on if editor's exit command is to be hidden
            EHideExit       = 0x00000004,
            /// Set this on if iStoreContact is user's own contact
            EOwnContact     = 0x00000008
            };
        
        enum TActiveView
        	{
        	EEditorUndefinied,
        	EEditorView,
        	EEditorAddressView,
        	EEditorAddressHomeView,
        	EEditorAddressOfficeView
        	};

    public: // Constructors and destructor

        /**
         * Constructor.
         *
         * @param aFlags                A collection of TFlags.
         * @param aFocusedContactField  The field to focus. If NULL
         *                              the topmost field is focused.
         * @param aHelpContext          External help context.
         * @param aExitCallback         Exit callback.
         * @param aFocusedIndex         The index of the field to focus.
         *                              Used only to return the focused
         *                              index.
         */
        inline TPbk2ContactEditorParams(
                TUint32 aFlags = 0,
                MVPbkBaseContactField* aFocusedContactField = NULL,
                TCoeHelpContext* aHelpContext = NULL,
                MPbk2ExitCallback* aExitCallback = NULL,
                TInt aFocusedIndex = KErrNotFound );

    public: // Data
        /// Ref: Focused contact field
        MVPbkBaseContactField* iFocusedContactField;
        /// Own: A set of TFlags
        TUint32 iFlags;
        /// Ref: External help context
        TCoeHelpContext* iHelpContext;
        /// Ref: Exit callback
        MPbk2ExitCallback* iExitCallback;
        /// Own: Used only to return focused field index.
        TInt iFocusedIndex;
        /// Own: Active view
        TActiveView iActiveView;
    };

// INLINE FUNCTIONS

inline TPbk2ContactEditorParams::TPbk2ContactEditorParams(
    TUint32 aFlags /*= 0*/,
    MVPbkBaseContactField* aFocusedContactField /*= NULL*/,
    TCoeHelpContext* aHelpContext /*= NULL*/,
    MPbk2ExitCallback* aExitCallback /*= NULL*/,
    TInt aFocusedIndex /*= KErrNotFound*/) :
        iFlags( aFlags ),
        iFocusedContactField( aFocusedContactField ),
        iHelpContext( aHelpContext ),
        iExitCallback( aExitCallback ),
        iFocusedIndex( aFocusedIndex ),
        iActiveView(EEditorView)
    {
    }

#endif // TPBK2CONTACTEDITORPARAMS_H

// End of File
