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
* Description: PhoneBook2 custom control. Code has been modified to
*       suit PhoneBook2 requirements. See CColumnListBoxData
*       in EIKCLBD.CPP. 
*       Ensure that this piece of code is in sync with Avkon EIKCLBD.CPP(CColumnListBoxData)
*
*/

#include <eikclbd.h>
#include <aknlists.h>
#include <AknsEffectAnim.h>
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>
#include <centralrepository.h>
#include <AvkonInternalCRKeys.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <AknMarqueeControl.h>
#include <eikpanic.h>
#include <eikcoctlpanic.h>
#include <gulicon.h>
#include <AknBidiTextUtils.h>
#include <skinlayout.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknphysics.h>


#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif //RD_UI_TRANSITION_EFFECTS_LIST

#include <touchfeedback.h>

//Start of Code Added for PhoneBook2
#include "cpbk2contactviewcustomlistboxdata.h"
#include "CPbk2ContactViewListBoxModel.h"
#include <MPbk2UiControlCmdItem.h>
#include <AknFepInternalCRKeys.h>
#include <AknLayoutFont.h>

//from PCS engine
#include <CPcsDefs.h>
#include <CPsRequestHandler.h>
#include <CPsQuery.h>
#include "CPbk2PredictiveSearchFilter.h"
//End of Code Added for PhoneBook2

const TInt KColumnListBoxGranularity=4;
const TInt KNoActualColumnFont=-1;
const TInt KMaxColumn = 6;

// colored tick marks support
const TInt KColorIconFlag = -1;
const TInt KColorIconIdx  =  0;

//Start of Code Added for PhoneBook2
/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_OutOfRangeListBoxData = 1,
    EPanicColumnListInvalidBitmapIndexListBoxData,
    EPanicNullPointerListBoxData,
    EPanicColumnListLayoutErrorListBoxData
    };
#endif // _DEBUG

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2ContactViewCustomListBoxData");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG
}


TInt CompareByStartIndex( const TPsMatchLocation& aFirst, const TPsMatchLocation& aSecond )
    {
    return aFirst.index - aSecond.index;
    }
//End of Code Added for PhoneBook2

/**
* About animation usage:
* All animation usage is implemented in this class. Animation is eye candy and
* low priority when it comes to resources -> on resource failures animation
* fails fast to release resources for more critical functionality. Also, no
* attempt to (re)construct failed animation is made.
*
* User inactivity is not being observed. Only one animation is used. Extension
* is CActive to make it possible to postpone animation deletion on animation
* failure.
*/
NONSHARABLE_CLASS(CPbk2ContactViewCustomListBoxDataExtension) :
    public CActive,
    public MAknPictographAnimatorCallBack,
    public MCoeForegroundObserver,
    public MAknsEffectAnimObserver,
    public MListBoxItemChangeObserver
    {
public:
    enum TFlag
        {
        // Set if current item background should be copied to animation as
        // input layer.
        EFlagUpdateBg      = 0,

        // Set if animation has been resized at least once
        EFlagHasLayers     = 1,

        // Set if foreground has been gained.
        EFlagHasForeground = 2
        };
    enum TSubCellType
        {
        EEikSLGraphic,
        EEikSLText,
        EEikSLNumeric
        };

public:
    CPbk2ContactViewCustomListBoxDataExtension();
    void ConstructL( CPbk2ContactViewCustomListBoxData* aListBoxData,
                     const TAknsItemID& aAnimationIID );
    ~CPbk2ContactViewCustomListBoxDataExtension();
    static TInt RedrawEvent(TAny* aControl);
    TBool IsMarqueeOn();
    struct SRowAndColumn
        {
        TInt iIndex; // 24 bits for row and 8 bits for column
        const CFont* iFont;
        };
    SRowAndColumn& At(TInt aArrayIndex);
    const SRowAndColumn& At(TInt aArrayIndex) const;
    void AddRowAndColumnL(TInt aRow,TInt aColumn);
    TInt FindRowAndColumnIndex(TInt& aArrayIndex,TInt aRow,TInt aColumn) const;
    void FindRowAndColumnIndexOrAddL(TInt& aArrayIndex,TInt aRow,TInt aColumn);

    inline void NoAnimIfError( TInt aError );
    void TryCreateAnimation();
    TBool SyncAnim( const TSize& aSize );
    TBool SyncAndDrawAnim( CBitmapContext& aGc, const TRect& aRect );
    MAknsControlContext* SkinBackgroundContext() const;
    void DeleteAnim();
    void FocusGained();
    void FocusLost();
    void SkinChanged();
    void SetControl( CCoeControl* aControl );

    // Implementation of MCoeForegroundObserver
    void HandleGainingForeground();
    void HandleLosingForeground();

    // Implementation of MAknsEffectAnimObserver
    void AnimFrameReady( TInt aError, TInt );

    // Implementation of MListBoxItemChangeObserver
    void ListBoxItemsChanged(CEikListBox* aListBox);

    // Overloads CActive::DoCancel
    void DoCancel();
    // Overloads CActive::RunL
    void RunL();

    void Play();
    void CreateColorBitmapsL();
    
    TBool DrawPressedDownEffect( MAknsSkinInstance* aInstance, 
                                 CWindowGc& aGc, 
                                 const TRect& aOutRect, 
                                 const TRect& aInnerRect ) const;
public: //for handling column alignment
    struct TColumnExt
        {
        TInt    iColumn; // Must be first entry 
        TBool   iLayoutAlign;//is layout alignment or Set by client 
        };    
    TInt  AddColumnExtL(TInt aColumn);        
    void  FindColumnExtIndexOrAddL(TInt& aArrayIndex,TInt aColumn);  
    void  SetColumnLayoutAlignmentL(TInt aColumn);  
            
          
    TBool FindColumnExtIndex(TInt& aArrayIndex,TInt aColumn) const; 
    TBool ColumnLayoutAlignment(TInt aColumn) const;  
    
public: // new list layout system
    struct SRowAndSubCell
        {
        TInt iIndex; // 24 bits for row and 8 bits for subcell
        const CFont* iFont;
        };
    struct SSLSubCell
        {
        TInt iSubCell; // Must be first entry
        TAknTextLineLayout iTextLayout;
        TAknWindowLineLayout iGraphicLayout;
        TInt iSubCellType;
        TInt iConditionValue; // used with conditional layouts for not always drawn subcells
        };

    SSLSubCell& AtSL(TInt aArrayIndex);
    const SSLSubCell& AtSL(TInt aArrayIndex) const;
    void AddSLSubCellL(TInt aSubCell);
    TInt FindSLSubCellIndex(TInt& aArrayIndex, TInt aSubCell) const;
    void FindSLSubCellIndexOrAddL(TInt& aArrayIndex, TInt aSubCell);
    void ResetSLSubCellArray();

    
private: // New internal methods
    TBool DrawHighlightBackground( CFbsBitGc& aGc );
    void PostDeleteAnimation();
    void CreateAnimationL();
    void DoResizeL( const TSize& aHighlightSize, TBool aAboutToStart );

private: // from MAknPictographAnimatorCallback
    void DrawPictographArea();

public:
    CArrayPtr<CGulIcon>* iIconArray;
    CCoeControl *iControl; // not owned
    CAknsListBoxBackgroundControlContext* iSkinControlContext;
    const TAknsItemID *iSkinHighlightFrameId;
    const TAknsItemID *iSkinHighlightFrameCenterId;
    TBool iSkinEnabled;
    CAknPictographInterface* iPictoInterface;
    CAknMarqueeControl*   iMarquee;
    CAknMarqueeControl*   i2ndLineMarquee;
    TInt                  iCurrentItem; // Marquee:
    TInt iCurrentRow; // Current list row being drawn.
    CArrayFix<SRowAndColumn>* iRowAndColumnArray;
    TSize iSubCellIconSize[6]; // Store icon sizes for each column
    TAknSeparatorLinePosition iSeparatorLinePosition;
    CAknsEffectAnim* iAnimation;
    /**
    * Stored flags are explained in enumeration TFlags.
    */
    TBitFlags32 iAnimFlags;
    CPbk2ContactViewCustomListBoxData* iListBoxData;
    TRgb iHighlightedTextColor;
    TRgb iTextColor;

    TAknsItemID iAnimIID;
    MColumnListBoxAnimBackgroundDrawer* iHighlightBgDrawer;
    TSize iAnimSize; // TODO Deprecating old style anim API and removing this
    TBool       iUnderlineFlagSet;  // underlining support for more than
    TBitFlags32 iUnderlineFlags;    // one text subcell
    TBool iUseLayoutData;
    TBool iStretchingEnabled;
    CArrayFix<SRowAndSubCell>* iRowAndSubCellArray;
    CArrayFix<SSLSubCell>* iSLSubCellArray;
    TBool iSubCellsMightIntersect;
    TRect iParent;  // parent for size calculations

    // colorskin highlight icons
    CFbsBitmap* iColorBmp;
    CFbsBitmap* iHiliBmp;
    TRgb        iIconColor;
    TRgb        iHiliIconColor;
    TInt        iConditionalCells;

    // which columns of highlighted item have clipped text ?
    TUint32     iClippedColumns;
    
    TBool iKineticScrolling;

   
    
    CArrayFix<TColumnExt>* iColumnExtArray; //for column alignment
    TRect iMarginRect;
    };

/**
* High priority is well argumented because running the active object will
* result in animation deletion -> results in freeing resources.
*/
CPbk2ContactViewCustomListBoxDataExtension::CPbk2ContactViewCustomListBoxDataExtension():
    CActive( EPriorityHigh )
    {}

void CPbk2ContactViewCustomListBoxDataExtension::ConstructL(
    CPbk2ContactViewCustomListBoxData* aListBoxData,
    const TAknsItemID& aAnimationIID )
    {
    ASSERT( aListBoxData );
    iListBoxData = aListBoxData;
    iRowAndColumnArray=new(ELeave) CArrayFixFlat<SRowAndColumn>(4);

    iSeparatorLinePosition = ENoLine;
    
    iAnimIID = aAnimationIID;
    CActiveScheduler::Add( this );
    TryCreateAnimation(); // Animations are created by default

    // try to set ESS text colors here so that they will be set at least somewhere...
    iTextColor = NULL; // just in case
    iHighlightedTextColor = NULL; // just in case
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb color;

    // set ESS text color
    TInt error = AknsUtils::GetCachedColor( skin,
                                            color,
                                            KAknsIIDQsnTextColors,
                                            EAknsCIQsnTextColorsCG6 );
    if ( !error )
        {
        iTextColor = color;
        }

    // set ESS highlighted text color
    error = AknsUtils::GetCachedColor( skin,
                                       color,
                                       KAknsIIDQsnTextColors,
                                       EAknsCIQsnTextColorsCG10 );
    if ( !error )
        {
        iHighlightedTextColor = color;
        }

    iRowAndSubCellArray=new(ELeave) CArrayFixFlat<SRowAndSubCell>(4);

    iSLSubCellArray=new(ELeave) CArrayFixFlat<SSLSubCell>(4);
    
    iColumnExtArray = new(ELeave) CArrayFixFlat<TColumnExt>(KColumnListBoxGranularity);

#ifdef RD_LIST_STRETCH
    // check list stretching from cenrep
    CRepository* cenRep = CRepository::NewL( KCRUidAvkon );
    cenRep->Get( KAknAutomaticListStretching, iStretchingEnabled );
    delete cenRep;
#endif // RD_LIST_STRETCH

    
    iKineticScrolling = CAknPhysics::FeatureEnabled();
    }

CPbk2ContactViewCustomListBoxDataExtension::~CPbk2ContactViewCustomListBoxDataExtension()
    {

    Cancel();

    // Stop receiving foreground events
    CCoeEnv* env = CCoeEnv::Static();
    env->RemoveForegroundObserver( *this );

    delete iRowAndColumnArray;
    iRowAndColumnArray = NULL;
    delete iSkinControlContext;
    delete iPictoInterface;
    delete iMarquee;
    delete i2ndLineMarquee;
    delete iAnimation;
    delete iRowAndSubCellArray;
    delete iSLSubCellArray;
    delete iColorBmp;
    delete iHiliBmp;
    delete  iColumnExtArray;
    }

