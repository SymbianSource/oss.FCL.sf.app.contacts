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

#include "cntabstractviewfactory.h"
#include "cntviewparameters.h"
#include "cntmainwindow.h"

// views
#include "cntnamesview.h"
#include "cntmycardview.h"
#include "cntfavoritesview.h"
#include "cntimageeditorview.h"
#include "cntcollectionview.h"

CntDefaultViewFactory::CntDefaultViewFactory( )
{
}

CntDefaultViewFactory::~CntDefaultViewFactory()
{
}

CntAbstractView* CntDefaultViewFactory::createView( int aId )
{
    switch ( aId )
    {
    case CntViewParameters::namesView:
    case CntViewParameters::defaultView:
        return new CntNamesView();
    case CntViewParameters::noView:
    case CntViewParameters::commLauncherView:
    case CntViewParameters::serviceContactCardView:
    case CntViewParameters::serviceAssignContactCardView:
        break;
    case CntViewParameters::myCardView:
        return new CntMyCardView();
    case CntViewParameters::myCardSelectionView:
    case CntViewParameters::serviceContactSelectionView:
        break;
    case CntViewParameters::collectionView:
        return new CntCollectionView();
    case CntViewParameters::collectionFavoritesView:
        return new CntFavoritesView();
    case CntViewParameters::FavoritesMemberView:
    case CntViewParameters::editView:
    case CntViewParameters::serviceEditView:
    case CntViewParameters::serviceSubEditView:
    case CntViewParameters::emailEditorView:
    case CntViewParameters::namesEditorView:
    case CntViewParameters::urlEditorView:
    case CntViewParameters::companyEditorView:
    case CntViewParameters::phoneNumberEditorView:
    case CntViewParameters::onlineAccountEditorView:
    case CntViewParameters::noteEditorView:
    case CntViewParameters::familyDetailEditorView:
    case CntViewParameters::addressEditorView:
    case CntViewParameters::dateEditorView:
        break;
    case CntViewParameters::imageEditorView:
        return new CntImageEditorView();
    case CntViewParameters::serviceContactFetchView:
    case CntViewParameters::groupEditorView:
    case CntViewParameters::groupMemberView:
    case CntViewParameters::groupActionsView:
    case CntViewParameters::historyView:
        break;
    default:
        break;
    }
    return NULL;
}

// End of File
