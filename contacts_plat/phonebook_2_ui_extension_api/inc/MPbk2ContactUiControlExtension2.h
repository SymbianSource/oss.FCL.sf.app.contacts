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
* Description:  Phonebook 2 UI control UI extension interface.
*
*/


#ifndef MPBK2CONTACTUICONTROLEXTENSION2_H
#define MPBK2CONTACTUICONTROLEXTENSION2_H

//  FORWARD DECLARATIONS
class CPbk2IconArray;

// CLASS DECLARATION
/**
 * This class is an extension to MPbk2ContactUiControlExtension.
 * See documentation of MPbk2ContactUiControlExtension from header
 * MPbk2ContactUiControlExtension.h 
 * 
 * You can access this extension by calling
 * MPbk2ContactUiControlExtension->ContactUiControlExtensionExtension()
 */
class MPbk2ContactUiControlExtension2
    {
    public: // Interface
        /**
         * Give access to listbox's icon array.
         *
         * @param aIconArray A modifiable icon array.
         */
        virtual void SetIconArray(CPbk2IconArray& aIconArray) = 0;           

    protected:
        /**
         * Destructor.
         */
        virtual ~MPbk2ContactUiControlExtension2()
                {}
    };

#endif // MPBK2CONTACTUICONTROLEXTENSION2_H

// End of File
