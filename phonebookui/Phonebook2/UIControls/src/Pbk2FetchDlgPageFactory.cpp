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
* Description:  Provides methods for Fetch dialog page interfaces
                  for Phonebook.
*
*/


#include "Pbk2FetchDlgPageFactory.h"

// Phonebook 2
#include "CPbk2FetchDlgPages.h"
#include "Pbk2UIControls.hrh"
#include "CPbk2FetchDlgPage.h"
#include "CPbk2FetchDlgGroupPage.h"
#include "MPbk2FetchDlg.h"
#include "CPbk2NamesListControl.h"
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// --------------------------------------------------------------------------
// Pbk2FetchDlgPageFactory::CreateCustomControlL
// --------------------------------------------------------------------------
//
CCoeControl* Pbk2FetchDlgPageFactory::CreateCustomControlL
        ( TInt aControlType,  const CCoeControl* aParent,
          MPbk2FetchDlg& aFetchDlg )
    {
    CPbk2NamesListControl* namesListControl = NULL;

    if ( aControlType == EPbk2CtNamesContactViewList )
        {
        namesListControl = new ( ELeave ) CPbk2NamesListControl
            ( aParent,
              Phonebook2::Pbk2AppUi()->ApplicationServices().
                ContactManager(),
              aFetchDlg.FetchDlgViewL( ECtrlFetchNamesList ),
              Phonebook2::Pbk2AppUi()->ApplicationServices().
                NameFormatter(),
              Phonebook2::Pbk2AppUi()->ApplicationServices().
                StoreProperties() );
        }
    else if ( aControlType == EPbk2CtGroupsContactViewList )
        {
        namesListControl = new ( ELeave ) CPbk2NamesListControl
            ( aParent,
              Phonebook2::Pbk2AppUi()->ApplicationServices().
                ContactManager(),
              aFetchDlg.FetchDlgViewL( ECtrlFetchGroupsList ),
              Phonebook2::Pbk2AppUi()->ApplicationServices().
                NameFormatter(),
              Phonebook2::Pbk2AppUi()->ApplicationServices().
                StoreProperties() );
        }

    return namesListControl;
    }

// --------------------------------------------------------------------------
// Pbk2FetchDlgPageFactory::CreatePagesL
// --------------------------------------------------------------------------
//
MPbk2FetchDlgPages* Pbk2FetchDlgPageFactory::CreatePagesL
        ( MPbk2FetchDlg& aFetchDlg, CVPbkContactManager& aContactManager )
    {
    CPbk2FetchDlgPages* pages = new ( ELeave ) CPbk2FetchDlgPages;
    CleanupStack::PushL( pages );

    if ( aFetchDlg.FetchDlgControl( ECtrlFetchNamesList ) )
        {
        CPbk2FetchDlgPage* page = CPbk2FetchDlgPage::NewL
            ( aFetchDlg, ECtrlFetchNamesList );

        CleanupStack::PushL( page );
        pages->AddL( page );
        CleanupStack::Pop( page );
        }

    if ( aFetchDlg.FetchDlgControl( ECtrlFetchGroupsList ) )
        {
        CPbk2FetchDlgGroupPage* page = CPbk2FetchDlgGroupPage::NewL
            ( aFetchDlg,  ECtrlFetchGroupsList, aContactManager );

        CleanupStack::PushL( page );
        pages->AddL( page );
        CleanupStack::Pop( page );
        }

    CleanupStack::Pop( pages );
    return pages;
    }

// End of File
