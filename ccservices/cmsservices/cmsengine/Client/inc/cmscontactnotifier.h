/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       
*
*/


#ifndef __CMSCONTACTNOTIFIER__
#define __CMSCONTACTNOTIFIER__

// INCLUDES
#include <s32std.h>

#include "cmscontact.h"
#include "cmscontactfielditem.h"
#include "cmsnotificationhandlerapi.h"

class CCmsContactField;

// CLASS DECLARATION
NONSHARABLE_CLASS( CCmsContactNotifier ) : public CActive 
    {
    public:
    
        /**
	    * Symbian default constructor
        * @param RCmsContact The main contact object
        * @param MCmsNotificationHandlerAPI The observer
        * @param CCmsContactFieldItem::TCmsContactNotification The type of the
                subscribed notification
        * @return CCmsContactNotifier* Self
	    */
        static CCmsContactNotifier* NewL( RCmsContact& aContact, 
                                          MCmsNotificationHandlerAPI& aObserver,
                                          CCmsContactFieldItem::TCmsContactNotification aType );   
    
        /**
        * Destructor
        */
        ~CCmsContactNotifier();    
    
        /**
	    * Activate this object
        * @return void
	    */
        void StartOrderL( TBool aAskMore = EFalse );
       

    private:
    
        /**
        * C++ constructor is private
        * 
        * @return CCmsContactFieldInfo
        */
        CCmsContactNotifier( RCmsContact& aContact, 
                             MCmsNotificationHandlerAPI& aObserver,
                             CCmsContactFieldItem::TCmsContactNotification aType );
    

        /**
        * Symbian second-phase constructor
        * 
        * @return void
        */
        void ConstructL();
        
    public: // new functions
        
        /**
        * Return the notification type
        * 
        * @return CCmsContactFieldItem::TCmsContactNotification
        */
        CCmsContactFieldItem::TCmsContactNotification Type();
    
    private:  //From CActive

        /**
        * An asynchrounous request has been completed
        * 
        * * @return void
        */
        void RunL();
            
        /**
        * An asynchrounous request has been cancelled
        * 
        * @return void
        */
        void DoCancel();
        
        /**
        * An asynchrounous request has failed
        * 
        * @return TInt Error
        */
        TInt RunError( TInt aError );

    private:
        
        /**
        * Start phonebook notifications
        * 
        * @return TInt Error
        */
        void StartPhoneboookNotifyL( TBool aAskMore );
        
        /**
        * Start presence notifications
        * 
        * @return TInt Error
        */
        void StartPresenceNotifyL( TBool aAskMore );
        
        /**
        * Start presence notifications
        * 
        * @return TInt Error
        */
        void CheckRestartL( TBool aDestroyed, TCmsPhonebookEvent aEvent );

    private : // data
    
        RCmsContact&                                   iContact;

        TPtr                                           iStreamDesc;

        HBufC*                                         iStreamBuffer;
    
        MCmsNotificationHandlerAPI&                    iObserver;
        
        TBool                                          iPresenceStarted;

        TBool                                          iPhonebookStarted;

        /**
        * Pointer to stack variable to detect the deletion of the heap
        * instance. This case takes place if a client
        * calls destructor in callback method.
        * Own.
        */
        TBool*                                         iDestroyedPtr; 

        CCmsContactFieldItem::TCmsContactNotification  iType;
    
        CCmsContactField *iContactField;
    };

#endif  //__CMSCONTACTBASENOTIFIER__


// End of File
