/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*    Public API for using contact list.
*
*/


#ifndef __CPbkContactListControlBase_H__
#define __CPbkContactListControlBase_H__


// INCLUDES
#include <coecntrl.h>   // CCoeControl
#include <MPbkContactUiControlUpdate.h>
#include <MPbkContactUiControl.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactViewBase;

// CLASS DECLARATION

/**
 * Public API for using contact list.
 */
class CPbkContactListControlBase : public CCoeControl,
                                   public MPbkContactUiControlUpdate,
                                   public MPbkContactUiControl
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine phonebook engine instance
         * @param aView the contact view this list control displays
         * @param aResourceId if of a PBK_CONTACTVIEWLISTCONTROL resource to
         *        use for intialisation
         * @param aParent container for this control. If NULL this control
         *       creates its own window
         */
        IMPORT_C static CPbkContactListControlBase* NewL
            (CPbkContactEngine& aEngine,
            CContactViewBase& aView,
            TInt aResourceId,
            const CCoeControl* aParent);
        /**
         * Destructor.
         */
        virtual ~CPbkContactListControlBase();
    };

#endif // __CPbkContactListControlBase_H__

// End of File
