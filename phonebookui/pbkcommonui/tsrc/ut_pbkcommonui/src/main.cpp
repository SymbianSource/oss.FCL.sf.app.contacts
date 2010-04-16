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

#include "testrunner.h"

#include "t_cntactions.h"
#include "t_cntaddresseditorview.h"
#include "t_cntbasedetaileditorview.h"
#include "t_cntbaselistview.h"
#include "t_cntbaseselectionview.h"
#include "t_cntbaseview.h"
#include "t_cntcollectionlistmodel.h"
#include "t_cntcollectionview.h"
#include "t_cntcommands.h"
#include "t_cntcompanyeditorview.h"
#include "t_cntcontactcarddatacontainer.h"
#include "t_cntcontactcarddetailitem.h"
#include "t_cntcontactcardheadingitem.h"
#include "t_cntcontactcardview.h"
#include "t_cntdateeditorview.h"
#include "t_cntdetailpopup.h"
#include "t_cnteditordatamodelitem.h"
#include "t_cnteditordataviewitem.h"
#include "t_cnteditviewdetailitem.h"
#include "t_cnteditviewheadingitem.h"
#include "t_cntemaileditorview.h"
#include "t_cntfamilydetaileditorview.h"
#include "t_cntfavoritesmemberview.h"
#include "t_cntfavoritesview.h"
#include "t_cntgroupactionsview.h"
#include "t_cntgroupeditorview.h"
#include "t_cntgroupmemberview.h"
#include "t_cntimageeditorview.h"
#include "t_cntmainwindow.h"
#include "t_cntmycardselectionview.h"
#include "t_cntmycardview.h"
#include "t_cntnameseditorview.h"
#include "t_cntnoteeditorview.h"
#include "t_cntonlineaccounteditorview.h"
#include "t_cntphonenumbereditorview.h"
#include "t_cntsnapshotwidget.h"
#include "t_cnturleditorview.h"
#include "t_cntmodelprovider.h"
#include "t_cntviewmanager.h"
#include "t_cntviewparameters.h"
#include "t_cntcommhistoryview.h"
#include "t_cntgroupselectionpopup.h"
#include "t_cntgroupdeletepopupmodel.h"
#include "t_cntgroupdeletepopup.h"

#include "t_addresseditor.h"
#include "t_companyeditor.h"
#include "t_dateeditor.h"
#include "t_emaileditor.h"
#include "t_familyeditor.h"
#include "t_nameeditor.h"
#include "t_noteeditor.h"
#include "t_numbereditor.h"
#include "t_urleditor.h"

#include "t_cntnavigator.h"
#include "t_namelist.h"
#include "t_cntdefaultviewfactory.h"
#include "t_cntdefaultviewmanager.h"

#include <QtTest/QtTest>

int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");
            
    QApplication app(argc, argv);
    
    TestRunner testRunner("ut_pbkcommonui");
    
    T_NameListTest namesTest;
    testRunner.runTests( namesTest );
    
    T_NavigatorTest navigatorTest;
    testRunner.runTests( navigatorTest );
       
    T_FamilyEditorTest family;
    testRunner.runTests( family );
    
    T_DateEditorTest date;
    testRunner.runTests( date );
    
    T_CompanyEditorTest company;
    testRunner.runTests( company );
    
    T_NameEditorTest name;
    testRunner.runTests( name );
    
    T_NoteEditorTest note;
    testRunner.runTests( note );
    
    T_NumberEditorTest test;
    testRunner.runTests( test );
    
    T_EmailEditorTest email;
    testRunner.runTests( email );
    
    T_UrlEditorTest url;
    testRunner.runTests( url );
    
    T_AddressEditorTest addr;
    testRunner.runTests( addr );
    
    TestCntModelProvider ut_cntModelProvider;
    testRunner.runTests(ut_cntModelProvider);
    
    TestCntViewManager ut_cntViewManager;
    testRunner.runTests(ut_cntViewManager);
    
    TestCntActions ut_cntActions;
    testRunner.runTests(ut_cntActions);
    
    TestCntBaseDetailEditorView ut_cntBaseDetailEditorView;
    testRunner.runTests(ut_cntBaseDetailEditorView);
    
    //  This was getting hanged in after stubbing HBMainwindow 
    // but changing the position helped 
    //  Need to check if stubbing is really needed 
