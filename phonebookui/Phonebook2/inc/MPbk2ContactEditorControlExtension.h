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
* Description:  Phonebook 2 contact editor dialog control extension.
*
*/

#ifndef MPBK2CONTACTEDITORCONTROLEXTENSION_H_
#define MPBK2CONTACTEDITORCONTROLEXTENSION_H_

// INCLUDES
#include <VPbkFieldType.hrh>
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI builder interface extension.
 */
class MPbk2ContactEditorControlExtension
    {
    public:  // Interface

    	/**
         * Update contact editor field controls.
         *
         */
	    virtual void UpdateControlsL() = 0;   
	    
	    /**
         * Returns focused editor field type.
         *
         * @return  ETrue if the control is empty.
         */
	    virtual TInt FocusedControlType() = 0;
	    
        virtual TAny* ContactEditorControlExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
        
    protected: // Disabled functions
         virtual ~MPbk2ContactEditorControlExtension()
            {}
    };

#endif // MPBK2CONTACTEDITORCONTROLEXTENSION_H_

// End of File
