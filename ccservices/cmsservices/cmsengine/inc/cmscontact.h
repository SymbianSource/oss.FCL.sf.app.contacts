/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CMSCONTACT_H__
#define __CMSCONTACT_H__

// INCLUDES
#include "cmscontactfielditem.h"
#include <VPbkFieldTypeSelectorFactory.h>

//FORWARD DECLARATIONS
class RCmsSession;
class CCmsContactBase;
class CCmsContactField;
class CCmsContactFieldInfo;
class CCmsContactNotifier;
class MCmsNotificationHandlerAPI;


/** 
* Store information.
*/
enum TCmsContactStore
    {    
    ECmsContactStorePbk = 0,
    ECmsContactStoreSim,
    ECmsContactStoreSdn,
    ECmsContactStoreXsp
    };


//CLASS DECLARATION
class RCmsContact : public RSubSessionBase
    {
    public:  // New functions
        
        /**
        * C++ Constructor.
        */
        IMPORT_C RCmsContact();

        /**
        * Open a contact
        *
        * @param RCmsSession& Session handle
        * @param TInt Contact ID
        * @return TInt Error code
        */
        IMPORT_C TInt Open( RCmsSession& aSession,
                            TInt32 aContactId );
        
        /**
        * Open a contact
        *
        * @param RCmsSession& Session handle
        * @param TDesC8 Contact link
        * @return TInt Error code
        */
        IMPORT_C TInt Open( RCmsSession& aSession, 
                            const TDesC8& aPackedLink );

        /**
        * Open a contact
        *
        * @param RCmsSession& Session handle
        * @param TDesC& Phone number
        * @return TInt Error code
        */
        IMPORT_C TInt Open( RCmsSession& aSession,
                            const TDesC& aPhoneNbr );

        /**
        * Get data for a contact field
        *
        * @param TRequestStatus& Request status of the client
        * @param CCmsContactField::TCmsContactField Type of the field
        * @return CCmsContactField* On completion, contains the data
        *       
        */
        IMPORT_C CCmsContactField* FieldDataL( TRequestStatus& aStatus,
                                               CCmsContactFieldItem::TCmsContactField aFieldType );

        /**
        * Get the enabled fields of this contact
        *
        * @param TRequestStatus& Request status of the caller
        * @return CCmsContactFieldInfo* Contact field information
        */
        IMPORT_C CCmsContactFieldInfo* EnabledFieldsL( TRequestStatus& aStatus );
        
        /**
        * See if this contact has links to external stores
        *
        * If the client wants to get ALL data for a contact, including the data
        * in external (XSP) stores, it must call this method before asking data.
        * On startup, the CMS server reads from Service Provider Settings the URIs
        * of the XSP stores that have been installed on the device. When this method
        * is called, the server goes through the XSP stores (in case there are any)
        * and tries to find a matching XSP ID from the contacts in the stores. Once 
        * the method has returned, all data fetching methods also return the data
        * in the possible matching XSP contacts.
        *
        * @return void
        */
        IMPORT_C void FindExternalContact( TRequestStatus& aStatus ) const;

        /**
        * Cancel a search for an external contact
        *
        * @return void
        */
        IMPORT_C void CancelExternalContactFind() const;
                
        /**
        * Returns a selected contact identifier as descriptor.
        * Contact must be opened with Open() before using this function. 
        *
        * Supported identifiers:
        * MVPbkContactLinkArray (only one contactlink on array) 
        *
        * Leaves with KErrArgument
        * HBufC8 ownership transferred.
        * @see TCmsContactIdentifierType
        * @see RCmsContact::Open
        * @see MVPbkContactLinkArray from VrtPhbk
        * @return HBufC8* packed MVPbkContactLinkArray.
        */
        IMPORT_C HBufC8* GetContactIdentifierL();
        
        /**
        * Check whether a service is available
        *
        * @param CCmsContactBase& The object that has a request pending
        * @return void
        */
        IMPORT_C TInt IsServiceAvailable( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aServiceType ) const;
        
        /**
        * Check from which store this contact is from
        *
        * @return TCmsContactStore Parent store
        */
        IMPORT_C TCmsContactStore ContactStore() const;

        /**
        * Cancel an asynchronous operation
        *
        * @param CCmsContactBase& The object that has a request pending
        * @return void
        */
        IMPORT_C void Cancel( CCmsContactBase& aContactBase );

        /**
        * Close this subsession
        *
        * @return void
        */
        IMPORT_C void Close();

        /**
        * Destructor
        */
        IMPORT_C ~RCmsContact();
        
        /**
        * Order change notifications for a contact field
        *
        * @param aHandler callback observer for subscription notifications
        * @param aNotificationType type of subscritpion
        * @return subscription-id
        *       
        */      
        IMPORT_C void OrderNotificationsL( 
            MCmsNotificationHandlerAPI* aHandler,
            CCmsContactFieldItem::TCmsContactNotification aNotificationType );            
            
        /**
        * Cancel notifications for a contact field
        *
        * @param aSubscriptionType type of subscritpion
        *       
        */       
        IMPORT_C void CancelNotifications( 
            CCmsContactFieldItem::TCmsContactNotification aNotificationType );            
            
        /**
         * Returns number of active fields which are mapped to contact action.  
         * 
         * @param aContactAction contact action requested
         */
        IMPORT_C TInt GetContactActionFieldCount(
            VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction);
        
    public:
        
        /**
        * Send a message
        *
        * @param CCmsContactBase& The contact object
        * @param CCmsContactFieldItem::TCmsContactField Contact field type
        * @return void
        */
        void SendMessage( TInt aServerMessage,
                          CCmsContactBase& aContactField,
                          CCmsContactFieldItem::TCmsContactField aFieldType );

        /**
        * Send a message
        *
        * @param CCmsContactBase& The contact object
        * @param CCmsContactFieldItem::TCmsContactFieldGroup Contact field type
        * @return void
        */
        void SendMessage( TInt aServerMessage,
                          CCmsContactBase& aContactField,
                          CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup );
        
        /**
        * Send a message
        *
        * @param TInt Server message
        * @param TRequestStatus Request status
        * @return TDesC& Stream descriptor
        */
        void SendMessage( TInt aServerMessage, TIpcArgs& aArguments );

        /**
        * Send a message
        *
        * @param TInt Server message
        * @param TRequestStatus Request status
        * @return TDesC& Stream descriptor
        */
        void SendMessage( TInt aServerMessage, TIpcArgs& aArguments, TRequestStatus& aStatus );
                          

        /**
        * Resend a message
        *
        * @param TInt The operation ID
        * @return void
        */
        void ResendReceive( TInt aOperation );        

        /**
        * Resend a message
        *
        * @param TInt The operation ID
        * @param TRequestStatus& Request status
        * @param const TIpcArgs& IPC arguments
        * @return void
        */
        void ResendReceive( TInt aOperation,
                            TRequestStatus& aStatus,
                            const TIpcArgs& aArguments );
                            
        /**
         * Delete notification handler
         *
         * @param aNotificationType notification type          
         */                        
        void DeleteNotifier( 
                CCmsContactFieldItem::TCmsContactNotification aNotificationType );                             

    private:
        
        /**
        * Resend a message
        *
        * @param TInt The operation ID
        * @param TRequestStatus& Request status
        * @param const TIpcArgs& IPC arguments
        * @return void
        */
        void CreateFieldGroupL( TRequestStatus& aStatus,
                                RPointerArray<CCmsContactField>& aResultArray,
                                CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup );
              
        /**
         * Create notification handler
         * 
         * @param aStatus aHandler client handler observer
         * @param aNotificationType notification type 
         * @return notification handler
         */                        
        CCmsContactNotifier* CreateNotifierL( 
                MCmsNotificationHandlerAPI* aHandler,
                CCmsContactFieldItem::TCmsContactNotification aNotificationType );
                
        /**
         * Search notification handler
         * 
         * @param aNotificationType notification type 
         * @return notification handler
         */                        
        CCmsContactNotifier* SearchNotifier( 
                CCmsContactFieldItem::TCmsContactNotification aNotificationType );                
                                                            
    private:

        TPtr8                               iContactIdentifierPtr;
        RPointerArray<CCmsContactNotifier>  iNotifiers;
    };
        

#endif      //__CMSCONTACT_H__



