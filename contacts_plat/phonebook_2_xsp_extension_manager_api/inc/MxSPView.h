/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*    Abstract interface for Phonebook xSP view extensions.
*
*/


#ifndef __MXSPVIEW_H__
#define __MXSPVIEW_H__


//  INCLUDES
#include "MxSPBase.h"


//  FORWARD DECLARATIONS
class CEikMenuPane;
class MxSPCommandMapper;
class MPbk2ContactUiControl;


// CLASS DECLARATION

/**
 * Abstract interface for Phonebook xSP view extensions. xSP:s can
 * extend Phonebook Names List view by implementing a class derived from this.
 * Extension Manager currently supports only Names List view extending.
 */
class MxSPView : public MxSPBase
    {
    public: // Interface
        /**
         * Filters the menu pane of the Phonebook view connected to this
         * extension.
         *
         * @param aResourceId Menu resource ID.
         * @param aMenuPane Menu pane which will be filtered.
         */
        virtual void DynInitMenuPaneL
            (TInt aResourceId, CEikMenuPane* aMenuPane) = 0;
            
		/**
         * Filters the Names List Mainmenu pane of the Phonebook view
         * connected to this extension.
         *
         * @param aResourceId Menu resource ID.
         * @param aMenuPane Menu pane which will be filtered.
         */
        virtual void DynInitNamesListMainMenuPaneL
            (TInt aResourceId, CEikMenuPane* aMenuPane) = 0;            

        /**
         * Handles Phonebook extension commands.
         *
         * @param aCommandId Command ID.
         * @param aUiControl UI control for this command
         *
         * @return ETrue if command was handled, otherwise EFalse.
         */
        virtual TBool HandleCommandL(TInt aCommandId, MPbk2ContactUiControl* aUiControl) = 0;
               
        /**
         * Registers command ID mapper. This method is called automatically
         * by Extension Manager framework at initialization phase.
         *
         * @param aMapper Command ID mapper
         */  
        virtual void RegisterCommandMapper( MxSPCommandMapper& aMapper ) = 0;
                
        /**
         * Gets XSP_COMMAND_INFO_ARRAY resource. Resource should be loaded.
         *
         * @return Resource ID or KErrNotFound
         */        
        virtual TInt CommandInfoResource() const = 0;                                           
                                
    protected:
        /**
         * Destructor.
         */
        virtual ~MxSPView() { }
    };

#endif // __MXSPVIEW_H__

// End of File
