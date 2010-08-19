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
* Description:  Temporary listbox model.
*
*/

#include <VPbkContactStoreUris.h>
#include <VPbkUtils.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStore.h>
#include <MVPbkFieldType.h>
#include <MVPbkStoreContact.h>
#include <CVPbkTopContactManager.h>
#include <MVPbkContactAttributeManager.h>
#include <CVPbkSpeedDialAttribute.h>
#include <CVPbkDefaultAttribute.h>
#include <MVPbkStoreContactField2.h>

#include <CPbk2StorePropertyArray.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldPropertyArray.h>
#include <Pbk2FieldPropertiesFactory.h>
#include <TPbk2StoreContactAnalyzer.h>

#include "TPbk2ContactEditorParams.h"
#include "CPbk2ContactEditorDlg.h"

#include "CPbk2ServiceManager.h"
#include "CPbk2FieldPropertyArray.h"
#include "CPbk2PresentationContact.h"
#include "CPbk2PresentationContactField.h"
#include "CPbk2PresentationContactFieldCollection.h"
#include "CPbk2StoreSpecificFieldPropertyArray.h"
#include "CPbk2IconArray.h"
#include <CPbk2UIExtensionManager.h>
#include <CPbk2ApplicationServices.h>
#include <Pbk2PresentationUtils.h>
#include <Pbk2AddressTools.h>
#include <TVPbkFieldTypeParameters.h>
#include <TVPbkFieldVersitProperty.h>
#include <StringLoader.h>

#include <TPbk2AppIconId.h>

#include <aknlists.h>

#include <avkon.rsg>
#include <VPbkEng.rsg>
#include <Pbk2UIControls.rsg>

#include "ccappdetailsviewlistboxmodel.h"
#include "ccappdetailsviewdefs.h"
#include <ccappdetailsviewpluginrsc.rsg>
#include "ccappdetailsviewplugin.h"
#include "ccappdetailsviewpluginuids.hrh"
#include "ccappdetailsview.hrh"

const TInt KCacheGranularity = 4;
const TInt KBufferSize = 256;

_LIT( KCharsToReplace,   "\t" );
_LIT( KReplacementChars, " "  );

const TText KReplacedChars = ' ';


// ======== LOCAL FUNCTIONS ========

