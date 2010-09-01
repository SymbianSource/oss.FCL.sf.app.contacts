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
* Description:  Phonebook 2 contact editor dialog array item.
*
*/

#ifndef CPBK2CONTACTEDITORARRAYITEM_H_
#define CPBK2CONTACTEDITORARRAYITEM_H_

// INCLUDE FILES
#include <e32def.h>
#include "MPbk2ContactEditorField.h"
#include "MPbk2ContactEditorUIField.h"
// FORWARD DECLARATIONS

/**
 * Phonebook 2 UI field interface.
 *
 */
class CPbk2ContactEditorArrayItem : public CBase
    {
    public: // Interface
    	/**
         * Creates a new instance of this class.
         *
         * @param aContactEdytorField       Field from storage.
         * @return  A new instance of this class.
         */
    	static CPbk2ContactEditorArrayItem* NewL(
        		MPbk2ContactEditorField* aContactEdytorField);
    	
        /**
         * Creates a new instance of this class.
         *
         * @param aContactEdytorUIField     UI field.
         * @return  A new instance of this class.
         */
    	static CPbk2ContactEditorArrayItem* NewL(
        		MPbk2ContactEditorUIField* aContactEdytorUIField);
        
        TInt ControlId();
      	CEikEdwin* Control();
      	void SetFocus();
		void ActivateL();
		TBool ConsumesKeyEvent(
                const TKeyEvent& aKeyEvent, 
                TEventCode aType );
      	
      	MPbk2ContactEditorUIField* ContactEditorUIField();
      	MPbk2ContactEditorField* ContactEditorField();
      	
        ~CPbk2ContactEditorArrayItem();
    private:
    	CPbk2ContactEditorArrayItem(
        		MPbk2ContactEditorField* aContactEdytorField);
    	CPbk2ContactEditorArrayItem(
    			MPbk2ContactEditorUIField* aContactEdytorUIField);
    	void ConstructL();
    	
    private:
    	MPbk2ContactEditorUIField* iContactEdytorUIField;
    	MPbk2ContactEditorField* iContactEdytorField;
    };

#endif /*CPBK2CONTACTEDITORARRAYITEM_H_*/

// End of File

