/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for Fetch dialog page interfaces for Phonebook.
*
*/


// INCLUDE FILES
#include "PbkFetchDlgPageFactory.h"
#include "CPbkFetchDlgPages.h"
#include <PbkView.hrh>
#include <CPbkContactViewListControl.h>
#include "CPbkNamesListFetchDlgPage.h"
#include "CPbkGroupsListFetchDlgPage.h"
#include "MPbkFetchDlg.h"
#include <PbkView.hrh>


// ================= MEMBER FUNCTIONS =======================

CCoeControl* PbkFetchDlgPageFactory::CreateCustomControlL
        (TInt aControlType)
    {
    CCoeControl* ctrl = NULL;
    if (aControlType == EPbkCtContactViewList)
        {
        ctrl = new(ELeave) CPbkContactViewListControl;
        }
    return ctrl;
    }

MPbkFetchDlgPages* PbkFetchDlgPageFactory::CreatePagesL
        (MPbkFetchDlg& aFetchDlg)
    {
    CPbkFetchDlgPages* pages = new(ELeave) CPbkFetchDlgPages;
    CleanupStack::PushL(pages);

    if (aFetchDlg.FetchDlgControl(ECtrlFetchNamesList))
        {
        CPbkNamesListFetchDlgPage* page = CPbkNamesListFetchDlgPage::NewL(aFetchDlg);
        CleanupStack::PushL(page);
        pages->AddL(page);
        CleanupStack::Pop(page);
        }

    if (aFetchDlg.FetchDlgControl(ECtrlFetchGroupsList))
        {
        CPbkGroupsListFetchDlgPage* page = CPbkGroupsListFetchDlgPage::NewL(aFetchDlg);
        CleanupStack::PushL(page);
        pages->AddL(page);
        CleanupStack::Pop(page);
        }

    CleanupStack::Pop(pages);
    return pages;
    }


// End of File
