/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 Nameslist UI control UI extension.
*
*/


#ifndef CPbk2EcePresenceEngine_H
#define CPbk2EcePresenceEngine_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2ContactUiControlExtension.h>

#include "CPbk2IconArray.h"
#include "TPbk2IconId.h"
#include "CPbk2NlxPresenceIconInfo.h"
#include "CPbk2NlxPresenceSubscriptionInfo.h"
#include "MPbk2EcePresenceEngineObserver.h"

#include <mcontactpresenceobs.h>
#include <mcontactpresenceinfo.h>
// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkContactLink;
class MContactPresence;
class CFbsBitmap;
class CBSFactory;

// CLASS DECLARATION

/**
 * Phonebook 2 ECE Presence Engine.
 *
 */
NONSHARABLE_CLASS(CPbk2EcePresenceEngine) : 
        public CBase,
        public MContactPresenceObs
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager       Virtual Phonebook contact manager.
         * @param aObserver Observer to this engine.
         * @param aIconSize Required icon size
         * @return  A new instance of this class.
         */
        static CPbk2EcePresenceEngine* NewL(
            CVPbkContactManager& aContactManager,
            MPbk2EcePresenceEngineObserver& aObserver,
            TSize aIconSize );
        
        /**
         * Destructor.
         */
        ~CPbk2EcePresenceEngine();

    private: // Construction & destruction
        CPbk2EcePresenceEngine(
            CVPbkContactManager& aContactManager,
            MPbk2EcePresenceEngineObserver& aObserver,
            TSize aIconSize );
        void ConstructL();
        
    public: // new methods

        /**
         * Subscribe for presence information of a certain contact.
         *
         * @param aLink A contact link.
         */
        void SubscribeContactPresenceL( const MVPbkContactLink& aLink );

        /**
         * Gets the ID of the current presence icon of a contact.
         * The TBool return value tells whether the icon ID and icon is available.
         * If the ID is successfully obtained, then CreateIconCopyLC() can be called.
         *
         * @param aLink A contact link.
         *
         * @return ETrue if the icon ID was available, and vice versa.
         */
        TBool GetIconIdForContact( const MVPbkContactLink& aLink, TPbk2IconId& aIconId );

        /**
         * Creates an icon instance. Call only after a succesfull call to GetIconIdForContactL().
         *
         * @exception Panics if called when icon is not available. See GetIconIdForContactL().
         */
        CGulIcon* CreateIconCopyLC( const TPbk2IconId& aIconId );
        
        /**
         * Gets the bitmap and mask for the element from the Branding Server
         * @param aLanguageId - Language for which Brand is requested        
         * @param aBrandedBitmap - Ptr to the Bitmap Ownership is transferred to the caller
         * @param aBrandedMask - Ptr to the BitmapMask Ownership is transferred to the caller
         * @param aBSFactory - interface for  access brandingserver 
         * @param aBrandId - brandingId,
         * @param aElementId - elementId
         */
         void GetBitmapFromBrandingServerL(
                             TLanguage aLanguageId, 
                             CFbsBitmap*& aBrandedBitmap, 
                             CFbsBitmap*& aBrandedMask,
                             CBSFactory& aBSFactory,
                             const TDesC8& aBrandId, 
                             const TDesC8& aElementId) ;
         
         /**
         * Loops thro the ServiceTable/SpSettings and finds the
         * BrandLanguage associated to the BrandId (aBrandId)
         * @param aBrandId - BrandId mentioned in the ServiceTable
         * @return TLanguage - Returns the matched Language Id, else 47(ELangInternationalEnglish)
         */
         TLanguage CPbk2EcePresenceEngine::FindBrandLanguageIdL( const TDesC8& aBrandId );


    private: // from MContactPresenceObs
        void ReceiveIconInfoL( 
            const TDesC8& aPackedLink, 
            const TDesC8& aBrandId, 
            const TDesC8& aElementId );
        void ReceiveIconFileL( 
            const TDesC8& aBrandId, 
            const TDesC8& aElementId, 
            CFbsBitmap* aBrandedBitmap, 
            CFbsBitmap* aMask );
        void ReceiveIconInfosL(
            const TDesC8& aPackedLink,
            RPointerArray <MContactPresenceInfo>& aInfoArray,
            TInt aOpId );
        void PresenceSubscribeError( 
            const TDesC8& aContactLink,
            TInt aStatus );        
        void ErrorOccured( 
            TInt aOpId, 
            TInt aStatus );   
        

    private: // New methods

        void InformObserverOfContactChanges(
            const TDesC8& aBrandId,
            const TDesC8& aElementId );

        CPbk2NlxPresenceSubscriptionInfo* FindSubscription( const MVPbkContactLink& aContactLink );
        CPbk2NlxPresenceIconInfo* FindIconInfo( const TDesC8& aBrandId, const TDesC8& aElementId );
        CPbk2NlxPresenceIconInfo* FindIconInfo( const TPbk2IconId& aIconId );
        void CleanUpOldSubscriptionsL();
        void CleanUpExtraIcons();

        static CFbsBitmap* CloneBitmapL( const CFbsBitmap& aOriginalBitmap );

        static TInt SubscriptionMonitorL(TAny* aPtr);
        TInt SubscriptionMonitorL();
        static TSize CalculateListIconSize();
    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;

        MContactPresence* iPresenceClient;
        RPointerArray<CPbk2NlxPresenceSubscriptionInfo> iSubscriptions; // owns the objects
        RPointerArray<CPbk2NlxPresenceIconInfo> iIconInfoArray; // owns the objects
        TInt iNextIconId;
        MPbk2EcePresenceEngineObserver& iObserver;
        CIdle* iSubscriptionMonitor;
        TSize iIconSize;
    };

#endif // CPbk2EcePresenceEngine_H
           
// End of File
