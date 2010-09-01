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


#ifndef CPBK2NAMESLISTUICONTROLEXTENSION_H
#define CPBK2NAMESLISTUICONTROLEXTENSION_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2ContactUiControlExtension.h>
#include <MPbk2ContactUiControlExtension2.h>
#include "MPbk2EcePresenceEngineObserver.h"
#include <pbk2contactuicontroldoublelistboxextension.h>
#include <spbcontentprovider.h>
#include <TPbk2IconId.h>
#include "mpbk2mycardobserver.h"
#include "cpbk2mycard.h"

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;
class CVPbkContactManager;
class MVPbkContactLink;
class CPbk2EcePresenceEngine;
class CPbk2IconArray;


// CLASS DECLARATION

/**
 * Phonebook 2 contact UI control UI extension and extension to extension.
 */
NONSHARABLE_CLASS(CPbk2NameslistUiControlExtension) : 
        public CBase,
        public MPbk2ContactUiControlExtension,
        public MPbk2ContactUiControlExtension2,
        public MPbk2EcePresenceEngineObserver,
        public MPbk2ContactUiControlDoubleListboxExtension,
        public MSpbContentProviderObserver,
        public MPbk2MyCardObserver
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager       Virtual Phonebook contact manager.
         * @param aStatusProvider		Statusprovider
         * @param aMyCard
         * @return  A new instance of this class.
         */
        static CPbk2NameslistUiControlExtension* NewL(
            CVPbkContactManager& aContactManager,
            CSpbContentProvider& aStatusProvider,
            CPbk2MyCard* aMyCard );
        
        /**
         * Destructor.
         */
        ~CPbk2NameslistUiControlExtension();

    private:	// new functions
        /*
         * Creates icon of the bitmap and adds it ot Pbk2IconArray
         */
        void AddMyCardThumbnailToIconArrayL( );
        
    private: // Construction & destruction
        CPbk2NameslistUiControlExtension( CVPbkContactManager& aContactManager,
										  CSpbContentProvider& aStatusProvider,
										  CPbk2MyCard* aMyCard );
        void ConstructL();

    public: // From UiExtensionManager's MPbk2ContactUiControlExtension
        void DoRelease();
        const TArray<TPbk2IconId> GetDynamicIconsL(
                const MVPbkViewContact* aContactHandle );
        void SetContactUiControlUpdate(
                MPbk2ContactUiControlUpdate* aContactUpdator );
        TAny* ContactUiControlExtensionExtension( TUid aExtensionUid );
                
    public: // From UiExtensionManager's MPbk2ContactUiControlExtension2
        void SetIconArray(CPbk2IconArray& aIconArray);

    private: // From MPbk2ContactUiControlDoubleListboxExtension
        void FormatDataL(
                const MVPbkContactLink& aContactLink, 
                MPbk2DoubleListboxDataElement& aDataElement );
        void SetCommandItemUpdater(
            MPbk2CommandItemUpdater* aCommandUpdater );
        
    private:    // From MSpbContentProviderObserver
        void ContentUpdated(
            MVPbkContactLink& aLink, 
            MSpbContentProviderObserver::TSpbContentEvent aEvent );        
        
    private: // From MPbk2EcePresenceEngineObserver
        void ContactPresenceChanged( const MVPbkContactLink& aLink );

    private: // From MPbk2MyCardObserver
        void MyCardEvent( TMyCardStatusEvent aEvent );
        
    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Dummy array
        RArray<TPbk2IconId> iDummyArray;

        /// Ref: observer for contact data changes
        MPbk2ContactUiControlUpdate* iContactUpdator;
        
        /// Not own. observer for command item changes
        MPbk2CommandItemUpdater* iCommandUpdater;

        CPbk2IconArray* iIconArray; // does not own
        CPbk2EcePresenceEngine* iPresenceEngine;
        
        ///REF: content provider
        CSpbContentProvider& iContentProvider;
        // Ref:
        CPbk2MyCard* iMyCard;
        // Own: my card thumbnail bitmap
        CFbsBitmap* iBitmap;
        // Ref: thumbnail on array
        CGulIcon* 	iThumbIcon;
    };

#endif // CPBK2NAMESLISTUICONTROLEXTENSION_H
           
// End of File
