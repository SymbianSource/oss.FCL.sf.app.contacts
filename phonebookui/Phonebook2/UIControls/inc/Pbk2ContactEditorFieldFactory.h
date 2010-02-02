/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor field factory.
*
*/


#ifndef PBK2CONTACTEDITORFIELDFACTORY_H
#define PBK2CONTACTEDITORFIELDFACTORY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "MPbk2UIFieldFactory.h"

// FORWARD DECLARATIONS
class MPbk2ContactEditorField;
class CPbk2PresentationContactField;
class MPbk2ContactEditorUiBuilder;
class CPbk2IconInfoContainer;
class MPbk2ContactEditorExtension;
class MPbk2ContactEditorUIField;
class MPbk2UIField;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor field factory.
 * Responsible for creating contact editor fields.
 */
class CPbk2ContactEditorFieldFactory : public CBase,
									   public MPbk2UIFieldFactory
    {
    public:
    	~CPbk2ContactEditorFieldFactory();
    	
        static CPbk2ContactEditorFieldFactory* NewL(MPbk2ContactEditorUiBuilder& aUiBuilder, 
        			MPbk2ContactEditorExtension& aEditorExtension);
        /**
         * Creates a new contact editor field.
         *
         * @param aField             Phonebook2 field.
         * @param aUiBuilder         Builder for adding the field into dialog.
         * @param aIconInfoContainer An icon container for setting field icon.
         * @param aEditorExtension   Contact editor extension.
         * @param aCustomPosition    Needed only for custom fields to indicate desired
                                     postion in form. For other fields can be NULL.
         * @return  A new contact editor field.
         */
        MPbk2ContactEditorField* CreateFieldLC(
                CPbk2PresentationContactField& aField,
                TInt aCustomPosition, CPbk2IconInfoContainer& aIconInfoContainer);
        
        /**
	     * Creates a new contact editor field.
	     *
	     * @param aField             Phonebook2 UI field.
	     * @param aUiBuilder         Builder for adding the field into dialog.
	     * @param aIconInfoContainer An icon container for setting field icon.
	     * @param aCustomPosition    Needed only for custom fields to indicate desired
	     *                           postion in form. For other fields can be NULL.
	     * @param aCustomText	     If not empty, text will be added into field.
	     * @return  A new contact editor UI field.
	     */
        MPbk2ContactEditorUIField* CreateFieldLC( 
                		MPbk2UIField& aField,
                		TInt aCustomPosition, 
                        MPbk2ContactEditorUiBuilder& aUiBuilder,
                        const TDesC& aCustomText, CPbk2IconInfoContainer& aIconInfoContainer );
              
    private:
    	void ConstructL();
    	CPbk2ContactEditorFieldFactory(
    			MPbk2ContactEditorUiBuilder& aUiBuilder, 
    			MPbk2ContactEditorExtension& aEditorExtension);
    	
    private:
    	MPbk2ContactEditorExtension& iEditorExtension;
    	MPbk2ContactEditorUiBuilder& iUiBuilder;
    };

#endif  // PBK2CONTACTEDITORFIELDFACTORY_H
            
// End of File
