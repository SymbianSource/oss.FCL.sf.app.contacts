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
*       Provides methods for Group name query dialog.
*
*/


// INCLUDE FILES
#include    "CPbkGroupNameQueryDlg.h"  // This class' declaration
#include    <cntdb.h>
#include    <cntitem.h>
#include    <aknnotewrappers.h>
#include    <StringLoader.h>
#include    <pbkview.rsg>
#include    <CPbkContactEngine.h>


namespace {

// ==================== LOCAL FUNCTIONS ====================

/**
 * Returns true if aDesCArray contains aText by excact comparison.
 * Discards all directionality markings when comparing texts
 */
TBool Contains(const MDesCArray& aDesCArray, const TDesC& aText)
    {
    TBool ret( EFalse );
    
    HBufC* text = aText.AllocLC();
    TPtr textPtr( text->Des() );
    
    // Strip any directionality markers to get pure text
    const TUint32 KPbkLeftToRightMarker = 0x200F;
    const TUint32 KPbkRightToLeftMarker = 0x200E;
    const TInt markersLength( 2 );
    TBuf<markersLength> bufMarkers;
    bufMarkers.Append( KPbkLeftToRightMarker );
    bufMarkers.Append( KPbkRightToLeftMarker );
    AknTextUtils::StripCharacters( textPtr, bufMarkers );

    const TInt count = aDesCArray.MdcaCount();
    for (TInt i=0; i < count && !ret; ++i)
        {
        HBufC* groupName = aDesCArray.MdcaPoint(i).AllocL();
        TPtr groupNamePtr( groupName->Des() );
        AknTextUtils::StripCharacters( groupNamePtr, bufMarkers );
        if ( groupNamePtr == textPtr)
            {
            ret =  ETrue;
            }
        delete groupName;
        }
    CleanupStack::PopAndDestroy( text );
    return ret;
    }

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkGroupNameQueryDlg::CPbkGroupNameQueryDlg
        (TDes& aDataText, 
        CPbkContactEngine& aEngine) :
    CAknTextQueryDialog(aDataText),
    iEngine(aEngine)
    {
    }

inline void CPbkGroupNameQueryDlg::ConstructL(TBool aNameGeneration)
    {
    // Create group name array
    iGroupLabelsArray = new(ELeave) CDesCArrayFlat(8);
    
    CContactIdArray* idArray = iEngine.Database().GetGroupIdListL();
    if (idArray)
        {
        CleanupStack::PushL(idArray);
        for (TInt i = 0; i < idArray->Count(); ++i)
            {
            CContactGroup* group = iEngine.ReadContactGroupL((*idArray)[i]);
            CleanupStack::PushL(group);
            iGroupLabelsArray->AppendL(group->GetGroupLabelL());
            CleanupStack::PopAndDestroy(); // group
            }
        CleanupStack::PopAndDestroy(idArray); 

        if (aNameGeneration)
            {
            UpdateGroupTitleL();
            }
        }
    }

EXPORT_C CPbkGroupNameQueryDlg* CPbkGroupNameQueryDlg::NewL
        (TDes& aDataText, 
        CPbkContactEngine& aEngine, 
        TBool aNameGeneration /*=ETrue*/)
    {
    CPbkGroupNameQueryDlg* self = new(ELeave) CPbkGroupNameQueryDlg(aDataText, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aNameGeneration);
    CleanupStack::Pop(self);
    return self;
    }

CPbkGroupNameQueryDlg::~CPbkGroupNameQueryDlg()
    {
    // delete data members
    delete iGroupLabelsArray;
    }

EXPORT_C TBool CPbkGroupNameQueryDlg::OkToExitL(TInt aButtonId)
    {
    TBool result = CAknTextQueryDialog::OkToExitL(aButtonId);

    // if entry with same name - not ok.
    if (Contains(*iGroupLabelsArray, Text()))
        {
        // display information note
        HBufC* prompt = StringLoader::LoadLC(R_QTN_FLDR_NAME_ALREADY_USED, Text());
        CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
        dlg->ExecuteLD(*prompt);
        CleanupStack::PopAndDestroy(); // prompt
        CAknQueryControl* queryControl = QueryControl();
        if (queryControl)
            {
            CEikEdwin* edwin = static_cast<CEikEdwin*>(queryControl->ControlByLayoutOrNull(EDataLayout));
            if (edwin)
                {
                edwin->SetSelectionL(edwin->TextLength(), 0);
                }
            }
        result = EFalse;
        }

    return result;
    }

void CPbkGroupNameQueryDlg::UpdateGroupTitleL()
    {
    TBool found = EFalse;
    for (TInt number = 1; !found; ++number)
        {
        HBufC* groupTitle = StringLoader::LoadLC(R_PBK_QTN_FLDR_DEFAULT_GROUP_NAME, number);
		
		// Convert the digits if necessary
		TPtr ptr = groupTitle->Des();
		AknTextUtils::DisplayTextLanguageSpecificNumberConversion(ptr);

        if (!Contains(*iGroupLabelsArray, *groupTitle))
            {
            Text().Copy(*groupTitle);
            found = ETrue;
            }
        CleanupStack::PopAndDestroy(groupTitle);
        }
    }


//  End of File  
