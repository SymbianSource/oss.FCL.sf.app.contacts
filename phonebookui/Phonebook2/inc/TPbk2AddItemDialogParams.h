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
* Description:  Add item dialog parameters.
*
*/


#ifndef TPBK2ADDITEMDIALOGPARAMS_H
#define TPBK2ADDITEMDIALOGPARAMS_H

// CLASS DECLARATION

/**
 * Phonebook 2 add item dialog parameters.
 */
class TPbk2AddItemDialogParams
    {
    public:  // Interface

        /**
         * Constructor.
         *
         * @param aTitle                Dialog title, if NULL no
         *                              title is shown.
         * @param aCbaResource          CBA resource id.
         * @param aSelectionIndex       Preselection index.
         */
         TPbk2AddItemDialogParams(
                const TDesC* aTitle,
                const TInt aCbaResource,
                const TInt aSelectionIndex );

    public: // Data
        /// Own: CBA resource
        const TInt iCbaResource;
        /// Ref: Dialog title
        const TDesC* iTitle;
        /// Own: Preselection index
        const TInt iSelectionIndex;
    };

// INLINE IMPLEMENTATION


// --------------------------------------------------------------------------
// TPbk2AddItemDialogParams::TPbk2AddItemDialogParams
// --------------------------------------------------------------------------
//
inline TPbk2AddItemDialogParams::TPbk2AddItemDialogParams
        ( const TDesC* aTitle,
        const TInt aCbaResource,
        const TInt aSelectionIndex ) :
            iTitle( aTitle ),
            iCbaResource( aCbaResource ),
            iSelectionIndex( aSelectionIndex )
    {
    }

#endif // TPBK2ADDITEMDIALOGPARAMS_H

// End of File
