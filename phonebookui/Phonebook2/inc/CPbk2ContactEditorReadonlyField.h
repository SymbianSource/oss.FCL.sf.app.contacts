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
* Description:  Phonebook 2 contact editor dialog field.
*
*/

#ifndef CPBK2CONTACTEDITORREADONLYFIELD_H
#define CPBK2CONTACTEDITORREADONLYFIELD_H

//  INCLUDES
#include <eikedwin.h>
#include <touchfeedback.h>
#include <touchlogicalfeedback.h>
#include <MPbk2ContactEditorField.h>
#include "MPbk2ContactEditorUiBuilderExtension.h"
#include "MPbk2ContactEditorUIField.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 *  Custom-editor field 
  */
class CPbk2ContactEditorReadonlyField : public CEikEdwin
    {
public:  // Constructors and destructor
    CPbk2ContactEditorReadonlyField( ); 

    virtual ~CPbk2ContactEditorReadonlyField();

public:   
	IMPORT_C void InitializeL(MPbk2ContactEditorUIField* aPbk2ContactEditorField );
	void InitializeL(MPbk2ContactEditorField* aPbk2ContactEditorField );
	

public: // from CEikEdwin

    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                TEventCode aType);

    IMPORT_C void SetTextL(const TDesC* aData);
    
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

private:    // Data
    // Ref to ContactEditorField
    MPbk2ContactEditorField* iPbk2ContactEditorField;
    MPbk2ContactEditorUIField* iPbk2ContactEditorUIField;
    // Ref to MTouchFeedback    
    MTouchFeedback* iTouchFeedbackRef; 
    TBool iButton1UpReceived;
    };

#endif      // CPBK2CONTACTEDITORREADONLYFIELD_H


// End of File
