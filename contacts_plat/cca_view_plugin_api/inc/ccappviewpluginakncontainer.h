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
* Description:  CCoeControl tailored for the needs of CCApp plugins.
*
*/

#ifndef CCAPPVIEWPLUGINAKNCONTAINER_H
#define CCAPPVIEWPLUGINAKNCONTAINER_H

#ifndef __CCAPPUTILUNITTESTMODE
#include <coecntrl.h>
#endif //__CCAPPUTILUNITTESTMODE

class CCoeAppUi;

/**
 *  CCoeControl tailored for the needs of CCApp plugins.
 *
 *  @code
 *
 *  Derive from CCCAppViewPluginAknContainer. Implement ConstructL()
 *  and construct the controls in there.
 *
 *   void CCCAppMyContainer::ConstructL()
 *       {
 *       iLabel = new (ELeave) CEikLabel;
 *       iLabel->SetContainerWindowL( *this );
 *       iLabel->SetTextL( _L("Hello CCApp-world") );
 *       }
 * 
 *  In case container is interested about key-events,
 *  remember to call base-class 1st and use the key-event
 *  only when base-class returns EKeyWasNotConsumed.
 * 
 *  TKeyResponse CCCAppMyContainer::OfferKeyEventL(
 *      const TKeyEvent& aKeyEvent, TEventCode aType)
 *       {
 *       // Call base implementation 1st
 *       TKeyResponse returnValue = 
 *           CCCAppViewPluginAknContainer::OfferKeyEventL( aKeyEvent, aType );
 *   
 *       if ( EKeyWasNotConsumed == returnValue )
 *           {
 *           ... my container key-handling stuff
 *           }
 *   
 *       return returnValue;
 *       }
 *
 *  @endcode
 *
 *  @lib ccapputil.dll
 *  @since S60 v5.0
 */
class CCCAppViewPluginAknContainer : public CCoeControl
{
    public: // Constructors and destructor

        /**
        * Meant for the sub-class to create the controls for the container.
        *
        * @since S60 v5.0        
        */
        virtual void ConstructL() = 0;

        /**
        * Called by the view-class.
        *
        * @since S60 v5.0        
        * @param aRect
        * @param aAppUi
        */
        void BaseConstructL( const TRect& aRect, CCoeAppUi& aAppUi );

        /**
        * Constructor.
        */
        IMPORT_C CCCAppViewPluginAknContainer();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCCAppViewPluginAknContainer();

    protected: // From CCoeControl

        /**
        * See more details from CCoeControl.
        */
        IMPORT_C virtual void HandleResourceChange( TInt aType );

        /**
        * Takes care of routing the reserved keys to CCApplication
        * to ensure view switching.
        * 
        * This should be called from the sub-class.
        *
        * See more details from CCoeControl.
        */
        IMPORT_C virtual TKeyResponse OfferKeyEventL( 
            const TKeyEvent& aKeyEvent,
            TEventCode aType );
            
    protected: // Data

        /**
        * AppUi.
        * Not owned.
        */
        CCoeAppUi* iAppUi;
};

#endif // CCAPPVIEWPLUGINAKNCONTAINER_H
// End of File
