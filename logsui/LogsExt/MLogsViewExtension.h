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
*    Abstract interface for making logs view extensions.
*
*/


#ifndef __MLogsViewExtension_H
#define __MLogsViewExtension_H


//  INCLUDES
#include "MLogsUiExtensionBase.h"


//  FORWARD DECLARATIONS
class CEikMenuPane;
class MLogsEventGetter;

class CSendUi;


// CLASS DECLARATION

/**
 * @internal This interface is internal to Logs.
 *
 * Abstract interface for Logs view extensions.
 */
class MLogsViewExtension : public MLogsUiExtensionBase
    {
    public: // Interface
        /**
         * Filters the menu pane of the Logs view connect to this 
         * extension.
         *
         * @param aResourceId menu resource id.
         * @param aMenuPane menu pane which will be filtered.
         * @param aEvent getter of current event
         */
        virtual void DynInitMenuPaneL
            (TInt aResourceId, CEikMenuPane* aMenuPane,
            const MLogsEventGetter* aEvent = NULL) = 0;

        /**
         * Handles Logs extension commands.
         *
         * @param aCommandId command id.
         * @return ETrue if command was handled.
         */
        virtual TBool HandleCommandL(TInt aCommandId) = 0;
         
// Sawfish VoIP changes  >>>>
        /**
         * Reference to SendUi from the Logs Application UI object.     
         * @since S60 v3.2
         * @param aSendUi reference to SendUi object
         */
        virtual void SetSendUi( CSendUi& aSendUi ) = 0;

        /**
         * Set SendUi menu command text     
         * @since S60 v3.2
         * @param aSendUiText menu command text     
         */    
        virtual void SetSendUiText( const TDesC& aSendUiText ) = 0;
// <<<<  Sawfish VoIP changes  
          
    protected:
        /**
         * Destructor.
         */
        virtual ~MLogsViewExtension() { }
    };

#endif
            

// End of File
