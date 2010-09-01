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
* Description: 
*     Phonebook 2 search pane filter. This acts as a wrapper to the FindBox in
*     NameslistView. Info needs to be passed to this filter before the FindBox
*
*/

// INCLUDE FILES
#include "CPbk2PredictiveSearchFilter.h"

// Phonebook 2
#include <Phonebook2PrivateCRKeys.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactNameFormatter2.h>
#include <MVPbkViewContact.h>
#include "MPbk2FilteredViewStack.h"
#include "cpbk2predictiveviewstack.h"
#include "TPbk2FindTextUtil.h"
// Debugging headers
#include <Pbk2Debug.h>

//Avkon System includes
#include <aknsfld.h>
#include <txtetext.h>
#include <eikenv.h>
#include <eikappui.h>
#include <EIKCOCTL.rsg>
#include <eikedwin.h>
#include <featmgr.h>
#include <AknFepInternalCRKeys.h>
#include <txtglobl.h>

//PS
#include <CPsRequestHandler.h>


/// Unnamed namespace for local definitions
namespace {

_LIT(KQuestionMark, "?");
_LIT(KspaceStr, " ");
const TInt Kspace = ' ';
const TInt Kquestion = '?';
const TInt KDefaultListFormatting =
        MPbk2ContactNameFormatter::EUseSeparator |
        MPbk2ContactNameFormatter::EPreserveLeadingSpaces;

} /// namespace

NONSHARABLE_CLASS( CRunAsynch ): public CActive
    {
    public:
        static CRunAsynch* NewL(CPbk2PredictiveSearchFilter* aParent);
        void ReDraw();
        
        ~CRunAsynch();
    private: 
        CRunAsynch(CPbk2PredictiveSearchFilter* aParent);
        
        void RunL();
        void DoCancel();
        
    private: //Data
        CPbk2PredictiveSearchFilter* iParent;
    };

// --------------------------------------------------------------------------
// CRunAsynch::CRunAsynch
// --------------------------------------------------------------------------
//
CRunAsynch::CRunAsynch(CPbk2PredictiveSearchFilter* aParent) :
    CActive(EPriorityIdle), 
    iParent(aParent) 
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CRunAsynch::~CRunAsynch
// --------------------------------------------------------------------------
//
inline CRunAsynch::~CRunAsynch()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CRunAsynch::NewL
// --------------------------------------------------------------------------
//
CRunAsynch* CRunAsynch::NewL(CPbk2PredictiveSearchFilter* aParent)
    {
    CRunAsynch* self = new ( ELeave ) CRunAsynch(aParent);
    return self;
    }

