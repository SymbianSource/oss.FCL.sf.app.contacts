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
*       Abstract Phonebook AIW interest item interface.
*
*/


#ifndef __MPbkAiwInterestItem_H__
#define __MPbkAiwInterestItem_H__

//  FORWARD DECLARATIONS
class CEikMenuPane;
class TPbkContactItemField;
class CContactIdArray;
class MPbkAiwCommandObserver;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Abstract Phonebook AIW interest item interface.
 */
class MPbkAiwInterestItem
    {
    public:  // Interface

        /**
         * Destructor.
         */
        virtual ~MPbkAiwInterestItem() { }

        /**
         * Initializes the menu pane.
         * @param aResourceId menu pane resource id
         * @param aMenuPane the menupane object
         * @param aFlags menu filtering flags (@see MenuFilteringFlags.h)
         * @ret ETrue if the menu pane was handled, EFalse otherwise
         */
        virtual TBool InitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane& aMenuPane,
            TUint aFlags) = 0;

        /**
         * Handles menu (and base) commands.
         * NOTE: Fill in parameter aServiceCommandId only if the function
         * is not called after menu item selection (when AIW is used without
         * menus).
         * @param aMenuCommandId id of the selected menu item
         * @param aContacts array of selected contacts
         * @param aFocusedField focused field or NULL if no field level focus
         * @param aServiceCommandId normally fetched from AIW framework
         *        based on supplied menu command id, but must be filled
         *        manually if command is to be called without menu item
         *        command activation
         * @param aObserver observer to be called if command is handled
         * @ret ETrue if command was handled, EFalse otherwise
         */
        virtual TBool HandleCommandL(
            TInt aMenuCommandId,
            const CContactIdArray& aContacts,
            const TPbkContactItemField* aFocusedField = NULL,
            TInt aServiceCommandId = KNullHandle,
            MPbkAiwCommandObserver* aObserver = NULL) = 0;

        /**
         * Attaches the interest resource to given menu resource.
         * @param aResourceId menu pane resource id
         * @param aResourceId interest resource id
         * @param aAttachBaseServiceInterest if ETrue, attaches also
         *        base services
         */
        virtual void AttachL(
            TInt aMenuResourceId,
            TInt aInterestResourceId,
            TBool aAttachBaseServiceInterest) = 0;
    };

#endif // __MPbkAiwInterestItem_H__

// End of File
