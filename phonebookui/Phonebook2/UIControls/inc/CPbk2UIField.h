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
* Description:  Phonebook 2 contact editor dialog UI field.
*
*/

#ifndef CPBK2UIFIELD_H_
#define CPBK2UIFIELD_H_

// INCLUDE FILES
#include "MPbk2UIField.h"


// FORWARD DECLARATIONS
class TResourceReader;

/**
 * Phonebook 2 UI field interface.
 *
 */
class CPbk2UIField : public CBase,
					 public MPbk2UIField
    {
    public: // Interface
    	/**
         * Creates a new instance of this class.
         *
         * @param aReader                   Resource reader pointed to a
         *                                  PHONEBOOK2_UI_FIELD_PROPERTY
         *                                  structure.
         * @return  A new instance of this class.
         */
        static CPbk2UIField* NewL( TResourceReader& aReader );
    	
        TPbk2FieldMultiplicity Multiplicity() const;
        TInt MaxLength() const;
        TPbk2FieldEditMode EditMode() const;
        TPbk2FieldDefaultCase DefaultCase() const;
        const TPbk2IconId& IconId() const;
        TPbk2FieldCtrlTypeExt CtrlType() const;
        TUint Flags() const;
        const TDesC& DefaultLabel() const;
        TPbk2FieldOrder Order() const;
        
        ~CPbk2UIField();
    private:
    	CPbk2UIField();
    	void ConstructL( TResourceReader& aReader );
    	
    	/// Own: Assorted flags for the field type
        TUint iFlags;					        // LONG flags
        /// Own: Default label for the field
        HBufC* iDefaultLabel;                   // LTEXT defaultLabel
        /// Own: Maximum length in characters
        TInt16 iMaxLength;					    // WORD maxLength
        /// Own: Allowed multiplicity (one/many)
        TInt8 iMultiplicity;                    // BYTE multiplicity
        /// Own: Default editing mode
        TInt8 iEditMode;		                // BYTE editMode
        /// Own: Order
        TInt8 iOrder;                    		// BYTE order
        /// Own: Default character case
        TInt8 iDefaultCase;                     // BYTE defaultCase
        /// Own: Index of an icon
        TPbk2IconId iIconId;                    // STRUCT iconId
        /// Own: Editor UI control type
        TInt8 iCtrlType;                        // BYTE ctrlType        
    };

#endif /*MPBK2UIFIELD_H_*/

// End of File