// --------------------------------------------------------------------------
// CRunAsynch::Redraw
// --------------------------------------------------------------------------
//
void CRunAsynch::ReDraw()
    {
    if(!IsActive())
        {
        TRequestStatus* status=&iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CRunAsynch::DoCancel
// --------------------------------------------------------------------------
//
void CRunAsynch::DoCancel()
    {
    
    }

// --------------------------------------------------------------------------
// CRunAsynch::RunL
// --------------------------------------------------------------------------
//
void CRunAsynch::RunL()
    {
    iParent->ReDrawL();
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::CPbk2PredictiveSearchFilter
// --------------------------------------------------------------------------
//
inline CPbk2PredictiveSearchFilter::CPbk2PredictiveSearchFilter() :
    iFiltering(EFalse),
    iAppUi( *(iEikonEnv->EikAppUi()) ),
    iPredictiveSearchEnabled(EFalse)
    {
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::~CPbk2PredictiveSearchFilter
// --------------------------------------------------------------------------
//
CPbk2PredictiveSearchFilter::~CPbk2PredictiveSearchFilter()
    {    
    RemoveFromViewStack();    
    iPredictiveTextEntered.Close();    
    delete iSearchText;
    delete iRunAsynch;
    // UnInitialize feature manager
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::NewL
// --------------------------------------------------------------------------
//
CPbk2PredictiveSearchFilter* CPbk2PredictiveSearchFilter::NewL()
    {
    CPbk2PredictiveSearchFilter* self =
        new ( ELeave ) CPbk2PredictiveSearchFilter();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::ConstructL()
    {
    // Initialize feature manager
    FeatureManager::InitializeLibL();

    if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))
        {
        //The implementation algorithm suppose that only this class is
        //responsible to change or return PCS input mode therefore we can use this value
        CRepository* repository = CRepository::NewLC(TUid::Uid(KCRUidPhonebook));    
        repository->Get( KPhonebookPredictiveSearchEnablerFlags, iPredictiveSearchEnabled );      
        CleanupStack::PopAndDestroy(); //repository
        
        }
 
    iRunAsynch = CRunAsynch::NewL(this);
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::FilterL
// --------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::FilterL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType,
        CAknSearchField* aSearchField )
    {    
    
    if( aType == EEventKeyDown && aKeyEvent.iCode == EKeyNull )
        {        
        if( IsPredictiveActivated() && aSearchField )
            {
            iFiltering = ETrue;
            
            // Before single click it does nothing, but between multitap 
            // clicks it breaks Fep transaction.
            // It removes multitap. 
            RBuf searchText;
            searchText.CreateL( aSearchField->TextLength() );
            CleanupClosePushL( searchText );
            aSearchField->GetSearchText( searchText );       
            aSearchField->SetSearchTextL( searchText );
            aSearchField->SetFocus( ETrue, EDrawNow );
            CleanupStack::PopAndDestroy( &searchText );
            iFiltering = EFalse;  
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::IsFiltering
// --------------------------------------------------------------------------
//
TBool CPbk2PredictiveSearchFilter::IsFiltering()
    {
    return iFiltering;
    }

// --------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::StartFindPaneInlineEditL
// --------------------------------------------------------------------------
//
TBool CPbk2PredictiveSearchFilter::StartFindPaneInlineEditL( TDes& aFindPaneText )
    {
    //Level 1 : Check if virtual keyboard was used
    if( !iVKbUsed && iSearchField && iSearchText->Length() < aFindPaneText.Length() &&
         (iSearchField->Editor().AknEdwinFlags() & EAknEditorFlagTouchInputModeOpened) )
        {
        iVKbUsed = ETrue;
        }
    //Level 2 : Check if predictive serach is activated or not
    //if predictive serach is not activated, avoid performing
    //any operation on the same string in FindPane
    if ( !IsPredictiveActivated() )
        {
        return ETrue;
        }
    TPtr searchPtr (iSearchText->Des()); //Text which holds the FindPane display text    
    TInt findPaneTxtLen = aFindPaneText.Length(); 
        
    //Level 3: If search text filtering is ongoing, avoid performing
    //any operation on the same string in FindPane 
    if (iFiltering )
        {
        return EFalse;
        }    
    
    //Level 4: HandleControlEventL wil be called more than once, avoid performing
    //any operation on the same string in FindPane 
    if (aFindPaneText.Compare(searchPtr) == 0)//HandleControlEventL wil be called more than once, avoid performing
                                          //any operation on the same string in FindPane
        {            
        return EFalse;
        }
    
    //Level 5: If editor is locked, avoid performing
    //any operation on the same string in FindPane 
    if ( iLocked ) 
        { 
        if( aFindPaneText.Length() > 1 && aFindPaneText[aFindPaneText.Length() - 2] == Kquestion )
            {
            TBuf <KSearchFieldLength> findPaneTxtCopy;
            findPaneTxtCopy.SetLength(0);            
            findPaneTxtCopy.Copy(aFindPaneText);
            findPaneTxtCopy.Replace(aFindPaneText.Length() - 1, 1, KspaceStr);            
            SetSearchBoxTextL(findPaneTxtCopy, findPaneTxtCopy.Length() - 1);
            iRemoveSpace = ETrue;
            }
        return EFalse;
        }
    
    TBool isCharRemoved (EFalse); //To know whether its a backspace char or not. If backspace nothing needs to be done
    if ( findPaneTxtLen )
        {    
        TInt len = iPredictiveTextEntered.Length();
        //Append the newly entered char into our predictive search query string
        if ( findPaneTxtLen >= len )
            {
            if ( iLongKeyPress )
                {
                //If it was a long key press
                //Normally during Long Key Press, there are 2 actions that happen
                //first the char is displayed and then followed by that the number
                //eg. the numeric key pad "2" denotes "abc2...", when u long press 2, 
                //first the char 'a' is shown
                //and then followed by that it shows '2'. 
                //We get 2 events one wih 'a' and next with '2'
                //So, we need to send first 'a' to the predictive search
                //and then followed by this '2'.
                //So During this case, the Fep transaction is broken because of us
                //and thus the FEP will send the event with any of the strings
                //' 2' or '*2'(* - denotes the matched char for the 1stquery a)
                //If Long Key Press, we remove the previous char and use the new one
                //for our search, otherwise we might end up sending a search req for 
                //as 'a2' if that would have been a long key press on mumeric keypad 2.
                iPredictiveTextEntered.Delete( len - 1, 1 );
                iLongKeyPress = EFalse;
                }
            if ( findPaneTxtLen == len )
                {
                len = len - 1;
                isCharRemoved = ETrue;
                }
            //Append the newly entered char to our predictive search query. 
            iPredictiveTextEntered.Append( aFindPaneText.Right(findPaneTxtLen - len) );
            }
        else
            {
            //Some chars have been removed so delete the app char from our predictive search query            
            iPredictiveTextEntered.Delete( findPaneTxtLen, iPredictiveTextEntered.Length() - findPaneTxtLen );
            isCharRemoved = ETrue;
            }
        
        TBuf <KSearchFieldLength> findPaneTxtCopy;
        findPaneTxtCopy.SetLength(0);            
        findPaneTxtCopy.Copy(aFindPaneText);
                    
        //Check whether a new text was entered    
        //Ignore if " " has been entered
        if ( !isCharRemoved )
            {
            //New text was entered
            //Now we cannot render this text untill we find the match char from the Predictive search engine
            //Hence remove the newly entered char and diplay the findpane
            //once we get the callback from predictive search with the matched char
            //then we can render that char as the newly entered char
            findPaneTxtCopy.Replace(aFindPaneText.Length()-1, 1, KspaceStr);            
            SetSearchBoxTextL(findPaneTxtCopy, findPaneTxtCopy.Length());
            iLongKeyPress = ETrue;
            }
        
        searchPtr.Copy( findPaneTxtCopy );
        aFindPaneText.Copy( iPredictiveTextEntered );
        }
    else
        {
        ResetL();
        }
       
    return ETrue;
    }   
       
// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::IsPredictiveActivated
// ---------------------------------------------------------------------------
//
TBool CPbk2PredictiveSearchFilter::IsPredictiveActivated() const
    {    
    return iPredictiveSearchEnabled && !iVKbUsed;
    }


// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::HandlePSResults
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::HandlePSNoMatchL( const TDesC& /*aQueryString*/, const TDesC& aMatchSeq )
    {
    //we have received our Predictive Search Result
    //aMatchSeq contains the string that needs to be displayed on the FindPane
    //now we can render the string on the findPane
    if ( !IsPredictiveActivated() )
        {
        return;
        }
    
    CommitFindPaneTextL( aMatchSeq );
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::LookupL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::LookupL( 
        const MVPbkViewContact& aContact, CPbk2PredictiveViewStack& aView, 
        MPbk2ContactNameFormatter& aNameFormatter, TDes& aMatchSeqChr )
    {
    HBufC* name(NULL);
    if( FeatureManager::FeatureSupported(KFeatureIdFfContactsCompanyNames) )
    {
        MPbk2ContactNameFormatter2* nameformatterExtension =
            reinterpret_cast<MPbk2ContactNameFormatter2*>(aNameFormatter.
                    ContactNameFormatterExtension( MPbk2ContactNameFormatterExtension2Uid ) );
        name = nameformatterExtension->GetContactTitleWithCompanyNameL(aContact.Fields(),
                KDefaultListFormatting);
         }
     else
         {
         // Append name
         name =
         aNameFormatter.GetContactTitleL(aContact.Fields(),
             KDefaultListFormatting);
         }
    CleanupStack::PushL( name );         
              
    TPtr namePtr = name->Des();
    TRAP_IGNORE (
        if ( IsMirroredInputLang() )
            {
            // temporary solution
            // PCS LookupLmethod doesn't work correctly for
            // bidirectional languages
            RArray<TPsMatchLocation> searchMatches; 
            
            aView.GetMatchingPartsL(*name, searchMatches);
            
            TInt matchCount = searchMatches.Count();
            if( matchCount )
                {
                for (TInt index1=0; index1<matchCount; index1++)
                    {
                    TPsMatchLocation& location = searchMatches[index1];                    
                    aMatchSeqChr.Append(namePtr.Mid(location.index, location.length));
                    aMatchSeqChr.Append(Kspace);
                    }
                aMatchSeqChr.LowerCase();
                aMatchSeqChr.Trim();
                }
            searchMatches.Close();
            }
        else
            {
            CPsQuery* lastQuery = const_cast<CPsQuery*> (aView.LastPCSQuery());
            CPSRequestHandler* psHandler = aView.PSHandler();
            if ( psHandler && lastQuery )
                {
                psHandler->LookupMatchL( *lastQuery, *name, aMatchSeqChr );
                }
            }
       );
        
    CleanupStack::PopAndDestroy( name );
    }
            
// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::CommitFindPaneTextL
// ---------------------------------------------------------------------------
//
TInt CPbk2PredictiveSearchFilter::CommitFindPaneTextL( MPbk2FilteredViewStack& aViewStack,
                                MPbk2ContactNameFormatter& aNameFormatter )
    {    
    //aViewStack contains list of the items that is currently shown in
    //the nameslist. 
    //The first item's matched characters must be shown in the Findpane    
    if ( !IsPredictiveActivated() )
        {
        return KErrNotFound;
        }    
    
    RBuf copyPredictiveTextEntered;
    CleanupClosePushL(copyPredictiveTextEntered);
    copyPredictiveTextEntered.CreateL(iPredictiveTextEntered);
    copyPredictiveTextEntered.Trim();
    TBool allCharsAreSpaces = (copyPredictiveTextEntered.Length() == 0) ? ETrue : EFalse;
    CleanupStack::PopAndDestroy(&copyPredictiveTextEntered);
    if( allCharsAreSpaces )
        {
        ReDrawL();
        return KErrNotFound;
        }
    
    TBuf<KSearchFieldLength> matchSeqChr; 
    CPbk2PredictiveViewStack& view = static_cast
                    <CPbk2PredictiveViewStack&>( aViewStack );
    TInt contactCount = aViewStack.ContactCountL();
    TInt matchedItemIndex = KErrNotFound;
    if ( &view && contactCount )
        {
   
        //Proceed, only if there are any contacts visible in the view
        TBool found = EFalse;
        CPsQuery* lastQuery = const_cast<CPsQuery*> (view.LastPCSQuery()); 
        for ( TInt index = 0; index<contactCount && lastQuery && !found; index++ )
            {
            //Iterate untill we find a match to our predictive search query      
            if ( view.IsNonMatchingMarkedContact( index ) )
                {
                //Avoid any mess with the marked contacts that are not part of our
                //search result
                continue;
                }
            
            //Get the item from the view stack
            const MVPbkViewContact& contact = aViewStack.ContactAtL( index );
            LookupL( contact, view, aNameFormatter, matchSeqChr );
            found = matchSeqChr.Length();
            if ( found )
                {
                matchedItemIndex = index;
                }
            }
        }
    
    CommitFindPaneTextL( matchSeqChr );
    return matchedItemIndex;
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::CommitFindPaneTextL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::CommitFindPaneTextL( MPbk2FilteredViewStack& aViewStack,
                                MPbk2ContactNameFormatter& aNameFormatter, const TInt aIndex )
    {
    CPbk2PredictiveViewStack& view = static_cast
                     <CPbk2PredictiveViewStack&>( aViewStack );
    const MVPbkViewContact& contact = aViewStack.ContactAtL( aIndex );
    TBuf<KSearchFieldLength> matchSeqChr; 
    LookupL( contact, view, aNameFormatter, matchSeqChr );
    CommitFindPaneTextL( matchSeqChr );
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::CommitFindPaneTextL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::CommitFindPaneTextL( const TDesC& aMatchSeq )
    {
    if ( !iSearchText )
        {
        return;
        }
    
    TPtr searchPtr (iSearchText->Des()); //Text with which previous search was initiated    
    TInt len = iPredictiveTextEntered.Length();
    TInt matchSeqLen = aMatchSeq.Length();
    TBuf <KSearchFieldLength> searchTextBuffer;    
    searchTextBuffer.Copy( searchPtr.Left( len ) );
    
    //if the last character was not a space  
    if ( len )            
        {
        if (matchSeqLen)
            {
            TInt matchSeqIndex( 0 );
            for ( TInt index = 0; index < len; index++ )
                {
                if ( (matchSeqIndex < matchSeqLen) && (aMatchSeq[matchSeqIndex] == Kspace) )
                    {
                    ++matchSeqIndex;
                    }
                if ( ( iPredictiveTextEntered[index] != Kspace ) 
                                && ( matchSeqIndex < matchSeqLen ) )
                    {
                    searchTextBuffer[index] = aMatchSeq[matchSeqIndex++];
                    }
                }
            
            iLocked = EFalse;          
            }
        else
            {                     
            for (TInt index=0; index<len; index++)
                {
                if ( searchTextBuffer[index] == Kspace )
                    {
                    searchTextBuffer[index] = iPredictiveTextEntered[index];
                    }
                }
            //there is no item to match this predictive serach query
            //hence replace the new character with ? and
            //lock the Findpane filter
            searchTextBuffer.Replace(
                    searchTextBuffer.Length()-1, 
                    KQuestionMark().Length(), 
                    KQuestionMark);
            iLocked = ETrue;    
            }

        SetSearchBoxTextL(searchTextBuffer, searchTextBuffer.Length());
        
        if (iLocked)
            {
            //Push the filter to the Top of the View Stack
            //AddToViewStackL will be run in OfferKeyEventL because it breaks FEP transaction
            iAddToViewStack = ETrue;
            }
        else
            {
            //Remove the filter from the Top of the View Stack
            RemoveFromViewStack();
            }
        iFiltering = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::IsLocked
// ---------------------------------------------------------------------------
//
TBool CPbk2PredictiveSearchFilter::IsLocked() const
    {
    TBool locked (EFalse);
    if ( IsPredictiveActivated() )
        {
        locked = iLocked;
        }
    return locked;
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CPbk2PredictiveSearchFilter::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse keyConsume( EKeyWasNotConsumed ); 
    iEditorMode = ENone;
    
    if (IsPredictiveActivated())
        {
        if( iAddToViewStack )
            {
			//Push the filter to the Top of the View Stack
 		    //run delayed AddToViewStackL
            AddToViewStackL();
            iAddToViewStack = EFalse;
            }
        TChar ch(aKeyEvent.iScanCode);
        //If the FindPane filter is locked, discard all
        //other keys here. Since the filter is of high priority
        //than the FEP, FEP will not get the keyevent if consumed
        //by the findpane filter
        if (iLocked && ch.IsAlphaDigit())
            {
            keyConsume = EKeyWasConsumed;
            }
        else
            {
            if ( aType == EEventKeyDown )
                {
                iLongKeyPress = EFalse;
                }
            TInt code=aKeyEvent.iCode;
            const TInt modifiers=aKeyEvent.iModifiers;            
            TBool magnify=modifiers&EModifierCtrl;            
            
            if ( code == EKeyBackspace)
                {
                //Release the findPane lock
                iLocked = EFalse;
                if( iSearchText && iRemoveSpace )
                    {
                    SetSearchBoxTextL(*iSearchText, iSearchText->Length());
                    iRemoveSpace = EFalse;
                    }
                }
            
            //This Checks whether a Paste Operation was performed
            //on the findpane or not
            if (magnify && ( code<100 && code!=' ' ) ) ////!!! magic number
                {        
                TBuf<KSearchFieldLength> buf;        
                iCoeEnv->ReadResourceL(buf,R_EIK_EDWIN_CTRL_HOTKEYS);
                TInt ret=buf.Locate(TChar(code+'a'-1));        
                if ( ret == CEikEdwin::EHotKeyPaste )
                    {
                    iEditorMode = EPaste;
                    }
                }
            }
        }        
    return keyConsume;
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::ResetL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::ResetL()
    {
    iVKbUsed = EFalse;
    RemoveFromViewStack();    
    
    delete iSearchText;
    iSearchText = NULL;    
            
    iSearchText = HBufC::NewL( KSearchFieldLength );
    iFiltering = EFalse;
    iLocked = EFalse;
    iLongKeyPress = EFalse;
    iPredictiveTextEntered.Close();
    iPredictiveTextEntered.CreateL( KSearchFieldLength );
    }
    
// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::FindPaneResetL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::FindPaneResetL()
    {
    if ( iSearchField )
        {
        iSearchField->ResetL();
        }
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::GetSearchTextL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::GetSearchTextL( RBuf& aSearchText )
    {
    //Reset
    aSearchText.Close();
    
    if (IsPredictiveActivated())
        {
        aSearchText.CreateL( iPredictiveTextEntered.Length() );
        aSearchText.Copy( iPredictiveTextEntered );        
        }
    else
        {
        aSearchText.CreateL ( iSearchField->TextLength() );
        iSearchField->GetSearchText( aSearchText );        
        }    
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::SetAknSearchFieldL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::SetAknSearchFieldL( CAknSearchField* aSearchField )
    {
    iSearchField = aSearchField;
    iPredictiveTextEntered.CreateL( KSearchFieldLength );
    iSearchText = HBufC::NewL( KSearchFieldLength );
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::IsPasteMode
// ---------------------------------------------------------------------------
//
TBool CPbk2PredictiveSearchFilter::IsPasteMode()
    {
    return (iEditorMode == EPaste) ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::AddToViewStackL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::AddToViewStackL()
    {
    if ( !iViewStack )
        {
        iViewStack = ETrue;
        iAppUi.AddToStackL(this, ECoeStackPriorityEnvironmentFilter, ECoeStackFlagRefusesFocus|ECoeStackFlagSharable);
        }
    }


// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::RemoveFromViewStack
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::RemoveFromViewStack()
    {
    if ( iViewStack )
        {
        iViewStack = EFalse;
        iAppUi.RemoveFromStack(this);
        }
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::HandleForegroundEventL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::HandleForegroundEventL(TBool aForeground)
    {
    if ( aForeground && iLocked )
        {
        AddToViewStackL();
        }
    else
        {
        RemoveFromViewStack();
        }
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::FindPaneTextL
// ---------------------------------------------------------------------------
//
HBufC* CPbk2PredictiveSearchFilter::FindPaneTextL()
    {
    HBufC* findTextBuf( NULL );
    if ( iSearchField )
        {
        TPbk2FindTextUtil::EnsureFindTextBufSizeL( 
                *iSearchField,
                &findTextBuf );
        TPtr bufPtr = findTextBuf->Des();
        iSearchField->GetSearchText(bufPtr);
        if ( iLocked )
            {
            TInt index = bufPtr.LocateReverse( Kquestion );
            if ( ( KErrNotFound != index )  && ( index == bufPtr.Length() - 1 ) )
                {
                bufPtr.Delete( index, 1 );
                }
            }        
        }

    return findTextBuf;
    }


// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::IsMirroredInputLang
// ---------------------------------------------------------------------------
//
TBool CPbk2PredictiveSearchFilter::IsMirroredInputLang()
    {
    // Check if we have mirrored input
    // It is possible to have mixed input in search string. Always ask repository file to get
    // correct input language
    
    TBool mirroredInput( EFalse );
    TInt inputLang( 0 );
    CRepository* centRep( NULL );
    TRAPD(leavecode, centRep = CRepository::NewL( KCRUidAknFep ) );
    if( KErrNone == leavecode )
        {
        centRep->Get( KAknFepInputTxtLang, inputLang );
        }
    delete centRep;
    
    mirroredInput = ( inputLang == ELangArabic ||
                      inputLang == ELangHebrew ||
                      inputLang == ELangFarsi);            
            
    return mirroredInput;
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::SetSearchBoxTextL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::ReDrawL(TBool aAsynchronous)
    {
    if( aAsynchronous )
        {
        iRunAsynch->ReDraw();
        return;
        }
    
    if( iSearchField )
        {
        RBuf searchText;
        searchText.CreateL ( iSearchField->TextLength() );
        CleanupClosePushL( searchText );
        iSearchField->GetSearchText( searchText );
        SetSearchBoxTextL( searchText, iSearchField->TextLength() );
        CleanupStack::PopAndDestroy(&searchText);
        }
    }

// ---------------------------------------------------------------------------
// CPbk2PredictiveSearchFilter::SetSearchBoxTextL
// ---------------------------------------------------------------------------
//
void CPbk2PredictiveSearchFilter::SetSearchBoxTextL( const TDesC& aText, TInt aCursorPos, TBool aDrawNow )
    {
    if( iSearchField )
        {
        iFiltering = ETrue;
        CEikEdwin& editor(iSearchField->Editor());
        if( IsPredictiveActivated() && aText.Length() )
            {
            CGlobalText* globalText = static_cast<CGlobalText*>(editor.Text());
            TCharFormat format;
            TCharFormatMask mask;
            TInt pos = 0;
            TInt length = aText.Length();
            globalText->GetCharFormat(format, mask, pos, length);
            format.iFontPresentation.iUnderline = EUnderlineOn;
            mask.SetAttrib(EAttFontUnderline);
            globalText->ApplyCharFormatL(format, mask, pos, length);
            editor.SetDocumentContentL(*globalText);
            }
        editor.SetTextL(&aText);
        editor.SetCursorPosL(aCursorPos, EFalse);
        if( aDrawNow )
            {
            editor.DrawNow();
            }
        iFiltering = EFalse;
        }
    }

 //End of File