// Utility to check file existence without BAFL
inline TBool FileExists(RFs& aFs, const TDesC& aFileName)
{
    TEntry ignore;
    return (aFs.Entry(aFileName, ignore) == KErrNone);
}

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::CCCAppDetailsViewListBoxRow
// --------------------------------------------------------------------------
//
inline CCCAppDetailsViewListBoxRow::CCCAppDetailsViewListBoxRow()
{
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::~CCCAppDetailsViewListBoxRow
// --------------------------------------------------------------------------
//
CCCAppDetailsViewListBoxRow::~CCCAppDetailsViewListBoxRow()
{
    iColumns.ResetAndDestroy();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::NewL
// --------------------------------------------------------------------------
//
CCCAppDetailsViewListBoxRow* CCCAppDetailsViewListBoxRow::NewL()
{
    return new(ELeave) CCCAppDetailsViewListBoxRow;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::ColumnCount
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewListBoxRow::ColumnCount() const
{
    return iColumns.Count();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::At
// --------------------------------------------------------------------------
//
TPtrC CCCAppDetailsViewListBoxRow::At(TInt aColumnIndex) const
{
    return *iColumns[aColumnIndex];
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::AppendColumnL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxRow::AppendColumnL(const TDesC& aColumnText)
{
    HBufC* buf = aColumnText.AllocLC();
    User::LeaveIfError(iColumns.Append(buf));
    CleanupStack::Pop(buf);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::IsClipRequired
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxRow::IsClipRequired() const
{
    return iClipBeginning;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::SetClipRequired
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxRow::SetClipRequired(TBool aFlag)
{
    iClipBeginning = aFlag;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::TotalLength
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewListBoxRow::TotalLength() const
{
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
    {
        result += At(i).Length();
    }
    return result;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxRow::MaxColumnLength
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewListBoxRow::MaxColumnLength() const
{
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
    {
        result = Max(result, At(i).Length());
    }
    return result;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::NewL
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewListBoxModel* CCCAppDetailsViewListBoxModel::NewL(
    CCoeEnv& aCoeEnv,
    CEikListBox& aListBox,
    CPbk2ApplicationServices* aApplicationServices,
    CPbk2IconArray& aIconArray,
    CCCAppDetailsViewPlugin& aPlugin)
{
    CCCAppDetailsViewListBoxModel* self =
        new(ELeave) CCCAppDetailsViewListBoxModel(
            aCoeEnv, aListBox, aApplicationServices, aIconArray,
            aPlugin);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::~CCCAppDetailsViewListBoxModel
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewListBoxModel::~CCCAppDetailsViewListBoxModel()
{
    Cancel();

    iRows.ResetAndDestroy();
    delete iLineBuf;
    delete iColumnBuf;
    delete iLinks;
    delete iStoreContact;
    delete iPresentationContact;
    delete iSpecificFieldProperties;
    delete iFieldProperties;
    delete iRetrieveOperation;
    iInxToPresentationIdx.Close();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::CCCAppDetailsViewListBoxModel
// ---------------------------------------------------------------------------
//
CCCAppDetailsViewListBoxModel::CCCAppDetailsViewListBoxModel(
    CCoeEnv& aCoeEnv,
    CEikListBox& aListBox,
    CPbk2ApplicationServices* aApplicationServices,
    CPbk2IconArray& aIconArray,
    CCCAppDetailsViewPlugin& aPlugin)
:
    CActive(EPriorityStandard),
    iCoeEnv(aCoeEnv),
    iListBox(aListBox),
    iRows(KCacheGranularity),
    iAppServices(aApplicationServices),
    iIconArray(aIconArray),
    iCommand(ECCAppDetailsViewCommandNone),
    iIsEditingAborted(EFalse),
    iPlugin(aPlugin)
{
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ConstructL()
{
    CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewListBoxModel::ConstructL()"));

    CCA_DP(KDetailsViewLogFile, CCA_L("iInitialFocusIndex = %d"), iInitialFocusIndex);

    iLineBuf   = HBufC::NewL(KBufferSize);
    iColumnBuf = HBufC::NewL(KBufferSize);

    CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewListBoxModel::ConstructL()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::SetLinks
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::SetLinks(MVPbkContactLinkArray* aLinks)
{
    delete iLinks;
    iLinks = aLinks;
    //iCurrentLink is now invalid since it points to old iLinks array
    iCurrentLink = NULL;

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    IssueRequest();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::Links
// ---------------------------------------------------------------------------
//
MVPbkContactLinkArray* CCCAppDetailsViewListBoxModel::Links() const
{
    return iLinks;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::OpenEditorL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::OpenEditorL()
{
    if (iStoreContact)
    {
        iCommand = ECCAppDetailsViewCommandEdit;
        iStoreContact->LockL(*this);
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::FocusedFieldIndex
// ---------------------------------------------------------------------------
//
TInt CCCAppDetailsViewListBoxModel::FocusedFieldIndex()
{
    TInt *index = iInxToPresentationIdx.Find(iListBox.CurrentItemIndex());

    if (index && *index >= KErrNone)
    {
        return iPresentationContact->PresentationFields().StoreIndexOfField(*index);
    }
    else
    {
        return KErrNotFound;
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::FocusedListIndex
// ---------------------------------------------------------------------------
//
TInt CCCAppDetailsViewListBoxModel::FocusedListIndex()
{
    return iListBox.CurrentItemIndex();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::SetFocusedListIndex
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::SetFocusedListIndex(TInt aIndex)
{
    TInt items = iListBox.Model()->NumberOfItems();

    if (aIndex >= items)
    {
        aIndex = items - 1;
    }
    if (aIndex < 0)
    {
        aIndex = 0;
    }

    CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewListBoxModel::SetFocusedListIndex( %d )"), aIndex);

    iListBox.SetCurrentItemIndexAndDraw(aIndex);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::SetInitialFocusIndex
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::SetInitialFocusIndex(TInt aIndex)
{
    CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewListBoxModel::SetInitialFocusIndex( %d )"), aIndex);

    if (aIndex >= 0)
    {
        iInitialFocusIndex = aIndex;
    }
}

TInt CCCAppDetailsViewListBoxModel::FindFocusListIndexByStoreIndex(TInt aIndex)
    {
    TInt fieldIndex = iPresentationContact->PresentationFields().FieldIndexOfStoreField( aIndex );
    
    TInt focusListIndex = KErrNotFound;
    
    TInt count = iInxToPresentationIdx.Count();
    
    for ( TInt i = 0; i < count && focusListIndex == KErrNotFound; i++ )
        {
        TInt *index = iInxToPresentationIdx.Find( i );
        if ( index && *index >= KErrNone && *index == fieldIndex )
            {
            focusListIndex = i;
            }
        }
    return focusListIndex;
    }


// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::FocusedFieldLC
// --------------------------------------------------------------------------
//
MVPbkBaseContactField* CCCAppDetailsViewListBoxModel::FocusedFieldLC()
{
    const TInt listIndex = iListBox.CurrentItemIndex();

    if (listIndex != KErrNotFound)
    {
        return FieldAtLC(listIndex);
    }

    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::StoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CCCAppDetailsViewListBoxModel::StoreContact()
{
    return iStoreContact;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::GetContactTitleL
// --------------------------------------------------------------------------
//
HBufC* CCCAppDetailsViewListBoxModel::GetContactTitleL()
{

    HBufC* title = iAppServices->NameFormatter().GetContactTitleL(
        iStoreContact->Fields(),
        MPbk2ContactNameFormatter::EPreserveLeadingSpaces |
        MPbk2ContactNameFormatter::EUseSeparator);

    return title;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsFocusedFieldTypeL
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsFocusedFieldTypeL(TInt aSelectorResId)
{
    TBool ret = EFalse;

    MVPbkBaseContactField* field = FocusedFieldLC();

    if (field)
    {
        TPbk2StoreContactAnalyzer analyzer(
            iAppServices->ContactManager(), NULL);

        ret = analyzer.IsFieldTypeIncludedL(*field, aSelectorResId);

        CleanupStack::PopAndDestroy(field);
    }

    return ret;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HasContactFieldTypeL
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::HasContactFieldTypeL(TInt aSelectorResId)
{
    TBool ret = EFalse;

    TPbk2StoreContactAnalyzer analyzer(
        iAppServices->ContactManager(), iStoreContact);

    ret = analyzer.HasFieldL(aSelectorResId);

    return (ret != KErrNotFound);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsTopContact
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsTopContact() const
{
    return CVPbkTopContactManager::IsTopContact(*iStoreContact);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsReadOnlyContact
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsReadOnlyContact() const
{
    return iStoreContact->ParentStore().StoreProperties().ReadOnly();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsSimContact
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsSimContact() const
{
    TVPbkContactStoreUriPtr adnUri(VPbkContactStoreUris::SimGlobalAdnUri());
    TVPbkContactStoreUriPtr fdnUri(VPbkContactStoreUris::SimGlobalFdnUri());

    TVPbkContactStoreUriPtr uri =
        iStoreContact->ParentStore().StoreProperties().Uri();

    return (
        (uri.Compare(adnUri,
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
        ||
        (uri.Compare(fdnUri,
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
        );
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsSdnContact
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsSdnContact() const
{
    TVPbkContactStoreUriPtr sdnUri(VPbkContactStoreUris::SimGlobalSdnUri());

    TVPbkContactStoreUriPtr uri =
        iStoreContact->ParentStore().StoreProperties().Uri();

    return (uri.Compare(sdnUri,
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HasSpeedDialL
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::HasSpeedDialL(TInt aIndex)
{
    TBool hasAttribute = EFalse;

    if (aIndex != KErrNotFound)
    {
        const MVPbkStoreContactField& field =
            iStoreContact->Fields().FieldAt(aIndex);

        hasAttribute = iAppServices->ContactManager().ContactAttributeManagerL().
            HasFieldAttributeL(CVPbkSpeedDialAttribute::Uid(), field);
    }

    return hasAttribute;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HasDefaultsL
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::HasDefaultsL(TInt aIndex)
{
    TBool hasAttribute = EFalse;

    if (aIndex != KErrNotFound)
    {
        const MVPbkStoreContactField& field =
            iStoreContact->Fields().FieldAt(aIndex);

        hasAttribute = iAppServices->ContactManager().ContactAttributeManagerL().
            HasFieldAttributeL(CVPbkDefaultAttribute::Uid(), field);
    }

    return hasAttribute;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::RunL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::RunL()
{
    TInt index = 0;

    if (iLinks && iCurrentLink)
    {
        index = iLinks->Find(*iCurrentLink) + 1;
    }

    if (iAppServices && iLinks && index < iLinks->Count())
    {
        iCurrentLink = &iLinks->At(index);

        iRetrieveOperation = iAppServices->ContactManager().RetrieveContactL
            (*iCurrentLink, *this);
    }
    else
    {
        iCurrentLink = NULL;
    }
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::RunError
// ---------------------------------------------------------------------------
//
TInt CCCAppDetailsViewListBoxModel::RunError(TInt aError)
{
    return aError;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::DoCancel
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::DoCancel()
{
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ContactOperationCompleted(
    TContactOpResult /*aResult*/)
{
    if (iCommand == ECCAppDetailsViewCommandEdit)
    {
        // open editing dialog
        TRAP_IGNORE(ExecuteEditorL());

        iCommand = ECCAppDetailsViewCommandNone;
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ContactOperationFailed(
    TContactOp /*aOpCode*/,
    TInt /*aErrorCode*/,
    TBool /*aErrorNotified*/)
{
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ContactEditingComplete
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ContactEditingComplete(
    MVPbkStoreContact* aEditedContact)
{
    iStoreContact = aEditedContact;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ContactEditingDeletedContact
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ContactEditingDeletedContact(
    MVPbkStoreContact* aEditedContact)
{
    iStoreContact = aEditedContact;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ContactEditingAborted
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ContactEditingAborted()
{
    iIsEditingAborted = ETrue;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& /*aOperation*/,
    MVPbkStoreContact* aContact)
{
    CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewListBoxModel::VPbkSingleContactOperationComplete()"));

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    delete iStoreContact;
    iStoreContact = aContact;

    TRAP_IGNORE(
        CreatePresentationContactL();
                AddDataL();
                SetTitleL();
                UpdateMSKinCbaL();
        );

    // check if any item highlighted
    TBool highLightEnabled = iListBox.IsHighlightEnabled();
    if ( !highLightEnabled )
        {
        SetFocusedListIndex(iInitialFocusIndex);
        }

    /*if (!iXspIdModifyState)
     {
     iXspIdModifyState++;
     switch (iXspIdModifyState)
     {
     case 1:
     aContact->LockL(*this);
     break;
     default:
     // Continue, the xspid is set now
     IssueRequest();
     break;
     }
     }
     else*/
    {
        IssueRequest();
    }

    CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewListBoxModel::VPbkSingleContactOperationComplete()"));
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/,
    TInt /*aError*/)
{
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    IssueRequest();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CCCAppDetailsViewListBoxModel::MdcaCount() const
{
    return iRows.Count();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC CCCAppDetailsViewListBoxModel::MdcaPoint(TInt aIndex) const
{
    const TText KSeparator = '\t';
    TPtr rowText(iLineBuf->Des());
    rowText.Zero();

    const CCCAppDetailsViewListBoxRow& row = *iRows[aIndex];
    const TInt columnCount = row.ColumnCount();
    TInt fieldCount = iPresentationContact->PresentationFields().FieldCount();

    for (TInt index = 0; index < columnCount; ++index)
    {
        TPtr columnText(iColumnBuf->Des());
        columnText.Copy(row.At(index));

        // Clip the column if required
        if (index == EContentColumn && row.IsClipRequired() && aIndex < fieldCount)
        {
            ClipFromBeginning(columnText, aIndex, index);
        }

        // Append the column and separator to the formatted row
        rowText.Append(columnText);
        rowText.Append(KSeparator);
    }

    return rowText;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IssueRequest
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::IssueRequest(TInt aError)
{
    if (IsActive())
    {
        return;
    }
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aError);
    SetActive();
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::CreatePresentationContactL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::CreatePresentationContactL()
{
    delete iSpecificFieldProperties;
    iSpecificFieldProperties = NULL;
    delete iFieldProperties;
    iFieldProperties = NULL;

    RFs& fsSession(iCoeEnv.FsSession());

    const MVPbkContactStoreProperties& storeProperties =
        iStoreContact->ParentStore().StoreProperties();
    const MVPbkFieldTypeList& supportedFieldTypes =
        storeProperties.SupportedFields();

    CPbk2ServiceManager& servMan = iAppServices->ServiceManager();

    iFieldProperties =
        CPbk2FieldPropertyArray::NewL(supportedFieldTypes, fsSession, &servMan);

    // Create a field property list of the supported
    // field types of the used store
    CPbk2StorePropertyArray& pbk2StoreProperties = iAppServices->StoreProperties();

    iSpecificFieldProperties = CPbk2StoreSpecificFieldPropertyArray::NewL(
        *iFieldProperties,
        pbk2StoreProperties,
        supportedFieldTypes,
        iStoreContact->ParentStore());

    delete iPresentationContact;
    iPresentationContact = NULL;

    iPresentationContact =
        CPbk2PresentationContact::NewL(*iStoreContact, *iSpecificFieldProperties);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::AddDataL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::AddDataL()
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewListBoxModel::AddDataL()"));

    iRows.ResetAndDestroy();
    iInxToPresentationIdx.Close();

    SetEmptyTextL(R_QTN_PHOB_NO_ITEMS); // in pbk2uicontrols.rsg

    TInt fieldCount = iPresentationContact->PresentationFields().FieldCount();

    TBuf<KBufferSize> buffer;

    TVPbkFieldTypeParameters fieldParams;
    TInt listIdx = -1;

    for (TInt index = 0; index < fieldCount; index++)
    {
        CPbk2PresentationContactField& field =
            iPresentationContact->PresentationFields().At(index);

        // Get master field type list and match field's type against it
        const MVPbkFieldType* fieldType = VPbkUtils::MatchFieldType(
            iAppServices->ContactManager().FieldTypes(), field);

        // TODO: what about field.IsVisibleInDetailsView() ?

        if (field.IsEditable() && fieldType && !IsHiddenField(fieldType))
        {
            TInt labelResId = 0;
            TVPbkFieldTypeName fieldTypeName = EVPbkVersitNameNone;
            TPbk2FieldGroupId groupId = EPbk2FieldGroupIdNone;
            TArray<TVPbkFieldVersitProperty> versitArr = fieldType->VersitProperties();

            if (versitArr.Count())
            {
                const TVPbkFieldVersitProperty& prop = versitArr[0];

                if (prop.Name() == EVPbkVersitNameADR)
                {
                    if (versitArr[0].Parameters().Contains(EVPbkVersitParamHOME))
                    {
                        if (fieldParams.Contains(EVPbkVersitParamHOME))
                        {
                            continue;
                        }
                        fieldParams.Add(EVPbkVersitParamHOME);
                        labelResId = R_QTN_PHOB_LBL_ADDRESS_HOME;
                        groupId = EPbk2FieldGroupIdHomeAddress;
                    }
                    else if (versitArr[0].Parameters().Contains(EVPbkVersitParamWORK))
                    {
                        if (fieldParams.Contains(EVPbkVersitParamWORK))
                        {
                            continue;
                        }
                        fieldParams.Add(EVPbkVersitParamWORK);
                        labelResId = R_QTN_PHOB_LBL_ADDRESS_WORK;
                        groupId = EPbk2FieldGroupIdCompanyAddress;
                    }
                    else
                    {
                        if (fieldParams.Contains(EVPbkVersitParamPREF))
                        {
                            continue;
                        }
                        fieldParams.Add(EVPbkVersitParamPREF);
                        labelResId = R_QTN_PHOB_LBL_ADDRESS;
                        groupId = EPbk2FieldGroupIdPostalAddress;
                    }
                    fieldTypeName = EVPbkVersitNameADR;
                }
            }

            if (fieldTypeName == EVPbkVersitNameADR)
            {
                HBufC* label = StringLoader::LoadLC(labelResId);
                CCCAppDetailsViewListBoxRow* row = CCCAppDetailsViewListBoxRow::NewL();
                CleanupStack::PushL(row);

                // add icon.
                buffer.Zero();
                TInt iconIndex = 0;
                if (IsAddressValidated(groupId))
                {
                    TPbk2IconId iconId(TUid::Uid(KPbk2UID3), EPbk2qgn_prop_locev_map);
                    iconIndex = iIconArray.FindIcon(iconId);
                    buffer.AppendNum(iconIndex);
                }
                else
                {
                    TPbk2IconId iconId(TUid::Uid(KPbk2UID3), EPbk2qgn_prop_pb_no_valid_lm);
                    iconIndex = iIconArray.FindIcon(iconId);
                    buffer.AppendNum(iconIndex);
                }
                row->AppendColumnL(buffer);

                // add label.
                row->AppendColumnL(*label);

                RBuf text;
                Pbk2AddressTools::GetAddressPreviewLC(*iStoreContact, groupId, text);

                row->AppendColumnL(text);
                CleanupStack::PopAndDestroy(&text);

                // Expand row formatting buffer if required
                ExpandBuffersL(row);

                // Add the row.
                iRows.AppendL(row);
                iInxToPresentationIdx.InsertL(++listIdx, index);
                CleanupStack::Pop(row);
                CleanupStack::PopAndDestroy(label);
            }
            else
            {
                HBufC* label = GetLabelLC(field);

                if (label->Length())
                {
                    TBool skipThisData(EFalse);   
                    CCCAppDetailsViewListBoxRow* row = CCCAppDetailsViewListBoxRow::NewL();
                    CleanupStack::PushL(row);

                    const MVPbkContactFieldData& fieldData = field.FieldData();

                    // add icon.
                    buffer.Zero();
                    TInt iconIndex = iIconArray.FindIcon(field.FieldProperty().IconId());
                    buffer.AppendNum(iconIndex);
                    row->AppendColumnL(buffer);

                    // add label.
                    row->AppendColumnL(*label);

                    // Add current index to Presentation Contact index array 
                    // to estimate the text is needed to clip or not.
                    if(!skipThisData) 
                        {
                        iInxToPresentationIdx.InsertL(++listIdx, index);
                        }


                    // add field content.
                    switch (fieldData.DataType())
                    {
                    case EVPbkFieldStorageTypeText:
                        {
                        TBool defaultTextSet = HandleTextTypeFieldL(
                            index,
                            fieldData,
                            row,
                            fieldType->FieldTypeResId());
                        //Don't show default ringtone
                        if (defaultTextSet && 
                            fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_RINGTONE)
                            {
                            skipThisData = ETrue;
                            }
                        }
                        break;

                    case EVPbkFieldStorageTypeDateTime:
                        HandleDateTimeTypeFieldL(fieldData, row);
                        break;

                    case EVPbkFieldStorageTypeUri:
                        HandleUriTypeFieldL(index, fieldData, row);
                        break;

                    default:
                        break;
                    }

                    AddIconsToEndL(index, row);

                    // Expand row formatting buffer if required
                    ExpandBuffersL(row);

                    // Add the row if data ok to show to user.
                    if(!skipThisData) 
                        {
                        iRows.AppendL(row);
                        CleanupStack::Pop(row);
                        }
                    else
                        {
                        CleanupStack::PopAndDestroy(row);
                        }
                }

                CleanupStack::PopAndDestroy(label);
            }
        }
    } // for

    iListBox.HandleItemAdditionL();

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewListBoxModel::AddDataL()"));
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsAddressValidated
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsAddressValidated(TInt aGroupId)
{
    CPbk2PresentationContactFieldCollection& fields =
        iPresentationContact->PresentationFields();

    for (TInt idx = 0; idx < fields.FieldCount(); idx++)
    {
        MVPbkStoreContactField& field = fields.FieldAt(idx);
        TInt countProps =
            field.BestMatchingFieldType()->VersitProperties().Count();
        TArray<TVPbkFieldVersitProperty> props =
            field.BestMatchingFieldType()->VersitProperties();

        for (TInt idx2 = 0; idx2 < countProps; idx2++)
        {
            if (props[idx2].Name() == EVPbkVersitNameGEO &&
                ((props[idx2].Parameters().Contains(EVPbkVersitParamHOME) &&
                    aGroupId == EPbk2FieldGroupIdHomeAddress) ||
                    (props[idx2].Parameters().Contains(EVPbkVersitParamWORK) &&
                        aGroupId == EPbk2FieldGroupIdCompanyAddress) ||
                    (!props[idx2].Parameters().Contains(EVPbkVersitParamHOME) &&
                        !props[idx2].Parameters().Contains(EVPbkVersitParamWORK) &&
                        aGroupId == EPbk2FieldGroupIdPostalAddress)))
            {
                return ETrue;
            }
        }
    }
    return EFalse;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsHiddenField
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsHiddenField(
    const MVPbkFieldType* aFieldType)
{
    TInt resId = aFieldType->FieldTypeResId();

    return (resId == R_VPBK_FIELD_TYPE_SYNCCLASS);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::FieldAtLC
// ---------------------------------------------------------------------------
//
MVPbkBaseContactField* CCCAppDetailsViewListBoxModel::FieldAtLC(TInt aIndex)
{
    TInt *presentationIdx = iInxToPresentationIdx.Find(aIndex);

    if (!presentationIdx)
    {
        return NULL;
    }

    TInt index = iPresentationContact->PresentationFields().StoreIndexOfField(
        *presentationIdx);

    if (index != KErrNotFound)
    {
        // Use FieldAtLC to avoid the unvalidity of the field after new
        // FieldAt call.
        return iStoreContact->Fields().FieldAtLC(index);
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsFieldTypeL
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsFieldTypeL(
    TInt aIndex,
    TInt aSelectorResId)
{
    TBool ret = EFalse;

    MVPbkBaseContactField* field = FieldAtLC(aIndex);

    if (field)
    {
        TPbk2StoreContactAnalyzer analyzer(iAppServices->ContactManager(), NULL);

        ret = analyzer.IsFieldTypeIncludedL(*field, aSelectorResId);

        CleanupStack::PopAndDestroy(field);
    }

    return ret;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HandleTextTypeFieldL
// ---------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::HandleTextTypeFieldL(
    TInt aIndex,
    const MVPbkContactFieldData& aFieldData,
    CCCAppDetailsViewListBoxRow* aRow,
    TInt aResId)
{
    TBool defaultTextInserted(EFalse);
    const TDesC& fieldText =
        MVPbkContactFieldTextData::Cast(aFieldData).Text();

    if (aResId == R_VPBK_FIELD_TYPE_CALLEROBJIMG ||
        aResId == R_VPBK_FIELD_TYPE_RINGTONE)
    {
        RFs& fsSession(iCoeEnv.FsSession());

        if (FileExists(fsSession, fieldText))
        {
            // parse path and show only filename.
            HBufC* textBuf = HBufC::NewLC(fieldText.Length());
            TPtr text = textBuf->Des();
            text.Append(fieldText);

            TParsePtr parse(text);
            TPtrC fileName(parse.Name());

            aRow->AppendColumnL(fileName);

            CleanupStack::PopAndDestroy(textBuf);
        }
        else
        {
            defaultTextInserted = ETrue;
            TInt textResId = R_QTN_PHOB_FIELD_DEFAULT_TONE; // in pbk2uicontrols.rsg

            if (aResId == R_VPBK_FIELD_TYPE_CALLEROBJIMG)
            {
                textResId = R_QTN_PHOB_FIELD_THUMBNAIL; // in pbk2uicontrols.rsg
            }

            // cold filename (e.g mem card not present), show default text.
            HBufC* defaultText = iCoeEnv.AllocReadResourceLC(textResId);
            aRow->AppendColumnL(*defaultText);
            CleanupStack::PopAndDestroy(defaultText);
        }
    }
    else
    {
        TBool isNumberField  = IsFieldTypeL(aIndex, R_DETAILSVIEW_PHONENUMBER_SELECTOR);
        TBool isAddressField = IsFieldTypeL(aIndex, R_DETAILSVIEW_EL_ADDRESS_SELECTOR);

        if (isNumberField || isAddressField)
        {
            aRow->SetClipRequired(ETrue);
        }

        TPtr columnBuf(iColumnBuf->Des());

        columnBuf.Set(ExpandColumnBufferL(fieldText.Length()));
        columnBuf.Zero();

        // replace listbox separator characters.
        Pbk2PresentationUtils::AppendAndReplaceChars(
            columnBuf, fieldText, KCharsToReplace, KReplacementChars);

        // Replace characters that can not be displayed correctly.
        Pbk2PresentationUtils::ReplaceNonGraphicCharacters(columnBuf, ' ');

        if ( isNumberField )
            {
            AknTextUtils::LanguageSpecificNumberConversion( columnBuf );
            }
        aRow->AppendColumnL(columnBuf);
    }
    return defaultTextInserted;
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HandleDateTimeTypeFieldL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::HandleDateTimeTypeFieldL(
    const MVPbkContactFieldData& aFieldData,
    CCCAppDetailsViewListBoxRow* aRow)
{
    TLocale locale;
    TBuf<64> dateBuffer;

    const MVPbkContactFieldDateTimeData& date =
        MVPbkContactFieldDateTimeData::Cast(aFieldData);

    TTime time(date.DateTime());

    HBufC* dateFormat = iCoeEnv.AllocReadResourceLC(
        R_QTN_DATE_USUAL_WITH_ZERO); // in avkon.rsg
    time.FormatL(dateBuffer, *dateFormat, locale);
    CleanupStack::PopAndDestroy(dateFormat);

    AknTextUtils::LanguageSpecificNumberConversion( dateBuffer );
    aRow->AppendColumnL(dateBuffer);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HandleUriTypeFieldL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::HandleUriTypeFieldL(
    TInt aIndex,
    const MVPbkContactFieldData& aFieldData,
    CCCAppDetailsViewListBoxRow* aRow)
{
    TBool isXspField = IsFieldTypeL( aIndex, R_DETAILSVIEW_EL_ADDRESS_SELECTOR );
    if ( isXspField )
        {
        aRow->SetClipRequired( ETrue );
        }
    
    const MVPbkContactFieldUriData& uri =
        MVPbkContactFieldUriData::Cast(aFieldData);

    HBufC* tempBuf = uri.Text().AllocLC();
    TPtr text = tempBuf->Des();
    // Replace characters that can not be displayed correctly.
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters( text, KReplacedChars );
    aRow->AppendColumnL(text);
    CleanupStack::PopAndDestroy(tempBuf);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::AddIconsToEndL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::AddIconsToEndL(
    TInt aIndex,
    CCCAppDetailsViewListBoxRow* aRow)
{
    TBuf<KBufferSize> buffer;
    TInt iconIndex = 0;
    TInt storeIndex = iPresentationContact->PresentationFields().
        StoreIndexOfField(aIndex);

    if (HasDefaultsL(storeIndex))
    {
        buffer.Zero();
        TPbk2IconId iconId(
            TUid::Uid(KCCADetailsViewPluginImplmentationUid),
            ECCAppDetailsViewQgn_indi_default_conn_add );
        iconIndex = iIconArray.FindIcon(iconId);
        buffer.AppendNum(iconIndex);
        aRow->AppendColumnL(buffer);
    }

    // Check and add speed dial icon
    MVPbkStoreContactField* storefield =
        iStoreContact->Fields().FieldAtLC(storeIndex);

    if (storefield)
    {
        MVPbkStoreContactField2* fieldext =
            reinterpret_cast<MVPbkStoreContactField2*>
            (storefield->StoreContactFieldExtension
                (KMVPbkStoreContactFieldExtension2Uid));

        if (fieldext)
        {
            CArrayFix<TInt>* array = fieldext->SpeedDialIndexesL();

            if (array)
            {
                CleanupStack::PushL(array);

                buffer.Zero();

                // find speed dial key (1 to 9) and set correct icon
                TInt key = 0;
                TInt count = array->Count();
                if ( count >0 )
                {
                    key = (*array)[0];
                    // Loop from end of array to find smallest key
                    // if more than one has been set to same field.
                    for ( TInt i = 1; i < count; i++ )
                    {
                        if ( (*array)[i] < key )
                        {
                            key = (*array)[i];
                        }
                    }
                }

                if (key > 0 && key <= 9)
                {
                    // icon id ECCAppDetailsViewQgn_indi_qdial_one_add = 1
                    // so icons from one to nine maps to key value.
                    TPbk2IconId iconId(
                        TUid::Uid(KCCADetailsViewPluginImplmentationUid), key);
                    iconIndex = iIconArray.FindIcon(iconId);

                    buffer.AppendNum(iconIndex);
                    aRow->AppendColumnL(buffer);
                }

                CleanupStack::PopAndDestroy(array);
            }
        }

        CleanupStack::PopAndDestroy(storefield);
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ClipFromBeginning
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::ClipFromBeginning(
    TDes& aBuffer,
    TInt aItemIndex,
    TInt aSubCellNumber) const
{
    CAknFormDoubleGraphicStyleListBox* listbox =
        static_cast<CAknFormDoubleGraphicStyleListBox*>(&iListBox);

    return AknTextUtils::ClipToFit(
        aBuffer,
        AknTextUtils::EClipFromBeginning,
        listbox,
        aItemIndex,
        aSubCellNumber);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ExpandBuffersL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ExpandBuffersL(
    CCCAppDetailsViewListBoxRow* aRow)
{
    // Row formatting buffer
    const TInt rowLength = aRow->TotalLength() + aRow->ColumnCount();  // for separator characters

    if (rowLength > iLineBuf->Des().MaxLength())
    {
        iLineBuf = iLineBuf->ReAllocL(rowLength);
    }

    ExpandColumnBufferL(aRow->MaxColumnLength());
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ExpandColumnBufferL
// --------------------------------------------------------------------------
//
TPtr CCCAppDetailsViewListBoxModel::ExpandColumnBufferL(TInt aRequiredLength)
{
    if (aRequiredLength > iColumnBuf->Des().MaxLength())
    {
        iColumnBuf = iColumnBuf->ReAllocL(aRequiredLength);
    }

    return (iColumnBuf->Des());
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::ExecuteEditorL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::ExecuteEditorL()
{
    // verify syncronization field existance and content
    /*
     VPbkUtils::VerifySyncronizationFieldL(
     CCoeEnv::Static()->FsSession(),
     iStoreContact->ParentStore().StoreProperties().SupportedFields(),
     *iStoreContact);
     */

    // adjust focus
    TPbk2ContactEditorParams params;

    params.iFocusedContactField = FocusedFieldLC();

    // create and execute editing dialog
    CPbk2ContactEditorDlg* dlg =
        CPbk2ContactEditorDlg::NewL(params, iStoreContact, *this);

    iStoreContact = NULL; // ownership went to editor
    dlg->ExecuteLD();

    // clean after dialog execution
    if (params.iFocusedContactField)
    {
        CleanupStack::PopAndDestroy(params.iFocusedContactField);
    }

    // If editing has been aborted we don't set focused contact.
    if (iStoreContact && !iIsEditingAborted)
    {
        // It is possible that the contact got changed while in edit
        //iUiControl->SetFocusedFieldIndex(params.iFocusedIndex);
        //iUiControl->SetFocusedContactL(*iStoreContact);
    }

    // If contact got deleted in the editor, this call ensures
    // that the contact info view is switched to the names list
    //iUiControl->UpdateAfterCommandExecution();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::GetLabelLC
// --------------------------------------------------------------------------
//
HBufC* CCCAppDetailsViewListBoxModel::GetLabelLC(
    const CPbk2PresentationContactField& aField)
{
    TPtr columnBuf(iColumnBuf->Des());
    columnBuf.Set(ExpandColumnBufferL(aField.FieldLabel().Length()));
    columnBuf.Zero();

    // replace listbox separator characters.
    Pbk2PresentationUtils::AppendAndReplaceChars(
        columnBuf, aField.FieldLabel(), KCharsToReplace, KReplacementChars);

    // Replace characters that can not be displayed correctly
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters(columnBuf, ' ');

    HBufC* data = HBufC::NewLC(columnBuf.Length());
    data->Des().Append(columnBuf);

    return data;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::SetEmptyTextL
// Sets empty text for list box from given resource id.
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::SetEmptyTextL(TInt aResourceId)
{
    HBufC* text = iCoeEnv.AllocReadResourceLC(aResourceId);
    iListBox.View()->SetListEmptyTextL(*text);
    CleanupStack::PopAndDestroy(text);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::SetTitleL
// Sets empty text for list box from given resource id.
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::SetTitleL()
{
    HBufC* title = GetContactTitleL();
    CleanupStack::PushL(title);
    iPlugin.SetTitleL(*title);
    CleanupStack::PopAndDestroy();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsContactEditable
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsContactEditable()
{
    return !(IsSdnContact() || IsReadOnlyContact());
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::UpdateMSKinCbaL
// Sets empty MSK or MSK with text "Edit" when contact is editable.
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::UpdateMSKinCbaL()
{
    iPlugin.UpdateMSKinCbaL(IsContactEditable());
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HandleEditRequestL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::HandleEditRequestL()
{
    if (iStoreContact != NULL && IsContactEditable())
    {
        iPlugin.HandleEditRequestL();
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::HandleDeleteRequestL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewListBoxModel::HandleDeleteRequestL()
{
    if ( iStoreContact != NULL && IsContactEditable() )
    {
        iPlugin.HandleDeleteRequestL();
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewListBoxModel::IsFieldMapped
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewListBoxModel::IsFieldMapped() const
{
    const TInt *index = iInxToPresentationIdx.Find(iListBox.CurrentItemIndex());
    if ( index && (*index >= KErrNone) && ( *index != iListBox.CurrentItemIndex()))
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
}

// End of File
