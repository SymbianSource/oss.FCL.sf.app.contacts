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
*       xSP extension base view.
*
*/



// INCLUDE FILES
#include "CxSPBaseView.h"


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CxSPBaseView::CxSPBaseView( MxSPViewMapper& aMapper,
                                     MxSPContactManager& aContactManager,
                                     CPbk2UIExtensionView& aView ) :
                                     iMapper( aMapper ),
                                     iContactManager( aContactManager ),
                                     iView( aView )
    {
    }

EXPORT_C void CxSPBaseView::ViewBaseConstructL( TUint32 aId, TInt aOldId )
    {
    TInt err = iMapper.GetNewView( aId, aOldId, iId );
    User::LeaveIfError( err );
    }

EXPORT_C void CxSPBaseView::DoActivateL( const TVwsViewId& aPrevViewId,
                                         TUid aCustomMessageId,
                                         const TDesC8& aCustomMessage )
    {
    iContactManager.ViewActivatedL( aPrevViewId, aCustomMessageId, aCustomMessage );
    DoActivateViewL( aPrevViewId, aCustomMessageId, aCustomMessage );
    }

// End of File
