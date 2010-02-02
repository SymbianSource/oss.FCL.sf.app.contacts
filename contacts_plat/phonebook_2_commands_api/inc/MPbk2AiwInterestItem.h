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
* Description:  Phonebook 2 AIW interest item interface.
*
*/


#ifndef MPBK2AIWINTERESTITEM_H
#define MPBK2AIWINTERESTITEM_H

//  FORWARD DECLARATIONS
class CEikMenuPane;
class MPbk2ContactUiControl;

// CLASS DECLARATION

/**
 * Phonebook 2 AIW interest item interface.
 */
class MPbk2AiwInterestItem
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2AiwInterestItem()
                {}

        /**
         * Returns the interest id.
         *
         * @return  Interest id.
         */
        virtual TInt InterestId() const = 0;

        /**
         * Initializes the menu pane.
         *
         * @param aResourceId   Menu pane resource id.
         * @param aMenuPane     Menupane.
         * @param aControl      Contact UI control.
         * @return  ETrue if the menu pane was handled, EFalse otherwise.
         */
        virtual TBool DynInitMenuPaneL(
                const TInt aResourceId,
                CEikMenuPane& aMenuPane,
                const MPbk2ContactUiControl& aControl ) = 0;

        /**
         * Handles menu and base commands.
         * NOTE: Fill in parameter aServiceCommandId only if the function
         * is not called after menu item selection (when AIW base services
         * are used).
         *
         * @param aMenuCommandId        Id of the selected menu item.
         * @param aControl              UI control.
         * @param aServiceCommandId     Normally fetched from AIW framework
         *                              based on supplied menu command id,
         *                              but must be filled manually if
         *                              command is to be called without
         *                              a menu item command activation.
         * @return  ETrue if command was handled, EFalse otherwise.
         */
        virtual TBool HandleCommandL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                TInt aServiceCommandId ) = 0;

        /**
         * Attaches an AIW interest resource to given menu resource.
         *
         * @param aMenuResourceId               Menu pane resource id.
         * @param aInterestResourceId           Interest resource id.
         * @param aAttachBaseServiceInterest    If ETrue, attaches also
         *                                      AIW base service.
         */
        virtual void AttachL(
                const TInt aMenuResourceId,
                const TInt aInterestResourceId,
                const TBool aAttachBaseServiceInterest ) = 0;
    };

#endif // MPBK2AIWINTERESTITEM_H

// End of File
