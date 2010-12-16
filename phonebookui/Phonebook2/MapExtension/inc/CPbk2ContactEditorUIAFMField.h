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
* Description:  Phonebook 2 contact editor dialog UI assign from maps field.
*
*/

#ifndef CPBK2CONTACTEDITORUIAFMFIELD_H_
#define CPBK2CONTACTEDITORUIAFMFIELD_H_

// INCLUDES
#include <w32std.h>
#include <e32base.h>
#include <CPbk2ContactEditorUIFieldBase.h>

// FORWARD DECLARATIONS
class CEikCaptionedControl;
class CPbk2ContactEditorReadonlyField;
// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI base field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorUIAFMField) : public CPbk2ContactEditorUIFieldBase
    {
    public:

		/**
		 * C++ constructor.
		 *
		 * @param aUiBuilder            UI builder for adding
		 *                              the field into dialog.
		 * @param aIconInfoContainer    An icon container for setting
		 *                              field icon.
		 */
    	static CPbk2ContactEditorUIAFMField* NewL(
    			MPbk2UIField* aField,
		        MPbk2ContactEditorUiBuilder& aUiBuilder,
		        CPbk2IconInfoContainer& aIconInfoContainer,
		        TInt aCustomPosition);
    	 
    public:
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorUIAFMField();

        CEikEdwin* Control();
        void SetControlTextL( const TDesC& aText );
        void ActivateL();
        TInt ControlId();
        TBool HandleCustomFieldCommandL(TInt aCommand);
  

    private:
    	CPbk2ContactEditorUIAFMField(
    	    			MPbk2UIField* aField,
    			        MPbk2ContactEditorUiBuilder& aUiBuilder,
    			        CPbk2IconInfoContainer& aIconInfoContainer,
    				    TInt aCustomPosition );
    	void ConstructL();

    private:
    	/// Ref: Editor control
    	CPbk2ContactEditorReadonlyField* iControl;
        /// Ref: Fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
    };


#endif /*CPBK2CONTACTEDITORUIAFMFIELD_H_*/

// End of File