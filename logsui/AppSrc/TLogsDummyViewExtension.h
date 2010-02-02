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
*    Dummy view extension, which does nothing.
*
*/


#ifndef __TLogsDummyViewExtension_H
#define __TLogsDummyViewExtension_H


//  INCLUDES
#include "MLogsViewExtension.h"

class CSendUi;

// CLASS DECLARATION

/**
 * Dummy view extension, which does nothing. Used as default implementation.
 */
class TLogsDummyViewExtension : public MLogsViewExtension
    {
    public: // Constructor and destructor
        /**
         * Constructor.
         */
        TLogsDummyViewExtension();

        /**
         * Destructor.
         */
        ~TLogsDummyViewExtension();       
        
// Sawfish VoIP changes  >>>>
        /**
         * Reference to SendUi from the Logs Application UI object.     
         * @since S60 v3.2
         * @param aSendUi reference to SendUi object
         */
        void SetSendUi( CSendUi& aSendUi );

        /**
        * Set SendUi menu command text     
        * @since S60 v3.2
        * @param aSendUiText  menu command text     
        */    
        void SetSendUiText( const TDesC& aSendUiText );
        
// <<<<  Sawfish VoIP changes  

    private: // Fron MLogsUiExtensionBase
        void DoRelease();
        
    public: // From MLogsViewExtension
        void DynInitMenuPaneL(  TInt aResourceId, 
                                CEikMenuPane* aMenuPane,
                                const MLogsEventGetter* aEvent); 
        TBool HandleCommandL(TInt aCommandId);
    };

#endif
            

// End of File
