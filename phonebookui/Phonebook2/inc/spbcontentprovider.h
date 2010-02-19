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
* Description:
*
*/


#ifndef SPBCONTENTPROVIDER_H_
#define SPBCONTENTPROVIDER_H_

//  INCLUDES
#include <e32base.h>

class CSpbContentProviderPrivate;
class MVPbkContactLink;
class CVPbkContactManager;
class CPbk2StoreManager;
class TPbk2IconId;
class CGulIcon;

/**
 * Interface to observe content changes
 */
class MSpbContentProviderObserver
    {
public:
    
    /**
     * Event types
     */
    enum TSpbContentEvent
        {
        EContentChanged = 0x0,
        EContentNotAvailable
        };
    
    /**
     * Provider send this event when there is updated content 
     * information available for the contact.
     * 
     * @param aLink Contact which the event is about
     * @param aEvent Type of the event
     */
    virtual void ContentUpdated(
        MVPbkContactLink& aLink, 
        MSpbContentProviderObserver::TSpbContentEvent aEvent ) = 0;

protected:
    MSpbContentProviderObserver(){};
    virtual ~MSpbContentProviderObserver() {};
    };


/**
 * CSpbContentProvider provides methods to access 
 * to extra content of the contact for Social phonebook features.
 * 
 * @since TB9.2
 * @lib pbk2spbcontentprovider.lib
 */
NONSHARABLE_CLASS( CSpbContentProvider ) : public CBase
    {
public: // Data types

    /// Content flags (binary)
    enum TSpbContentFlags
        {
        ENone           = 0x0,
        /**
         * If Feature Manager FF_CONTACTS_SOCIAL flag is not defined  
         * status message is not fetched
         */ 
        EStatusMessage  = 0x1,
        /**
         * If Feature Manager FF_CONTACTS_SOCIAL flag is not defined  
         * service icon is not fetched
         */ 
        EServiceIcon    = 0x2,
        /**
         * If status message is not available or Feature Manager 
         * FF_CONTACTS_SOCIAL is not defined phone number is returned
         */ 
        EPhoneNumber    = 0x4
        };

    /// Type of the retrieved content
    enum TSpbContentType
        {
        /// No content available
        ETypeNone       = 0x0,
                
        /// Content is contact's social status (text & icon)  
        ETypeSocialStatus,
        
        /// Content is contact's phone number (text)
        ETypePhoneNumber,
        
        /// Content is contact's phone number count (text [only number])
        /// This type is used when contact has multiple numbers and no default number
        ETypePhoneNumberMultiple
        };
    
public: // Construction & destruction

    /**
     * Constructor
     * 
     * @param aContactManager Contact Manager instance
     * @param aFeatures Required content (see TSpbContentFlags)
     */
    IMPORT_C static CSpbContentProvider* NewL(
        CVPbkContactManager& aContactManager,
        CPbk2StoreManager& aStoreManager,
        TInt32 aFeatures );
    
    /// Destructor
    ~CSpbContentProvider();

public:
    /**
     * Add observer
     */
    IMPORT_C void AddObserverL(
        MSpbContentProviderObserver& aObserver );

    /**
     * Removes observer
     */
    IMPORT_C void RemoveObserver(
        MSpbContentProviderObserver& aObserver );
            
    /**
     * Provides Spb content information for the contact. 
     * If current extra content is not know by provider then provider 
     * starts fetch extra information and send observer event when content 
     * information is available.
     */
    IMPORT_C void GetContentL(
        MVPbkContactLink& aLink,
        HBufC*& aText,
        TPbk2IconId& aIconId, 
        TSpbContentType& aType );

    /**
     * Create service icon. Ownership transferred to caller
     * Note! Function returns a NULL pointer if Feature Manager Flag 
     * FF_CONTACTS_SOCIAL is not supported or CSpbContentProvider::EServiceIcon
     * flag is not defined
     * 
     * @param aIconId icon id for created icon
     * @return Created icon object 
     */
    IMPORT_C CGulIcon* CreateServiceIconLC( 
        const TPbk2IconId& aIconId );
    
private: // Constructors
    
    /// Constructor
    inline CSpbContentProvider();
    
    /// Constructor
    inline void ConstructL(
        CVPbkContactManager& aContactManager,
        CPbk2StoreManager& aStoreManager,
        TInt32 aFeatures);

private: // data
    
    // Own. Private implementation
    CSpbContentProviderPrivate* iImpl;
    };

#endif /*SPBCONTENTPROVIDER_H_*/