MAknsControlContext* CPbk2ContactViewCustomListBoxDataExtension::SkinBackgroundContext() const
    {
    if ( iSkinEnabled )
        {
        return iSkinControlContext;
        }
        
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::NoAnimIfError
// -----------------------------------------------------------------------------
//
inline void CPbk2ContactViewCustomListBoxDataExtension::NoAnimIfError( TInt aError )
    {
    if( KErrNone != aError )
        {
        DeleteAnim();
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::TryCreateAnimation
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::TryCreateAnimation()
    {
    if( !iControl )
        {
        return;
        }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST  
    CEikListBox* list = static_cast<CEikListBox*>( iControl );
    CListBoxView* view = list->View();
    if ( !view || !view->ItemDrawer() )
        {
        return;
        }
    CWindowGc* gc = view->ItemDrawer()->Gc();
    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( gc );
    if ( transApi && transApi->VerifyKml() == KErrNone )
        {
        return;
        }
#endif //RD_UI_TRANSITION_EFFECTS_LIST      

    // Ideally we should not create animation if the list has zero items.
    // Unfortunately, this will cause problems elsewhere as setting item text
    // array to list requires not calling HandleItemAddition (or similar
    // method) -> in some situations animation would not be created at all as
    // we don't receive item change event. Fortunately, creating animation to
    // empty list doesn't carry much overhead as layer creation is delayed to
    // first render.

    if( !iAnimation )
        {
        // This must be the first call because animation does not exist.
        // Animation layers are created when the animation is rendered for the
        // first time.
        TRAPD( err, CreateAnimationL() );
        NoAnimIfError( err );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::SyncAnim
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxDataExtension::SyncAnim( const TSize& aSize )
    {
    if( !iAnimation || aSize.iWidth <= 0 || aSize.iHeight <= 0 )
        {
        return EFalse;
        }

    if( iAnimation->Size() != aSize || iAnimation->NeedsInputLayer() )
        {
        // Resizing will update animation background
        iAnimFlags.Clear( EFlagUpdateBg );

        // Animation exists but its size is out of sync or input layers have
        // been released
        TRAPD( err, DoResizeL( aSize, iAnimFlags.IsSet( EFlagHasForeground ) ) );

        if( err )
            {
            DeleteAnim();
            return EFalse;
            }
        iAnimFlags.Set( EFlagHasLayers );
        }

    // Highlight animation background needs update (current item has changed)
    if( iAnimFlags.IsSet( EFlagUpdateBg ) )
        {
        iAnimFlags.Clear( EFlagUpdateBg );

        if( iAnimation->InputRgbGc() )
            {
            DrawHighlightBackground( *iAnimation->InputRgbGc() );
            // We need to update the output frame (otherwise the highlight
            // would drawn with the old output before the next new animation
            // frame).
            NoAnimIfError( iAnimation->UpdateOutput() );
            if( !iAnimation )
                {
                return EFalse;
                }
            }
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::SyncAndDrawAnim
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxDataExtension::SyncAndDrawAnim(
        CBitmapContext& aGc, const TRect& aRect )
    {
    if( iAnimation )
        {
        // When application loses foreground or has not yet gained foreground
        // animation is in stopped state and input layers are not present. It
        // is possible that list is repainted in this situation. Calling
        // SyncAnim will create the non-present layers -> WSERV flush ->
        // flicker. To prevent flickering we just render the existing frame.
        // This can lead to incorrect draw if the current item index is changed
        // when the app has no foreground (very unlikely). If EFlagHasLayers is
        // not set we need to do SyncAnim because it is the first call to draw
        // (and flicker is not an issue).
        if( EAknsAnimStateStopped == iAnimation->State() &&
            !iAnimFlags.IsSet( EFlagHasForeground ) &&
            iAnimFlags.IsSet( EFlagHasLayers )  &&
            !iAnimFlags.IsSet( EFlagUpdateBg ) )
            {
            return iAnimation->Render( aGc, aRect );
            }
        }

    if( SyncAnim( aRect.Size() ) )
        {
        return iAnimation->Render( aGc, aRect );
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::DeleteAnim
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::DeleteAnim()
    {
    // Stop receiving foreground events
    CCoeEnv* env = CCoeEnv::Static();
    env->RemoveForegroundObserver( *this );

    delete iAnimation;
    iAnimation = NULL;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::FocusGained
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::FocusGained()
    {
    Play();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::FocusLost
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::FocusLost()
    {
    if( iAnimation )
        {
        NoAnimIfError( iAnimation->Pause() );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::SkinChanged
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::SkinChanged()
    {
    DeleteAnim();
    TryCreateAnimation();
    TRAP_IGNORE( CreateColorBitmapsL() );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::SetControl
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::SetControl( CCoeControl* aControl )
    {
    if( iControl )
        {
        CEikListBox* list = (CEikListBox*) iControl;
        list->RemoveItemChangeObserver( this );
        }

    iControl = aControl;

    if( iControl )
        {
        CEikListBox* list = (CEikListBox*) iControl;
        TRAP_IGNORE( list->AddItemChangeObserverL(this) );

        if( !iAnimation )
            {
            TryCreateAnimation();
            }

        if( iAnimation )
            {
            Play();
            }
        }
    else
        {
        DeleteAnim();
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::HandleGainingForeground
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::HandleGainingForeground()
    {
    // Most of the time focus focus events happen between foreground events.
    // Unfortunately, there are embedded app related cases where this does not
    // always hold (it is possible to focus in non-foreground application or
    // gain foreground without getting a focus gained). Therefore animations
    // must be started here. This causes potential problem case: foreground
    // events are broadcasted to all animations, if there are multiple lists
    // that have focus the result will be multiple animations running at the
    // same time.
    iAnimFlags.Set( EFlagHasForeground );

    if( iAnimation )
        {
        // Because we are gaining foreground we must restore input layers
        // (which are released when animation stops to reduce memory usage).
        SyncAnim( iAnimation->Size() );
        }

    if( iControl )
        {
        if( iControl->IsFocused() )
            {
            Play();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::HandleLosingForeground
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::HandleLosingForeground()
    {
    iAnimFlags.Clear( EFlagHasForeground );
    if( iAnimation )
        {
        NoAnimIfError( iAnimation->Stop() );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::AnimFrameReady
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::AnimFrameReady( TInt aError, TInt )
    {
    if( KErrNone != aError )
        {
        // Animation has failed to run -> schedule the animation for
        // deletion to fall back to normal rendering.
        PostDeleteAnimation();
        return;
        }

    // This situation should never happen because we start/stop animation when
    // the extension's control is set.
    if( !iControl )
        return;

    // From now on, we have a valid control pointer
    CEikColumnListBox* list = static_cast<CEikColumnListBox*>( iControl );
    CListBoxView* view = list->View();

    if( !view )
        return;

    // We should not run animation when control is in certain states. When
    // control is in these states we idle the animation until the control state
    // becomes valid again.
    TBool invalid = !iControl->IsVisible() ||
                    iControl->IsDimmed() ||
                    (view->ItemDrawer()->Flags() & CListItemDrawer::EDisableHighlight);

    // Check for idling
    if( iAnimation->IsIdling() )
        {
        if( invalid )
            {
            // We are idling and the control is still invalid -> keep on
            // idling.
            return;
            }
        else
            {
            // Control is in valid state, animation should be continued
            TInt err = iAnimation->Continue();
            if( err )
                PostDeleteAnimation();
            return;
            }
        }
    else if( invalid ) // Not idling and invalid control -> start idling
        {
        iAnimation->SetIdling( KAknsEffectAnimDefaultIdleInterval );
        // If the highlight has been disabled, render once to clear the
        // highlight (not returning in that case).
        if( !(view->ItemDrawer()->Flags() & CListItemDrawer::EDisableHighlight) )
            return;
        }

    // No items, no drawing
    if( list->Model()->NumberOfItems() == 0 )
        {
        return;
        }

    // Column array is not ready yet
    if( iListBoxData->LastColumn() < 0 )
        {
        return;
        }

    // Repaint the highlighted cell only
    view->DrawItem( view->CurrentItemIndex() );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::ListBoxItemsChanged

// we get here from:
// - CEikListBox::Reset()
// - CEikListBox::HandleItemRemoval{WithoutSelections}L()
// - CEikListBox::HandleItemAdditionL()
// - CEikListBox::FireItemChange()
// In all cases:
// - feedback areas might be invalid
// - application is responsible for redrawing the listbox.
//
// since re-drawing listbox fixes feedback areas, those can be reset here.

// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::ListBoxItemsChanged(
        CEikListBox* aListBox)
    {
    TInt items = aListBox->Model()->NumberOfItems();

    if( iAnimation )
        {
        if( 0 == items )
            {
            DeleteAnim();
            }
        else
            {
            Play();
            }
        }
    else if( !iAnimation )
        {
        // Animation doesn't exist and we have > 0 items
        TryCreateAnimation();
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::DoCancel
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::DoCancel()
    {
    // Required method, but not needed
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::RunL
// Postponed animation deletion is done here.
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::RunL()
    {
    DeleteAnim();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::Play
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::Play()
    {
    if( !iAnimation )
        {
        return;
        }

    // No need to start running/finished animation
    if( EAknsAnimStateRunning == iAnimation->State() ||
        EAknsAnimStateFinished == iAnimation->State() )
        {
        return;
        }

    // Check that application is on foreground because there are cases where
    // focus changes are done after foreground is lost -> potentially leads to
    // multiple running animations.
    if( !iAnimFlags.IsSet( EFlagHasForeground ) )
        {
        return;
        }

    // Animation cannot run if we don't have control (nowhere to draw)
    if( !iControl )
        {
        return;
        }

    // The control must also have the focus
    if( !iControl->IsFocused() )
        {
        return;
        }

    // Don't animate empty list
    CEikColumnListBox* list = static_cast<CEikColumnListBox*>( iControl );
    if( list->Model()->NumberOfItems() == 0 )
        {
        return;
        }

    // All preconditions are met: we have animation, foreground, focus, more
    // than zero items and animation is either paused or stopped. Invisibility,
    // dimming and disabled highlight are handled by idling the animation (see
    // AnimFrameReady).

    if( EAknsAnimStatePaused == iAnimation->State() )
        {
        NoAnimIfError( iAnimation->Continue() );
        }
    else if( EAknsAnimStateStopped == iAnimation->State() )
        {
        NoAnimIfError( iAnimation->Start() );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::DrawHighlightBackground
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxDataExtension::DrawHighlightBackground( CFbsBitGc& aGc )
    {
    if( iHighlightBgDrawer ) // Bg drawing is done externally (in derived class)
        {
        return iHighlightBgDrawer->DrawHighlightAnimBackground( aGc );
        }

    // Draw the background under the current highlight. This is simplified
    // drawing, we only grab a piece from the list background bitmap.
    CEikColumnListBox* list =
        static_cast<CEikColumnListBox*>( iControl );
    CListBoxView* view = list->View();
    TRect itemRect;
    TInt index = view->CurrentItemIndex();

    // It is possible that the animation is constructed when the list is
    // empty. In this case draw the first element background (drawing works ok
    // even if the list has no items).
    if( list->Model()->NumberOfItems() == 0 )
        {
        index = 0;
        }

    itemRect.SetRect( view->ItemPos( index ), iAnimation->Size() );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( iControl );

    if( !cc )
        cc = SkinBackgroundContext();

    return AknsDrawUtils::DrawBackground( skin, cc, iControl, aGc, TPoint(0,0),
                                          itemRect, KAknsDrawParamBottomLevelRGBOnly );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::PostDeleteAnimation
// Schedules the animation for deletion by activating the extension itself.
// Deletion is postponed because in many error/failure occasions the caller has
// been animation and direct deletion is possibly not safe (because function
// stack would return through the deleted object).
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::PostDeleteAnimation()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::CreateAnimationL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::CreateAnimationL()
    {
    DeleteAnim();

    // Check if derived class wants to disable highligh animation
    if( KAknsIIDNone == iAnimIID )
        {
        return;
        }

    // Create animation
    CCoeEnv* env = CCoeEnv::Static();
    env->AddForegroundObserverL( *this );

    iAnimation = CAknsEffectAnim::NewL( this );
    TBool ok = iAnimation->ConstructFromSkinL( iAnimIID );

    if( !ok ) // Animation for the ID was not found from the skin
        {
        User::Leave( KErrNotFound );
        }

    // Sync the local foreground flag state. Foreground state is stored locally
    // because calling AppUi::IsForeground causes WSERV flush (shocking) and
    // therefore it cannot be used in draw routines.
    CAknAppUi* aui = static_cast<CAknAppUi*>(CEikonEnv::Static()->AppUi());
    iAnimFlags.Assign( EFlagHasForeground, aui->IsForeground() );

    Play();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxDataExtension::DoResizeL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxDataExtension::DoResizeL(
    const TSize& aHighlightSize, TBool aAboutToStart )
    {
    iAnimation->BeginConfigInputLayersL( aHighlightSize, aAboutToStart );

    if( iAnimation->InputRgbGc() )
        {
        DrawHighlightBackground( *iAnimation->InputRgbGc() );
        }

    iAnimation->EndConfigInputLayersL();
    }

void CPbk2ContactViewCustomListBoxDataExtension::DrawPictographArea()
    {
    iControl->DrawNow();
    }

TInt CPbk2ContactViewCustomListBoxDataExtension::RedrawEvent(TAny* aControl)
    {
    if (!((CCoeControl*)aControl)->IsVisible())
        {
        return EFalse;
        }
    CEikColumnListBox* listBox = 
            (CEikColumnListBox*)aControl;
    
    // HWET-7BDDD4
    CListBoxView* view = listBox->View();
    TInt index = listBox->CurrentItemIndex();
    if ( view->ItemIsVisible( index ) )
        {
        listBox->DrawItem( index );
        }
     return ETrue;
    }

TBool CPbk2ContactViewCustomListBoxDataExtension::IsMarqueeOn()
    {
    TBool isOn = EFalse;
    if (iMarquee)
        isOn = iMarquee->IsMarqueeOn();
    return isOn;
    }
void CPbk2ContactViewCustomListBoxDataExtension::AddSLSubCellL(TInt aSubCell)
    {
    SSLSubCell subcell;
    subcell.iSubCell = aSubCell;
    subcell.iTextLayout = NULL;
    subcell.iGraphicLayout = NULL;
    subcell.iSubCellType = 0;
    subcell.iConditionValue = -1;

    TKeyArrayFix key(0,ECmpTInt32);
    iSLSubCellArray->InsertIsqL(subcell,key);
    }

void 
CPbk2ContactViewCustomListBoxDataExtension::AddRowAndColumnL(TInt aRow,TInt aColumn)
    {
    SRowAndColumn column;
    column.iIndex = (aRow << 8) | (aColumn & 0xff);
    TKeyArrayFix key(0,ECmpTInt32);
    iRowAndColumnArray->InsertIsqL(column,key);
    }


CPbk2ContactViewCustomListBoxDataExtension::SRowAndColumn& 
CPbk2ContactViewCustomListBoxDataExtension::At(TInt aArrayIndex)
    {
    __ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iRowAndColumnArray->Count(),Panic(EPanicPreCond_OutOfRangeListBoxData));
    return(iRowAndColumnArray->At(aArrayIndex));
    }

const CPbk2ContactViewCustomListBoxDataExtension::SRowAndColumn& 
CPbk2ContactViewCustomListBoxDataExtension::At(TInt aArrayIndex) const
    {
    __ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iRowAndColumnArray->Count(),Panic(EPanicPreCond_OutOfRangeListBoxData));
    return(iRowAndColumnArray->At(aArrayIndex));
    }

TInt CPbk2ContactViewCustomListBoxDataExtension::FindRowAndColumnIndex(TInt& aArrayIndex,
                                                                TInt aRow,
                                                                TInt aColumn) const
    {
    if (iRowAndColumnArray->Count()==0)
        return(KErrNotFound);
    TKeyArrayFix key(0,ECmpTInt32);
    SRowAndColumn rowAndColumn;
    rowAndColumn.iIndex = (aRow << 8) | (aColumn & 0xff);
    return(iRowAndColumnArray->FindIsq(rowAndColumn,key,aArrayIndex));
    }

void CPbk2ContactViewCustomListBoxDataExtension::FindRowAndColumnIndexOrAddL(TInt& aArrayIndex,
                                                                      TInt aRow,
                                                                      TInt aColumn)
    {
    if (FindRowAndColumnIndex(aArrayIndex,aRow,aColumn)!=0) //==KErrNotFound)
        {
        AddRowAndColumnL(aRow,aColumn);
        FindRowAndColumnIndex(aArrayIndex,aRow,aColumn);
        }
    }
    
/////////////handling TColumnExt,start

TInt CPbk2ContactViewCustomListBoxDataExtension::AddColumnExtL(TInt aColumn)
    {
    TColumnExt column;
    column.iColumn=aColumn;
    column.iLayoutAlign = ETrue;
    TKeyArrayFix key(0,ECmpTInt);
    return iColumnExtArray->InsertIsqL(column,key);
    }
void CPbk2ContactViewCustomListBoxDataExtension::FindColumnExtIndexOrAddL(TInt& aArrayIndex,TInt aColumn)
    {
    if (!FindColumnExtIndex(aArrayIndex,aColumn))
        {
        aArrayIndex = AddColumnExtL(aColumn);
        
        }
    }
    
void CPbk2ContactViewCustomListBoxDataExtension::SetColumnLayoutAlignmentL(TInt aColumn)
    {
    TInt index;
    FindColumnExtIndexOrAddL(index,aColumn);  
    iColumnExtArray->At(index).iLayoutAlign = EFalse;
    }
    
    
TBool CPbk2ContactViewCustomListBoxDataExtension::FindColumnExtIndex(TInt& aArrayIndex,TInt aColumn) const
    {
    if (aColumn<0 || iColumnExtArray->Count()==0)
        return EFalse;
    TKeyArrayFix key(0,ECmpTInt);
    TColumnExt column;
    column.iColumn=aColumn;

    // Returns zero if key was found, otherwise non-zero
    TInt ret = iColumnExtArray->FindIsq(column,key,aArrayIndex);
    return (ret == 0);
    }
        
TBool CPbk2ContactViewCustomListBoxDataExtension::ColumnLayoutAlignment(TInt aColumn) const
    {
    TInt index;
    if (!FindColumnExtIndex(index,aColumn))
        return(ETrue);
    return(iColumnExtArray->At(index).iLayoutAlign); 
    }    
    
/////////////handling TColumnExt,end    
CPbk2ContactViewCustomListBoxDataExtension::SSLSubCell& 
CPbk2ContactViewCustomListBoxDataExtension::AtSL(TInt aArrayIndex)
    {
    __ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iSLSubCellArray->Count(),Panic(EPanicPreCond_OutOfRangeListBoxData));
    return(iSLSubCellArray->At(aArrayIndex));
    }

const CPbk2ContactViewCustomListBoxDataExtension::SSLSubCell& 
CPbk2ContactViewCustomListBoxDataExtension::AtSL(TInt aArrayIndex) const
    {
    __ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iSLSubCellArray->Count(),Panic(EPanicPreCond_OutOfRangeListBoxData));
    return(iSLSubCellArray->At(aArrayIndex));
    }

TInt CPbk2ContactViewCustomListBoxDataExtension::FindSLSubCellIndex(TInt& aArrayIndex,
                                                            TInt aSubCell) const
    {
    if (iSLSubCellArray->Count()==0)
        return(KErrNotFound);
    TKeyArrayFix key(0,ECmpTInt32);
    SSLSubCell SLSubCell;
    SLSubCell.iSubCell = aSubCell;
    return(iSLSubCellArray->FindIsq(SLSubCell,key,aArrayIndex));
    }

void CPbk2ContactViewCustomListBoxDataExtension::FindSLSubCellIndexOrAddL(TInt& aArrayIndex,
                                                                  TInt aSubCell)
    {
    if (FindSLSubCellIndex(aArrayIndex, aSubCell)!=0) //==KErrNotFound)
        {
        AddSLSubCellL(aSubCell);
        FindSLSubCellIndex(aArrayIndex,aSubCell);
        }
    }

void CPbk2ContactViewCustomListBoxDataExtension::ResetSLSubCellArray()
    {
    iSLSubCellArray->Reset();
    iRowAndSubCellArray->Reset();
    }

TBool CPbk2ContactViewCustomListBoxDataExtension::DrawPressedDownEffect( MAknsSkinInstance* aInstance, 
                                                          CWindowGc& aGc,const TRect& aOutRect, 
                                                          const TRect& aInnerRect ) const
    {
    return AknsDrawUtils::DrawFrame( aInstance,
                                     aGc,
                                     aOutRect,
                                     aInnerRect,
                                     KAknsIIDQsnFrListPressed,
                                     KAknsIIDQsnFrListCenterPressed );
    }

CCoeControl *CPbk2ContactViewCustomListBoxData::Control() const 
    {
    return iExtension->iControl;
    }

void CPbk2ContactViewCustomListBoxData::SetControl(CCoeControl *aControl)
    {
    iExtension->SetControl( aControl );
    }


MAknsControlContext* CPbk2ContactViewCustomListBoxData::SkinBackgroundContext() const
    {
    if (iExtension->iSkinEnabled)
        {
        return iExtension->iSkinControlContext;
        }
        
    return NULL;
    }

void CPbk2ContactViewCustomListBoxData::SetSkinBackgroundContext(CAknsListBoxBackgroundControlContext *aContext)
    {
    delete iExtension->iSkinControlContext;
    iExtension->iSkinControlContext = aContext;
    }

void CPbk2ContactViewCustomListBoxData::CreatePictographInterfaceL()
    {
    if ( !iExtension->iPictoInterface )
        {
        iExtension->iPictoInterface = CAknPictographInterface::NewL(
            *iExtension->iControl, *iExtension );
        }
    }

void CPbk2ContactViewCustomListBoxData::SetSkinHighlightFrame(const TAknsItemID *aFrameId, const TAknsItemID *aFrameCenterId)
    {
    iExtension->iSkinHighlightFrameId = aFrameId;
    iExtension->iSkinHighlightFrameCenterId = aFrameCenterId;
    }

void CPbk2ContactViewCustomListBoxData::SetSkinEnabledL(TBool aEnabled)
    {
    CListBoxData::SetSkinEnabledL(aEnabled);
    iExtension->iSkinEnabled = aEnabled;
    }

void CPbk2ContactViewCustomListBoxData::SetSkinStyle(const TAknsItemID *aId, const TRect &aTileRect)
    {
    if (iExtension->iSkinControlContext)
        {
        iExtension->iSkinControlContext->SetTiledBitmap(*aId);
        iExtension->iSkinControlContext->SetTiledRect(aTileRect);
        }
    }

void CPbk2ContactViewCustomListBoxData::SetListEndSkinStyle(const TAknsItemID *aId, const TRect &aTileRect)
    {
    if (iExtension->iSkinControlContext)
        {
        iExtension->iSkinControlContext->SetBottomBitmap(*aId);
        iExtension->iSkinControlContext->SetBottomRect(aTileRect);
        }
    }

void CPbk2ContactViewCustomListBoxData::CreateMarqueeControlL()
    {
    if ( !iExtension->iMarquee )
        {
        iExtension->iMarquee = CAknMarqueeControl::NewL();
        }
    
     if ( !iExtension->i2ndLineMarquee )
        {
        iExtension->i2ndLineMarquee = CAknMarqueeControl::NewL();
        }
    
    TCallBack callBack(CPbk2ContactViewCustomListBoxDataExtension::RedrawEvent, 
                        iExtension->iControl);
    iExtension->iMarquee->SetRedrawCallBack(callBack);
    iExtension->i2ndLineMarquee->SetRedrawCallBack(callBack);
    }

void CPbk2ContactViewCustomListBoxData::ResetMarquee()
    {
    if (iExtension->iMarquee)
        {
        iExtension->iMarquee->Reset();
        }
    if (iExtension->i2ndLineMarquee)
        {
        iExtension->i2ndLineMarquee->Reset();
        }
    }

TInt CPbk2ContactViewCustomListBoxData::CurrentMarqueeItemIndex()
    {
    return iExtension->iCurrentItem;
    }

void CPbk2ContactViewCustomListBoxData::SetCurrentMarqueeItemIndex(TInt aIndex)
    {
    iExtension->iCurrentItem = aIndex;
    }

//
// Enables or disables marquee.
//
void CPbk2ContactViewCustomListBoxData::EnableMarqueeL(TBool aEnable)
    {
    // CreateMarqueeControlL does nothing if marquee already exists,
    // so let's just call it just in case.
    CreateMarqueeControlL();
    iExtension->iMarquee->EnableMarquee(aEnable);
    iExtension->i2ndLineMarquee->EnableMarquee(aEnable);
    }

void CPbk2ContactViewCustomListBoxData::SetSeparatorLinePosition(TAknSeparatorLinePosition aPosition)
    {
    if (iExtension)
        iExtension->iSeparatorLinePosition = aPosition;
    }
TAknSeparatorLinePosition CPbk2ContactViewCustomListBoxData::SeparatorLinePosition() const
    {
    if (iExtension)
        return iExtension->iSeparatorLinePosition;
    else
        return ENoLine;
    }
CAknLayoutData *CPbk2ContactViewCustomListBoxData::LayoutData() const
    {
    return NULL;
    }

TBool CPbk2ContactViewCustomListBoxData::LayoutInit() const
    {
    return EFalse;
    }

void CPbk2ContactViewCustomListBoxData::SetLayoutInit(TBool /*aValue*/) const
    {
    }


//
//    Class CPbk2ContactViewCustomListBoxData::TColors
//

/**
 * Constructs and intialises the data colors to blacks and whites.
 */
CPbk2ContactViewCustomListBoxData::TColors::TColors()
    : iText(KRgbBlack), iBack(KRgbWhite), iHighlightedText(KRgbWhite), iHighlightedBack(KRgbBlack)
    {
    }

//
//    Class CPbk2ContactViewCustomListBoxData
//

CPbk2ContactViewCustomListBoxData* CPbk2ContactViewCustomListBoxData::NewL(
        CPbk2PredictiveSearchFilter &aSearchFilter )
//
//    Public c'tor
//
    {
    CPbk2ContactViewCustomListBoxData* self=new(ELeave) 
                        CPbk2ContactViewCustomListBoxData( aSearchFilter );
    self->ConstructLD();
    return(self);
    }

CPbk2ContactViewCustomListBoxData::CPbk2ContactViewCustomListBoxData( 
        CPbk2PredictiveSearchFilter &aSearchFilter )
                : CListBoxData(), iSearchFilter( aSearchFilter )
//
//  Protected c'tor
//
    {
    
    }

CPbk2ContactViewCustomListBoxData::~CPbk2ContactViewCustomListBoxData()
//
//    D'tor
//
    {
    delete iColumnArray;
    if (IconArray())
        {
        IconArray()->ResetAndDestroy();
        if (iExtension)
            delete iExtension->iIconArray;
        }
    delete iExtension;    
    }

void CPbk2ContactViewCustomListBoxData::ConstructLD()
    {
    TRAPD( err, DoConstructL( KAknsIIDQsnAnimList ) );
    if( err )
        {
        delete(this);
        User::Leave( err );
        }
    }

void CPbk2ContactViewCustomListBoxData::ConstructLD(
    const TAknsItemID& aAnimationIID )
    {    
    TRAPD( err, DoConstructL( aAnimationIID ) );
    if( err )
        {
        delete(this);
        User::Leave( err );
        }
    }

void CPbk2ContactViewCustomListBoxData::DoConstructL( const TAknsItemID& aAnimationIID )
    {
    iColumnArray = new(ELeave) CArrayFixFlat<SColumn>(KColumnListBoxGranularity);
    iExtension = new(ELeave) CPbk2ContactViewCustomListBoxDataExtension;

    iExtension->iSkinEnabled = AknsUtils::AvkonSkinEnabled();
    SetupSkinContextL();

    iExtension->ConstructL( this, aAnimationIID );

    CListBoxData::SetSkinBackgroundControlContextL(iExtension->iSkinControlContext);

    // preallocate columns so that we dont need to leave on
    // SizeChanged().
    // AddColumnL(0);
    // AddColumnL(1);
    // AddColumnL(2);
    // AddColumnL(3);
    // AddColumnL(4);
    }

TInt CPbk2ContactViewCustomListBoxData::LastColumn() const
//
//    The last column defined
//
    {
    TInt count=iColumnArray->Count();
    if (!count)
        return(KErrNotFound);
    return(At(count-1).iColumn);
    }

TInt CPbk2ContactViewCustomListBoxData::ColumnWidthPixel(TInt aColumn) const
//
//    Return the width for a column
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(0);
    return(At(index).iWidth);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnWidthPixelL(TInt aColumn,TInt aWidth)
//
//    Set a columns width
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iWidth=aWidth;
    if ( iExtension && iExtension->iUseLayoutData )
        {
        iExtension->iUseLayoutData = EFalse;
        CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
        MAknListBoxTfxInternal* transApi =
             CAknListLoader::TfxApiInternal( view->ItemDrawer()->Gc() );
        if ( transApi )
            {
            transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, TPoint( 0, 0 ) );
            transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin, TPoint( 0, 0 ) );
            }
        }
    }

TInt CPbk2ContactViewCustomListBoxData::ColumnHorizontalGap(TInt aColumn) const
//
//  Return the vertical for a column
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(0);
    return(At(index).iVerticalCap);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnHorizontalGapL(TInt aColumn,TInt aGap)
//
//  Set a columns vertical cap
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iVerticalCap=aGap;
    }


TInt 
CPbk2ContactViewCustomListBoxData::ColumnTextClipGap(TInt aColumnIndex) const
    {
    TInt index;
    if (!FindColumnIndex(index,aColumnIndex))
        return TInt(0);
    return(iColumnArray->At(index).iTextClipGap);
    }

void 
CPbk2ContactViewCustomListBoxData::SetColumnTextClipGapL(TInt aColumnIndex, TInt aGap)
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumnIndex);
    At(index).iTextClipGap=aGap;
    }

TInt CPbk2ContactViewCustomListBoxData::ColumnBaselinePos(TInt aColumn) const
//
//  Return the vertical for a column
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(0);
    return(At(index).iBaseline);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnBaselinePosL(TInt aColumn,TInt aPos)
//
//  Set a columns baseline position
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iBaseline=aPos;
    
    // can not be nicely supported by new drawing system
    // --> revert to Symbian style drawing
    if ( iExtension && iExtension->iUseLayoutData )
        {
        iExtension->iUseLayoutData = EFalse;
        CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
        MAknListBoxTfxInternal* transApi =
             CAknListLoader::TfxApiInternal( view->ItemDrawer()->Gc() );
        if ( transApi )
            {
            transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, TPoint( 0, 0 ) );
            transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin, TPoint( 0, 0 ) );
            } 
        }
    }

TMargins CPbk2ContactViewCustomListBoxData::ColumnMargins(TInt aColumn) const
//
//  Return the vertical for a column
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        {    
        TMargins m = {0,0,0,0};
        return m;
        }
    return(At(index).iMargins);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnMarginsL(TInt aColumn,TMargins aMargins)
//
//  Set a columns vertical cap
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iMargins=aMargins;

    // can not be nicely supported by new drawing system
    // --> revert to Symbian style drawing
    if ( iExtension && iExtension->iUseLayoutData )
        {
        iExtension->iUseLayoutData = EFalse;
        CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
        MAknListBoxTfxInternal* transApi =
             CAknListLoader::TfxApiInternal( view->ItemDrawer()->Gc() );
        if ( transApi )
            {
            transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, TPoint( 0, 0 ) );
            transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin, TPoint( 0, 0 ) );
            } 
        }
    }


TInt CPbk2ContactViewCustomListBoxData::ColumnX(TInt aColumn) const
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        {    
        return 0;

        }
    return(At(index).iX);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnXL(TInt aColumn,TInt aX) const
    {
    TInt index;
    MUTABLE_CAST(CPbk2ContactViewCustomListBoxData*,this)->FindColumnIndexOrAddL(index,aColumn);
    MUTABLE_CAST(TInt&,At(index).iX)=aX;
    }


TInt CPbk2ContactViewCustomListBoxData::ColumnEndX(TInt aColumn) const
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        {    
        return 0;
        }
    return(At(index).iEndX);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnEndXL(TInt aColumn,TInt aEndX) const
    {
    TInt index;
    MUTABLE_CAST(CPbk2ContactViewCustomListBoxData*,this)->FindColumnIndexOrAddL(index,aColumn);
    MUTABLE_CAST(TInt&,At(index).iEndX)=aEndX;
    }

const CFont* CPbk2ContactViewCustomListBoxData::ColumnFont(TInt aColumn) const
//
//    Return the font for a column
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(NULL);
    return(iColumnArray->At(index).iBaseFont);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnFontL(TInt aColumn,const CFont* aFont)
//
//    Set a font for a column
//
    {
// This corrects error report KBAA-53GEQ5.
#if NOT_NEEDED_IN_SERIES60
    TInt columns=iColumnArray->Count();
#endif
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    if (!At(index).iBaseFont)
        {
// This corrects error report KBAA-53GEQ5.
#if NOT_NEEDED_IN_SERIES60
        TInt actualFontIndex=0;
        TRAPD(err, actualFontIndex=AddActualFontL(aFont));
        if (err)
            {
            if (columns!=iColumnArray->Count())
                {
                iColumnArray->Delete(index);
                }
            User::Leave(err);
            }
        At(index).iActualFontIndex=actualFontIndex;
#endif
        // TPFIX(moved down) At(index).iBaseFont=aFont;
        }
    At(index).iBaseFont=aFont;
    }

CGraphicsContext::TTextAlign CPbk2ContactViewCustomListBoxData::ColumnAlignment(TInt aColumn) const
//
//    Return a columns alignment
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(CGraphicsContext::ELeft);
    return(iColumnArray->At(index).iAlign);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnAlignmentL(TInt aColumn,CGraphicsContext::TTextAlign aAlign)
//
//    Set a columns alignment
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iAlign=aAlign;
    iExtension->SetColumnLayoutAlignmentL( aColumn);  
    }


CGraphicsContext::TPenStyle CPbk2ContactViewCustomListBoxData::ColumnSeparatorStyle(TInt aColumn) const
//
//    Return a columns separator style
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(CGraphicsContext::ENullPen);
    return(iColumnArray->At(index).iSeparator);
    }

void CPbk2ContactViewCustomListBoxData::SetColumnSeparatorStyleL(TInt aColumn,CGraphicsContext::TPenStyle aSeparator)
//
//    Set a columns separator style
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iSeparator=aSeparator;
    }


TBool CPbk2ContactViewCustomListBoxData::ColumnIsGraphics(TInt aColumn) const
//
//    Return true if the column contains graphics
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(EFalse);
    return(iColumnArray->At(index).iGraphics);
    }

void CPbk2ContactViewCustomListBoxData::SetGraphicsColumnL(TInt aColumn,TBool aIsGraphics)
//
//    Sets whether a column shows graphics
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iGraphics=aIsGraphics;
    }

TBool CPbk2ContactViewCustomListBoxData::ColumnIsNumber(TInt aColumn) const
//
//    Return true if the column contains graphics
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(EFalse);
    return(iColumnArray->At(index).iNumberColumn);
    }

void CPbk2ContactViewCustomListBoxData::SetNumberColumnL(TInt aColumn,TBool aIsNumber)
//
//    Sets whether a column shows graphics
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iNumberColumn=aIsNumber;
    }


TBool CPbk2ContactViewCustomListBoxData::ColumnIsOptional(TInt aColumn) const
//
//    Return true if the column contains graphics
//
    {
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(EFalse);
    return(iColumnArray->At(index).iOptional);
    }

void CPbk2ContactViewCustomListBoxData::SetOptionalColumnL(TInt aColumn,TBool aIsOptional)
//
//    Sets whether a column shows graphics
//
    {
    TInt index;
    FindColumnIndexOrAddL(index,aColumn);
    At(index).iOptional=aIsOptional;
    }

CArrayPtr<CGulIcon>* CPbk2ContactViewCustomListBoxData::IconArray() const
//
//    Return the list of icons
//
    {
    if (iExtension)
        return iExtension->iIconArray;
    else
        return 0;
    }

void CPbk2ContactViewCustomListBoxData::SetIconArray(CArrayPtr<CGulIcon>* aArray)
//
//    Passes ownership of the icon list aArray (assumes any previous list has been deleted by the caller)
//
    {
    if (iExtension)
        iExtension->iIconArray=aArray;
    else
        {
        aArray->ResetAndDestroy();
        delete aArray;
        }    
    }

CFont* CPbk2ContactViewCustomListBoxData::Font(const TListItemProperties& /*aItemProperties*/, TInt aColumn) const
    {
    return const_cast<CFont*>(this->ColumnFont(aColumn));
#if NOT_NEEDED_IN_SERIES60
    TInt index;
    if (!FindColumnIndex(index,aColumn))
        return(NULL);
    TInt actualFontIndex=iColumnArray->At(index).iActualFontIndex;
    if (actualFontIndex==KNoActualColumnFont)
        return(NULL);
    if ((aItemProperties.IsBold()) && (aItemProperties.IsItalics()))
        {
        return iBoldItalicFont.iFonts->At(actualFontIndex);
        }
    else if (aItemProperties.IsBold())
        {
        return iBoldFont.iFonts->At(actualFontIndex);
        }
    else if (aItemProperties.IsItalics())
        {
        return iItalicFont.iFonts->At(actualFontIndex);
        }
    else return iNormalFont.iFonts->At(actualFontIndex);
#endif
    }

void CPbk2ContactViewCustomListBoxData::DrawHighLight( CWindowGc& aGc, const TRect& aRect, TBool aHighlight, MAknsSkinInstance* aSkin ) const
    {
    // SERIES60 Highlight drawing.
    if( aHighlight )
        {
        // same as CFormattedCellListBoxData::DrawDefaultHighlight
        CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
        TBool pressedDown = view->ItemDrawer()->Flags() & CListItemDrawer::EPressedDownState;

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( &aGc );
        if ( transApi )
            {
            transApi->Invalidate(MAknListBoxTfxInternal::EListHighlight ); // This will remove the old bitmap
            transApi->BeginRedraw( MAknListBoxTfxInternal::EListHighlight, aRect );
            transApi->StartDrawing( MAknListBoxTfxInternal::EListHighlight );
            aGc.SetClippingRect( view->ViewRect() );
            }
#endif //RD_UI_TRANSITION_EFFECTS_LIST      
        // Try drawing the animated highlight
        TBool highlightDrawn = EFalse;
        if ( iExtension->iAnimation )
            {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST        
            if ( transApi && transApi->VerifyKml() == KErrNone )
                {
                iExtension->DeleteAnim();
                }
            else
                {
#endif
            TAknLayoutRect topLeft;
            topLeft.LayoutRect(aRect, SkinLayout::List_highlight_skin_placing__general__Line_2());
            
            TAknLayoutRect bottomRight;
            bottomRight.LayoutRect(aRect, SkinLayout::List_highlight_skin_placing__general__Line_5());
            
            TRect outerRect(topLeft.Rect().iTl, bottomRight.Rect().iBr);
            
            aGc.SetPenStyle(CGraphicsContext::ENullPen);
            highlightDrawn = iExtension->SyncAndDrawAnim( aGc, outerRect );
#ifdef RD_UI_TRANSITION_EFFECTS_LIST        
                }
#endif
            }
        
        if (!highlightDrawn)
            {
            // Animated highlight was not available, use normal skinned
            // rendering.
            MAknsControlContext *cc = AknsDrawUtils::ControlContext( Control() );
            if (!cc) cc = SkinBackgroundContext();
            if (cc)
                {
                TAknLayoutRect topLeft;
                topLeft.LayoutRect(aRect, SkinLayout::List_highlight_skin_placing__general__Line_2());
                
                TAknLayoutRect bottomRight;
                bottomRight.LayoutRect(aRect, SkinLayout::List_highlight_skin_placing__general__Line_5());
                
                TRect outerRect(topLeft.Rect().iTl, bottomRight.Rect().iBr);
                TRect innerRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);
                aGc.SetPenStyle(CGraphicsContext::ENullPen);

                
                if ( pressedDown )
                    {
                    highlightDrawn = iExtension->DrawPressedDownEffect( aSkin, aGc, outerRect, innerRect );
                    //view->ItemDrawer()->ClearFlags( CListItemDrawer::EItemPressedDown );
                    }
                else
                    {
                    highlightDrawn = AknsDrawUtils::DrawFrame( aSkin,
                                                           aGc,
                                                           outerRect,
                                                           innerRect,
                                                           *iExtension->iSkinHighlightFrameId,
                                                           *iExtension->iSkinHighlightFrameCenterId );
                    }
                }
            }
        
        // Both animated highlight and normal highlight drawing have failed.
        if (!highlightDrawn)
            {
            TAknLayoutRect highlightshadow;
            TAknLayoutRect highlight;
            highlightshadow.LayoutRect(aRect, AKN_LAYOUT_WINDOW_List_pane_highlight_graphics__various__Line_1(aRect));
            highlight.LayoutRect(aRect, AKN_LAYOUT_WINDOW_List_pane_highlight_graphics__various__Line_2(aRect));
            highlightshadow.DrawRect(aGc);
            highlight.DrawRect(aGc);
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST    
        if ( transApi )
            {
            aGc.CancelClippingRect();
            transApi->StopDrawing();
            transApi->EndRedraw( MAknListBoxTfxInternal::EListHighlight );
            }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
        }
    }

    
void CPbk2ContactViewCustomListBoxData::BitBltColored( CWindowGc&      aGc,
                                        TBool           aHighlight,
                                        const CGulIcon* aIcon,
                                        TInt            aSubcell,
                                        TBool           aColorIcon,
                                        const TRect&    aGraphicRect ) const
    {
    // see also eikfrlbd.cpp ( sigh ).
    CFbsBitmap* bitmap( aIcon->Bitmap() );
    CFbsBitmap* mask( aIcon->Mask() );

    // possibly colorskinnable icon. Check must be after SetSize()
    TBool bw( bitmap->DisplayMode() == EGray2 ); 
    
    // center graphics
    TSize size=bitmap->SizeInPixels();
    TInt yDiff = ( aGraphicRect.Height() - size.iHeight ) / 2;
    TInt xDiff = ( aGraphicRect.Width()  - size.iWidth )  / 2;
    TPoint posInBitmap( 0,0 );

    if (xDiff < 0) // icon's width bigger than subcell's width
        {          // possible, if icon is not a aknicon
        posInBitmap.iX = -xDiff;
        xDiff = 0;
        }
    
    if (yDiff < 0) // icon's height bigger than subcell's height
        {
        posInBitmap.iY = -yDiff;
        yDiff = 0;
        }
    
    TPoint bmpPos( aGraphicRect.iTl + TPoint( xDiff, yDiff ) );
    TRect sourcerect( posInBitmap, aGraphicRect.Size() );

    if ( mask )
        {
        TInt i( 0x01 );
        i = i << aSubcell;
        if ( ((i & iExtension->iConditionalCells) && bw) || aColorIcon  )
            {
            aGc.BitBltMasked( bmpPos,
                              aHighlight ? iExtension->iHiliBmp : iExtension->iColorBmp,
                              TRect(posInBitmap, size), mask, ETrue );
            }
        else
            {
            aGc.BitBltMasked( bmpPos, bitmap, sourcerect, mask, ETrue );
            }
        }
    else
        {
        aGc.BitBlt( bmpPos, bitmap ,sourcerect );
        }
    }

void CPbk2ContactViewCustomListBoxData::DrawSimpleL( const TListItemProperties& aItemProperties,
                                     CWindowGc& aGc,
                                     const TDesC* aText,
                                     const TRect& aRect,
                                     TBool aHighlight,
                                     const TColors& aColors,
                                     TInt aItemIndex ) const
    {
    TInt lastColumn = Min( LastColumn(), KMaxColumn );
    TInt column=0;
    TPtrC text;
    TPtrC tempText;

    TRgb aTextColor = aHighlight ? aColors.iHighlightedText : aColors.iText;

    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
    MAknsControlContext *cc = AknsDrawUtils::ControlContext( Control() );
    if (!cc)
        {
        cc = SkinBackgroundContext();
        }

    TAknTextLineLayout textLines[KMaxColumn];
    TBool rectClipped[KMaxColumn];
    
    Mem::FillZ( &rectClipped[0], KMaxColumn * sizeof( TBool ) );
    
    if ( iExtension->iSubCellsMightIntersect )
        {
        CheckIfSubCellsIntersectL( &textLines[0], &rectClipped[0], *aText, aRect );
        }

    CEikListBox* listbox = static_cast<CEikListBox*>( iExtension->iControl );
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( &aGc );
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    if ( !listbox || !listbox->BackgroundDrawingSuppressed() )
        {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        if ( transApi )
            {
            transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        aGc.SetPenStyle(CGraphicsContext::ESolidPen);

        if ( listbox )
            {
            AknsDrawUtils::Background( skin, cc, listbox, aGc, aRect );
            }
        else
            {
            aGc.Clear(aRect);
            }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST   
        if ( transApi )
            {
            transApi->StopDrawing();
            }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
        }

    DrawHighLight( aGc, aRect, aHighlight, skin );
    
    // The column draw loop
    column = 0;
    TInt subCellIndex = 0;

    if ( !iExtension ) { return; }
    
#ifdef RD_UI_TRANSITION_EFFECTS_LIST   
    if ( transApi )
        {
        transApi->StartDrawing( MAknListBoxTfxInternal::EListItem );
        CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
        aGc.SetClippingRect( view->ViewRect() );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    
    for( column = 0; column <= lastColumn; column++ )
        {
        TextUtils::ColumnText( text, column, aText );
        if ( text == KNullDesC ) { continue; }

        if ( iExtension->FindSLSubCellIndex( subCellIndex, column )!=0 ) { continue; }
        
        CGraphicsContext::TTextAlign align=ColumnAlignment(column);
        TBool isLayoutAlignment = iExtension->ColumnLayoutAlignment(column);
        
        if ( iExtension->AtSL(subCellIndex).iSubCellType == CPbk2ContactViewCustomListBoxDataExtension::EEikSLText )
            {
            aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
            
            TRgb textColor( aTextColor );
            
            if (aHighlight)
                {
                textColor = aColors.iHighlightedText;
                aGc.SetBrushColor(aColors.iHighlightedBack);    
                }
            if (AknsUtils::AvkonSkinEnabled())
                {
                if (iExtension->iTextColor != NULL)
                    {
                    textColor = iExtension->iTextColor;
                    }
                
                if (aHighlight && iExtension->iHighlightedTextColor != NULL)
                    {
                    textColor = iExtension->iHighlightedTextColor;
                    }
                }
                
            const CFont* rowAndColFont=RowAndColumnFont(iExtension->iCurrentRow,column);
            const CFont* colFont=Font(aItemProperties, column);
            const CFont* tempFont=(colFont) ? colFont : NULL;
            const CFont* usedFont=(rowAndColFont) ? rowAndColFont : tempFont;
            
            TAknTextLineLayout textLineLayout = NULL;

            if ( rectClipped[column] )
                {
                textLineLayout = textLines[column];
                }
            else
                {
                // check if there are icons affecting this text layout
                TInt gSC = iExtension->AtSL(subCellIndex).iConditionValue; // graphical subcell which might affect this text subcell
                
                if (gSC > -1)
                    {
                    TInt tempIndex;
                    while (gSC > -1) // when gSC == -1, we have found our graphical subcell
                        {
                        if (iExtension->FindSLSubCellIndex(tempIndex,gSC)!=0) break;
                        TextUtils::ColumnText(tempText,gSC, aText);
                        if (tempText != KNullDesC)
                            {
                            textLineLayout = iExtension->AtSL(tempIndex).iTextLayout;
                            break;                      
                            }
                        gSC = iExtension->AtSL(tempIndex).iConditionValue;
                        }
                    }
                    
                if (gSC == -1) // no affecting icons -> use default layout
                    {
                    textLineLayout = iExtension->AtSL(subCellIndex).iTextLayout;
                    }
                }
            
  
            if( !isLayoutAlignment )
                { 
                switch(align) 
                    {
                    case CGraphicsContext::ELeft : 
                        {
                        textLineLayout.iJ = ELayoutAlignLeft;    
                        }
                        break; 
                    case CGraphicsContext::ECenter: 
                        {
                        textLineLayout.iJ = ELayoutAlignCenter;
                        }
                        break; 
                    case CGraphicsContext::ERight:  
                        {
                        textLineLayout.iJ = ELayoutAlignRight;
                        }
                        break; 
                    default:  break;
                    };
                }
            
            TAknLayoutText textLayout;
            textLayout.LayoutText( aRect, textLineLayout, usedFont );
            TRect layoutedRect( aRect );
                          
            // *2 == leave some room for marquee
            const TInt maxlen( KMaxColumnDataLength * 2 ); 
            TBuf<maxlen> convBuf = text.Left(maxlen);

            // Note that this potentially modifies the text so its lenght in pixels
            // might increase. Therefore, this should always be done before
            // wrapping/clipping text. In some cases, WordWrapListItem is called
            // before coming here. Is it certain that it is not done for number subcells?

            // Do number conversion if required.
            if ( ColumnIsNumber( column ) )
                {
                AknTextUtils::LanguageSpecificNumberConversion( convBuf );
                }

            // Check whether logical to visual conversion should be done here or not.
            TBool bidiConv = ETrue;  // TODO

//             if (ColumnIsNumber(column))
//                 {
//                 AknTextUtils::LanguageSpecificNumberConversion( convBuf );
//                }

            // marquee & clipping detection
            TBool doesNotFit( aHighlight &&
                              textLayout.Font()->TextWidthInPixels( convBuf ) > textLayout.TextRect().Width() );

            // iClippedColumns was cleared in ::Draw() so no need to
            // clear anything here
            if ( doesNotFit )
                {
                iExtension->iClippedColumns |= ( 1 << column );
                }

            TBool marqueeDisabled =ETrue;
            if(listbox)
                {
                marqueeDisabled = listbox->View()->ItemDrawer()->Flags()
                    & CListItemDrawer::EDisableMarquee;
                }
            
            if ( iExtension->IsMarqueeOn() && doesNotFit && !marqueeDisabled )
                {
                iExtension->iMarquee->UseLogicalToVisualConversion( bidiConv );
                
                if ( iExtension->iMarquee->DrawText( aGc, aRect, textLineLayout, convBuf, usedFont, textColor ) )
                    {
                    // all the loops have been executed
                    textLayout.DrawText( aGc, convBuf, bidiConv, textColor );
                    }
                }
            else
                {
                if ( iExtension->iMarquee && marqueeDisabled )
                    {
                    iExtension->iMarquee->Stop();
                    }

//Start of Code Added for PhoneBook2
                TBool isContactItem = IsContactAtListboxIndex( aItemIndex );
                if ( iSearchFilter.IsPredictiveActivated() && isContactItem )
                    {
                    DrawEnhancedTextL( aItemProperties, aGc, aRect, convBuf, text, 
                            textLayout, textLineLayout, bidiConv, usedFont, textColor );                
                    }
                else
                    {
                    textLayout.DrawText( aGc, convBuf, bidiConv, textColor );
                    }
                
//End of Code Added for PhoneBook2
                }
            
            if ( iExtension->iPictoInterface )
                {                
                TRect  pictoRect = textLayout.TextRect();
                pictoRect.Normalize();
                if ( convBuf.Length() && bidiConv )
                    {                      
                    TInt maxWidth = pictoRect.Size().iWidth; 
                    TInt charsCanBeDisplayed = textLayout.Font()->TextCount( 
                                                convBuf, maxWidth );  
                    if ( charsCanBeDisplayed < convBuf.Length() )
                        {
                        TInt clipCharWidth = textLayout.Font()->CharWidthInPixels( 
                                                KEllipsis /*aClipChar*/ ); 
                         // Check how many characters fit in given space with truncation char. 
                         charsCanBeDisplayed = textLayout.Font()->TextCount( 
                         convBuf, maxWidth - clipCharWidth );

                        // This is "avkon rule": should not insert ellipsis right after a space.
                        if ( charsCanBeDisplayed > 1 && 
                             convBuf[charsCanBeDisplayed - 1] == ' ' &&
                             convBuf[charsCanBeDisplayed - 2] != ' ' )
                            {
                            charsCanBeDisplayed--;
                            }

                        TInt pictoRectWidth = textLayout.Font()->TextWidthInPixels( 
                        convBuf.Left( charsCanBeDisplayed ) );
                        pictoRect.SetWidth( pictoRectWidth );
                        } 
                    } 

               
                iExtension->iPictoInterface->Interface()->DrawPictographsInText(
                    aGc, *usedFont, convBuf,
                    pictoRect,
                    usedFont->AscentInPixels(),
                    textLayout.Align(), 0 );
                }

            aGc.SetUnderlineStyle(EUnderlineOff); // disable underline after first text.
            }
        else if ( iExtension->AtSL(subCellIndex).iSubCellType == CPbk2ContactViewCustomListBoxDataExtension::EEikSLGraphic ) 
            {
            TLex lex(text);
            TInt index(0);
			TInt err( lex.Val(index) );
            __ASSERT_DEBUG(err == KErrNone,Panic(EPanicColumnListInvalidBitmapIndexListBoxData));

            TBool colorIcon( EFalse );
            if ( index == KColorIconFlag )
                {
                if ( iExtension->iColorBmp && iExtension->iHiliBmp )
                    {
                    colorIcon = ETrue;
                    }
                index = KColorIconIdx;
                }

            if ( !IconArray() )
                {
                continue;
                }

            if (aHighlight && (index > 0xffff))
                {
                index = index >> 16; // we have different icon for highlight
                }

             index = index & 0xffff; // mask off possible highlight icon
            __ASSERT_DEBUG((index>=0 && index<IconArray()->Count()),Panic(EPanicColumnListInvalidBitmapIndexListBoxData));
            
            TAknWindowLineLayout graphicLayout = iExtension->AtSL(subCellIndex).iGraphicLayout;
            TAknLayoutRect graphicRect; 
            
            graphicRect.LayoutRect(aRect,graphicLayout);
            TSize graphicSize( graphicRect.Rect().Size() );
            
            if (index>=0 && IconArray())
                {
                CGulIcon* icon=(*IconArray())[index];
                CFbsBitmap* bitmap=icon->Bitmap();
                // Sometimes we get fake bitmap...
                if ( bitmap )
                    {
                    TSize size( bitmap->SizeInPixels() );
                    
                    if (size.iWidth != graphicSize.iWidth &&
                        size.iHeight != graphicSize.iHeight )
                        {
                        AknIconUtils::SetSize( bitmap,
                                               graphicSize,
                                               EAspectRatioPreservedAndUnusedSpaceRemoved );
                        }

                    BitBltColored( aGc, aHighlight, icon, column, colorIcon, graphicRect.Rect() );
                    }
                }
            aGc.SetPenStyle(CGraphicsContext::ESolidPen);
            }
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        aGc.CancelClippingRect();
        transApi->StopDrawing();
        }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
    }


void CPbk2ContactViewCustomListBoxData::Draw( const TListItemProperties& aItemProperties,
                               CWindowGc& aGc,
                               const TDesC* aText,
                               const TRect& aRect,
                               TBool aHighlight,
                               const TColors& aColors,
                               TInt aItemIndex ) const
    {
    if ( aHighlight )
        {
        iExtension->iClippedColumns = 0;
        }

    if ( UsesScalableLayoutData() )
        {
        TRAP_IGNORE ( DrawSimpleL( aItemProperties, aGc, aText, aRect, aHighlight, aColors, aItemIndex ) );
        return;
        }
    
    const CFont* font=ColumnFont(0);
    CEikonEnv* eikonEnv=CEikonEnv::Static();
    if (font==NULL)
        {
        font=eikonEnv->NormalFont();
        }

    TInt lastColumn=LastColumn();
    if (lastColumn==KErrNotFound)
        {
        if ( font )
            {
            // use draw text so that dont need to change pen color/style
            aGc.UseFont( font );
            aGc.DrawText(TPtrC(),aRect,0);
            aGc.DiscardFont();
            }
        return;
        }

    TInt extraVerticalSpace=(aRect.Height()-font->HeightInPixels());
    TInt baseLineOffset=extraVerticalSpace/2+font->AscentInPixels();
    TRect textRect( aRect );
    TRect textMRect; // textrect with margins.
    textRect.iBr.iX=aRect.iTl.iX;
    TInt column=0;
    TPtrC text;

    TRgb aTextColor = aHighlight ? aColors.iHighlightedText : aColors.iText;

    // Turned the drawing to multipass algorithm, because optionalcolumns drawing will
    // be too difficult to do without - and this is cleaner solution.
    
    // pass 1, figures out x-coordinates for columns.
    TInt x = aRect.iTl.iX;
    TBool mirror = AknLayoutUtils::LayoutMirrored();
    if (mirror)
        {
        x = aRect.iBr.iX;
        }
    TInt maxx = x;
    while(column <= lastColumn)
        {
        // order of instructions is important here, do not change.
        if (!ColumnIsOptional(column)) 
            {
            TRAP_IGNORE(SetColumnXL(column,maxx));

            if (!mirror)
                x = maxx + ColumnWidthPixel(column);
            else
                x = maxx - ColumnWidthPixel(column);

            maxx = x;
            }
        else
            {
            if (!mirror)
                x -= ColumnWidthPixel(column);
            else
                x += ColumnWidthPixel(column);

            TRAP_IGNORE(SetColumnXL(column,x));
            }
        column++;
        }

    // pass 2, figures out the end x coordinates
    column = 0;
    TInt endx;
    TInt tmpcolumn;
    while(column <= lastColumn)
        {
        if (!mirror)
            endx = ColumnX(column) + ColumnWidthPixel(column);
        else
            endx = ColumnX(column) - ColumnWidthPixel(column);
        
        if (!ColumnIsOptional(column)) 
            {
            tmpcolumn = column+1;
            while(ColumnIsOptional(tmpcolumn))
                {
                TextUtils::ColumnText(text,tmpcolumn,aText);
                if (text!=KNullDesC) endx = ColumnX(tmpcolumn);
                tmpcolumn++;
                }
            }
        if (!mirror) 
            {
            __ASSERT_DEBUG(ColumnX(column)<endx,Panic(EPanicColumnListLayoutErrorListBoxData));
            }
        else
            {
            __ASSERT_DEBUG(ColumnX(column)>endx,Panic(EPanicColumnListLayoutErrorListBoxData));
            }
        TRAP_IGNORE(SetColumnEndXL(column,endx));
        
        column++;
        }

    // pass 3 drawing

    // SERIES60 Background drawing.
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
    MAknsControlContext *cc = AknsDrawUtils::ControlContext( Control() );
    if (!cc)
        {
        cc = SkinBackgroundContext();
        }

    // aRect is list item's rect, width of which is smaller than width of the list
    // with a scroll bar. List needs to draw under scroll bar anyway, so we need to
    // modify given rect here.
    TRect r(aRect);
    
    CEikListBox* listbox = static_cast<CEikListBox*>( iExtension->iControl );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST

    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( &aGc );

#endif // RD_UI_TRANSITION_EFFECTS_LIST

    if ( !listbox || !listbox->BackgroundDrawingSuppressed() )
        {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        if ( transApi )
            {
            transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
            }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        aGc.SetPenStyle(CGraphicsContext::ESolidPen);

        if ( listbox )
            {
            AknsDrawUtils::Background( skin, cc, listbox, aGc, aRect );
            }
        else
            {
            aGc.Clear(aRect);
            }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST   
        if ( transApi )
            {
            transApi->StopDrawing();
            }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
        }
    
 // LISTBOX LINES NEED TO BE DRAWN HERE.
    DrawHighLight( aGc, aRect, aHighlight, skin );

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        transApi->StartDrawing( MAknListBoxTfxInternal::EListItem );
        CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
        aGc.SetClippingRect( view->ViewRect() );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    // The column draw loop
    column = 0;
    FOREVER
        {
        if (column > lastColumn) break;

        if (!mirror)
            {
            textRect.iTl.iX = ColumnX(column);
            textRect.iBr.iX = ColumnEndX(column);
            }
        else
            {
            textRect.iBr.iX = ColumnX(column);
            textRect.iTl.iX = ColumnEndX(column);
            }

        TextUtils::ColumnText(text,column,aText);

        TBool exit_condition = textRect.iBr.iX==textRect.iTl.iX;
        if (ColumnIsOptional(column) && text == KNullDesC)
            {
            exit_condition = ETrue;
            }

        if (!exit_condition)
            {
            // Margins support.
            TMargins m = ColumnMargins(column);
            textMRect.SetRect(textRect.iTl+TSize(m.iLeft,m.iTop),textRect.Size()-TSize(m.iRight+m.iLeft,m.iBottom+m.iTop));
            // end of margins support.
            
            CGraphicsContext::TTextAlign align=ColumnAlignment(column);

            if (!ColumnIsGraphics(column))
                {

                //if (align == CGraphicsContext::ECenter || align == CGraphicsContext::ERight)
                //    {
                //    textMRect.iTl.iX+=1;
                //    textMRect.iBr.iX+=1;
                //    }
                aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
                aGc.SetPenStyle(CGraphicsContext::ESolidPen);
                aGc.SetPenColor(aTextColor);
                SetUnderlineStyle( aItemProperties, aGc, column );

                if (aHighlight)
                    {
                    aGc.SetPenColor(aColors.iHighlightedText);
                    aGc.SetBrushColor(aColors.iHighlightedBack);    
                    }

                if (AknsUtils::AvkonSkinEnabled() && iExtension)
                    {
                    if (iExtension->iTextColor != NULL)
                        {
                        aGc.SetPenColor(iExtension->iTextColor);
                        }

                    if (aHighlight && iExtension->iHighlightedTextColor != NULL)
                        {
                        aGc.SetPenColor(iExtension->iHighlightedTextColor);
                        }
                    }

                const CFont* rowAndColFont=RowAndColumnFont(iExtension->iCurrentRow,column);
                const CFont* colFont=Font(aItemProperties, column);
                const CFont* tempFont=(colFont) ? colFont : font;
                const CFont* usedFont=(rowAndColFont) ? rowAndColFont : tempFont;

                aGc.UseFont(usedFont);

                // baseline calc needed for each column because of margins.
                baseLineOffset = ColumnBaselinePos(column);
                if (!baseLineOffset)
                    {
                    baseLineOffset = (textMRect.Size().iHeight-usedFont->HeightInPixels())/2 + usedFont->AscentInPixels();
                    }
                baseLineOffset -= m.iTop;

                // SERIES60 LAF
                TBuf<200+KAknBidiExtraSpacePerLine> clipbuf;
                
                TInt clipgap = ColumnTextClipGap(column);
                                   
                TBool clipped = AknBidiTextUtils::ConvertToVisualAndClip(
                    text.Left(200), 
                    clipbuf,
                    *usedFont,
                    textMRect.Size().iWidth, 
                    textMRect.Size().iWidth+clipgap);

                if (clipped)
                    {
                    if (!mirror)
                        {
                        textMRect.iBr.iX+=clipgap;
                        }
                    else
                        {
                        textMRect.iTl.iX-=clipgap;
                        }
                    
                    if ( aHighlight )
                        {
                        iExtension->iClippedColumns |= ( 1 << column );
                        }
                    }

                if (ColumnIsNumber(column))
                    {
                    AknTextUtils::LanguageSpecificNumberConversion(clipbuf);
                    }

                // end of SERIES60 LAF

                CAknMarqueeControl* marquee =
                    column == 1 || column == 3 ? iExtension->iMarquee :
                                                 iExtension->i2ndLineMarquee;
                                                 
                TBool marqueeDisabled =
                    listbox->View()->ItemDrawer()->Flags() & CListItemDrawer::EDisableMarquee;

                if ( iExtension->IsMarqueeOn() && aHighlight && clipped && !marqueeDisabled )
                    {
                    // Bidi conversion has already been done.
                    marquee->UseLogicalToVisualConversion( EFalse );
                    
                    if ( marquee->DrawText( aGc, textMRect, text, baseLineOffset, align, *usedFont ) )
                        {
                        // All the loops have been executed -> the text needs to be truncated.
                        aGc.DrawText( clipbuf, textMRect, baseLineOffset, align, 0 );
                        }
                    }
                else
                    {
                    if ( marquee && marqueeDisabled )
                        {
                        marquee->Stop();
                        }

                    aGc.DrawText( clipbuf, textMRect, baseLineOffset, align, 0 );
                    }

                if ( iExtension->iPictoInterface )
                    {
                    iExtension->iPictoInterface->Interface()->DrawPictographsInText(
                        aGc, *usedFont, clipbuf, textMRect, baseLineOffset, align, 0 );
                    }

                aGc.SetUnderlineStyle(EUnderlineOff); // disable underline after first text.
                }
            else
                {
                aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
                if (aHighlight  /*&&  (LastColumn()==0) */ )
                    {
                    aGc.SetBrushColor(aColors.iHighlightedBack);
                    }

                TLex lex(text);
                TInt index;
                TInt err = lex.Val(index);
                __ASSERT_DEBUG(err==KErrNone,Panic(EPanicColumnListInvalidBitmapIndexListBoxData));
                
                if ( index == KColorIconFlag )
                    {
                    index = KColorIconIdx;
                    }

                if ( !err )
                    {
                    if ( !IconArray() )
                        {
                        ++column;
                        continue;
                        }

                    __ASSERT_DEBUG((index>=0 && index<IconArray()->Count()),Panic(EPanicColumnListInvalidBitmapIndexListBoxData));
                    TRect bmpRect;
                    bmpRect.iTl=textMRect.iTl;
                    if (index>=0 && IconArray())
                        {
                        CGulIcon* icon=(*IconArray())[index];
                        
                        if ( iExtension
                             && iExtension->iSubCellIconSize[column] != TSize(0,0) )
                            {
                            TSize wanted( iExtension->iSubCellIconSize[column] );
                            TSize got( icon->Bitmap()->SizeInPixels() );
                            
                            // correct w or h is enough
                            if ( wanted.iWidth != got.iWidth
                                 && wanted.iHeight != got.iHeight )
                                {
                                AknIconUtils::SetSize( icon->Bitmap(),
                                                       wanted,
                                                       EAspectRatioPreservedAndUnusedSpaceRemoved );
                                }
                            }

                        CFbsBitmap* bitmap=icon->Bitmap();
                        
                        TSize size(bitmap->SizeInPixels());
                        
                        // next two lines are SERIES60 additions
                        if (size.iWidth>textMRect.Size().iWidth) size.iWidth = textMRect.Size().iWidth;
                        if (size.iHeight>textMRect.Size().iHeight) size.iHeight = textMRect.Size().iHeight;
                        
                        TPoint bmpPos(textMRect.iTl); // was aRect, I dunno why
                        bmpPos.iY+=(textMRect.Height()-size.iHeight)>>1; // was aRect too...
                        switch (align)
                            {
                            case ELeft:
                                break;
                            case ECenter:
                                bmpPos.iX+=(textMRect.Width()-size.iWidth)>>1;
                                break;
                            case ERight:
                                bmpPos.iX=textMRect.iBr.iX-size.iWidth;
                                break;
                            }
                        bmpRect.SetRect(bmpPos,size);
                        TPoint posInBitmap;
                        posInBitmap += bitmap->SizeInPixels() - textMRect.Size();
                        posInBitmap.iX >>= 1;
                        posInBitmap.iY >>= 1;
                        if (posInBitmap.iX < 0) posInBitmap.iX = 0;
                        if (posInBitmap.iY < 0) posInBitmap.iY = 0;
                        TRect rect(posInBitmap,textMRect.Size());

                        
                        if (icon->Mask())
                            {
                            aGc.BitBltMasked( bmpPos, bitmap, rect, icon->Mask(), ETrue );
                            }
                        else
                            {
                            aGc.BitBlt(bmpPos, bitmap,rect);
                            }
                        }
                    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
                    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
                    }
                }
            }
        
        ++column;
        }
    
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        aGc.CancelClippingRect();
        transApi->StopDrawing();
        }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
    }


void CPbk2ContactViewCustomListBoxData::AddColumnL(TInt aColumn)
//
//    Add a column's worth of data
//
    {
    SColumn column;
    column.iColumn=aColumn;
    column.iWidth=0;
    TMargins m = {0,0,0,0};
    column.iMargins=m;
    column.iBaseFont=NULL;
    column.iActualFontIndex=KNoActualColumnFont;
    column.iGraphics=EFalse;
    column.iNumberColumn = EFalse;
    column.iOptional=EFalse;
    column.iBaseline = 0;
    column.iAlign=CGraphicsContext::ELeft;
    column.iSeparator=CGraphicsContext::ENullPen;
    column.iTextClipGap = 0;
    TKeyArrayFix key(0,ECmpTInt);
    iColumnArray->InsertIsqL(column,key);
    }

CPbk2ContactViewCustomListBoxData::SColumn& CPbk2ContactViewCustomListBoxData::At(TInt aArrayIndex)
//
//    Return a column of data
//
    {
    __ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iColumnArray->Count(),Panic(EPanicPreCond_OutOfRangeListBoxData));
    return(iColumnArray->At(aArrayIndex));
    }

const CPbk2ContactViewCustomListBoxData::SColumn& CPbk2ContactViewCustomListBoxData::At(TInt aArrayIndex) const
//
//    Return a column of data
//
    {
    __ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iColumnArray->Count(),Panic(EPanicPreCond_OutOfRangeListBoxData));
    return(iColumnArray->At(aArrayIndex));
    }

TBool CPbk2ContactViewCustomListBoxData::FindColumnIndex(TInt& aArrayIndex,TInt aColumn) const
//
//    Find a column if it has been set
//
    {
    if (aColumn<0 || iColumnArray->Count()==0)
        return EFalse;
    TKeyArrayFix key(0,ECmpTInt);
    SColumn column;
    column.iColumn=aColumn;

    // Returns zero if key was found, otherwise non-zero
    TInt ret = iColumnArray->FindIsq(column,key,aArrayIndex);
    return (ret ? EFalse: ETrue);
    }

void CPbk2ContactViewCustomListBoxData::FindColumnIndexOrAddL(TInt& aArrayIndex,TInt aColumn)
//
//    Find a column or add it
//
    {
    if (!FindColumnIndex(aArrayIndex,aColumn))
        {
        AddColumnL(aColumn);
        FindColumnIndex(aArrayIndex,aColumn);
        }
    }

TInt CPbk2ContactViewCustomListBoxData::AddActualFontL(const CFont* aBaseFont)
    {
    if (!iNormalFont.iFonts)
        {
        iNormalFont.iFonts=new (ELeave) CArrayPtrFlat<CFont> (1);
        iBoldFont.iFonts=new (ELeave) CArrayPtrFlat<CFont> (1);
        iItalicFont.iFonts=new (ELeave) CArrayPtrFlat<CFont> (1);
        iBoldItalicFont.iFonts=new (ELeave) CArrayPtrFlat<CFont> (1);
        }

    // Reserves extra space for each font array.
    TInt fontCount=iNormalFont.iFonts->Count()+1;
    iNormalFont.iFonts->SetReserveL(fontCount);
    iBoldFont.iFonts->SetReserveL(fontCount);
    iItalicFont.iFonts->SetReserveL(fontCount);
    iBoldItalicFont.iFonts->SetReserveL(fontCount);

    // Add Fonts.
    TFontStyle style=aBaseFont->FontSpecInTwips().iFontStyle;
    ConstructFontL(aBaseFont, style, iNormalFont);
    ConstructFontL(aBaseFont, style, iBoldFont);
    ConstructFontL(aBaseFont, style, iItalicFont);
    ConstructFontL(aBaseFont, style, iBoldItalicFont);

    return fontCount-1;
    }

TBool CPbk2ContactViewCustomListBoxData::MakeColumnRect( TInt aColumn, TRect& aRect )
    // Returns given column rect
    {
    TInt lastColumn = LastColumn();
    if ( ( aColumn < 0 ) || ( aColumn > lastColumn ) ) return EFalse;

    TRect colRect(aRect);  // we need to save original item rect
    for ( TInt column=0; column<lastColumn; column++ )
        // loop throuhg columns except last one (scroll bar eats small rect from it)
        {
        TInt colWidth = ColumnWidthPixel( column );
        colRect.iBr.iX = colRect.iTl.iX + colWidth;     // set column rect width
        if ( column == aColumn ) break;              // if the column, quit
        aRect.iTl.iX += colWidth;               // shrink origial rect by column with
        colRect = aRect;                            // set colRect for the LAST column
        }
    aRect = colRect;

    return ETrue;
    }

void CPbk2ContactViewCustomListBoxData::SetColumnFontForRowL(TInt aRow,
                                                       TInt aColumn,
                                                       const CFont* aFont)
    {
    if (!iExtension) return;
    TInt index = 0;
    iExtension->FindRowAndColumnIndexOrAddL(index,aRow,aColumn);
    iExtension->At(index).iFont=aFont;
    }

const CFont* 
CPbk2ContactViewCustomListBoxData::RowAndColumnFont(TInt aRow,TInt aColumn) const
    {
    if (!iExtension) return 0;
    TInt index = 0;
    if (iExtension->FindRowAndColumnIndex(index,aRow,aColumn)!=0)
        return 0;
    return(iExtension->At(index).iFont);
    }

void CPbk2ContactViewCustomListBoxData::SetCurrentItemIndex(TInt aIndex)
    {
    if( iExtension->iCurrentRow != aIndex )
        {
        iExtension->iAnimFlags.Set( CPbk2ContactViewCustomListBoxDataExtension::EFlagUpdateBg );
        }
    iExtension->iCurrentRow = aIndex;
    }


// ---------------------------------------------------------------------------
// Wraps the text drawing functinality with & without marquee into one
// function, so that inherited classes can modify their own drawing
// behaviour more easily.
// ---------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::DrawText(
    CWindowGc& aGc,
    const TRect& aTextRect,
    const TDesC& aText,
    const TDesC& aClippedText,
    const TInt aBaselineOffset,
    const CGraphicsContext::TTextAlign aAlign,
    const CFont& aFont,
    const TBool aHighlight,
    const TBool aIsTextClipped ) const
    {
    TBool marqueeDisabled =
        static_cast<CEikListBox*>(
            Control() )->View()->ItemDrawer()->Flags() & CListItemDrawer::EDisableMarquee;

    if ( iExtension->IsMarqueeOn() && aHighlight && aIsTextClipped && !marqueeDisabled )
        {
        if ( iExtension->iMarquee->DrawText( aGc, aTextRect, aText, aBaselineOffset, aAlign, aFont ) )
            {
            // All the loops have been executed -> the text needs to be truncated.
            aGc.DrawText( aClippedText, aTextRect, aBaselineOffset, aAlign, 0 );    
            }
        }
    else
        {
        if ( iExtension->iMarquee && marqueeDisabled )
            {
            iExtension->iMarquee->Stop();
            }

        aGc.DrawText( aClippedText, aTextRect, aBaselineOffset, aAlign, 0 );
        }                           
    }


void CPbk2ContactViewCustomListBoxData::SetMarqueeParams(const TInt aLoops, const TInt aScrollAmount, const TInt aScrollDelay)
    {
    if ( iExtension->iMarquee )
        {
        iExtension->iMarquee->SetLoops( aLoops );
        iExtension->iMarquee->SetSpeedInPixels( aScrollAmount );
        iExtension->iMarquee->SetDelay( aScrollDelay );
        }
    if ( iExtension->i2ndLineMarquee )
        {
        iExtension->i2ndLineMarquee->SetLoops( aLoops );
        iExtension->i2ndLineMarquee->SetSpeedInPixels( aScrollAmount );
        iExtension->i2ndLineMarquee->SetDelay( aScrollDelay );
        }
    }
    
void CPbk2ContactViewCustomListBoxData::SetMarqueeParams(const TInt aLoops, const TInt aScrollAmount, 
                                                   const TInt aScrollDelay, const TInt aInterval)
    {
    if ( iExtension->iMarquee )
        {
        iExtension->iMarquee->SetInterval( aInterval );
        iExtension->i2ndLineMarquee->SetInterval( aInterval );
        SetMarqueeParams( aLoops, aScrollAmount, aScrollDelay );
        }
    }    



void CPbk2ContactViewCustomListBoxData::SetSubCellIconSize(TInt aIndex, TSize aSize)
    {
    if (iExtension && aIndex < 6 && aIndex >= 0)
        {
        iExtension->iSubCellIconSize[aIndex] = aSize;
        }
    }

TSize CPbk2ContactViewCustomListBoxData::GetSubCellIconSize(TInt aIndex)
    {
    if (iExtension && aIndex < 6 && aIndex >= 0)
        {
        return iExtension->iSubCellIconSize[aIndex];
        }
    return TSize(0,0);
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::HighlightAnim
// -----------------------------------------------------------------------------
//
const CAknsEffectAnim* CPbk2ContactViewCustomListBoxData::HighlightAnim() const
    {
    if( iExtension )
        return iExtension->iAnimation;
    return NULL;
    }

// -----------------------------------------------------------------------------
// AboutToDrawHighlightAnim
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::AboutToDrawHighlightAnim() const
    {
    if( !iExtension )
        {
        return;
        }
    if( !iExtension->iAnimation || !iExtension->iControl )
        {
        return;
        }

    CEikListBox* list = static_cast<CEikListBox*>( iExtension->iControl );
    CListBoxView* view = list->View();
    if( !view )
        {
        return;
        }

    if( view->CurrentItemIndex() != iExtension->iCurrentRow )
        {
        iExtension->iAnimFlags.Set( CPbk2ContactViewCustomListBoxDataExtension::EFlagUpdateBg );
        iExtension->iCurrentRow = view->CurrentItemIndex();
        }

    iExtension->SyncAnim( iExtension->iAnimSize );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetHighlightAnimBackgroundDrawer
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetHighlightAnimBackgroundDrawer(
    MColumnListBoxAnimBackgroundDrawer* aDrawer )
    {
    if( iExtension )
        {
        iExtension->iHighlightBgDrawer = aDrawer;
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetItemCellSize
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetItemCellSize( const TSize& aSizeInPixels )
    {
    if( iExtension )
        {
        if( iExtension->iAnimSize != aSizeInPixels )
            {
            iExtension->Play();
            }
        iExtension->iAnimSize = aSizeInPixels;
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::HasHighlightAnim
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxData::HasHighlightAnim() const
    {
    if( !iExtension )
        return EFalse;
    if( !iExtension->iAnimation )
        return EFalse;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::DrawHighlightAnim
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxData::DrawHighlightAnim(
        CBitmapContext& aGc, const TRect& aRect ) const
    {
    if( !iExtension )
        return EFalse;

    if( !iExtension->iAnimation )
        return EFalse;

    return iExtension->SyncAndDrawAnim( aGc, aRect );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::FocusGained
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::FocusGained()
    {
    iExtension->FocusGained();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::FocusLost
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::FocusLost()
    {
    iExtension->FocusLost();
    }

void CPbk2ContactViewCustomListBoxData::HandleResourceChange( TInt aType )
    {
    // Animation is skin dependent, whenever skin changes animation changes
    // too.
    if( KAknsMessageSkinChange == aType )
        {
        iExtension->SkinChanged();
        }
    else if(aType == KEikDynamicLayoutVariantSwitch)
        {
        // What is under highlight may have changed -> we need to update
        // highlight background to animation.
        iExtension->iAnimFlags.Set( CPbk2ContactViewCustomListBoxDataExtension::EFlagUpdateBg );
        }
    }

void CPbk2ContactViewCustomListBoxData::SetupSkinContextL()
    {
    __ASSERT_DEBUG( iExtension, Panic( EPanicNullPointerListBoxData ));

    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    
    TRect mainPaneRect( mainPane.Size() );
    
    if ( !iExtension->iSkinControlContext )
        {
        iExtension->iSkinControlContext = CAknsListBoxBackgroundControlContext::NewL(
            KAknsIIDQsnBgAreaMainListGene,
            mainPaneRect,
            EFalse,
            KAknsIIDQsnBgColumnA,
            mainPaneRect );

        iExtension->iSkinHighlightFrameId = &KAknsIIDQsnFrList;
        iExtension->iSkinHighlightFrameCenterId = &KAknsIIDQsnFrListCenter;
        }
    else
        {
        iExtension->iSkinControlContext->SetRect( mainPaneRect );
        }
    }

void CPbk2ContactViewCustomListBoxData::SetESSTextColor(TRgb aTextColor)
    {
    if (iExtension)
        iExtension->iTextColor = aTextColor;
    }

void CPbk2ContactViewCustomListBoxData::SetESSHighlightedTextColor(TRgb aHighlightedTextColor)
    {
    if (iExtension)
        iExtension->iHighlightedTextColor = aHighlightedTextColor;
    }

void CPbk2ContactViewCustomListBoxData::SetColumnUnderlined( TBitFlags32 aUnderlinedColumns )
    {
    if ( iExtension )
        {
        iExtension->iUnderlineFlagSet = ETrue;
        iExtension->iUnderlineFlags   = aUnderlinedColumns;
        }
    }

void CPbk2ContactViewCustomListBoxData::SetUnderlineStyle( TListItemProperties aProperties,
                                            CWindowGc& aGc,
                                            TInt aColumn ) const
    {
    if ( !iExtension )
        {
        return;
        }

    if ( !iExtension->iUnderlineFlagSet )
        {
        // underlining is already either on or off and
        // hardcoded off turning will ensure old style
        // behaviour
        return;
        }
    
    if ( aProperties.IsUnderlined()
         && iExtension->iUnderlineFlagSet
         && iExtension->iUnderlineFlags.IsSet( aColumn ) )
        {
        aGc.SetUnderlineStyle( EUnderlineOn );
        }
    else
        {
        aGc.SetUnderlineStyle( EUnderlineOff );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::ResetSLSubCellArray
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::ResetSLSubCellArray()
    {
    if ( !iExtension )
        {
        return;
        }

    iExtension->iMarginRect = TRect::EUninitialized;
    if ( iExtension && iExtension->iUseLayoutData )
        {
        iExtension->iUseLayoutData = EFalse;
        CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
        MAknListBoxTfxInternal* transApi =
             CAknListLoader::TfxApiInternal( view->ItemDrawer()->Gc() );
        if ( transApi )
            {
            transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, TPoint( 0, 0 ) );
            transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin, TPoint( 0, 0 ) );
            } 
        }
    iExtension->ResetSLSubCellArray();
    // This function gets called always when size changes, so here is called
    // SetupSkinContextL to update the layout rect of the background skin 
    // context (mainpane rect can change for example if toolbar is hidden.
    TRAP_IGNORE( SetupSkinContextL() );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetGraphicSubCellL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetGraphicSubCellL(TInt aSubCell,
                                                   const TAknWindowLineLayout &aGraphicLayout)       
    {
    if (!iExtension) return;
    TInt index = 0;
    iExtension->FindSLSubCellIndexOrAddL(index,aSubCell);
    iExtension->AtSL(index).iTextLayout=NULL;
    iExtension->AtSL(index).iGraphicLayout=aGraphicLayout;
    iExtension->AtSL(index).iSubCellType=CPbk2ContactViewCustomListBoxDataExtension::EEikSLGraphic;
    
    // needed for compatibility
    CEikColumnListBox* list = static_cast<CEikColumnListBox*>( iExtension->iControl );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( list->View()->ItemDrawer()->ItemCellSize(), aGraphicLayout );
    TRect rect( layoutRect.Rect() );
    
    SetSubCellIconSize( aSubCell, rect.Size() );
    SetColumnWidthPixelL( aSubCell, rect.Width() );
    SetGraphicsColumnL( aSubCell, ETrue );
    SetColumnXL( aSubCell, rect.iTl.iX );
    SetColumnEndXL( aSubCell, rect.iBr.iX );
    iExtension->iUseLayoutData=ETrue; // might go to own method...
    if ( iExtension->iMarginRect == TRect::EUninitialized )
        {
        iExtension->iMarginRect = rect;
        }
    else
        {
        iExtension->iMarginRect.BoundingRect( rect );
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal* transApi =
        CAknListLoader::TfxApiInternal( list->View()->ItemDrawer()->Gc() );
    if ( transApi )
        {
        transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, iExtension->iMarginRect.iTl );
        TSize size = list->View()->ItemDrawer()->ItemCellSize();
        TPoint br( size.AsPoint() - iExtension->iMarginRect.iBr );
        transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin,
            br );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetTextSubCellL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetTextSubCellL(TInt aSubCell,
                                                const TAknTextLineLayout &aTextLayout)
    {
    if (!iExtension) return;
    TInt index = 0;
    iExtension->FindSLSubCellIndexOrAddL(index,aSubCell);
    iExtension->AtSL(index).iTextLayout=aTextLayout;
    iExtension->AtSL(index).iGraphicLayout=NULL;
    iExtension->AtSL(index).iSubCellType=CPbk2ContactViewCustomListBoxDataExtension::EEikSLText;
    
    // needed for compatibility
    CEikColumnListBox* list = static_cast<CEikColumnListBox*>( iExtension->iControl );
    TAknLayoutText textLayout;
    textLayout.LayoutText( list->View()->ItemDrawer()->ItemCellSize(), aTextLayout );
    TRect rect( textLayout.TextRect() );
    
    SetColumnWidthPixelL( aSubCell, rect.Width() );
    SetColumnFontL( aSubCell, textLayout.Font() );
    SetColumnXL( aSubCell, rect.iTl.iX );
    SetColumnEndXL( aSubCell, rect.iBr.iX );
    SetColumnBaselinePosL( aSubCell, aTextLayout.iB );
    iExtension->iUseLayoutData=ETrue; // might go to own method...
    if ( iExtension->iMarginRect == TRect::EUninitialized )
        {
        iExtension->iMarginRect = rect;
        }
    else
        {
        iExtension->iMarginRect.BoundingRect( rect );
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal* transApi =
        CAknListLoader::TfxApiInternal( list->View()->ItemDrawer()->Gc() );
    if ( transApi )
        {
        transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, iExtension->iMarginRect.iTl );
        TSize size = list->View()->ItemDrawer()->ItemCellSize();
        TPoint br( size.AsPoint() - iExtension->iMarginRect.iBr );
        transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin,
            br );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetConditionalSubCellL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetConditionalSubCellL(TInt aSubCell,
                                                         const TAknTextLineLayout &aTextLayout,
                                                         TInt aAffectedSubCell)  
    {
    // iConditionValue of affected subcell (=text subcell, which has different layouts)
    // contains index of graphical subcell, which existence should be checked first.
    // This graphical subcell has in iConditionValue index of graphical subcell,
    // which existence should be checked second etc. Each graphical subcell can
    // affect only to 1 text subcell (or none).

    // for compabitility - needed at least for text wrapping
    SetOptionalColumnL( aSubCell, ETrue );
    if (!iExtension) return;

    TInt i = 0x01;
    i = i << aSubCell;
    iExtension->iConditionalCells = iExtension->iConditionalCells | i;

    TInt graphicalIndex = 0;
    if (iExtension->FindSLSubCellIndex(graphicalIndex, aSubCell)!=0) return; // subcell not found
    // conditional layoutline can be only added to graphical subcells
    if (iExtension->AtSL(graphicalIndex).iSubCellType!=CPbk2ContactViewCustomListBoxDataExtension::EEikSLGraphic) return;
    
    TInt textIndex = 0; // index of affected subcell
    if (iExtension->FindSLSubCellIndex(textIndex, aAffectedSubCell)!=0) return; // subcell not found
    // affected subcell can only be text subcell
    if (iExtension->AtSL(textIndex).iSubCellType==CPbk2ContactViewCustomListBoxDataExtension::EEikSLGraphic) return;
    
    TInt gSC = iExtension->AtSL(textIndex).iConditionValue; // text subcell to be added in priority chain

    while (gSC > -1)
        {
        if (iExtension->FindSLSubCellIndex(textIndex, gSC)!=0) return; // subcell not found
        gSC = iExtension->AtSL(textIndex).iConditionValue;
        }
    iExtension->AtSL(textIndex).iConditionValue = aSubCell; // add next subcell to chain
    iExtension->AtSL(graphicalIndex).iTextLayout=aTextLayout;
    iExtension->CreateColorBitmapsL();

    CEikColumnListBox* list = static_cast<CEikColumnListBox*>( iExtension->iControl );
    TAknLayoutText textLayout;
    textLayout.LayoutText( list->View()->ItemDrawer()->ItemCellSize(), aTextLayout );

    if ( iExtension->iMarginRect == TRect::EUninitialized )
        {
        iExtension->iMarginRect = textLayout.TextRect();
        }
    else
        {
        iExtension->iMarginRect.BoundingRect( textLayout.TextRect() );
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal* transApi =
        CAknListLoader::TfxApiInternal( list->View()->ItemDrawer()->Gc() );
    if ( transApi )
        {
        transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, iExtension->iMarginRect.iTl );
        TSize size = list->View()->ItemDrawer()->ItemCellSize();
        TPoint br( size.AsPoint() - iExtension->iMarginRect.iBr );
        transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin,
            br );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetStretchableGraphicSubCellL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetStretchableGraphicSubCellL(
    TInt aSubCell, 
    const TAknWindowComponentLayout& aNormalLayout, 
    const TAknWindowComponentLayout& aStretchedLayout )
    {
    if ( Layout_Meta_Data::IsLandscapeOrientation() &&
         Layout_Meta_Data::IsListStretchingEnabled() &&
         StretchingEnabled() )
        {
        SetGraphicSubCellL( aSubCell, aStretchedLayout.LayoutLine() );
        }
    else
        {
        SetGraphicSubCellL( aSubCell, aNormalLayout.LayoutLine() );
        }
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetStretchableTextSubCellL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetStretchableTextSubCellL(
    TInt aSubCell, 
    const TAknTextComponentLayout& aNormalLayout, 
    const TAknTextComponentLayout& aStretchedLayout )
    {
    if ( Layout_Meta_Data::IsLandscapeOrientation() &&
         Layout_Meta_Data::IsListStretchingEnabled() &&
         StretchingEnabled() )
        {
        SetTextSubCellL( aSubCell, aStretchedLayout.LayoutLine() );
        }
    else
        {
        SetTextSubCellL( aSubCell, aNormalLayout.LayoutLine() );
        }
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetStretchableConditionalSubCellL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SetStretchableConditionalSubCellL(
    TInt aSubCell,
    const TAknTextComponentLayout& aNormalLayout,
    const TAknTextComponentLayout& aStretchedLayout,
    TInt aNormalSubCell,
    TInt aStretchedSubCell )
    {
    if ( Layout_Meta_Data::IsLandscapeOrientation() &&
         Layout_Meta_Data::IsListStretchingEnabled() &&
         StretchingEnabled() )
        {
        SetConditionalSubCellL( aSubCell, aStretchedLayout.LayoutLine(), aStretchedSubCell );
        }
    else
        {
        SetConditionalSubCellL( aSubCell, aNormalLayout.LayoutLine(), aNormalSubCell );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::UsesScalableLayoutData
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxData::UsesScalableLayoutData() const
    {
    if (iExtension)
        return iExtension->iUseLayoutData;

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::EnableStretching
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::EnableStretching( const TBool aEnabled )
    {
    if ( !iExtension )
        {
        return;
        }
        
    iExtension->iStretchingEnabled = aEnabled;        
    }

#ifdef RD_LIST_STRETCH
// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::StretchingEnabled
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxData::StretchingEnabled() const
    {
    if ( !iExtension )
        {
        return EFalse;
        }
        
    return iExtension->iStretchingEnabled;        
    }

#else
// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::StretchingEnabled
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactViewCustomListBoxData::StretchingEnabled() const
    {
    return EFalse;
    }

#endif // RD_LIST_STRETCH

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::CheckIfSubCellsIntersectL
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::CheckIfSubCellsIntersectL( 
    TAknTextLineLayout* aLayouts, 
    TBool* aResults, 
    const TDesC& aText, 
    const TRect& aItemRect ) const
    {
    TInt column = 0;
    TInt column2;
    TInt subCellIndex;
    TInt subCellIndex2;
    TInt lastColumn = Min( LastColumn(), KMaxColumn );
    TPtrC text;
    TBool isEmpty[KMaxColumn];

    // cache the empty text states
    while ( ETrue )
        {
        if ( column > lastColumn )
            {
            break;
            }

        TextUtils::ColumnText( text, column, &aText );

        if ( text == KNullDesC || ColumnIsOptional( column ) )
            {
            isEmpty[column] = ETrue;
            }
        else
            {
            isEmpty[column] = EFalse;
            }

        ++column;
        }

    column = 0;

    while ( ETrue )
        {
        if ( column > lastColumn )
            {
            break;
            }

        if ( iExtension->FindSLSubCellIndex( subCellIndex, column ) != 0 ) 
            {
            break;
            }
        
        if ( isEmpty[column] )
            {
            ++column;
            continue;
            }
        
        TRect bRect( ColumnX( column ), 0, ColumnEndX( column ), 10 );
        
        for ( column2 = column + 1; column2 <= lastColumn; column2++ )
            {
            if ( isEmpty[column2] )
                {
                continue;
                }

            if ( iExtension->FindSLSubCellIndex( subCellIndex2, column2 ) != 0 ) 
                {
                break;
                }

            TRect bRect2( ColumnX( column2 ), 0, ColumnEndX( column2 ), 10 );

            if ( bRect.Intersects( bRect2 ) ) 
                {
                aResults[column] = ETrue;

                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    bRect.iBr.iX = bRect2.iTl.iX;
                    }
                else
                    {
                    bRect.iTl.iX = bRect2.iBr.iX;
                    }
                }
            }
            
        if ( aResults[column] )
            {
            if ( iExtension->AtSL( subCellIndex ).iSubCellType == CPbk2ContactViewCustomListBoxDataExtension::EEikSLText )
                {
                TAknTextLineLayout textLine = iExtension->AtSL( subCellIndex ).iTextLayout;
                
                textLine.iW = bRect.Width();

                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    textLine.ir = aItemRect.iBr.iX - bRect.iBr.iX;
                    }
                else
                    {
                    textLine.il = bRect.iTl.iX - aItemRect.iTl.iX;
                    }
                
                aLayouts[column] = textLine;
                }
            }

        ++column;
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SubCellsMightIntersect
// -----------------------------------------------------------------------------
//
void CPbk2ContactViewCustomListBoxData::SubCellsMightIntersect( const TBool aMightIntersect )
    {
    iExtension->iSubCellsMightIntersect = aMightIntersect;
    }
    
void CPbk2ContactViewCustomListBoxDataExtension::CreateColorBitmapsL()
    {
    /* size calculation relies on fact, that currently all additional
    * icons in column lists are same size. Size could be calculated
    * from sizes availble in setconditionalsubcelll(), but that would
    * be too unreliable. And there is no way to calculate size of
    * icons created by symbian's old api.
    */
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect r;
    r.LayoutRect( mainPane, AknLayoutScalable_Avkon::listscroll_gen_pane(0) );
    r.LayoutRect( r.Rect(), AknLayoutScalable_Avkon::list_gen_pane(0) );
    r.LayoutRect( r.Rect(), AknLayoutScalable_Avkon::list_single_pane(0) );
    r.LayoutRect( r.Rect(), AknLayoutScalable_Avkon::list_single_pane_g1(0) );
    TSize aSize = r.Rect().Size();
    
    TRgb color, hiliColor;
    TInt error;
    // icon #13 main area   additional list icons   #215
    // icon #16 list highlight  additional list iconsform checkbox, radio button    #215

    error = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                       color,
                                       KAknsIIDQsnIconColors,
                                       EAknsCIQsnIconColorsCG13 );
    if ( error )
        {
        return;
        }
    error = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                       hiliColor,
                                       KAknsIIDQsnIconColors,
                                       EAknsCIQsnIconColorsCG16 );
    if ( error )
        {
        return;
        }
    
     if ( iColorBmp && iColorBmp->SizeInPixels() == aSize
          && color == iIconColor && hiliColor == iHiliIconColor )
         {
         return;
         }

    iIconColor = color;
    iHiliIconColor = hiliColor;

    if ( !iColorBmp )
        {
        iColorBmp = new( ELeave ) CFbsBitmap();
        iColorBmp->Create( aSize, CCoeEnv::Static()->ScreenDevice()->DisplayMode() );
        }
    else if ( iColorBmp->SizeInPixels() != aSize )
        {
        iColorBmp->Resize( aSize );
        }
    if ( !iHiliBmp )
        {
        iHiliBmp = new( ELeave ) CFbsBitmap();
        iHiliBmp->Create( aSize, CCoeEnv::Static()->ScreenDevice()->DisplayMode() );
        }
    else if ( iHiliBmp->SizeInPixels() != aSize )
        {
        iHiliBmp->Resize( aSize );
        }

    CFbsBitGc* fbsBitGc = CFbsBitGc::NewL();
    CleanupStack::PushL( fbsBitGc );

    CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL( iColorBmp );
    CleanupStack::PushL( bmpDevice );
    fbsBitGc->Activate( bmpDevice );
    fbsBitGc->SetPenStyle(CGraphicsContext::ENullPen);
    fbsBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    fbsBitGc->SetBrushColor( color );
    fbsBitGc->Clear();
    CleanupStack::PopAndDestroy( bmpDevice );
    bmpDevice = NULL;
    
    bmpDevice = CFbsBitmapDevice::NewL( iHiliBmp );
    CleanupStack::PushL( bmpDevice );
    fbsBitGc->Activate( bmpDevice );
    fbsBitGc->SetPenStyle(CGraphicsContext::ENullPen);
    fbsBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    fbsBitGc->SetBrushColor( hiliColor );
    fbsBitGc->Clear();
    CleanupStack::PopAndDestroy( bmpDevice );
    bmpDevice = NULL;

    CleanupStack::PopAndDestroy( fbsBitGc );
    }


TUint32 CPbk2ContactViewCustomListBoxData::CurrentItemTextWasClipped() const
    {
    return iExtension ? iExtension->iClippedColumns : 0;
    }

TBool CPbk2ContactViewCustomListBoxData::KineticScrollingEnabled() const
    {
    if ( iExtension )
        {
        return iExtension->iKineticScrolling;       
        }
    return EFalse;
    }

//Start of Code Added for PhoneBook2
// ---------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetDataModel, sets the internal PCS data model.
// ---------------------------------------------------------------------------
void CPbk2ContactViewCustomListBoxData::SetDataModel(CPbk2ContactViewListBoxModel* aDataModel)
    {
    iDataModel = aDataModel;
    iPsHandler = iDataModel->PSHandler();    
    }

// ---------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::DrawEnhancedTextL, sets the internal PCS data model.
// ---------------------------------------------------------------------------
void CPbk2ContactViewCustomListBoxData::DrawEnhancedTextL(
        const TListItemProperties& aItemProperties,
        CWindowGc& aGc,
        const TRect& aRect,
        const TDesC& aTextToDraw,
        const TPtrC& aText,
        TAknLayoutText &aTextLayout,
        TAknTextLineLayout &aTextLineLayout, 
        TBool aUseLogicalToVisualConversion,
        const CFont* aUsedFont,        
        TRgb aTextColor
        ) const
    {    
    TRect layoutedRect( aRect );                
    RArray<TPsMatchLocation> searchMatches;                        
    TInt currentChar(0);
    TInt charactersToNextMatch(0);         
    TBool mirroredInput( EFalse );
    HBufC* visualBuf = NULL;
    TPtrC text( aTextToDraw );
    TBool textClipped (EFalse);
    TPtrC textForPSEngine (aTextToDraw);
    
    // Get current matches
    // Data model should be always initialized in not NULL value
    // This query supports bidirectional order
    CPsQuery* queryData = NULL;
    TRAP_IGNORE(queryData = const_cast<CPsQuery*>(iDataModel->LastPCSQuery()) );
    if (queryData && queryData->Count() > 0)
        {
        
        //Find the clipped text that will fit into the layout
        //we need to perform our underline operation on this clipped text        
        if ( aTextToDraw.Length() )
            {
            visualBuf = HBufC::NewLC( aTextToDraw.Length() + KAknBidiExtraSpacePerLine );
            
            // In OOM, logical to visual conversion is not performed...
            
            if ( visualBuf )
                {
                *visualBuf = aTextToDraw; // copy
                TPtr ptr = visualBuf->Des();                
                TInt maxWidth = aTextLayout.TextRect().Size().iWidth;
                
                // Logical to visual conversion.
                textClipped = AknBidiTextUtils::ConvertToVisualAndClip(
                    aText,
                    ptr,
                    *aUsedFont,
                    maxWidth,
                    maxWidth );
                
                if ( textClipped )
                    {
                    //Remove the ellipsis and pass the text to the PS Engine
                    text.Set( ptr.Left( ptr.Length() - 1 ) );                     
                    }
                else
                    {
                    text.Set( ptr );
                    }
                }
            }
        
        // Check if we have mirrored input
        // It is possible to have mixed input in search string. Always ask repository file to get
        // correct input language
        TInt inputLang = 0;
        CRepository* centRep = NULL;
        TRAPD(leavecode, centRep = CRepository::NewL( KCRUidAknFep ) );
        if( leavecode == KErrNone)
            {
            centRep->Get( KAknFepInputTxtLang, inputLang );
            }
        delete centRep;
        
        mirroredInput = ( inputLang == ELangArabic ||
                          inputLang == ELangHebrew ||
                          inputLang == ELangFarsi);
        
        // We will suggest that memory is enough
        if (mirroredInput)
            {
            // temporary solution
            // PCS LookupLmethod doesn't work correctly for
            // bidirectional languages
            iDataModel->GetMatchingPartsL(text, searchMatches);
            }
        else
            {
            CDesCArray* matchSeq =  new( ELeave ) CDesCArrayFlat(2);
            TRAP_IGNORE(iPsHandler->LookupL( *queryData, text, *matchSeq, searchMatches) );
            delete matchSeq;
            }
        if( searchMatches.Count() )
            {
            TLinearOrder<TPsMatchLocation> byStartIndex( CompareByStartIndex );
            searchMatches.Sort( byStartIndex );
            }
        
    
        if( mirroredInput )
            {
            currentChar = text.Length(); //aTextToDraw.Length();
            //TInt originalLayoutTlX( textMRect.iTl.iX )
            layoutedRect.iTl.iX = layoutedRect.iBr.iX;
            for( TInt i = 0 ; i < searchMatches.Count(); i++ )
                {
    #ifdef _DEBUG                            
                TPsMatchLocation& location = searchMatches[i];
                RDebug::Print(_L("%S: searchMatch[%d] = (%d,%d), direction %d"), &text, i, 
                                                                        location.index, 
                                                                        location.length, location.direction );
    #endif //_DEBUG                                                                                     
                searchMatches[i].index = Min( text.Length(), searchMatches[i].index );
                searchMatches[i].length = Min( text.Length() - searchMatches[i].index, 
                        searchMatches[i].length);
                }                                
            if( !aItemProperties.IsSelectionHidden() )
                {
                for( TInt i=searchMatches.Count()-1 ; i>=0 ; i-- )
                    {
                    charactersToNextMatch = currentChar - ( searchMatches[i].index + searchMatches[i].length );
                    if( charactersToNextMatch > 0 )
                        {                                    
                        layoutedRect.iTl.iX -= AknBidiTextUtils::MeasureTextBoundsWidth( *aUsedFont,
                                text.Mid( currentChar-charactersToNextMatch, 
                                                                             charactersToNextMatch ), 
                                                                CFont::TMeasureTextInput::EFVisualOrder );
                        //aGc.DrawText(convBuf.Mid(currentChar - charactersToNextMatch, charactersToNextMatch),layoutedRect,baseLineOffset, align, 0);
                        aTextLayout.LayoutText( layoutedRect, aTextLineLayout, aUsedFont );
                        aTextLayout.DrawText( aGc, text.Mid(currentChar - charactersToNextMatch, charactersToNextMatch), aUseLogicalToVisualConversion, aTextColor );
                        currentChar -= charactersToNextMatch;
                        }
                    layoutedRect.iTl.iX -= AknBidiTextUtils::MeasureTextBoundsWidth( *aUsedFont,
                            text.Mid( searchMatches[i].index, 
                                                                         searchMatches[i].length ), 
                                                            CFont::TMeasureTextInput::EFVisualOrder );
                    aTextLayout.LayoutText( layoutedRect, aTextLineLayout, aUsedFont );
                    aGc.SetUnderlineStyle( EUnderlineOn );                                       
                    //aGc.DrawText(convBuf.Mid(searchMatches[i].index, searchMatches[i].length),layoutedRect,baseLineOffset, align, 0);
                    aTextLayout.DrawText( aGc, text.Mid(searchMatches[i].index, searchMatches[i].length), aUseLogicalToVisualConversion, aTextColor );
                    aGc.SetUnderlineStyle( EUnderlineOff );
                    currentChar -= searchMatches[i].length;
                    }
                }
            if( currentChar >= 0 )
                {
                layoutedRect.iTl.iX -= AknBidiTextUtils::MeasureTextBoundsWidth( *aUsedFont,       
                                                        text.Left(currentChar),
                                                        CFont::TMeasureTextInput::EFVisualOrder );
                //aGc.DrawText(convBuf.Left(currentChar),layoutedRect,baseLineOffset, align, 0);
                aTextLayout.LayoutText( layoutedRect, aTextLineLayout, aUsedFont );
                aTextLayout.DrawText( aGc, text.Left(currentChar), aUseLogicalToVisualConversion, aTextColor );
                }
            }
        else // mirroredInput
            {
            if( !aItemProperties.IsSelectionHidden() )
                {
                for( TInt i=0 ; i<searchMatches.Count() ; i++ )
                    {
    #ifdef _DEBUG                            
                    TPsMatchLocation& location = searchMatches[i];
                    RDebug::Print(_L("%S: searchMatch[%d] = (%d,%d)"), &text, i, 
                                                                        location.index, 
                                                                        location.length );
    #endif //_DEBUG                                                                                     
                    searchMatches[i].index = Min( text.Length(), searchMatches[i].index );
                    searchMatches[i].length = Min( text.Length() - searchMatches[i].index, 
                                                    searchMatches[i].length );
                        
                    charactersToNextMatch = searchMatches[i].index-currentChar;
                    if( charactersToNextMatch > 0 )
                        {
                        aTextLayout.DrawText( aGc, text.Mid(currentChar, charactersToNextMatch), aUseLogicalToVisualConversion, aTextColor );
                        //aGc.DrawText(convBuf.Mid(currentChar, charactersToNextMatch),layoutedRect,baseLineOffset, CGraphicsContext::ELeft);
                        layoutedRect.iTl.iX += AknBidiTextUtils::MeasureTextBoundsWidth( *aUsedFont,
                                text.Mid( currentChar,charactersToNextMatch ), 
                                                                CFont::TMeasureTextInput::EFVisualOrder );
                        
                        aTextLayout.LayoutText( layoutedRect, aTextLineLayout, aUsedFont );                        
                        currentChar+=charactersToNextMatch;
                        }
                    aGc.SetUnderlineStyle( EUnderlineOn );                                       
                    //aGc.DrawText(convBuf.Mid(searchMatches[i].index, searchMatches[i].length),layoutedRect,baseLineOffset, CGraphicsContext::ELeft);
                    aTextLayout.DrawText( aGc, text.Mid(searchMatches[i].index, searchMatches[i].length), aUseLogicalToVisualConversion, aTextColor );
                    aGc.SetUnderlineStyle( EUnderlineOff );
                    layoutedRect.iTl.iX += AknBidiTextUtils::MeasureTextBoundsWidth( *aUsedFont,
                            text.Mid( searchMatches[i].index, 
                                                                searchMatches[i].length ), 
                                                                CFont::TMeasureTextInput::EFVisualOrder );
                    
                    aTextLayout.LayoutText( layoutedRect, aTextLineLayout, aUsedFont );                    
                    currentChar+=searchMatches[i].length;
                    }
                }
            if( currentChar<text.Length() )
                {
                //aGc.DrawText(convBuf.Mid(currentChar),layoutedRect,baseLineOffset, CGraphicsContext::ELeft);
                aTextLayout.DrawText( aGc, text.Mid(currentChar), aUseLogicalToVisualConversion, aTextColor ); 
                layoutedRect.iTl.iX += AknBidiTextUtils::MeasureTextBoundsWidth( *aUsedFont, text.Mid(currentChar), 
                                                       CFont::TMeasureTextInput::EFVisualOrder );
                aTextLayout.LayoutText( layoutedRect, aTextLineLayout, aUsedFont );
                }
            }
        
        if ( textClipped )
            {
            //Draw the ellipsis
            TBuf<1> buffer;
            buffer.Append( KEllipsis );
            aTextLayout.DrawText( aGc, buffer, EFalse, aTextColor );                    
            }
        searchMatches.Reset();
        
        if ( visualBuf )
            {
            CleanupStack::PopAndDestroy(); //visualBuf
            }
        }                
    else
        {
        aTextLayout.DrawText( aGc, aTextToDraw, aUseLogicalToVisualConversion, aTextColor );
        }    
    }

// ---------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::SetListCommands
// --------------------------------------------------------------------------- 
void CPbk2ContactViewCustomListBoxData::SetListCommands( 
                    const RPointerArray<MPbk2UiControlCmdItem>* aCommands )
    {
    iCommandItems = aCommands;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::IsContactAtListboxIndex
// Tells whether the listbox line contains a contact or not.
// Does not check that the aListboxIndex is within range of listbox.
// --------------------------------------------------------------------------
//    
TBool CPbk2ContactViewCustomListBoxData::IsContactAtListboxIndex( TInt aListboxIndex ) const
    {
    // There can be command items at the top of the list.
    // The command items are not contacts.
    const TInt enabledCommandCount = CommandItemCount();
    return aListboxIndex >= enabledCommandCount;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewCustomListBoxData::CommandItemCount
// --------------------------------------------------------------------------
//
TInt CPbk2ContactViewCustomListBoxData::CommandItemCount() const
    {
    // Some of the commands might be disabled. Don't count those.
    TInt enabledCommandCount = 0;
    if ( iCommandItems )
        {
        for ( TInt n = 0; n < iCommandItems->Count(); ++n ) 
            {
            const MPbk2UiControlCmdItem& cmd = *(*iCommandItems)[n];
            if ( cmd.IsEnabled() )
                {
                enabledCommandCount++;
                }
            }
        }
    return enabledCommandCount;
    }

//End of Code Added for PhoneBook2

// End of File
