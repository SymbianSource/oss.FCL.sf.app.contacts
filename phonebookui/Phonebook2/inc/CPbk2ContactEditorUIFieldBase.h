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

#ifndef CPBK2CONTACTEDITORUIFIELDBASE_H_
#define CPBK2CONTACTEDITORUIFIELDBASE_H_

// INCLUDES
#include "MPbk2ContactEditorUIField.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI base field.
 */
class CPbk2ContactEditorUIFieldBase : public CBase,
									  public MPbk2ContactEditorUIField
    {
    protected:

		/**
		 * C++ constructor.
		 *
		 * @param aUiBuilder            UI builder for adding
		 *                              the field into dialog.
		 * @param aIconInfoContainer    An icon container for setting
		 *                              field icon.
		 */
    	IMPORT_C CPbk2ContactEditorUIFieldBase(
    			MPbk2UIField* aField,
		        MPbk2ContactEditorUiBuilder& aUiBuilder,
		        CPbk2IconInfoContainer& aIconInfoContainer,
			    TInt aCustomPosition );
    	 
    public:
        /**
         * Destructor.
         */
    	IMPORT_C virtual ~CPbk2ContactEditorUIFieldBase();

    
    	IMPORT_C virtual TInt ControlId();
    	IMPORT_C virtual CEikEdwin* Control() = 0;
    	IMPORT_C virtual MPbk2UIField* UIField() const;
    	IMPORT_C virtual TBool HandleCustomFieldCommandL(TInt aCommand);
    	IMPORT_C virtual const TDesC& FieldLabel() const;
    	IMPORT_C virtual void SetFieldLabelL(
                const TDesC& aLabel );
    	IMPORT_C virtual const TDesC& ControlText() const;
    	IMPORT_C virtual void SetControlTextL(
                const TDesC& aText ) = 0;
    	IMPORT_C virtual void SetFocus();
    	IMPORT_C virtual void ActivateL() = 0;
    	IMPORT_C virtual TBool ConsumesKeyEvent(
                const TKeyEvent& aKeyEvent, 
                TEventCode aType );
    	IMPORT_C virtual void LoadBitmapToFieldL( const TPbk2IconId& aIconId );

    protected:
    	/// Take ownership of this object
    	MPbk2UIField* iField;
    	/// Ref: UI builder for adding the field into dialog
        MPbk2ContactEditorUiBuilder& iUiBuilder;
        /// Ref: Icon container for setting field icon
        CPbk2IconInfoContainer& iIconInfoContainer;
        /// Lebel
        RBuf	iLebel;
        RBuf	iText;
        TInt 	iCustomPosition;
        TBool   iStdKeyDevice3Down;
    };


#endif /*CPBK2CONTACTEDITORUIFIELDBASE_H_*/

// End of File