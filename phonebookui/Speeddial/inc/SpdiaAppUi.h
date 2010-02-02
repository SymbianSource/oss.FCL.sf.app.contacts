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
* Description:     Declares UI class for application.
*
*/






#ifndef SPDIAAPPUI_H
#define SPDIAAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>

#include "speeddial.hrh"
#include "CVPbkContactManager.h"

// FORWARD DECLARATIONS
class CSpdiaContainer;

class CSpeedDialPrivate;//for migration


// CONSTANTS
//const ?type ?constant_var = ?constant;


// CLASS DECLARATION

/**
*  Speeddial application UI class. 
*  An object of this class is created by the Symbian framework by 
*  a call to CPbkDocument::CreateAppUiL(). 
*  The application UI object creates and owns the application's views and 
*  handles system commands selected from the menu.
*
*  @since 
*/
class CSpdiaAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * Symbian default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CSpdiaAppUi();
		/**
        */  
		TBool IsSkinChanged();
		/**
        * Launchs inforamtion note and exits from application
        */
		void LaunchInfoNoteL();
	
       /**
        *  Command handler: Performs call using AIW
        *  @return  ETrue if is lanuched from general settings
        */	
        TBool IsSettingType();
    protected:
       /**
        * From CEikAppUi, takes care of command handling.
        */
        //TBool ProcessCommandParametersL(TApaCommand aCommand, TFileName& aDocumentName, const TDesC8& aTail);	    
        
    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

		 /**
        * From CAknViewAppUi takes care of skin changes.
        * @param aType skin event identifier.
        */
		void HandleResourceChangeL(TInt aType); 
		
	private:

		TBool iSkinChange;//owned by this.
		
		CVPbkContactManager *iContactManager;
		
		CSpeedDialPrivate *iSpeedPrivate;//owned by this.//for migration
		
		TBool iSettingType;

    };

#endif  // SPDIAAPPUI_H

// End of File
