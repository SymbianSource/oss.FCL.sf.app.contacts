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
*     Implements Thumbnail adder functionality.
*
*/


#ifndef __CPbkAiwProviderBase_H__
#define __CPbkAiwProviderBase_H__

//  INCLUDES
#include <AiwServiceIfMenu.h>
#include <RPbkResourceFile.h>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkCommandStore;
class MPbkCommand;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;

//  CLASS DECLARATION 

class CPbkAiwProviderBase :
        public CAiwServiceIfMenu
    {
    protected: // constructor & destructor
        CPbkAiwProviderBase();
        ~CPbkAiwProviderBase();

    protected: // from CAiwServiceIfMenu
        virtual void HandleServiceCmdL(const TInt& aCmdId,
		                const CAiwGenericParamList& aInParamList,
                        CAiwGenericParamList& aOutParamList,
		                TUint aCmdOptions = 0,
		                const MAiwNotifyCallback* aCallback = NULL) =0;
        virtual void InitializeMenuPaneL(CAiwMenuPane& aMenuPane,
                        TInt aIndex,
		                TInt aCascadeId,
		                const CAiwGenericParamList& aInParamList) =0;
	    virtual void HandleMenuCmdL(TInt aMenuCmdId,
		                const CAiwGenericParamList& aInParamList,
		                CAiwGenericParamList& aOutParamList,
		                TUint aCmdOptions = 0,
		                const MAiwNotifyCallback* aCallback = NULL) =0;

    protected: // implementation
        void BaseConstructL();
        void SaveStatusPaneL();
        void RestoreStatusPaneL();
        void AddAndExecuteCommandL(MPbkCommand* aCmd);

    private: // from CAiwServiceIfMenu
        void InitialiseL(MAiwNotifyCallback& aFrameworkCallback, 
                        const RCriteriaArray& aInterest);

    private: // implementation
        

    protected: // data
        /// Own: phonebook engine
        CPbkContactEngine* iEngine;
        /// Ref: list criteria items which invoked this provider
        const RCriteriaArray* iInterest;

    private: // data
        /// Own: resource file
        RPbkResourceFile iResourceFile;
        /// Own: command store
        CPbkCommandStore* iCommandStore;
        /// Ref: navigation container
        CAknNavigationControlContainer* iNaviContainer;
        /// Ref: stored navi pane
        CAknNavigationDecorator* iNaviPane;
        /// Own: status pane resource id to be restored
        TInt iStatusPaneResId;
    };

#endif // __CPbkAiwProviderBase_H__

// End of File