//    TestCntBaseListView ut_cntBaseListView;
//    testRunner.runTests(ut_cntBaseListView);  

    TestCntBaseSelectionView ut_cntBaseSelectionView;
    testRunner.runTests(ut_cntBaseSelectionView);
    
    TestCntBaseView ut_cntBaseView;
    testRunner.runTests(ut_cntBaseView);
    
    TestCntCollectionListModel ut_cntCollectionListModel;
    testRunner.runTests(ut_cntCollectionListModel);
    
    TestCntCollectionView ut_cntCollectionView;
    testRunner.runTests(ut_cntCollectionView);
    
    TestCntCommands ut_cntCommands;
    testRunner.runTests(ut_cntCommands);
    
    TestCntContactCardDataContainer ut_cntContactCardDataContainer;
    testRunner.runTests(ut_cntContactCardDataContainer);
    
    TestCntContactCardDetailItem ut_cntContactCardDetailItem;
    testRunner.runTests(ut_cntContactCardDetailItem);

    TestCntContactCardHeadingItem ut_cntContactCardHeadingItem;
    testRunner.runTests(ut_cntContactCardHeadingItem);

    TestCntContactCardView ut_cntContactCardView;
    testRunner.runTests(ut_cntContactCardView);

    TestCntDetailPopup ut_cntDetailPopup;
    testRunner.runTests(ut_cntDetailPopup);
    
    TestCntEditViewHeadingItem ut_cntEditViewHeadingItem;
    testRunner.runTests(ut_cntEditViewHeadingItem);
    
    TestCntFavoritesMemberView ut_cntFavoritesMemberView;
    testRunner.runTests(ut_cntFavoritesMemberView);
    
    TestCntFavoritesView ut_cntFavoritesView;
    testRunner.runTests(ut_cntFavoritesView); 
    
    TestCntGroupActionsView ut_cntGroupActionsView;
    testRunner.runTests(ut_cntGroupActionsView); 
    
    TestCntGroupEditorView ut_cntGroupEditorView;
    testRunner.runTests(ut_cntGroupEditorView);
    
    TestCntGroupMemberView ut_cntGroupMemberView;
    testRunner.runTests(ut_cntGroupMemberView);
    
    TestCntGroupSelectionPopup ut_cntgroupselectionpopup;
    testRunner.runTests(ut_cntgroupselectionpopup);
    
    TestCntGroupDeletePopupModel ut_cntgroupdeletepopup;
    testRunner.runTests(ut_cntgroupdeletepopup);
    
    TestCntGroupDeletePopup ut_cntgroupdeletepopupmodel;
    testRunner.runTests(ut_cntgroupdeletepopupmodel);
   
    TestCntImageEditorView ut_cntImageEditorView;
    testRunner.runTests(ut_cntImageEditorView);
    
//    TestCntMainWindow ut_cntMainWindow;
//    testRunner.runTests(ut_cntMainWindow);
    
    TestCntMyCardSelectionView ut_cntMyCardSelectionView;
    testRunner.runTests(ut_cntMyCardSelectionView);
    
    TestCntMyCardView ut_cntMyCardView;
    testRunner.runTests(ut_cntMyCardView);
    
//  This was getting hanged in close find after stubbing HBMainwindow 
//  Need to check if stubbing is really needed    
//    TestCntNamesView ut_cntNamesView;
//    testRunner.runTests(ut_cntNamesView);
    
    
//    TestCntNoteEditorView ut_cntNoteEditorView;
//    testRunner.runTests(ut_cntNoteEditorView);
    
    TestCntOnlineAccountEditorView ut_cntOnlineAccountEditorView;
    testRunner.runTests(ut_cntOnlineAccountEditorView);
    
//    TestCntPhoneNumberEditorView ut_cntPhoneNumberEditorView;
//    testRunner.runTests(ut_cntPhoneNumberEditorView);
    
    TestCntSnapshotWidget ut_cntSnapshotWidget;
    testRunner.runTests(ut_cntSnapshotWidget);
    
//    TestCntUrlEditorView ut_cntUrlEditorView;
//    testRunner.runTests(ut_cntUrlEditorView);
    
    TestCntViewParameters ut_cntViewParameters;
    testRunner.runTests(ut_cntViewParameters);
    
    TestCntCommHistoryView ut_cntCommHistoryView;
    testRunner.runTests(ut_cntCommHistoryView);
    
    testRunner.printResults();

    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}

