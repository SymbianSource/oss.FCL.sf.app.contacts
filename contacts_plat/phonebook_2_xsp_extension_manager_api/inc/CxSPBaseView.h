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

*    	xSP extension base view.

*

*/





#ifndef __CXSPBASEVIEW_H__

#define __CXSPBASEVIEW_H__



//  INCLUDES

#include <MPbk2UIExtensionView.h>

#include <Pbk2MenuFilteringFlags.hrh>

#include "MxSPViewMapper.h"

#include "MxSPContactManager.h"



//  FORWARD DECLARATIONS

class CPbk2UIExtensionView;



/**

 *	xSP extension base view.

 */

class CxSPBaseView : public CBase,

						public MPbk2UIExtensionView

    {

    public: // From MPbk2UIExtensionView

        void HandleStatusPaneSizeChange();

        CPbk2ViewState* ViewStateLC() const;

        TBool HandleCommandKeyL(

                const TKeyEvent& aKeyEvent,

                TEventCode aType );

        void HandlePointerEventL(

                const TPointerEvent& aPointerEvent );

        TInt GetViewSpecificMenuFilteringFlagsL() const;

        IMPORT_C void DoActivateL(

                const TVwsViewId& aPrevViewId,

                TUid aCustomMessageId,

                const TDesC8& aCustomMessage );

        void DoDeactivate();

        void HandleCommandL(

                TInt aCommand );

        void DynInitMenuPaneL(

                TInt aResourceId,

                CEikMenuPane* aMenuPane );

        void HandleLongTapEventL(

                const TPoint& aPenEventLocation,

                const TPoint& aPenEventScreenLocation);



    protected:



    	/**

         * Standard C++ constructor

         *

         * @param aMapper reference to view mapper

         * @param aContactManager reference to contact manager

         * @param aView reference to CAknView-derived view

         */

        IMPORT_C CxSPBaseView( MxSPViewMapper& aMapper,

                               MxSPContactManager& aContactManager,

                               CPbk2UIExtensionView& aView );



        /**

         *  Base constructor of xSP view. Initializes the view ID.

         *

         *  @param aId xSP implementation ID for identification

         *  @param aOldView original view ID

         */

        IMPORT_C void ViewBaseConstructL( TUint32 aId, TInt aOldId );



        /**

         *  View activation method. Same parameters as in DoActivateL()

         *

         *  @param aPrevViewId  Specifies the view previously active

         *  @param aCustomMessageId  Specifies the message type

         *  @param aCustomMessage  The activation message

         */

        virtual void DoActivateViewL( const TVwsViewId& aPrevViewId,

                                               TUid aCustomMessageId,

                                               const TDesC8& aCustomMessage ) = 0;



    protected: // Data

        /// view ID mapper

        MxSPViewMapper& iMapper;

        /// contact manager/mapper

        MxSPContactManager& iContactManager;

        /// view mapped ID

        TInt iId;

        /// Ref: View that contains this extension view

        CPbk2UIExtensionView& iView;

    };



inline void CxSPBaseView::DoDeactivate()

	{

	}



inline void CxSPBaseView::HandleCommandL( TInt /*aCommand*/ )

	{

	}



inline void CxSPBaseView::DynInitMenuPaneL( TInt /*aResourceId*/,

                							CEikMenuPane* /*aMenuPane*/ )

	{

	}



inline void CxSPBaseView::HandleStatusPaneSizeChange()

	{

	}



inline void CxSPBaseView::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,

                								const TPoint& /*aPenEventScreenLocation*/ )

	{

	}



inline TBool CxSPBaseView::HandleCommandKeyL( const TKeyEvent& /*aKeyEvent*/,

                								TEventCode /*aType*/ )

	{

	return EFalse;

	}



inline void CxSPBaseView::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )

	{

	}



inline TInt CxSPBaseView::GetViewSpecificMenuFilteringFlagsL() const

	{

	return KPbk2MenuFilteringFlagsNone;

	}



inline CPbk2ViewState* CxSPBaseView::ViewStateLC() const

	{

	// Default implementation

    CPbk2ViewState* state = NULL;

    CleanupStack::PushL(state);

    return state;

	}



#endif // __CXSPBASEVIEW_H__



// End of File

