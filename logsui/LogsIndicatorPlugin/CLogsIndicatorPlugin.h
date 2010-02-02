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
*    Ecom plugin to implement touch support for universal missed calls indicator. 
*                  
*
*/


#ifndef CLOGSINDICATORPLUGIN_H
#define CLOGSINDICATORPLUGIN_H


// Until aknstylusactivatedpopupcontent.h is fixed
class TAknWindowComponentLayout; 

//  INCLUDES
#include <AknIndicatorPlugin.h>
#include <eikcmobs.h>
#include <aknstylusactivatedpopupcontent.h>

#include <cenrepnotifyhandler.h>  
#include <centralrepository.h>  

// FORWARD DECLARATION
class CRepository;

// The maximum length of the event number string 
const TInt KMaxEventNumberLength = 4; //9999 = 4 numbers
 
// CLASS DECLARATION

/**
 *  Implements shared text pool
 */
class CLogsIndicatorPlugin :     public CAknIndicatorPlugin, MCenRepNotifyHandlerCallback
    {
    public:
    
    static CLogsIndicatorPlugin* NewL();
    
 
    public: // from CAknIndicatorPlugin
   /**
    * Implements stylus tap actions for the indicator in question
    * @param aUid UID of an indicator 
    * @return none
    */
    void HandleIndicatorTapL(const TInt aUid);
    
   /**
    * Returns the description used as link text, displayed with
    * the indicator icon in the indicator popup.
    *
    * @param       aUid       UID of the indicator.
    * @param[out]  aTextType  Type of the text,
    *                         @see TAknIndicatorPluginTextTypes.
    *
    * @return  Indicator description or link text.
    *          Ownership is transferred.
    */
    HBufC* TextL( const TInt aUid, TInt& aTextType );
    
    public: // from MCenRepNotifyHandlerCallback
    
  /** 
    * This callback method is used to notify the client about
    * changes for integer value keys, i.e. key type is EIntKey.
    *
    * @param aId Id of the key that has changed.
    * @param aNewValue The new value of the key.
    */
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );


    ~CLogsIndicatorPlugin();

    private:
    
    CLogsIndicatorPlugin();
    
   /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    /**
     * Create singular or plural content message for the popup
     * @return constructed text
     * 
     * NOTE! Ownership of the text is transferred to caller.
     */
    HBufC* CreatePopupContentMessageL(); 
    
    
    private:
    
    // Command link array for popup
    RArray<TAknContentLink> iLinkArray;
    
    // Owns. Popup heading
    HBufC* iPopupHeading;
    
    // Owns. Popup content singular (template)
    HBufC* iPopupTextOne;

    // Owns. Popup content plural (template)
    HBufC* iPopupTextMultiple;
   
    // Owns. Command link text (Show)
    HBufC* iLinkText;
    
    // Owns. Reference to central repository for fetching the missed call count
    CRepository* iRepositoryLogs;
    // Owns. Reference to central repository for observing the missed call count changes
    CCenRepNotifyHandler* iNotifyHandlerLogs;
    
    TInt iUid;
    
    TInt iResourceFileOffset;
    };
#endif      // CLOGSINDICATORPLUGIN_H



// End of File
