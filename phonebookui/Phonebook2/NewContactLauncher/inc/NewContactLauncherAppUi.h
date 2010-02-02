/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declares UI class for application.
*
*/

#ifndef __NEWCONTACTLAUNCHERAPPUI_h__
#define __NEWCONTACTLAUNCHERAPPUI_h__

// INCLUDES
#include <aknappui.h>
#include <aiwdialdataext.h>
#include <aknPopup.h>
#include <AiwCommon.hrh>
#include <AiwServiceHandler.h>
#include <aiwdialdata.h>
#include <AiwCommon.h>
#include <AiwContactSelectionDataTypes.h>
#include <AiwContactAssignDataTypes.h>
#include <AiwVariant.h>

// FORWARD DECLARATIONS
class CNewContactLauncherAppView;

// CLASS DECLARATION
/**
 * CNewContactLauncherAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CNewContactLauncherAppUi : public CAknAppUi,public MAiwNotifyCallback
    {
public:
    // Constructors and destructor

    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CNewContactLauncherAppUi.
     * C++ default constructor. This needs to be public due to
     * the way the framework constructs the AppUi
     */
    CNewContactLauncherAppUi();

    /**
     * ~CNewContactLauncherAppUi.
     * Virtual Destructor.
     */
    virtual ~CNewContactLauncherAppUi();
    
    /**
    * Proceses message from external application
    * Used from Phonebooks external tab (Buddylist) to start instant
    * messaging or from xmpp server to show new messages
    *
    * @since S60 v3.0
    * @param aUid       Uid somethig. Needed to get this function called
    * @param aParamData Data for processing
    */

    void ProcessMessageL( TUid aUid, const TDesC8& aParamData );
    
private:
    // Functions from base classes

    void HandleStatusPaneSizeChange();

private: //from MAiwNotifyCallback.
        TInt HandleNotifyL(
                    TInt aCmdId,
                    TInt aEventId,
                    CAiwGenericParamList& aEventParamList,
                    const CAiwGenericParamList& aInParamList);
                
private:
    // Data

    /**
     * The application view
     * Owned by CNewContactLauncherAppUi
     */
    CNewContactLauncherAppView* iAppView;
    
    /**
     * Owned by CNewContactLauncherAppUi
     */
    CAiwGenericParamList* iAiwParamList;
        
    /**
     * Owned by CNewContactLauncherAppUi
     */
    CAiwServiceHandler *iServiceHandler;
    
    TInt iResourceOffset;

    };

#endif // __NEWCONTACTLAUNCHERAPPUI_h__
// End of File
