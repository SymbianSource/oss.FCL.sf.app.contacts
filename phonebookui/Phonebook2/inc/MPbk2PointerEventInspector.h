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
* Description:  Phonebook 2 pointer event observer.
*
*/


#ifndef MPBK2POINTEREVENTINSPECTOR_H
#define MPBK2POINTEREVENTINSPECTOR_H

// CLASS DECLARATION

/**
 * Phonebook 2 pointer event inspector.
 */
class MPbk2PointerEventInspector
    {
    public: // Interface

        /**
         * Returns true if the focused item was pointed.
         *
         * @return  ETrue if the focused item was pointed, EFalse otherwise.
         */
        virtual TBool FocusedItemPointed() = 0;

        /**
         * Returns true if a focusable item was pointed.
         *
         * @return  ETrue if a focusable item was pointed, EFalse otherwise.
         */
        virtual TBool FocusableItemPointed() = 0;

        /**
         * Returns true if search field was pointed.
         *
         * @return ETrue if search field was pointed, EFalse otherwise.
         */
        virtual TBool SearchFieldPointed() = 0;

    protected: // Disabled functions
        virtual ~MPbk2PointerEventInspector()
            {}
    };

#endif // MPBK2POINTEREVENTINSPECTOR_H

// End of File
