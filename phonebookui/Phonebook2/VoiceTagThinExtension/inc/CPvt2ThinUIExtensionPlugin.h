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
* Description:  Phonebook 2 Voice Tag Thin UI extension plugin implementation.
*
*/


#ifndef CPVT2THINUIEXTENSIONPLUGIN_H
#define CPVT2THINUIEXTENSIONPLUGIN_H

#include <cpbk2uiextensionthinplugin.h>
#include <mpbk2startupobserver.h>
#include <mpbk2commandobserver.h>
#include <ecom/ecom.h>


// FORWARD DECLARATIONS
class CEikMenuPane;
class MPbk2ContactUiControl;

/**
 *  Phonebook 2 Voice tag Thin UI extension plugin implementation.
 *
 */
class CPvt2ThinUIExtensionPlugin : public CPbk2UIExtensionThinPlugin
    {
    public: // Construction and destruction
        /**
         * Two-phased constructor.
         *
         * @return  A new instance of this class.
         */
        static CPvt2ThinUIExtensionPlugin* NewL();

        /**
         * Destructor.
         */
        ~CPvt2ThinUIExtensionPlugin();

    public: // From CPbk2UIExtensionThinPlugin
        void DynInitMenuPaneL(TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aControl );
        void ExtensionStartupL(
                MPbk2StartupMonitor& aStartupMonitor );

    private: // Implementation
        CPvt2ThinUIExtensionPlugin();
        void ConstructL();

    private: // Data

    };

#endif // CPVT2THINUIEXTENSIONPLUGIN_H

// End of File
