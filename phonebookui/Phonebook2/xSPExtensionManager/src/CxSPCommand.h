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
*       xSP command
*
*/


#ifndef __CXSPCOMMAND_H__
#define __CXSPCOMMAND_H__

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>

// forward declarations
class CxSPViewManager;
class MPbk2ContactUiControl;
class MPbk2CommandObserver;

/**
 * xSP command class
 */
class CxSPCommand : public CBase,
                    public MPbk2Command
    {
    public:

        /**
         * Static factory function that performs the 2-phased construction.
         *
         * @param CommandId Command id for this instance
         * @param aUiControl ui control of this command
         * @param aViewManager View manager to be used
         *
         * @return Newly created CxSPCommand.
         */
        static CxSPCommand* NewL( TInt aCommandId,
        						  MPbk2ContactUiControl& aUiControl,
        						  CxSPViewManager& aViewManager );

        /**
         * Destructor.
         */
        ~CxSPCommand();

    public: // from MPbk2Command

      	void ExecuteLD();
      	void ResetUiControl( MPbk2ContactUiControl& aUiControl );
      	void AddObserver( MPbk2CommandObserver& aObserver );


    private:
        /**
         * Standard C++ constructor.
         *
         * @param CommandId Command id for this instance
         * @param aUiControl ui control of this command
         * @param aViewManager View manager to be used
         */
        CxSPCommand( TInt aCommandId,
        			 MPbk2ContactUiControl& aUiControl,
        			 CxSPViewManager& aViewManager );

    private: // Data
    	/// Command id of this instance
    	TInt iCommandId;

    	/// Own: ui control of this command
    	MPbk2ContactUiControl* iUiControl;

    	/// Ref: view manager
    	CxSPViewManager& iViewManager;

    	/// Not owned: command observer
    	MPbk2CommandObserver* iCommandObserver;
    };


#endif // __CXSPCOMMAND_H__

// End of File
