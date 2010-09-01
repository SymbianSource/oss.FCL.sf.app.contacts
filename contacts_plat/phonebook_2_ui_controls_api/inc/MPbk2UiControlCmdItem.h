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
* Description:  UI control Command item interface.
*
*/


#ifndef MPBK2UICONTROLCMDITEM_H_
#define MPBK2UICONTROLCMDITEM_H_

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MPbk2CmdItemVisibilityObserver;

// CLASS DECLARATION

/**
 * UI control Command item interface.
 */
class MPbk2UiControlCmdItem
    {
    public:

        /// Destructor
        virtual ~MPbk2UiControlCmdItem() {}

        /// Localized name.
        virtual TPtrC NameForUi() const = 0;

        /// Tells whether the command is enabled or not.
        virtual TBool IsEnabled() const = 0;

        /** Sets the command as enabled or disabled.
         * @param aEnabled If ETrue is given, the command is enabled, and
         *     vice versa.
         */
        virtual void SetEnabled( TBool aEnabled ) = 0;

        /**
         * The command ID.
         */
        virtual TInt CommandId() const = 0;

        /**
         * Can be used if command visibility can change.
         *
         * @param aObserver valid observer or NULL to remove current observer.
         */
        virtual void SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ControlCmdItemExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    };

#endif /*MPBK2UICONTROLCMDITEM_H_*/
