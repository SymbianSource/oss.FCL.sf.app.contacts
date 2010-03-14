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
* Description: CCA customized control. Code has been modified to suit 
               CCA requirements. See CFormattedCellListBoxData
*       	   in EIKCLBD.CPP. 
*              Ensure that this piece of code is in sync with Avkon EIKFRLBD.CPP(CFormattedCellListBoxData)
*
*/


#include <eikfrlbd.h>
#include <aknlists.h>
#include <AknMarqueeControl.h>
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>
#include <AknsEffectAnim.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknPanic.h>
#include <AknBidiTextUtils.h>
#include <centralrepository.h>
#include <AvkonInternalCRKeys.h>
#include <gulicon.h>
#include <eikslbd.h>

#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknphysics.h>
#include <AknUtils.h> 
#include <AknFontId.h>
#include <AknLayoutFont.h>

#include "ccappcommlaunchercustomlistboxdata.h"

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif //RD_UI_TRANSITION_EFFECTS_LIST


#include <touchfeedback.h>
// there are 17(!) subcells in qdial grid (0 ... 16)
const TInt KMaxSubCellIndex = 16 + 1;

// colored tick marks support
const TInt KColorIconFlag = -1;
const TInt KColorIconIdx = 0;

/**
 * This class needs to be in .cpp file so that we do not accidentally make it
 * derivable; that would destroy all the binary compability advantages this
 * class has..
 *
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
NONSHARABLE_CLASS(CCCAppCommLauncherCustomListBoxDataExtension) : public CActive,
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
		EFlagUpdateBg = 0,

		// Set if animation has been resized at least once
		EFlagHasLayers = 1,

		// Set if foreground has been gained.
		EFlagHasForeground = 2
		};

	enum TSubCellType
		{
		EAknSLGraphic, EAknSLText, EAknSLNumeric
		};

public:
	CCCAppCommLauncherCustomListBoxDataExtension();
	void ConstructL(CCCAppCommLauncherCustomListBoxData* aListBoxData,
			const TAknsItemID& aAnimationIID);
	~CCCAppCommLauncherCustomListBoxDataExtension();
	void CreatePictographInterfaceL();
	void CreateMarqueeControlL();
	static TInt RedrawEvent(TAny* aControl); // Marquee draw callback
	TBool IsMarqueeOn();
	struct SRowAndSubCell
		{
		TInt iIndex; // 24 bits for row and 8 bits for subcell
		const CFont* iFont;
		//TBool iUnderline;
		//TBool iStrikethrough;
		};
	struct SSLSubCell
		{
		TInt iSubCell; // Must be first entry
		TAknTextLineLayout iTextLayout;
		TAknWindowLineLayout iGraphicLayout;
		TInt iSubCellType;
		TInt iConditionValue; // used with conditional layouts for not always drawn subcells
		};

	SRowAndSubCell& At(TInt aArrayIndex);
	const SRowAndSubCell& At(TInt aArrayIndex) const;
	void AddRowAndSubCellL(TInt aRow, TInt aSubCell);
	TInt
			FindRowAndSubCellIndex(TInt& aArrayIndex, TInt aRow, TInt aSubCell) const;
	void FindRowAndSubCellIndexOrAddL(TInt& aArrayIndex, TInt aRow,
			TInt aSubCell);

	SSLSubCell& AtSL(TInt aArrayIndex);
	const SSLSubCell& AtSL(TInt aArrayIndex) const;
	void AddSLSubCellL(TInt aSubCell);
	TInt FindSLSubCellIndex(TInt& aArrayIndex, TInt aSubCell) const;
	void FindSLSubCellIndexOrAddL(TInt& aArrayIndex, TInt aSubCell);
	void ResetSLSubCellArray();

	inline void NoAnimIfError(TInt aError);
	void TryCreateAnimation();
	TBool SyncAnim(const TSize& aSize);
	TBool SyncAndDrawAnim(CBitmapContext& aGc, const TRect& aRect);
	MAknsControlContext* SkinBackgroundContext() const;
	void DeleteAnim();
	void FocusGained();
	void FocusLost();
	void SkinChanged();
	void SetControl(CCoeControl* aControl);

	// Implementation of MCoeForegroundObserver
	void HandleGainingForeground();
	void HandleLosingForeground();

	// Implementation of MAknsEffectAnimObserver
	void AnimFrameReady(TInt aError, TInt);

	// Implementation of MListBoxItemChangeObserver
	void ListBoxItemsChanged(CEikListBox* aListBox);

	// Overloads CActive::DoCancel
	void DoCancel();
	// Overloads CActive::RunL
	void RunL();

	void Play();

	void CreateColorBitmapsL(TSize aSize);
	void CreateColorBitmapsL();

	TBool DrawPressedDownEffectL(MAknsSkinInstance* aSkin, CWindowGc& aGc,
			const TRect& aOutRect, const TRect& aInnerRect) const;
private:
	// New internal methods
	TBool DrawHighlightBackground(CFbsBitGc& aGc);
	void PostDeleteAnimation();
	void CreateAnimationL();
	void DoResizeL(const TSize& aHighlightSize, TBool aAboutToStart);

public:
	struct TSubCellExt
		{
		TInt iSubCell; // Must be first entry 
		TBool iLayoutAlign;
		};

	TInt AddSubCellExtL(TInt aSubCell);
	void FindSubCellExtIndexOrAddL(TInt& aArrayIndex, TInt aSubCell);

	void SetSubCellLayoutAlignmentL(TInt aSubCellIndex);

	TInt FindSubCellExtIndex(TInt& aArrayIndex, TInt aSubCell) const;
	TBool SubCellLayoutAlignment(TInt aSubCellIndex) const;

private:
	// From MAknPictographAnimatorCallBack
	void DrawPictographArea();

public:
	CCoeControl *iControl; // not owned
	CCCAppCommLauncherCustomListBoxData* iListBoxData; // Not owned
	CAknsListBoxBackgroundControlContext* iSkinControlContext;
	TBool iSkinEnabled;
	const TAknsItemID *iSkinHighlightFrameId;
	const TAknsItemID *iSkinHighlightFrameCenterId;
	TBool iRespectFocus;
	// not used ?    HBufC *iTempTextBuffer;
	TRect iSkinPopupInnerRect;
	TRect iSkinPopupOuterRect;
	CAknsFrameBackgroundControlContext* iPopupFrame;
	TBool iUseLogicalToVisualConversion;
	TInt16 iFirstWordWrappedSubcellIndex;
	TInt16 iSecondWordWrappedSubcellIndex;
	CAknPictographInterface* iPictoInterface;
	CAknMarqueeControl* iMarquee;
	CAknMarqueeControl* i2ndLineMarquee;
	TInt iCurrentItem; // Current list item index that marquee draws/has drawn.
	TInt iCurrentRow; // Current list row being drawn.  // not true - cache of currentitemindex 
	TInt iCurrentlyDrawnItemIndex;
	CArrayFix<SRowAndSubCell>* iRowAndSubCellArray;
	CArrayFix<SSLSubCell>* iSLSubCellArray;
	TBool iDrawBackground; // Determines if background should be drawn (transparency)
	TSize iSubCellIconSize[KMaxSubCellIndex]; // Store icon sizes for each subcell
	TAknSeparatorLinePosition iSeparatorLinePosition;
	CAknsEffectAnim* iAnimation;
	TBitFlags32 iAnimFlags;
	TRgb iHighlightedTextColor;
	TRgb iTextColor;
	TBool iDrawScrollbarBackground;
	TBool iSimpleList;

	TBool iUnderlineFlagSet; // underlining support for more than
	TBitFlags32 iUnderlineFlags; // one text subcell
	TBool iHideSecondRow;
	TBool iSubCellsMightIntersect;
	TBool iStretchingEnabled;
	TAknsItemID iAnimIID;
	MListBoxAnimBackgroundDrawer* iHighlightBgDrawer;
	TSize iAnimSize; // TODO Deprecating old style anim API and removing this
	// Drop shadows can be toggled on/off in
	// "transparent" lists (eg. transparent camera setting page).
	TBool iUseDropShadows;

	// highlight color skinning for setstyle lists - to be deprecated once correct color skinning is available
	TBool iUseHighligthIconSwapping;

	// colorskin highlight icons
	CFbsBitmap* iColorBmp;
	CFbsBitmap* iHiliBmp;
	TRgb iIconColor;
	TRgb iHiliIconColor;

	TInt iConditionalCells;

	// which columns of highlighted item have clipped text ?
	TUint32 iClippedSubcells;
	TUint32 iClippedByWrap;
	TBool iUseClippedByWrap;

	CArrayFix<TSubCellExt>* iSubCellExtArray; //for subcell alignment

	TRect iMarginRect;
	TBool iKineticScrolling;
	};

/**
 * High priority is well argumented because running the active object will
 * result in animation deletion -> results in freeing resources.
 */
CCCAppCommLauncherCustomListBoxDataExtension::CCCAppCommLauncherCustomListBoxDataExtension() :
	CActive(EPriorityHigh)
	{
	}

void CCCAppCommLauncherCustomListBoxDataExtension::ConstructL(
		CCCAppCommLauncherCustomListBoxData* aListBoxData,
		const TAknsItemID& aAnimationIID)
	{
	iListBoxData = aListBoxData;
	iRowAndSubCellArray = new (ELeave) CArrayFixFlat<SRowAndSubCell> (4);
	iSLSubCellArray = new (ELeave) CArrayFixFlat<SSLSubCell> (4);

	iSubCellExtArray = new (ELeave) CArrayFixFlat<TSubCellExt> (4);

	// bi-di algorithm support (NON_LEAVING_VERSION)
	// not used ?    iTempTextBuffer = HBufC::NewL(256);
	iUseLogicalToVisualConversion = ETrue;

	iFirstWordWrappedSubcellIndex = -1;
	iSecondWordWrappedSubcellIndex = -1;

	iDrawBackground = ETrue;
	iSeparatorLinePosition = ENoLine;

	iAnimIID = aAnimationIID;
	CActiveScheduler::Add(this);
	TryCreateAnimation(); // Animations are created by default

	iTextColor = NULL; // just in case
	iHighlightedTextColor = NULL; // just in case
	iDrawScrollbarBackground = ETrue;

#ifdef RD_LIST_STRETCH
	// check list stretching from cenrep
	CRepository* cenRep = CRepository::NewL(KCRUidAvkon);
	cenRep->Get(KAknAutomaticListStretching, iStretchingEnabled);
	delete cenRep;
#endif

	iKineticScrolling = CAknPhysics::FeatureEnabled();
	}

CCCAppCommLauncherCustomListBoxDataExtension::~CCCAppCommLauncherCustomListBoxDataExtension()
	{
	Cancel();

	// Stop receiving foreground events
	CCoeEnv* env = CCoeEnv::Static();
	env->RemoveForegroundObserver(*this);

	delete iRowAndSubCellArray;
	iRowAndSubCellArray = NULL;
	delete iSLSubCellArray;
	iSLSubCellArray = NULL;
	delete iSubCellExtArray;
	iSubCellExtArray = NULL;
	delete iPopupFrame;
	delete iSkinControlContext;
	// not used ?    delete iTempTextBuffer;
	delete iPictoInterface;
	delete iMarquee;
	delete i2ndLineMarquee;
	delete iAnimation;
	delete iColorBmp;
	delete iHiliBmp;
	}

void CCCAppCommLauncherCustomListBoxDataExtension::AddRowAndSubCellL(TInt aRow,
		TInt aSubCell)
	{
	SRowAndSubCell subcell;
	subcell.iIndex = (aRow << 8) | (aSubCell & 0xff);
	subcell.iFont = NULL;
	TKeyArrayFix key(0, ECmpTInt32);
	iRowAndSubCellArray->InsertIsqL(subcell, key);
	}

CCCAppCommLauncherCustomListBoxDataExtension::SRowAndSubCell&
CCCAppCommLauncherCustomListBoxDataExtension::At(TInt aArrayIndex)
	{
	/*    SRowAndSubCell subcell;
	 for (int a=0;a<iRowAndSubCellArray->Count();a++)
	 subcell = iRowAndSubCellArray->At(a); */

	//__ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iRowAndSubCellArray->Count(),Panic(EAknPanicOutOfRange));
	return (iRowAndSubCellArray->At(aArrayIndex));
	}

const CCCAppCommLauncherCustomListBoxDataExtension::SRowAndSubCell&
CCCAppCommLauncherCustomListBoxDataExtension::At(TInt aArrayIndex) const
	{
	//__ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iRowAndSubCellArray->Count(),Panic(EAknPanicOutOfRange));
	return (iRowAndSubCellArray->At(aArrayIndex));
	}

TInt CCCAppCommLauncherCustomListBoxDataExtension::FindRowAndSubCellIndex(
		TInt& aArrayIndex, TInt aRow, TInt aSubCell) const
	{
	if (iRowAndSubCellArray->Count() == 0)
		return (KErrNotFound);
	TKeyArrayFix key(0, ECmpTInt32);
	SRowAndSubCell rowAndSubcell;
	rowAndSubcell.iIndex = (aRow << 8) | (aSubCell & 0xff);
	return (iRowAndSubCellArray->FindIsq(rowAndSubcell, key, aArrayIndex));
	}

void CCCAppCommLauncherCustomListBoxDataExtension::FindRowAndSubCellIndexOrAddL(
		TInt& aArrayIndex, TInt aRow, TInt aSubCell)
	{
	if (FindRowAndSubCellIndex(aArrayIndex, aRow, aSubCell) != 0) //==KErrNotFound)
		{
		AddRowAndSubCellL(aRow, aSubCell);
		FindRowAndSubCellIndex(aArrayIndex, aRow, aSubCell);
		}
	/*       SRowAndSubCell rowAndSubcell;
	 rowAndSubcell = iRowAndSubCellArray->At(aArrayIndex); */
	}

void CCCAppCommLauncherCustomListBoxDataExtension::AddSLSubCellL(TInt aSubCell)
	{
	SSLSubCell subcell;
	subcell.iSubCell = aSubCell;
	subcell.iTextLayout = NULL;
	subcell.iGraphicLayout = NULL;
	subcell.iSubCellType = 0;
	subcell.iConditionValue = -1;

	TKeyArrayFix key(0, ECmpTInt32);
	iSLSubCellArray->InsertIsqL(subcell, key);
	}

CCCAppCommLauncherCustomListBoxDataExtension::SSLSubCell&
CCCAppCommLauncherCustomListBoxDataExtension::AtSL(TInt aArrayIndex)
	{
	//__ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iSLSubCellArray->Count(),Panic(EAknPanicOutOfRange));
	return (iSLSubCellArray->At(aArrayIndex));
	}

const CCCAppCommLauncherCustomListBoxDataExtension::SSLSubCell&
CCCAppCommLauncherCustomListBoxDataExtension::AtSL(TInt aArrayIndex) const
	{
	//__ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iSLSubCellArray->Count(),Panic(EAknPanicOutOfRange));
	return (iSLSubCellArray->At(aArrayIndex));
	}

TInt CCCAppCommLauncherCustomListBoxDataExtension::FindSLSubCellIndex(
		TInt& aArrayIndex, TInt aSubCell) const
	{
	if (iSLSubCellArray->Count() == 0)
		return (KErrNotFound);
	TKeyArrayFix key(0, ECmpTInt32);
	SSLSubCell SLSubCell;
	SLSubCell.iSubCell = aSubCell;
	return (iSLSubCellArray->FindIsq(SLSubCell, key, aArrayIndex));
	}

void CCCAppCommLauncherCustomListBoxDataExtension::FindSLSubCellIndexOrAddL(
		TInt& aArrayIndex, TInt aSubCell)
	{
	if (FindSLSubCellIndex(aArrayIndex, aSubCell) != 0) //==KErrNotFound)
		{
		AddSLSubCellL(aSubCell);
		FindSLSubCellIndex(aArrayIndex, aSubCell);
		}
	}

void CCCAppCommLauncherCustomListBoxDataExtension::ResetSLSubCellArray()
	{
	iSLSubCellArray->Reset();
	iRowAndSubCellArray->Reset();
	}

void CCCAppCommLauncherCustomListBoxDataExtension::CreatePictographInterfaceL()
	{
	if (!iPictoInterface)
		{
		iPictoInterface = CAknPictographInterface::NewL(*iControl, *this);
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::NoAnimIfError
// -----------------------------------------------------------------------------
//
inline void CCCAppCommLauncherCustomListBoxDataExtension::NoAnimIfError(
		TInt aError)
	{
	if (KErrNone != aError)
		{
		DeleteAnim();
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::TryCreateAnimation
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::TryCreateAnimation()
	{
	if (!iControl)
		{
		return;
		}

#ifdef RD_UI_TRANSITION_EFFECTS_LIST  
	CEikListBox* list = static_cast<CEikListBox*> (iControl);
	CListBoxView* view = list->View();
	if (!view || !view->ItemDrawer())
		{
		return;
		}
	CWindowGc* gc = view->ItemDrawer()->Gc();
	MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal(gc);
	if (transApi && transApi->VerifyKml() == KErrNone)
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

	if (!iAnimation)
		{
		// This must be the first call because animation does not exist.
		// Animation layers are created when the animation is rendered for the
		// first time.
		TRAPD( err, CreateAnimationL() );
		NoAnimIfError(err);
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::SyncAnim
// -----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherCustomListBoxDataExtension::SyncAnim(const TSize& aSize)
	{
	if (!iAnimation || aSize.iWidth <= 0 || aSize.iHeight <= 0)
		{
		return EFalse;
		}

	if (iAnimation->Size() != aSize || iAnimation->NeedsInputLayer())
		{
		// Resizing will update animation background
		iAnimFlags.Clear(EFlagUpdateBg);
		// Animation exists but its size is out of sync or input layers have
		// been released
		TRAPD( err, DoResizeL( aSize, iAnimFlags.IsSet( EFlagHasForeground ) ) );

		if (err)
			{
			DeleteAnim();
			return EFalse;
			}
		iAnimFlags.Set(EFlagHasLayers);
		}

	// Highlight animation background needs update (current item has changed)
	if (iAnimFlags.IsSet(EFlagUpdateBg))
		{
		iAnimFlags.Clear(EFlagUpdateBg);

		if (iAnimation->InputRgbGc())
			{
			DrawHighlightBackground(*iAnimation->InputRgbGc());
			// We need to update the output frame (otherwise the highlight
			// would drawn with the old output before the next new animation
			// frame).
			NoAnimIfError(iAnimation->UpdateOutput());
			if (!iAnimation)
				{
				return EFalse;
				}
			}
		}

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::SyncAndDrawAnim
// -----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherCustomListBoxDataExtension::SyncAndDrawAnim(
		CBitmapContext& aGc, const TRect& aRect)
	{
	if (iAnimation)
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
		if (EAknsAnimStateStopped == iAnimation->State() && !iAnimFlags.IsSet(
				EFlagHasForeground) && iAnimFlags.IsSet(EFlagHasLayers)
				&& !iAnimFlags.IsSet(EFlagUpdateBg))
			{
			return iAnimation->Render(aGc, aRect);
			}
		}

	if (SyncAnim(aRect.Size()))
		{
		return iAnimation->Render(aGc, aRect);
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::SkinBackgroundContext
// -----------------------------------------------------------------------------
//
MAknsControlContext* CCCAppCommLauncherCustomListBoxDataExtension::SkinBackgroundContext() const
	{
	if (iSkinEnabled)
		return iSkinControlContext;
	else
		return NULL;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::DeleteAnim
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::DeleteAnim()
	{
	// Stop receiving foreground events
	CCoeEnv* env = CCoeEnv::Static();
	env->RemoveForegroundObserver(*this);

	delete iAnimation;
	iAnimation = NULL;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::FocusGained
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::FocusGained()
	{
	Play();
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::FocusLost
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::FocusLost()
	{
	if (iAnimation)
		{
		NoAnimIfError(iAnimation->Pause());
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::SkinChanged
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::SkinChanged()
	{
	DeleteAnim();
	TryCreateAnimation();
	TRAP_IGNORE( CreateColorBitmapsL() );
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::SetControl
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::SetControl(
		CCoeControl* aControl)
	{
	if (iControl)
		{
		CEikListBox* list = (CEikListBox*) iControl;
		list->RemoveItemChangeObserver(this);
		}

	iControl = aControl;

	if (iControl)
		{
		CEikListBox* list = (CEikListBox*) iControl;
			TRAP_IGNORE( list->AddItemChangeObserverL(this) );

		if (!iAnimation)
			{
			TryCreateAnimation();
			}

		if (iAnimation)
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
// CCCAppCommLauncherCustomListBoxDataExtension::HandleGainingForeground
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::HandleGainingForeground()
	{
	// Most of the time focus focus events happen between foreground events.
	// Unfortunately, there are embedded app related cases where this does not
	// always hold (it is possible to focus in non-foreground application or
	// gain foreground without getting a focus gained). Therefore animations
	// must be started here. This causes potential problem case: foreground
	// events are broadcasted to all animations, if there are multiple lists
	// that have focus the result will be multiple animations running at the
	// same time.
	iAnimFlags.Set(EFlagHasForeground);

	if (iAnimation)
		{
		// Because we are gaining foreground we must restore input layers
		// (which are released when animation stops to reduce memory usage).
		SyncAnim(iAnimation->Size());
		}

	if (iControl)
		{
		if (iControl->IsFocused())
			{
			Play();
			}
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::HandleLosingForeground
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::HandleLosingForeground()
	{
	iAnimFlags.Clear(EFlagHasForeground);
	if (iAnimation)
		{
		NoAnimIfError(iAnimation->Stop());
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::AnimFrameReady
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::AnimFrameReady(TInt aError,
		TInt)
	{
	if (KErrNone != aError)
		{
		// Animation has failed to run -> schedule the animation for
		// deletion to fall back to normal rendering.
		PostDeleteAnimation();
		return;
		}

	// This situation should never happen because we start/stop animation when
	// the extension's control is set.
	if (!iControl)
		return;

	// From now on, we have a valid control pointer
	CEikListBox* list = static_cast<CEikListBox*> (iControl);
	CListBoxView* view = list->View();

	if (!view)
		return;

	// We should not run animation when control is in certain states. When
	// control is in these states we idle the animation until the control state
	// becomes valid again.
	TBool invalid = !iControl->IsVisible() || iControl->IsDimmed()
			|| (view->ItemDrawer()->Flags()
					& CListItemDrawer::EDisableHighlight);

	// Check for idling
	if (iAnimation->IsIdling())
		{
		if (invalid)
			{
			// We are idling and the control is still invalid -> keep on
			// idling.
			return;
			}
		else
			{
			// Control is in valid state, animation should be continued
			TInt err = iAnimation->Continue();
			if (err)
				PostDeleteAnimation();
			return;
			}
		}
	else if (invalid) // Not idling and invalid control -> start idling
		{
		iAnimation->SetIdling(KAknsEffectAnimDefaultIdleInterval);
		// If the highlight has been disabled, render once to clear the
		// highlight (not returning in that case).
		if (!(view->ItemDrawer()->Flags() & CListItemDrawer::EDisableHighlight))
			return;
		}

	// From now on, we have a valid control pointer and control has enabled
	// highlight, is visible and is not dimmed.

	// No items, no drawing
	if (list->Model()->NumberOfItems() == 0)
		return;

	if (iListBoxData->LastSubCell() < 0)
		return;

	TBool customGcInUse = list->GetGc() != &CEikonEnv::Static()->SystemGc();

	// if remoteGc is in use (CCoeControl has custom gc set)
	// then draw the whole control as the transition system 
	// expects the draw to be initiated from CONE control

	if (!customGcInUse)
		{
		// Repaint the highlighted cell only
		list->RedrawItem(view->CurrentItemIndex());
		}
	else
		{
		CCoeControl* ctrl = (CCoeControl*) list;
		ctrl->DrawNow();
		}

	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::ListBoxItemsChanged
//
// this is called from from:
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
void CCCAppCommLauncherCustomListBoxDataExtension::ListBoxItemsChanged(
		CEikListBox* aListBox)
	{

	TInt items = aListBox->Model()->NumberOfItems();

	if (iAnimation)
		{
		if (0 == items)
			{
			DeleteAnim();
			}
		else
			{
			Play();
			}
		}
	else if (!iAnimation)
		{
		// Animation doesn't exist and we have > 0 items
		TryCreateAnimation();
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::DoCancel
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::DoCancel()
	{
	// Required method, but not needed
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::RunL
// Postponed animation deletion is done here.
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::RunL()
	{
	DeleteAnim();
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::Play
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::Play()
	{
	if (!iAnimation)
		{
		return;
		}

	// No need to start running/finished animation
	if (EAknsAnimStateRunning == iAnimation->State() || EAknsAnimStateFinished
			== iAnimation->State())
		{
		return;
		}

	// Check that application is on foreground because there are cases where
	// focus changes are done after foreground is lost -> potentially leads to
	// multiple running animations.
	if (!iAnimFlags.IsSet(EFlagHasForeground))
		{
		return;
		}

	// Animation cannot run if we don't have control (nowhere to draw)
	if (!iControl)
		{
		return;
		}

	// The control must also have the focus
	if (!iControl->IsFocused())
		{
		return;
		}

	// Don't animate empty list
	CEikListBox* list = static_cast<CEikListBox*> (iControl);
	if (list->Model()->NumberOfItems() == 0)
		{
		return;
		}

	// All preconditions are met: we have animation, foreground, focus, more
	// than zero items and animation is either paused or stopped. Invisibility,
	// dimming and disabled highlight are handled by idling the animation (see
	// AnimFrameReady).

	if (EAknsAnimStatePaused == iAnimation->State())
		{
		NoAnimIfError(iAnimation->Continue());
		}
	else if (EAknsAnimStateStopped == iAnimation->State())
		{
		NoAnimIfError(iAnimation->Start());
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::DrawHighlightBackground
// -----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherCustomListBoxDataExtension::DrawHighlightBackground(
		CFbsBitGc& aGc)
	{
	if (iHighlightBgDrawer) // Bg drawing is done externally (in derived class)
		{
		return iHighlightBgDrawer->DrawHighlightAnimBackground(aGc);
		}

	// Draw the background under the current highlight. This is simplified
	// drawing, we only grab a piece from the list background bitmap.
	CEikListBox* list = static_cast<CEikListBox*> (iControl);
	CListBoxView* view = list->View();
	TRect itemRect;
	TInt index = view->CurrentItemIndex();

	// It is possible that the animation is constructed when the list is
	// empty. In this case draw the first element background (drawing works ok
	// even if the list has no items).
	if (list->Model()->NumberOfItems() == 0)
		{
		index = 0;
		}
	itemRect.SetRect(view->ItemPos(index), iAnimation->Size());

	MAknsControlContext* cc = AknsDrawUtils::ControlContext(iControl);

	if (!cc)
		{
		cc = SkinBackgroundContext();
		}

    TBool ret = AknsDrawUtils::DrawBackground( AknsUtils::SkinInstance(), cc, iControl, aGc, TPoint(0,0),
                                          itemRect, KAknsDrawParamBottomLevelRGBOnly );
    return ret;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::PostDeleteAnimation
// Schedules the animation for deletion by activating the extension itself.
// Deletion is postponed because in many error/failure occasions the caller has
// been animation and direct deletion is possibly not safe (because function
// stack would return through the deleted object).
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::PostDeleteAnimation()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::CreateAnimationL
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::CreateAnimationL()
	{
	DeleteAnim();

	// Check if derived class wants to disable highlight animation. Also, no
	// highlight animation on lists that don't have background
	if ((KAknsIIDNone == iAnimIID)
			|| !iListBoxData->IsBackgroundDrawingEnabled())
		{
		return;
		}

	// Create animation
	CCoeEnv* env = CCoeEnv::Static();
	env->AddForegroundObserverL(*this);

	iAnimation = CAknsEffectAnim::NewL(this);
	TBool ok = iAnimation->ConstructFromSkinL(iAnimIID);

	if (!ok) // Animation for the ID was not found from the skin
		{
		User::Leave(KErrNotFound);
		}

	// Sync the local foreground flag state. Foreground state is stored locally
	// because calling AppUi::IsForeground causes WSERV flush (shocking) and
	// therefore it cannot be used in draw routines.
	CAknAppUi* aui = static_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
	iAnimFlags.Assign(EFlagHasForeground, aui->IsForeground());

	Play();
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxDataExtension::DoResizeL
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxDataExtension::DoResizeL(
		const TSize& aHighlightSize, TBool aAboutToStart)
	{
	iAnimation->BeginConfigInputLayersL(aHighlightSize, aAboutToStart);

	if (iAnimation->InputRgbGc())
		{
		DrawHighlightBackground(*iAnimation->InputRgbGc());
		}

	iAnimation->EndConfigInputLayersL();
	}

///////////handling TSubCellExt,start

TInt CCCAppCommLauncherCustomListBoxDataExtension::AddSubCellExtL(TInt aSubCell)
	{
	TSubCellExt subcell;
	subcell.iSubCell = aSubCell;
	subcell.iLayoutAlign = ETrue;

	TKeyArrayFix key(0, ECmpTInt);
	return iSubCellExtArray->InsertIsqL(subcell, key);

	}

void CCCAppCommLauncherCustomListBoxDataExtension::FindSubCellExtIndexOrAddL(
		TInt& aArrayIndex, TInt aSubCell)
	{
	if (FindSubCellExtIndex(aArrayIndex, aSubCell) == KErrNotFound)
		{
		aArrayIndex = AddSubCellExtL(aSubCell);
		//FindSubCellExtIndex(aArrayIndex,aSubCell);
		}
	}

void CCCAppCommLauncherCustomListBoxDataExtension::SetSubCellLayoutAlignmentL(
		TInt aSubCellIndex)
	{
	TInt index = 0;
	FindSubCellExtIndexOrAddL(index, aSubCellIndex);
	iSubCellExtArray->At(index).iLayoutAlign = EFalse;
	}

TInt CCCAppCommLauncherCustomListBoxDataExtension::FindSubCellExtIndex(
		TInt& aArrayIndex, TInt aSubCell) const
	{
	if (aSubCell < 0 || iSubCellExtArray->Count() == 0)
		return (KErrNotFound);
	TKeyArrayFix key(0, ECmpTInt);
	TSubCellExt subcell;
	subcell.iSubCell = aSubCell;
	TInt retVal = iSubCellExtArray->FindIsq(subcell, key, aArrayIndex);
	if (retVal != 0)
		{
		return KErrNotFound;
		}

	return retVal;
	}

TBool CCCAppCommLauncherCustomListBoxDataExtension::SubCellLayoutAlignment(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellExtIndex(index, aSubCellIndex) == KErrNotFound)
		return (ETrue);
	return (iSubCellExtArray->At(index).iLayoutAlign);
	}

///////////handling TSubCellExt,end

void CCCAppCommLauncherCustomListBoxDataExtension::DrawPictographArea()
	{
	// This callback is never invoked, if iControl is NULL.
	iControl->DrawNow();
	}

void CCCAppCommLauncherCustomListBoxDataExtension::CreateMarqueeControlL()
	{
	if (!iMarquee)
		{
		iMarquee = CAknMarqueeControl::NewL();
		}

	if (!i2ndLineMarquee)
		{
		i2ndLineMarquee = CAknMarqueeControl::NewL();
		}
	}

TInt CCCAppCommLauncherCustomListBoxDataExtension::RedrawEvent(TAny* aControl)
	{
	if (!((CCoeControl*) aControl)->IsVisible())
		{
		return EFalse;
		}
	CEikFormattedCellListBox* listBox = (CEikFormattedCellListBox*) aControl;
	if (listBox->CurrentItemIndex() >= 0)
		{
		listBox->RedrawItem(listBox->CurrentItemIndex());
		}
	return ETrue;
	}

TBool CCCAppCommLauncherCustomListBoxDataExtension::IsMarqueeOn()
	{
	TBool isOn = EFalse;
	if (iMarquee)
		isOn = iMarquee->IsMarqueeOn();
	return isOn;
	}

TBool CCCAppCommLauncherCustomListBoxDataExtension::DrawPressedDownEffectL(
		MAknsSkinInstance* aInstance, CWindowGc& aGc, const TRect& aOutRect,
		const TRect& aInnerRect) const
	{
	return AknsDrawUtils::DrawFrame(aInstance, aGc, aOutRect, aInnerRect,
			KAknsIIDQsnFrListPressed, KAknsIIDQsnFrListCenterPressed);
	}

 CCoeControl *CCCAppCommLauncherCustomListBoxData::Control() const
	{
	return iExtension->iControl;
	}

 void CCCAppCommLauncherCustomListBoxData::SetSeparatorLinePosition(
		TAknSeparatorLinePosition aPosition)
	{
	if (iExtension)
		iExtension->iSeparatorLinePosition = aPosition;
	}
 TAknSeparatorLinePosition CCCAppCommLauncherCustomListBoxData::SeparatorLinePosition() const
	{
	if (iExtension)
		return iExtension->iSeparatorLinePosition;
	else
		return ENoLine;
	}
 CAknLayoutData *CCCAppCommLauncherCustomListBoxData::LayoutData() const
	{
	return NULL;
	}

 TBool CCCAppCommLauncherCustomListBoxData::LayoutInit() const
	{
	return EFalse;
	}
 void CCCAppCommLauncherCustomListBoxData::SetLayoutInit(TBool /*aValue*/)
	{
	}

CCCAppCommLauncherCustomListBoxDataExtension *CCCAppCommLauncherCustomListBoxData::Extension()
	{
	return iExtension;
	}
CCCAppCommLauncherCustomListBoxDataExtension *CCCAppCommLauncherCustomListBoxData::Extension() const
	{
	return iExtension;
	}

 void CCCAppCommLauncherCustomListBoxData::SetControl(
		CCoeControl *aControl)
	{
	iExtension->SetControl(aControl);
	}

 MAknsControlContext* CCCAppCommLauncherCustomListBoxData::SkinBackgroundContext() const
	{
	if (iExtension->iSkinEnabled)
		{
		if (iExtension->iPopupFrame)
			{
			return iExtension->iPopupFrame;
			}
		else
			{
			return iExtension->iSkinControlContext;
			}
		}
	else
		{
		return NULL;
		}
	}

 void CCCAppCommLauncherCustomListBoxData::SetSkinEnabledL(
		TBool aEnabled)
	{
	CListBoxData::SetSkinEnabledL(aEnabled);
	iExtension->iSkinEnabled = aEnabled;
	}

 TBool CCCAppCommLauncherCustomListBoxData::SkinEnabled() const
	{
	return iExtension->iSkinEnabled;
	}

 void CCCAppCommLauncherCustomListBoxData::SetSkinStyle(
		const TAknsItemID *id, const TRect &aTileRect)
	{

	if (iExtension->iSkinControlContext)
		{
		iExtension->iSkinControlContext->SetTiledBitmap(*id);
		iExtension->iSkinControlContext->SetTiledRect(aTileRect);
		}
	}
 void CCCAppCommLauncherCustomListBoxData::SetSkinParentPos(
		const TPoint &aPos)
	{
	if (iExtension->iSkinControlContext)
		{
		iExtension->iSkinControlContext->SetParentPos(aPos);
		}
	}
 void CCCAppCommLauncherCustomListBoxData::SetBackgroundSkinStyle(
		const TAknsItemID *aId, const TRect &aRect)
	{
	if (iExtension->iSkinControlContext)
		{
		iExtension->iSkinControlContext->SetBitmap(*aId);
		iExtension->iSkinControlContext->SetRect(aRect);
		}
	}
 void CCCAppCommLauncherCustomListBoxData::SetListEndSkinStyle(
		const TAknsItemID *aListEndId, const TRect &aRect)
	{
	if (iExtension->iSkinControlContext)
		{
		iExtension->iSkinControlContext->SetBottomBitmap(*aListEndId);
		iExtension->iSkinControlContext->SetBottomRect(aRect);
		}
	}

 void CCCAppCommLauncherCustomListBoxData::SetSkinHighlightFrame(
		const TAknsItemID *aFrameId, const TAknsItemID *aFrameCenterId)
	{
	iExtension->iSkinHighlightFrameId = aFrameId;
	iExtension->iSkinHighlightFrameCenterId = aFrameCenterId;
	}

 void CCCAppCommLauncherCustomListBoxData::SetSkinPopupFrame(
		const TAknsItemID *aFrameId, const TAknsItemID *aFrameCenterId)
	{
    if ( iExtension )
        {
        if ( iExtension->iPopupFrame )
		    {
		    iExtension->iPopupFrame->SetFrame(*aFrameId);
            iExtension->iPopupFrame->SetCenter(*aFrameCenterId);
	    	}
	    else
	    	{
	        TRAPD(err, iExtension->iPopupFrame = CAknsFrameBackgroundControlContext::NewL(
						KAknsIIDNone,
						TRect(0,0,1,1), // these must be set by using SetSkinPopupFramePosition
						TRect(0,0,1,1),
						EFalse ) );
            if (!err)
			    {
			    iExtension->iPopupFrame->SetFrame(*aFrameId);
			    iExtension->iPopupFrame->SetCenter(*aFrameCenterId);
                }
			}
		}
	}

 void CCCAppCommLauncherCustomListBoxData::SetSkinPopupFramePosition(
		const TRect &aOuterRect, const TRect &aInnerRect)
	{
	if (iExtension && iExtension->iPopupFrame)
		iExtension->iPopupFrame->SetFrameRects(aOuterRect, aInnerRect);
	}

 void CCCAppCommLauncherCustomListBoxData::UseLogicalToVisualConversion(
		TBool aUseConversion)
	{
	if (iExtension)
		iExtension->iUseLogicalToVisualConversion = aUseConversion;
	}

void CCCAppCommLauncherCustomListBoxData::CreatePictographInterfaceL()
	{
	iExtension->CreatePictographInterfaceL();
	}

void CCCAppCommLauncherCustomListBoxData::CreateMarqueeControlL()
	{
	TCallBack callBack(
			CCCAppCommLauncherCustomListBoxDataExtension::RedrawEvent,
			iExtension->iControl);
	//iExtension->iMarquee = CAknMarqueeControl::NewL();
	iExtension->CreateMarqueeControlL();
	iExtension->iMarquee->SetRedrawCallBack(callBack);
	iExtension->i2ndLineMarquee->SetRedrawCallBack(callBack);
	}

const TInt KSubCellListBoxGranularity = 4;
const TInt KNoActualSubCellFont = -1;


CCCAppCommLauncherCustomListBoxData* CCCAppCommLauncherCustomListBoxData::NewL()
	{
	CCCAppCommLauncherCustomListBoxData* self =
			new (ELeave) CCCAppCommLauncherCustomListBoxData;
	self->ConstructLD(KAknsIIDQsnAnimList);
	return (self);
	}

 CCCAppCommLauncherCustomListBoxData::~CCCAppCommLauncherCustomListBoxData()
	{
	delete iExtension;
	delete iSubCellArray;
	if (iIconArray)
		{
		iIconArray->ResetAndDestroy();
		delete iIconArray;
		}
	}

 TSize CCCAppCommLauncherCustomListBoxData::SubCellSize(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return TSize(0, 0);
	return (iSubCellArray->At(index).iSize);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellSizeL(
		TInt aSubCellIndex, TSize aSize)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iSize = aSize;
	// if someone uses this api directly, revert to old draw
	// new api will turn this to ETrue again.
	UseScalableLayoutData(EFalse);
	}

 TInt CCCAppCommLauncherCustomListBoxData::SubCellBaselinePos(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return TInt(0);
	return (iSubCellArray->At(index).iBaseline);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellBaselinePosL(
		TInt aSubCellIndex, TInt aPos)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iBaseline = aPos;
	}

 TInt CCCAppCommLauncherCustomListBoxData::SubCellTextClipGap(
		TInt aSubCellIndex) const
	{
	TInt index;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return TInt(0);
	return (iSubCellArray->At(index).iTextClipGap);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellTextClipGapL(
		TInt aSubCellIndex, TInt aGap)
	{
	TInt index;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iTextClipGap = aGap;
	}

TSize CCCAppCommLauncherCustomListBoxData::SubCellRealSize(TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return TSize(0, 0);
	return (iSubCellArray->At(index).iRealSize);
	}

void CCCAppCommLauncherCustomListBoxData::SetSubCellRealSize(
		TInt aSubCellIndex, TSize aRealSize) const
	{
	TInt index = 0;
	if (!(FindSubCellIndex(index, aSubCellIndex) == KErrNotFound))
		{
		MUTABLE_CAST(TSize&,At(index).iRealSize) = aRealSize;
		}
	}

TSize CCCAppCommLauncherCustomListBoxData::SubCellRealTextSize(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return TSize(0, 0);
	return (iSubCellArray->At(index).iRealTextSize);
	}

void CCCAppCommLauncherCustomListBoxData::SetSubCellRealTextSize(
		TInt aSubCellIndex, TSize aRealTextSize) const
	{
	TInt index = 0;
	if (!(FindSubCellIndex(index, aSubCellIndex) == KErrNotFound))
		{
		MUTABLE_CAST(TSize&,At(index).iRealTextSize) = aRealTextSize;
		}
	}

 TPoint CCCAppCommLauncherCustomListBoxData::SubCellPosition(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return TPoint(0, 0);
	return (iSubCellArray->At(index).iPosition);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellPositionL(
		TInt aSubCellIndex, TPoint aPosition)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iPosition = aPosition;
	// if someone uses this api directly, revert to old draw
	// new api will turn this to ETrue again.
	UseScalableLayoutData(EFalse);
	}

 TMargins CCCAppCommLauncherCustomListBoxData::SubCellMargins(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		{
		TMargins tm =
			{
			0, 0, 0, 0
			};
		return tm;
		}
	return (iSubCellArray->At(index).iMargin);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellMarginsL(
		TInt aSubCellIndex, TMargins aMargins)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iMargin = aMargins;
	}

 const CFont*
CCCAppCommLauncherCustomListBoxData::SubCellFont(TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return 0;
	return (iSubCellArray->At(index).iBaseFont);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellFontL(
		TInt aSubCellIndex, const CFont* aFont)
	{
#if 0
	TInt subcells=iSubCellArray->Count();
#endif
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	if (At(index).iBaseFont != aFont)
		{
#if 0
		TInt actualFontIndex=0;
		TRAPD(err, actualFontIndex=AddActualFontL(aFont));
		if (err)
			{
			if (subcells!=iSubCellArray->Count())
				{
				iSubCellArray->Delete(index);
				}
			User::Leave(err);
			}
		At(index).iActualFontIndex=actualFontIndex;
#endif
		At(index).iBaseFont = aFont;
		}
	}

 CGraphicsContext::TTextAlign CCCAppCommLauncherCustomListBoxData::SubCellAlignment(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return CGraphicsContext::ELeft;
	return (iSubCellArray->At(index).iAlign);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellAlignmentL(
		TInt aSubCellIndex, CGraphicsContext::TTextAlign aAlign)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iAlign = aAlign;

	iExtension->SetSubCellLayoutAlignmentL(aSubCellIndex);
	}

 const CCCAppCommLauncherCustomListBoxData::TColors &
CCCAppCommLauncherCustomListBoxData::SubCellColors(TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return defaultcolors;
	return (iSubCellArray->At(index).iColors);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellColorsL(
		TInt aSubCellIndex, const TColors & aColors)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iColors = aColors;
	At(index).iUseSubCellColors = ETrue;
	}

 TBool CCCAppCommLauncherCustomListBoxData::UseSubCellColors(
		TInt aSubCell) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCell) == KErrNotFound)
		return EFalse;
	return At(index).iUseSubCellColors;
	}

 CGraphicsContext::TPenStyle CCCAppCommLauncherCustomListBoxData::SubCellRightSeparatorStyle(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return CGraphicsContext::ENullPen;
	return (iSubCellArray->At(index).iRightSeparatorStyle);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellRightSeparatorStyleL(
		TInt aSubCellIndex, CGraphicsContext::TPenStyle aStyle)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iRightSeparatorStyle = aStyle;
	}

 TBool CCCAppCommLauncherCustomListBoxData::SubCellIsGraphics(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return EFalse;
	return (iSubCellArray->At(index).iGraphics);
	}

 void CCCAppCommLauncherCustomListBoxData::SetGraphicsSubCellL(
		TInt aSubCellIndex, TBool aIsGraphics)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iGraphics = aIsGraphics;
	// if someone uses this api directly, revert to old draw
	// new api will turn this to ETrue again.
	UseScalableLayoutData(EFalse);
	}

 TBool CCCAppCommLauncherCustomListBoxData::SubCellIsNumberCell(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return EFalse;
	return (iSubCellArray->At(index).iNumberCell);
	}

 void CCCAppCommLauncherCustomListBoxData::SetNumberCellL(
		TInt aSubCellIndex, TBool aIsNumberCell)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iNumberCell = aIsNumberCell;
	}

 TBool CCCAppCommLauncherCustomListBoxData::SubCellIsTransparent(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return EFalse;
	return (iSubCellArray->At(index).iTransparent);
	}

 void CCCAppCommLauncherCustomListBoxData::SetTransparentSubCellL(
		TInt aSubCellIndex, TBool aIsTransparent)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iTransparent = aIsTransparent;
	}

 TBool CCCAppCommLauncherCustomListBoxData::SubCellIsNotAlwaysDrawn(
		TInt aSubCellIndex) const
	{
	TInt index = 0;
	if (FindSubCellIndex(index, aSubCellIndex) == KErrNotFound)
		return EFalse;
	return (iSubCellArray->At(index).iNotAlwaysDrawn);
	}

 void CCCAppCommLauncherCustomListBoxData::SetNotAlwaysDrawnSubCellL(
		TInt aSubCellIndex, TBool aIsNotAlwaysDrawn)
	{
	TInt index = 0;
	FindSubCellIndexOrAddL(index, aSubCellIndex);
	At(index).iNotAlwaysDrawn = aIsNotAlwaysDrawn;
	}

 CArrayPtr<CGulIcon>*
CCCAppCommLauncherCustomListBoxData::IconArray() const
	{
	return iIconArray;
	}

 void CCCAppCommLauncherCustomListBoxData::SetIconArrayL(CArrayPtr<
		CGulIcon>* aArray)
	{
	iIconArray = aArray;
	}

 void CCCAppCommLauncherCustomListBoxData::SetIconArray(CArrayPtr<
		CGulIcon>* aArray)
	{
	iIconArray = aArray;
	}

 TBool CCCAppCommLauncherCustomListBoxData::RespectFocus() const
	{
	if (iExtension)
		return iExtension->iRespectFocus;
	return EFalse;
	}

 void CCCAppCommLauncherCustomListBoxData::SetRespectFocus(TBool aBool)
	{
	if (iExtension)
		iExtension->iRespectFocus = aBool;
	}

 CFont*
CCCAppCommLauncherCustomListBoxData::Font(
		const TListItemProperties& /*aItemProperties*/, TInt aSubCellIndex) const
	{
	return const_cast<CFont*> (SubCellFont(aSubCellIndex));
#if 0
	TInt index = 0;
	if (FindSubCellIndex(index,aSubCellIndex)!=0/*KErrNotFound*/)
	return(NULL);
	TInt actualFontIndex=iSubCellArray->At(index).iActualFontIndex;
	if (actualFontIndex==KNoActualSubCellFont)
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

 void CCCAppCommLauncherCustomListBoxData::Draw(
		TListItemProperties aProperties, CWindowGc& aGc, const TDesC* aText,
		const TRect& aRect, TBool aHighlight, const TColors& aColors) const
	{
	DrawDefaultHighlight(aGc, aRect, aHighlight);

	// Draw the actual items.
	DrawFormatted(aProperties, aGc, aText, aRect, aHighlight, aColors);
	}

 void CCCAppCommLauncherCustomListBoxData::DrawFormatted(
		TListItemProperties aProperties, CWindowGc& aGc, const TDesC* aText,
		const TRect& aItemRect, TBool aHighlight, const TColors& aColors) const
	{
    CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
    if (!view->ViewRect().Intersects(aItemRect))
        {
        // outside of the clipping rect -> don't process this item
        return;
        }
	if (aHighlight)
		{
		iExtension->iClippedSubcells = 0;
		}

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( &aGc );
    if ( transApi )
        {
        transApi->StartDrawing( MAknListBoxTfxInternal::EListItem );
        if(transApi->EffectsDisabled())
            {
            aGc.SetClippingRect( view->ViewRect() );
            }
        }
#else
	CListBoxView* view = static_cast<CEikListBox*>( iExtension->iControl )->View();
	aGc.SetClippingRect( view->ViewRect() );
#endif //RD_UI_TRANSITION_EFFECTS_LIST
	if (UsesScalableLayoutData())
		{
		/* this is a AvKon list or list is created using methods in aknlists.cpp
		 * which is _the_ way to set up a list
		 */
		DrawFormattedSimple(aProperties, aGc, aText, aItemRect, aHighlight,
				aColors);
		}
	else
		{
		/* someone used directly the (unfortunately) exported methods
		 * from this file. These should be fixed in such way, that
		 * DrawFormattedOld could be removed.
		 */
		DrawFormattedOld(aProperties, aGc, aText, aItemRect, aHighlight,
				aColors);
		}

#ifdef RD_UI_TRANSITION_EFFECTS_LIST  
	if (transApi)
		{
        if(transApi->EffectsDisabled())
			{
			aGc.CancelClippingRect();
			}

		transApi->StopDrawing();
		}
#else
	aGc.CancelClippingRect();
#endif //RD_UI_TRANSITION_EFFECTS_LIST 
	}

void CCCAppCommLauncherCustomListBoxData::BitBltColored(CWindowGc& aGc,
		TBool aHighlight, const CGulIcon* aIcon, TInt aSubcell,
		TBool aColorIcon, const TRect& aGraphicRect) const
	{
	// se also eikclbd.cpp ( sigh ).
	CFbsBitmap* bitmap(aIcon->Bitmap());
	CFbsBitmap* mask(aIcon->Mask());

	// possibly colorskinnable icon. Check must be after SetSize()
	TBool bw(bitmap->DisplayMode() == EGray2);

	// center graphics
	TSize size = bitmap->SizeInPixels();
	TInt yDiff = (aGraphicRect.Height() - size.iHeight) / 2;
	TInt xDiff = (aGraphicRect.Width() - size.iWidth) / 2;
	TPoint posInBitmap(0, 0);

	if (xDiff < 0) // icon's width bigger than subcell's width
		{ // possible, if icon is not a aknicon
		posInBitmap.iX = -xDiff;
		xDiff = 0;
		}

	if (yDiff < 0) // icon's height bigger than subcell's height
		{
		posInBitmap.iY = -yDiff;
		yDiff = 0;
		}

	TPoint bmpPos(aGraphicRect.iTl + TPoint(xDiff, yDiff));
	TRect sourcerect(posInBitmap, aGraphicRect.Size());

	if (mask)
		{
		TInt i(0x01);
		i = i << aSubcell;
		if (((i & iExtension->iConditionalCells) && bw) || aColorIcon)
			{
			aGc.BitBltMasked(bmpPos, aHighlight ? iExtension->iHiliBmp
					: iExtension->iColorBmp, TRect(posInBitmap, size), mask,
					ETrue);
			}
		else
			{
			aGc.BitBltMasked(bmpPos, bitmap, sourcerect, mask, ETrue);
			}
		}
	else
		{
		aGc.BitBlt(bmpPos, bitmap, sourcerect);
		}
	}

void CCCAppCommLauncherCustomListBoxData::DrawFormattedSimple(
		TListItemProperties& aProperties, CWindowGc& aGc, const TDesC* aText,
		const TRect& aItemRect, TBool aHighlight, const TColors& aColors) const
	{
	TRect textRect(aItemRect);
	const TColors *subcellColors = &aColors;

	TInt lastSubCell = Min(LastSubCell(), KMaxSubCellIndex);

	TInt subcell = 0;
	TPtrC text;
	TPtrC tempText;
	TRect textShadowRect; // For transparent list
	TRgb textShadowColour = AKN_LAF_COLOR_STATIC(215); // Black shadow for item text.
    MAknsControlContext *cc = AknsDrawUtils::ControlContext( Control() );
    if (!cc)
        {
        cc = SkinBackgroundContext();
        }

    TAknTextLineLayout textLines[KMaxSubCellIndex];
    TBool rectClipped[KMaxSubCellIndex];
    
    Mem::FillZ( &rectClipped[0], KMaxSubCellIndex * sizeof( TBool ) );
    
    if ( iExtension->iSubCellsMightIntersect )
        {
        CheckIfSubCellsIntersect( &textLines[0], &rectClipped[0], *aText, aItemRect );
        }

	TInt SCindex = 0;

    // This loop does the drawing.    
    aGc.SetPenStyle(CGraphicsContext::ENullPen);
    subcell=0;
    FOREVER
        {        
        if (subcell>lastSubCell)
            {
            break;
            }
        
        TextUtils::ColumnText(text,subcell, aText);
        if (text == KNullDesC)
            {
            // empty subcell, continue to draw next subcell
            ++ subcell;
            continue;
            }
        if (!iExtension) break;
        if (iExtension->FindSLSubCellIndex(SCindex,subcell)!=0) break;

        if (UseSubCellColors(subcell))
            {
            subcellColors = &SubCellColors(subcell);
            }
        else
            {
            subcellColors = &aColors;
            }
            
        TRgb color;

        if (aHighlight)
            {
            color = subcellColors->iHighlightedText;
            aGc.SetBrushColor(subcellColors->iHighlightedBack); 
            if ( AknsUtils::AvkonSkinEnabled() )
                {
                if ( iExtension->iHighlightedTextColor != NULL )
                    {
                    color = iExtension->iHighlightedTextColor;
                    }
                }
            }
        else
            {
            color = subcellColors->iText;
            aGc.SetBrushColor(subcellColors->iBack);
            if ( AknsUtils::AvkonSkinEnabled() )
                {
                if ( iExtension->iTextColor != NULL )
                    {
                    color = iExtension->iTextColor;
                    }
                }
            }
        
        // graphics or text column
        if (iExtension->AtSL(SCindex).iSubCellType == CCCAppCommLauncherCustomListBoxDataExtension::EAknSLText)
            {
            const CFont* rowAndCellFont=RowAndSubCellFont(iExtension->iCurrentlyDrawnItemIndex,subcell);
            const CFont* cellFont=Font(aProperties, subcell);
            const CFont* tempFont=(cellFont) ? cellFont : NULL;
            const CFont* usedFont=(rowAndCellFont) ? rowAndCellFont : tempFont;
            
            TAknTextLineLayout textLineLayout = NULL;

            if ( rectClipped[subcell] )
                {
                textLineLayout = textLines[subcell];
                }
            else
                {
                // check if there are icons affecting this text layout
                TInt gSC = iExtension->AtSL(SCindex).iConditionValue; // graphical subcell which might affect this text subcell
                
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
                    textLineLayout = iExtension->AtSL(SCindex).iTextLayout;
                    }
                }

                      
            CGraphicsContext::TTextAlign align=SubCellAlignment(subcell); //gumq
            TBool isLayoutAlignment = iExtension->SubCellLayoutAlignment(subcell); //gumq  
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
            textLayout.LayoutText(textRect, textLineLayout, usedFont);

            SetUnderlineStyle( aProperties, aGc, subcell );

            // * 2 == leave some room for marquee
            const TInt maxlen( KMaxColumnDataLength * 2 ); 
            TBuf<maxlen> convBuf = text.Left(maxlen);

            // Note that this potentially modifies the text so its lenght in pixels
            // might increase. Therefore, this should always be done before
            // wrapping/clipping text. In some cases, WordWrapListItem is called
            // before coming here. Is it certain that it is not done for number subcells?

            // Do number conversion if required.
            if (SubCellIsNumberCell(subcell))
                {
                AknTextUtils::LanguageSpecificNumberConversion(convBuf);
                }

            // Check whether logical to visual conversion should be done here or not.
            TBool bidiConv =
                iExtension->iUseLogicalToVisualConversion &&
                subcell != iExtension->iFirstWordWrappedSubcellIndex &&
                subcell != iExtension->iSecondWordWrappedSubcellIndex;

            TBool doesNotFit( EFalse );
            if ( aHighlight )
                {
                doesNotFit = usedFont->TextWidthInPixels( convBuf ) > textLayout.TextRect().Width();
            
                if ( doesNotFit )
                    {
                    iExtension->iClippedSubcells |= ( 1 << subcell );
                    }
                else 
                    {
                    iExtension->iClippedSubcells &= ~( 1 << subcell );
                    }

			if ( iExtension->iUseClippedByWrap ) // override if already clipped

				{
				iExtension->iClippedSubcells = iExtension->iClippedByWrap;
				}
			}

		// marquee
		CAknMarqueeControl* marquee =
		subcell == 1 || subcell == 3 ? iExtension->iMarquee :
		iExtension->i2ndLineMarquee;
		TBool marqueeDisabled =
		static_cast<CEikListBox*>(
				Control() )->View()->ItemDrawer()->Flags() & CListItemDrawer::EDisableMarquee;

		if ( aHighlight && iExtension->IsMarqueeOn() && doesNotFit && !marqueeDisabled )
			{
			iExtension->iMarquee->UseLogicalToVisualConversion( bidiConv );

			if ( marquee->DrawText( aGc, textRect, textLineLayout, convBuf, usedFont, color ) )
				{
				// all the loops have been executed
				textLayout.DrawText( aGc, convBuf, bidiConv, color );
				}
			}
		else
			{
			if ( marquee && marqueeDisabled )
				{
				marquee->Stop();
				}

			textLayout.DrawText( aGc, convBuf, bidiConv, color );
			}

		if ( iExtension->iPictoInterface )
			{

			TRect pictoRect = textLayout.TextRect();
			pictoRect.Normalize();
			if ( convBuf.Length() && bidiConv )
				{
				TInt maxWidth = pictoRect.Size().iWidth;
				TInt charsCanBeDisplayed = textLayout.Font()->TextCount(
						convBuf, maxWidth );
				if ( charsCanBeDisplayed < convBuf.Length() )
					{
					TInt clipCharWidth = textLayout.Font()->CharWidthInPixels(
							KEllipsis /*aClipChar*/);
					// Check how many characters fit in given space with truncation char. 
					charsCanBeDisplayed = textLayout.Font()->TextCount(
							convBuf, maxWidth - clipCharWidth );

					// This is "avkon rule": should not insert ellipsis right after a space.
					if ( charsCanBeDisplayed> 1 &&
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

			/*iExtension->iPictoInterface->Interface()->DrawPictographsInText(
					aGc, *usedFont, convBuf,
					pictoRect,
					textLayout.BaselineOffset(),
					textLayout.Align(),
					0 );*/ //qiang
			}

		aGc.SetUnderlineStyle( EUnderlineOff );
		}
	else // Graphics subcell

		{
		TLex lex(text);
		TInt index;
		TInt indexValue;

		indexValue=lex.Val(index);

		if (indexValue!=KErrNone)
			{
			// wrong icon index
			++ subcell;
			continue;
			}

		//__ASSERT_DEBUG(indexValue==KErrNone, Panic(EAknPanicFormattedCellListInvalidBitmapIndex));

		TBool colorIcon( EFalse );
		if ( index == KColorIconFlag )
			{
			if ( iExtension->iColorBmp && iExtension->iHiliBmp )
				{
				colorIcon = ETrue;
				}
			index = KColorIconIdx;
			}

		if( !iIconArray )
			{
			continue;
			}

		if (aHighlight && (index> 0xffff))
			{
			index = index >> 16; // we have different icon for highlight
			}

		index = index & 0xffff; // mask off possible highlight icon
		//__ASSERT_DEBUG((index>=0 && index<iIconArray->Count()),Panic(EAknPanicFormattedCellListInvalidBitmapIndex));

		TAknWindowLineLayout graphicLayout = iExtension->AtSL(SCindex).iGraphicLayout;
		TAknLayoutRect graphicRect;

		graphicRect.LayoutRect(textRect,graphicLayout);
		TSize graphicSize( graphicRect.Rect().Size() );

		if (index>=0 && iIconArray)
			{
			CGulIcon* icon=(*iIconArray)[index];
			CFbsBitmap* bitmap=icon->Bitmap();
			// Sometimes we get fake bitmap...
			if ( bitmap )
				{
				TSize size( bitmap->SizeInPixels() ); // set size if not already
				if ( size.iWidth != graphicSize.iWidth && size.iHeight != graphicSize.iHeight )
					{
					AknIconUtils::SetSize( bitmap,
							graphicSize,
							EAspectRatioPreservedAndUnusedSpaceRemoved);
					}
				BitBltColored( aGc, aHighlight, icon, subcell, colorIcon, graphicRect.Rect() );
				aGc.SetPenStyle(CGraphicsContext::ESolidPen);
				}
			}
		}
	++subcell;
	}

// Clear information of wordwrapped and bidi-reordered subcells.
const_cast<CCCAppCommLauncherCustomListBoxData*>( this )->
SetWordWrappedSubcellIndices( -1, -1 );
}

 CCCAppCommLauncherCustomListBoxData::CCCAppCommLauncherCustomListBoxData() :
	CListBoxData()
	{
	}

 void CCCAppCommLauncherCustomListBoxData::ConstructLD()
	{
	TRAPD( err, DoConstructL( KAknsIIDQsnAnimList ) );
	if (err)
		{
		delete (this);
		User::Leave(err);
		}
	}

 void CCCAppCommLauncherCustomListBoxData::ConstructLD(
		const TAknsItemID& aAnimationIID)
	{
	TRAPD( err, DoConstructL( aAnimationIID ) );
	if (err)
		{
		delete (this);
		User::Leave(err);
		}
	}

void CCCAppCommLauncherCustomListBoxData::DoConstructL(
		const TAknsItemID& aAnimationIID)
	{
	iSubCellArray = new (ELeave) CArrayFixFlat<SSubCell> (
			KSubCellListBoxGranularity);
	iExtension = new (ELeave) CCCAppCommLauncherCustomListBoxDataExtension;

	iExtension->iSkinEnabled = AknsUtils::AvkonSkinEnabled();
	SetupSkinContextL();

	// Preallocate so that SizeChanged() methods do not leave so often.
	CListBoxData::ConstructL(NULL);
	iExtension->ConstructL(this, aAnimationIID);
	CListBoxData::SetSkinBackgroundControlContextL(
			iExtension->iSkinControlContext);
	AddSubCellL(0);
	AddSubCellL(1);
	AddSubCellL(2);
	AddSubCellL(3);
	AddSubCellL(4);
	}

TInt CCCAppCommLauncherCustomListBoxData::LastSubCell() const
	{
	TInt count = iSubCellArray->Count();
	if (!count)
		return (KErrNotFound);
	return (At(count - 1).iSubCell);
	}

void CCCAppCommLauncherCustomListBoxData::AddSubCellL(TInt aSubCell)
	{
	SSubCell subcell;
	TMargins m =
		{
		0, 0, 0, 0
		};
	subcell.iSubCell = aSubCell;
	subcell.iWidth = 0;
	subcell.iMargin = m;
	subcell.iBaseFont = NULL;
	subcell.iActualFontIndex = KNoActualSubCellFont;
	subcell.iGraphics = EFalse;
	subcell.iTransparent = EFalse;
	subcell.iNotAlwaysDrawn = EFalse;
	subcell.iRightSeparatorStyle = CGraphicsContext::ENullPen;
	subcell.iAlign = CGraphicsContext::ELeft;
	subcell.iPosition = TPoint(0, 0);
	subcell.iBaseline = 0;
	subcell.iSize = TSize(0, 0);
	subcell.iUseSubCellColors = EFalse;
	subcell.iTextClipGap = 0;
	subcell.iNumberCell = 0;
	TKeyArrayFix key(0, ECmpTInt);
	iSubCellArray->InsertIsqL(subcell, key);
	}

CCCAppCommLauncherCustomListBoxData::SSubCell&
CCCAppCommLauncherCustomListBoxData::At(TInt aArrayIndex)
	{
	//__ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iSubCellArray->Count(),Panic(EAknPanicOutOfRange));
	return (iSubCellArray->At(aArrayIndex));
	}

const CCCAppCommLauncherCustomListBoxData::SSubCell&
CCCAppCommLauncherCustomListBoxData::At(TInt aArrayIndex) const
	{
	//__ASSERT_DEBUG(aArrayIndex>=0 && aArrayIndex<iSubCellArray->Count(),Panic(EAknPanicOutOfRange));
	return (iSubCellArray->At(aArrayIndex));
	}

TInt CCCAppCommLauncherCustomListBoxData::FindSubCellIndex(TInt& aArrayIndex,
		TInt aSubCell) const
	{
	if (aSubCell < 0 || iSubCellArray->Count() == 0)
		return (KErrNotFound);
	TKeyArrayFix key(0, ECmpTInt);
	SSubCell subcell;
	subcell.iSubCell = aSubCell;
	TInt retVal = iSubCellArray->FindIsq(subcell, key, aArrayIndex);
	if (retVal != 0)
		{
		return KErrNotFound;
		}

	return retVal;
	}

void CCCAppCommLauncherCustomListBoxData::FindSubCellIndexOrAddL(
		TInt& aArrayIndex, TInt aSubCell)
	{
	if (FindSubCellIndex(aArrayIndex, aSubCell) == KErrNotFound)
		{
		AddSubCellL(aSubCell);
		FindSubCellIndex(aArrayIndex, aSubCell);
		}
	}

 CCCAppCommLauncherCustomListBoxData::TColors::TColors() :
			iText(KRgbBlack), iBack(KRgbWhite), iHighlightedText(KRgbWhite), iHighlightedBack(
					KRgbBlack),
			iRightSeparatorColor(AKN_LAF_COLOR_STATIC(215))
	{
	}

TInt CCCAppCommLauncherCustomListBoxData::AddActualFontL(const CFont* aBaseFont)
	{
	if (!iNormalFont.iFonts)
		{
		iNormalFont.iFonts = new (ELeave) CArrayPtrFlat<CFont> (1);
		iBoldFont.iFonts = new (ELeave) CArrayPtrFlat<CFont> (1);
		iItalicFont.iFonts = new (ELeave) CArrayPtrFlat<CFont> (1);
		iBoldItalicFont.iFonts = new (ELeave) CArrayPtrFlat<CFont> (1);
		}

	// Reserves extra space for each font array.
	TInt fontCount = iNormalFont.iFonts->Count() + 1;
	iNormalFont.iFonts->SetReserveL(fontCount);
	iBoldFont.iFonts->SetReserveL(fontCount);
	iItalicFont.iFonts->SetReserveL(fontCount);
	iBoldItalicFont.iFonts->SetReserveL(fontCount);

	// Add Fonts.
	ConstructFontL(aBaseFont, TFontStyle(EPostureUpright, EStrokeWeightNormal,
			EPrintPosNormal), iNormalFont);
	ConstructFontL(aBaseFont, TFontStyle(EPostureUpright, EStrokeWeightBold,
			EPrintPosNormal), iBoldFont);
	ConstructFontL(aBaseFont, TFontStyle(EPostureItalic, EStrokeWeightNormal,
			EPrintPosNormal), iItalicFont);
	ConstructFontL(aBaseFont, TFontStyle(EPostureItalic, EStrokeWeightBold,
			EPrintPosNormal), iBoldItalicFont);

	return fontCount - 1;
	}

void CCCAppCommLauncherCustomListBoxData::DrawDefaultHighlight(CWindowGc &aGc,
		const TRect &aItemRect, TBool aHighlight) const
	{
	// When this flag set, draw pressed down state and don't use animation.
	// There are several derived CListboxData in this file. All of them do  
	// this same thing.
	CListBoxView* view =
			static_cast<CEikListBox*> (iExtension->iControl)->View();
	TBool pressedDown = view->ItemDrawer()->Flags()
			& CListItemDrawer::EPressedDownState;

	if (aHighlight)
		{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
		MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(&aGc);
		if (transApi)
			{
			transApi->Invalidate(MAknListBoxTfxInternal::EListHighlight);
			if (!transApi->BeginRedraw(MAknListBoxTfxInternal::EListHighlight,
					aItemRect))
				{
				// No need to execute drawing if no drawing will occur
				transApi->EndRedraw(MAknListBoxTfxInternal::EListHighlight);
				return;
				}
			transApi->StartDrawing(MAknListBoxTfxInternal::EListHighlight);

			}
#endif //RD_UI_TRANSITION_EFFECTS_LIST
		TBool highlightDrawn = EFalse;

		TAknLayoutRect outerRect;
		TAknLayoutRect innerRect;
		outerRect.LayoutRect(
				aItemRect,
				TAknWindowComponentLayout::Compose(
						AknLayoutScalable_Avkon::list_highlight_pane_cp1(),
						AknLayoutScalable_Avkon::list_highlight_pane_g10_cp1()).LayoutLine());
		innerRect.LayoutRect(
				aItemRect,
				TAknWindowComponentLayout::Compose(
						AknLayoutScalable_Avkon::list_highlight_pane_cp1(),
						AknLayoutScalable_Avkon::list_highlight_pane_g1_cp1()).LayoutLine());

		if (iExtension->iAnimation) // animated highlight
			{
			aGc.SetPenStyle(CGraphicsContext::ENullPen);
			highlightDrawn = iExtension->SyncAndDrawAnim(aGc, outerRect.Rect());
			}

		if (!highlightDrawn) // normal skinned highlight
			{
			if (pressedDown)
				{
				TRAP_IGNORE( highlightDrawn = iExtension->DrawPressedDownEffectL(
						AknsUtils::SkinInstance(), aGc, outerRect.Rect(),
						innerRect.Rect()) );
				}
			else
				{
				MAknsControlContext *cc = AknsDrawUtils::ControlContext(
						Control());

				if (!cc)
					{
					cc = SkinBackgroundContext();
					}

				if (cc)
					{

					aGc.SetPenStyle(CGraphicsContext::ENullPen);

					highlightDrawn = AknsDrawUtils::DrawFrame(
							AknsUtils::SkinInstance(), aGc, outerRect.Rect(),
							innerRect.Rect(),
							*iExtension->iSkinHighlightFrameId,
							*iExtension->iSkinHighlightFrameCenterId);
					}
				}
			}

		if (!highlightDrawn) // legacy fallback
			{
			TRgb color(AKN_LAF_COLOR( 244 ));
			aGc.SetPenColor(color);
			aGc.SetBrushColor(color);
			aGc.SetPenStyle(CGraphicsContext::ESolidPen);
			aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGc.DrawRect(aItemRect);
			}
#ifdef RD_UI_TRANSITION_EFFECTS_LIST  
		if (transApi)
			{
			transApi->StopDrawing();
			transApi->EndRedraw(MAknListBoxTfxInternal::EListHighlight);
			}
#endif
		}
	}

void CCCAppCommLauncherCustomListBoxData::DrawSettingHighlight(CWindowGc &aGc,
		const TRect &aItemRect, TBool aHighlight) const
	{
	DrawDefaultHighlight(aGc, aItemRect, aHighlight);
	}

void CCCAppCommLauncherCustomListBoxData::DrawPopupHighlight(CWindowGc &aGc,
		const TRect &aItemRect, TBool aHighlight) const
	{
	DrawDefaultHighlight(aGc, aItemRect, aHighlight);
	}

void CCCAppCommLauncherCustomListBoxData::DrawPopupFrame(CWindowGc &aGc) const
	{
	CCoeControl* control = Control();

	if (control)
		{
		aGc.SetPenStyle(CGraphicsContext::ENullPen);

		TBool done = AknsDrawUtils::Background(AknsUtils::SkinInstance(),
				iExtension->iPopupFrame, control, aGc, control->Rect());

		if (!done)
			{
			aGc.Clear(control->Rect());
			}
		}
	}

void CCCAppCommLauncherCustomListBoxData::SetWordWrappedSubcellIndices(
		TInt aFirstIndex, TInt aSecondIndex)
	{
	iExtension->iFirstWordWrappedSubcellIndex = (TInt16) aFirstIndex;
	iExtension->iSecondWordWrappedSubcellIndex = (TInt16) aSecondIndex;
	}

 void CCCAppCommLauncherCustomListBoxData::EnableMarqueeL(TBool aEnable)
	{
	// CreateMarqueeControlL does nothing if marquee already exists,
	// so let's just call it just in case.
	CreateMarqueeControlL();
	iExtension->iMarquee->EnableMarquee(aEnable);
	iExtension->i2ndLineMarquee->EnableMarquee(aEnable);
	}

// Two versions to eliminate compiler warnings.
#ifdef __WINS__
 const TBool CCCAppCommLauncherCustomListBoxData::IsMarqueeOn()
	{
	return iExtension->IsMarqueeOn();
	}
#else
 TBool CCCAppCommLauncherCustomListBoxData::IsMarqueeOn()
	{
	return iExtension->IsMarqueeOn();
	}
#endif // __WINS__

void CCCAppCommLauncherCustomListBoxData::FocusGained()
	{
	iExtension->FocusGained();
	}

void CCCAppCommLauncherCustomListBoxData::FocusLost()
	{
	iExtension->FocusLost();
	}

void CCCAppCommLauncherCustomListBoxData::HandleResourceChange(TInt aType)
	{
	// Animation is skin dependent, whenever skin changes animation changes
	// too.
	if (KAknsMessageSkinChange == aType)
		{
		iExtension->SkinChanged();
		}
	else if (aType == KEikDynamicLayoutVariantSwitch)
		{
		// What is under highlight may have changed -> we need to update
		// highlight background to animation.
		iExtension->iAnimFlags.Set(
				CCCAppCommLauncherCustomListBoxDataExtension::EFlagUpdateBg);
		}
    else if( ( aType == KEikMessageUnfadeWindows ) ||
             ( aType == KEikMessageFadeAllWindows ) )
        {
        if( iExtension->iMarquee )
            {
            iExtension->iMarquee->HandleResourceChange( aType );
            }
        if( iExtension->i2ndLineMarquee )
            {
            iExtension->i2ndLineMarquee->HandleResourceChange( aType );
            }
        }
	}
// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::HighlightAnim
// -----------------------------------------------------------------------------
//
 const CAknsEffectAnim* CCCAppCommLauncherCustomListBoxData::HighlightAnim() const
	{
	if (iExtension)
		return iExtension->iAnimation;
	return NULL;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::AboutToDrawHighlightAnim
// -----------------------------------------------------------------------------
//
 void CCCAppCommLauncherCustomListBoxData::AboutToDrawHighlightAnim() const
	{
	if (!iExtension)
		{
		return;
		}
	if (!iExtension->iAnimation || !iExtension->iControl)
		{
		return;
		}

	CEikListBox* list = static_cast<CEikListBox*> (iExtension->iControl);
	CListBoxView* view = list->View();
	if (!view)
		{
		return;
		}

	if (view->CurrentItemIndex() != iExtension->iCurrentRow)
		{
		iExtension->iAnimFlags.Set(
				CCCAppCommLauncherCustomListBoxDataExtension::EFlagUpdateBg);
		iExtension->iCurrentRow = view->CurrentItemIndex();
		}

	iExtension->SyncAnim(iExtension->iAnimSize);
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::SetHighlightAnimBackgroundDrawer
// -----------------------------------------------------------------------------
//
 void CCCAppCommLauncherCustomListBoxData::SetHighlightAnimBackgroundDrawer(
		MListBoxAnimBackgroundDrawer* aDrawer)
	{
	if (iExtension)
		{
		iExtension->iHighlightBgDrawer = aDrawer;
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::SetItemCellSize
// -----------------------------------------------------------------------------
//
 void CCCAppCommLauncherCustomListBoxData::SetItemCellSize(
		const TSize& aSize)
	{
	if (iExtension)
		{
		if (iExtension->iAnimSize != aSize)
			{
			iExtension->Play();
			}
		iExtension->iAnimSize = aSize;
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::HasHighlightAnim
// -----------------------------------------------------------------------------
//
 TBool CCCAppCommLauncherCustomListBoxData::HasHighlightAnim() const
	{
	if (!iExtension)
		return EFalse;
	if (!iExtension->iAnimation)
		return EFalse;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::DrawHighlightAnim
// -----------------------------------------------------------------------------
//
 TBool CCCAppCommLauncherCustomListBoxData::DrawHighlightAnim(
		CBitmapContext& aGc, const TRect& aRect) const
	{
	if (!iExtension)
		return EFalse;

	if (!iExtension->iAnimation)
		return EFalse;

	return iExtension->SyncAndDrawAnim(aGc, aRect);
	}

void CCCAppCommLauncherCustomListBoxData::ResetMarquee()
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

TInt CCCAppCommLauncherCustomListBoxData::CurrentMarqueeItemIndex()
	{
	return iExtension->iCurrentItem;
	}

void CCCAppCommLauncherCustomListBoxData::SetCurrentMarqueeItemIndex(
		TInt aIndex)
	{
	iExtension->iCurrentItem = aIndex;
	}

void CCCAppCommLauncherCustomListBoxData::SetCurrentItemIndex(TInt aIndex)
	{
	if (iExtension->iCurrentRow != aIndex)
		{
		iExtension->iAnimFlags.Set(
				CCCAppCommLauncherCustomListBoxDataExtension::EFlagUpdateBg);
		}
	iExtension->iCurrentRow = aIndex;
	}

void CCCAppCommLauncherCustomListBoxData::SetCurrentlyDrawnItemIndex(
		TInt aIndex)
	{
	if (iExtension)
		{
		iExtension->iCurrentlyDrawnItemIndex = aIndex;
		}
	}

 const CFont*
CCCAppCommLauncherCustomListBoxData::RowAndSubCellFont(TInt aRow,
		TInt aSubCellIndex) const
	{
	if (!iExtension)
		return 0;
	TInt index = 0;
	if (iExtension->FindRowAndSubCellIndex(index, aRow, aSubCellIndex) != 0)
		return 0;
	return (iExtension->At(index).iFont);
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellFontForRowL(
		TInt aRowIndex, TInt aSubCellIndex, const CFont* aFont)
	{
	if (!iExtension)
		return;
	TInt index = 0;
	iExtension->FindRowAndSubCellIndexOrAddL(index, aRowIndex, aSubCellIndex);
	iExtension->At(index).iFont = aFont;
	}

/**
 * Enables / disables transparency effect, ie. does the listbox draw it's own background or not.
 * @param aDrawBackground  EFalse enables transparency
 */
 void CCCAppCommLauncherCustomListBoxData::SetDrawBackground(
		const TBool aDrawBackground)
	{
	iExtension->iDrawBackground = aDrawBackground;
	if (!aDrawBackground)
		{
		iExtension->DeleteAnim();
		}
	}

/**
 * Is the listbox drawing the background itself or not (= "transparency")
 */
TBool CCCAppCommLauncherCustomListBoxData::IsBackgroundDrawingEnabled() const
	{
	return iExtension->iDrawBackground;
	}

/**
 * Enables / disables drawing background after scrollbar
 * @param aDrawBackground  default ETrue
 */
void CCCAppCommLauncherCustomListBoxData::SetDrawScrollbarBackground(
		const TBool aDrawBackground)
	{
	iExtension->iDrawScrollbarBackground = aDrawBackground;
	}

/**
 * Is the listbox drawing the background after scrollbar or not
 */
TBool CCCAppCommLauncherCustomListBoxData::IsScrollbarBackgroundDrawingEnabled() const
	{
	return iExtension->iDrawScrollbarBackground;
	}

 void CCCAppCommLauncherCustomListBoxData::SetMarqueeParams(
		const TInt aLoops, const TInt aScrollAmount, const TInt aScrollDelay,
		const TInt aInterval)
	{
	if (iExtension->iMarquee)
		{
		iExtension->iMarquee->SetLoops(aLoops);
		iExtension->iMarquee->SetSpeedInPixels(aScrollAmount);
		iExtension->iMarquee->SetDelay(aScrollDelay);
		iExtension->iMarquee->SetInterval(aInterval);
		}
	if (iExtension->i2ndLineMarquee)
		{
		iExtension->i2ndLineMarquee->SetLoops(aLoops);
		iExtension->i2ndLineMarquee->SetSpeedInPixels(aScrollAmount);
		iExtension->i2ndLineMarquee->SetDelay(aScrollDelay);
		iExtension->i2ndLineMarquee->SetInterval(aInterval);
		}
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubCellIconSize(
		TInt aIndex, TSize aSize)
	{
    if ( iExtension && aIndex < KMaxSubCellIndex && aIndex >= 0 )
		{
		iExtension->iSubCellIconSize[aIndex] = aSize;
		}
	}

TSize CCCAppCommLauncherCustomListBoxData::GetSubCellIconSize(TInt aIndex)
	{
    if ( iExtension && aIndex < KMaxSubCellIndex && aIndex >= 0 )
		{
		return iExtension->iSubCellIconSize[aIndex];
		}
	return TSize(0, 0);
	}

void CCCAppCommLauncherCustomListBoxData::SetupSkinContextL()
	{
	//__ASSERT_DEBUG( iExtension, Panic( EAknPanicNullPointer ));

	// list's own rect cannot be used since it might be unknown at this point
	TRect mainPane;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPane);

	TRect mainPaneRect(mainPane.Size());

	if (!iExtension->iSkinControlContext)
		{
		iExtension->iSkinControlContext
				= CAknsListBoxBackgroundControlContext::NewL(
						KAknsIIDQsnBgAreaMainListGene, mainPaneRect, EFalse, // Changed to EFalse: See TSW defect avao-6eelpv
						KAknsIIDNone, mainPaneRect);
		}
	else
		{
		iExtension->iSkinControlContext->SetRect(mainPaneRect);
		}

	if (iExtension->iPopupFrame)
		{
		iExtension->iPopupFrame->SetRect(mainPaneRect);
		}

	iExtension->iSkinHighlightFrameId = &KAknsIIDQsnFrList;
	iExtension->iSkinHighlightFrameCenterId = &KAknsIIDQsnFrListCenter;
	}

// extended skin support
void CCCAppCommLauncherCustomListBoxData::SetESSTextColor(
		TAknsQsnTextColorsIndex aIndex)
	{
	TRgb color;

	TInt error = AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
			KAknsIIDQsnTextColors, aIndex);
	if (!error && iExtension)
		{
		iExtension->iTextColor = color;
		}
	}

// extended skin support
void CCCAppCommLauncherCustomListBoxData::SetESSHighlightedTextColor(
		TAknsQsnTextColorsIndex aIndex)
	{
	TRgb color;

	TInt error = AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
			KAknsIIDQsnTextColors, aIndex);
	if (!error)
		{
		iExtension->iHighlightedTextColor = color;
		}
	}

void CCCAppCommLauncherCustomListBoxData::SetGraphicSubCellL(TInt aSubCell,
		const TAknWindowLineLayout &aGraphicLayout)
	{
	if (!iExtension)
		return;
	CEikFormattedCellListBox* list =
			static_cast<CEikFormattedCellListBox*> (iExtension->iControl);
	// this does happen with caknsetstyle, caknform(wide)style lists.
	// eventually they'll get here again with list!=0, so this check is enough
	if (!list)
		return;

	TInt index = 0;
	iExtension->FindSLSubCellIndexOrAddL(index, aSubCell);
	iExtension->AtSL(index).iTextLayout = NULL;
	iExtension->AtSL(index).iGraphicLayout = aGraphicLayout;
	iExtension->AtSL(index).iSubCellType
			= CCCAppCommLauncherCustomListBoxDataExtension::EAknSLGraphic;

	// For compabitility - needed at least for text wrapping.
	// Beware - some of these WILL be overriden if you got here trough
	// old layout methods like AknListBoxLayouts::SetupFormTextCell()
	// & friends.
	TAknLayoutRect r;

	r.LayoutRect(list->View()->ItemDrawer()->ItemCellSize(), aGraphicLayout);
	SetSubCellSizeL(aSubCell, r.Rect().Size());
	SetSubCellPositionL(aSubCell, r.Rect().iTl);
	SetSubCellAlignmentL(aSubCell, CGraphicsContext::ECenter);
	SetGraphicsSubCellL(aSubCell, ETrue);
	UseScalableLayoutData(ETrue);
	if (iExtension->iMarginRect == TRect::EUninitialized)
		{
		iExtension->iMarginRect = r.Rect();
		}
	else
		{
		iExtension->iMarginRect.BoundingRect(r.Rect());
		}
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(
			list->View()->ItemDrawer()->Gc());
	if (transApi)
		{
		transApi->SetPosition(MAknListBoxTfxInternal::EListTLMargin,
				iExtension->iMarginRect.iTl);
		TSize size = list->View()->ItemDrawer()->ItemCellSize();
		TPoint br(size.AsPoint() - iExtension->iMarginRect.iBr);
		transApi->SetPosition(MAknListBoxTfxInternal::EListBRMargin, br);
		}
#endif
	}

void CCCAppCommLauncherCustomListBoxData::SetTextSubCellL(TInt aSubCell,
		const TAknTextLineLayout &aTextLayout)
	{
	if (!iExtension)
		return;
	CEikFormattedCellListBox* list =
			static_cast<CEikFormattedCellListBox*> (iExtension->iControl);
	// this does happen with caknsetstyle, caknform(wide)style lists.
	// eventually they'll get here again with list!=0, so this check is enough
	if (!list)
		return;

	TInt index = 0;
	iExtension->FindSLSubCellIndexOrAddL(index, aSubCell);
	iExtension->AtSL(index).iTextLayout = aTextLayout;
	iExtension->AtSL(index).iGraphicLayout = NULL;
	iExtension->AtSL(index).iSubCellType
			= CCCAppCommLauncherCustomListBoxDataExtension::EAknSLText;

	// For compabitility - needed at least for text wrapping.
	// Beware - some of these WILL be overriden if you got here trough
	// old layout methods like AknListBoxLayouts::SetupFormTextCell()
	// & friends.
	TAknLayoutText t;
	t.LayoutText(list->View()->ItemDrawer()->ItemCellSize(), aTextLayout);
	SetSubCellSizeL(aSubCell, t.TextRect().Size());
	SetSubCellPositionL(aSubCell, t.TextRect().iTl);
	SetSubCellFontL(aSubCell, t.Font());
	SetSubCellAlignmentL(aSubCell, AknLayoutUtils::TextAlignFromId(
			aTextLayout.iJ));
	TInt B = CorrectBaseline(
			list->View()->ItemDrawer()->ItemCellSize().iHeight, aTextLayout.iB,
			aTextLayout.FontId());
	SetSubCellBaselinePosL(aSubCell, B);
	UseScalableLayoutData(ETrue);
	if (iExtension->iMarginRect == TRect::EUninitialized)
		{
		iExtension->iMarginRect = t.TextRect();
		}
	else
		{
		iExtension->iMarginRect.BoundingRect(t.TextRect());
		}
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(
			list->View()->ItemDrawer()->Gc());
	if (transApi)
		{
		transApi->SetPosition(MAknListBoxTfxInternal::EListTLMargin,
				iExtension->iMarginRect.iTl);
		TSize size = list->View()->ItemDrawer()->ItemCellSize();
		TPoint br(size.AsPoint() - iExtension->iMarginRect.iBr);
		transApi->SetPosition(MAknListBoxTfxInternal::EListBRMargin, br);
		}
#endif

	}


TInt CCCAppCommLauncherCustomListBoxData::CorrectBaseline(TInt aParentHeight, TInt aBaseline, TInt aFontId)
    {
    TInt B = aBaseline;

    if( TAknFontId::IsEncodedFont(aFontId) && aFontId != ELayoutEmpty ) // This tells us that the font id has come from scalable layout API
        {
        TInt b = aBaseline;
        if (IsParentRelative(b)) { b = aParentHeight - ELayoutP + b; }

        const CAknLayoutFont *font = AknLayoutUtils::LayoutFontFromId(aFontId);
        TInt descent = font->BaselineToTextPaneBottom();

        // calculate distance down from top of parent to bottom of maximal glyph
        TInt top2bog = aParentHeight - b;

        // distance down to baseline is distance down to bottom of glyph minus descent
        // A further 1 is subtracted to account for the definition of baseline in the
        // Series 60 pre-2.8 layout specifications.
        B = top2bog - descent - 1;
        }

    return B;
    }
TBool CCCAppCommLauncherCustomListBoxData::IsParentRelative(TInt aVal)
    {
    return aVal > ELayoutP-ELayoutPRange && aVal <= ELayoutP;
    }

void CCCAppCommLauncherCustomListBoxData::SetConditionalSubCellL(TInt aSubCell,
		const TAknTextLineLayout &aTextLayout, TInt aAffectedSubCell)
	{
	// iConditionValue of affected subcell (=text subcell, which has different layouts)
	// contains index of graphical subcell, which existence should be checked first.
	// This graphical subcell has in iConditionValue index of graphical subcell,
	// which existence should be checked second etc. Each graphical subcell can
	// affect only to 1 text subcell (or none).

	// for compabitility - needed at least for text wrapping
	SetNotAlwaysDrawnSubCellL(aSubCell, ETrue);

	if (!iExtension)
		return;

	TInt i = 0x01;
	i = i << aSubCell;
	iExtension->iConditionalCells = iExtension->iConditionalCells | i;

	TInt graphicalIndex = 0;
	if (iExtension->FindSLSubCellIndex(graphicalIndex, aSubCell) != 0)
		return; // subcell not found
	// conditional layoutline can be only added to graphical subcells
	if (iExtension->AtSL(graphicalIndex).iSubCellType
			!= CCCAppCommLauncherCustomListBoxDataExtension::EAknSLGraphic)
		return;

	TInt textIndex = 0; // index of affected subcell
	if (iExtension->FindSLSubCellIndex(textIndex, aAffectedSubCell) != 0)
		return; // subcell not found
	// affected subcell can only be text subcell
	if (iExtension->AtSL(textIndex).iSubCellType
			== CCCAppCommLauncherCustomListBoxDataExtension::EAknSLGraphic)
		return;

	TInt gSC = iExtension->AtSL(textIndex).iConditionValue; // text subcell to be added in priority chain

	while (gSC > -1)
		{
		if (iExtension->FindSLSubCellIndex(textIndex, gSC) != 0)
			return; // subcell not found
		gSC = iExtension->AtSL(textIndex).iConditionValue;
		}
	iExtension->AtSL(textIndex).iConditionValue = aSubCell; // add next subcell to chain
	iExtension->AtSL(graphicalIndex).iTextLayout = aTextLayout;

	iExtension->CreateColorBitmapsL(SubCellSize(aSubCell));

	TRect r(SubCellPosition(aSubCell), SubCellSize(aSubCell));
	if (iExtension->iMarginRect == TRect::EUninitialized)
		{
		iExtension->iMarginRect = r;
		}
	else
		{
		iExtension->iMarginRect.BoundingRect(r);
		}
	CEikFormattedCellListBox* list =
			static_cast<CEikFormattedCellListBox*> (iExtension->iControl);
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
	MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal(
			list->View()->ItemDrawer()->Gc());
	if (transApi)
		{
		transApi->SetPosition(MAknListBoxTfxInternal::EListTLMargin,
				iExtension->iMarginRect.iTl);
		TSize size = list->View()->ItemDrawer()->ItemCellSize();
		TPoint br(size.AsPoint() - iExtension->iMarginRect.iBr);
		transApi->SetPosition(MAknListBoxTfxInternal::EListBRMargin, br);
		}
#endif
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::UsesScalableLayoutData
// -----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherCustomListBoxData::UsesScalableLayoutData() const
	{
	if (iExtension)
		return iExtension->iSimpleList;

	return EFalse;
	}
void CCCAppCommLauncherCustomListBoxData::UseScalableLayoutData(TBool aUse)
	{
	if (iExtension)
		{
		if (!aUse && iExtension->iSimpleList)
			{
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
            CEikFormattedCellListBox* list = static_cast<CEikFormattedCellListBox*>( iExtension->iControl );
            MAknListBoxTfxInternal* transApi =
                CAknListLoader::TfxApiInternal( list->View()->ItemDrawer()->Gc() );
            if ( transApi )
                {
                transApi->SetPosition( MAknListBoxTfxInternal::EListTLMargin, TPoint( 0, 0 ) );
                transApi->SetPosition( MAknListBoxTfxInternal::EListBRMargin, TPoint( 0, 0 ) );
                }
#endif
			}
		iExtension->iSimpleList = aUse;
		}
	}

 void CCCAppCommLauncherCustomListBoxData::SetSubcellUnderlined(
		TBitFlags32 aUnderlinedCells)
	{
	if (iExtension)
		{
		iExtension->iUnderlineFlagSet = ETrue;
		iExtension->iUnderlineFlags = aUnderlinedCells;
		}
	}

void CCCAppCommLauncherCustomListBoxData::SetUnderlineStyle(
		TListItemProperties aProperties, CWindowGc& aGc, TInt aSubCell) const
	{
	if (!iExtension)
		{
		return;
		}

	if (!iExtension->iUnderlineFlagSet)
		{
		// underlining is already either on or off and
		// hardcoded off turning will ensure old style
		// behaviour
		return;
		}

	if (aProperties.IsUnderlined() && iExtension->iUnderlineFlagSet
			&& iExtension->iUnderlineFlags.IsSet(aSubCell))
		{
		aGc.SetUnderlineStyle(EUnderlineOn);
		}
	else
		{
		aGc.SetUnderlineStyle(EUnderlineOff);
		}
	}

void CCCAppCommLauncherCustomListBoxData::UseHighlightIconSwapping(TBool aUse)
	{
	if (iExtension)
		{
		iExtension->iUseHighligthIconSwapping = aUse;
		}
	}

TBool CCCAppCommLauncherCustomListBoxData::UseHighlightIconSwapping() const
	{
	if (iExtension)
		{
		return iExtension->iUseHighligthIconSwapping;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::SetStretchableGraphicSubCellL
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::SetStretchableGraphicSubCellL(
		TInt aSubCell, const TAknWindowComponentLayout& aNormalLayout,
		const TAknWindowComponentLayout& aStretchedLayout)
	{
	if (Layout_Meta_Data::IsLandscapeOrientation()
			&& Layout_Meta_Data::IsListStretchingEnabled()
			&& StretchingEnabled())
		{
		SetGraphicSubCellL(aSubCell, aStretchedLayout.LayoutLine());
		}
	else
		{
		SetGraphicSubCellL(aSubCell, aNormalLayout.LayoutLine());
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::SetStretchableTextSubCellL
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::SetStretchableTextSubCellL(
		TInt aSubCell, const TAknTextComponentLayout& aNormalLayout,
		const TAknTextComponentLayout& aStretchedLayout)
	{
	if (Layout_Meta_Data::IsLandscapeOrientation()
			&& Layout_Meta_Data::IsListStretchingEnabled()
			&& StretchingEnabled())
		{
		SetTextSubCellL(aSubCell, aStretchedLayout.LayoutLine());
		}
	else
		{
		SetTextSubCellL(aSubCell, aNormalLayout.LayoutLine());
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::SetStretchableConditionalSubCellL
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::SetStretchableConditionalSubCellL(
		TInt aSubCell, const TAknTextComponentLayout& aNormalLayout,
		const TAknTextComponentLayout& aStretchedLayout, TInt aNormalSubCell,
		TInt aStretchedSubCell)
	{
	if (Layout_Meta_Data::IsLandscapeOrientation()
			&& Layout_Meta_Data::IsListStretchingEnabled()
			&& StretchingEnabled())
		{
		SetConditionalSubCellL(aSubCell, aStretchedLayout.LayoutLine(),
				aStretchedSubCell);
		}
	else
		{
		SetConditionalSubCellL(aSubCell, aNormalLayout.LayoutLine(),
				aNormalSubCell);
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::ResetSLSubCellArray
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::ResetSLSubCellArray()
	{
	if (!iExtension)
		{
		return;
		}

	iExtension->iMarginRect = TRect::EUninitialized;

	UseScalableLayoutData(EFalse);
	iExtension->ResetSLSubCellArray();
		// This function gets called always when size changes, so here is called
		// SetupSkinContextL to update the layout rect of the background skin 
		// context (mainpane rect can change for example if toolbar is hidden.
		TRAP_IGNORE( SetupSkinContextL() );
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::StretchingEnabled
// -----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherCustomListBoxData::StretchingEnabled() const
	{
#ifdef RD_LIST_STRETCH    
	if (!iExtension)
		{
		return EFalse;
		}

	return iExtension->iStretchingEnabled;
#else
	return EFalse;
#endif // #ifdef RD_LIST_STRETCH        
	}

#ifdef RD_LIST_STRETCH    
// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::EnableStretching
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::EnableStretching(const TBool aEnabled)
	{
	if (!iExtension)
		{
		return;
		}

	iExtension->iStretchingEnabled = aEnabled;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::HideSecondRow
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::HideSecondRow(const TBool aHide)
	{
	if (!iExtension)
		{
		return;
		}

	iExtension->iHideSecondRow = aHide;
	}
#endif // RD_LIST_STRETCH
// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::SecondRowHidden
// -----------------------------------------------------------------------------
//
TBool CCCAppCommLauncherCustomListBoxData::SecondRowHidden() const
	{
	if (!iExtension)
		{
		return EFalse;
		}

	return iExtension->iHideSecondRow;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::ToggleDropShadows
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::ToggleDropShadows(const TBool aEnable)
	{
	if (!iExtension)
		{
		return;
		}
	iExtension->iUseDropShadows = aEnable;
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::CheckIfSubCellsIntersect
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::CheckIfSubCellsIntersect(
		TAknTextLineLayout* aLayouts, TBool* aResults, const TDesC& aText,
		const TRect& aItemRect) const
	{
	TInt subCell = 0;
	TInt subCell2;
	TInt subCellIndex;
	TInt subCellIndex2;
	TInt lastSubCell = Min(KMaxSubCellIndex, LastSubCell());
	TPtrC text;
	TBool isEmpty[KMaxSubCellIndex];

	// cache the empty text states
	while (ETrue)
		{
		if (subCell > lastSubCell)
			{
			break;
			}

		TextUtils::ColumnText(text, subCell, &aText);

		if (text == KNullDesC && SubCellIsNotAlwaysDrawn(subCell))
			{
			isEmpty[subCell] = ETrue;
			}
		else
			{
			isEmpty[subCell] = EFalse;
			}

		++subCell;
		}

	subCell = 0;

	while (ETrue)
		{
		if (subCell > lastSubCell)
			{
			break;
			}

		if (iExtension->FindSLSubCellIndex(subCellIndex, subCell) != 0)
			{
			break;
			}

		if (isEmpty[subCell])
			{
			++subCell;
			continue;
			}

		TRect bRect(SubCellPosition(subCell), SubCellSize(subCell));

		for (subCell2 = subCell + 1; subCell2 <= lastSubCell; ++subCell2)
			{
			if (isEmpty[subCell2])
				{
				continue;
				}

			if (iExtension->FindSLSubCellIndex(subCellIndex2, subCell2) != 0)
				{
				break;
				}

			TRect bRect2(SubCellPosition(subCell2), SubCellSize(subCell2));

            if ( bRect.Intersects( bRect2 ) && !SubCellIsTransparent( subCell ) && !SubCellIsTransparent( subCell2 ) ) 
                {
                aResults[subCell] = ETrue;

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
            
        if ( aResults[subCell] )
            {
            if ( iExtension->AtSL( subCellIndex ).iSubCellType == CCCAppCommLauncherCustomListBoxDataExtension::EAknSLText )
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
                
                aLayouts[subCell] = textLine;
                }
            }

		++subCell;
		}
	}

// -----------------------------------------------------------------------------
// CCCAppCommLauncherCustomListBoxData::SubCellsMightIntersect
// -----------------------------------------------------------------------------
//
void CCCAppCommLauncherCustomListBoxData::SubCellsMightIntersect(
		const TBool aMightIntersect)
	{
	iExtension->iSubCellsMightIntersect = aMightIntersect;
	}

void CCCAppCommLauncherCustomListBoxDataExtension::CreateColorBitmapsL()
	{
	if (iColorBmp)
		{
		CreateColorBitmapsL(iColorBmp->SizeInPixels());
		}
	}

void CCCAppCommLauncherCustomListBoxDataExtension::CreateColorBitmapsL(
		TSize aSize)
	{
	TRgb color, hiliColor;
	TInt error;
	// icon #13	main area	additional list icons	#215
	// icon #16	list highlight	additional list iconsform checkbox, radio button	#215

	error = AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
			KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13);
	if (error)
		return;

	error = AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), hiliColor,
			KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG16);
	if (error)
		return;

	if (iColorBmp && iColorBmp->SizeInPixels() == aSize && color == iIconColor
			&& hiliColor == iHiliIconColor)
		{
		return;
		}

	iIconColor = color;
	iHiliIconColor = hiliColor;

	if (!iColorBmp)
		{
		iColorBmp = new (ELeave) CFbsBitmap();
		iColorBmp->Create(aSize,
				CCoeEnv::Static()->ScreenDevice()->DisplayMode());
		}
	else if (iColorBmp->SizeInPixels() != aSize)
		{
		iColorBmp->Resize(aSize);
		}
	if (!iHiliBmp)
		{
		iHiliBmp = new (ELeave) CFbsBitmap();
		iHiliBmp->Create(aSize,
				CCoeEnv::Static()->ScreenDevice()->DisplayMode());
		}
	else if (iHiliBmp->SizeInPixels() != aSize)
		{
		iHiliBmp->Resize(aSize);
		}

	CFbsBitGc* fbsBitGc = CFbsBitGc::NewL();
	CleanupStack::PushL(fbsBitGc);

	CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(iColorBmp);
	CleanupStack::PushL(bmpDevice);
	fbsBitGc->Activate(bmpDevice);
	fbsBitGc->SetPenStyle(CGraphicsContext::ENullPen);
	fbsBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	fbsBitGc->SetBrushColor(color);
	fbsBitGc->Clear();
	CleanupStack::PopAndDestroy(bmpDevice);
	bmpDevice = NULL;

	bmpDevice = CFbsBitmapDevice::NewL(iHiliBmp);
	CleanupStack::PushL(bmpDevice);
	fbsBitGc->Activate(bmpDevice);
	fbsBitGc->SetPenStyle(CGraphicsContext::ENullPen);
	fbsBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	fbsBitGc->SetBrushColor(hiliColor);
	fbsBitGc->Clear();
	CleanupStack::PopAndDestroy(bmpDevice);
	bmpDevice = NULL;

	CleanupStack::PopAndDestroy(fbsBitGc);
	}

void CCCAppCommLauncherCustomListBoxData::DrawFormattedOld(
		TListItemProperties& aProperties, CWindowGc& aGc, const TDesC* aText,
		const TRect& aItemRect, TBool aHighlight, const TColors& aColors) const
	{
	TRect aRect(aItemRect);
	const TColors *subcellColors = &aColors;

	TInt lastSubCell = LastSubCell();
	if (lastSubCell == KErrNotFound)
		{
		aGc.UseFont(CEikonEnv::Static()->NormalFont());
		aGc.DrawText(TPtrC(), aRect, 0); // use draw text so that don't need to change pen color/style
		aGc.DiscardFont(); // Release the font cache
		return;
		}
	const CFont* font = SubCellFont(0);
	if (font == NULL)
		{
		font = CEikonEnv::Static()->NormalFont();
		}

	TInt extraVerticalSpace = (aRect.Height() - font->HeightInPixels());
	TInt baseLineOffset = extraVerticalSpace / 2 + font->AscentInPixels();
	TRect textRect = aRect;
	textRect.iBr.iX = aRect.iTl.iX;
	TInt subcell = 0;
	TInt subcell2 = 0;
	TPtrC text;
	TBool textNull[30];
	TRgb bmpBackColor, bmpForeColor;
	TRect textShadowRect; // For transparent list
	TRgb textShadowColour = AKN_LAF_COLOR_STATIC(215); // Black shadow for item text.
	MAknsControlContext *cc = AknsDrawUtils::ControlContext(Control());
	TBool layoutMirrored = AknLayoutUtils::LayoutMirrored();
	TBool skinEnabled = AknsUtils::AvkonSkinEnabled();

	if (!cc)
		{
		cc = SkinBackgroundContext();
		}


	Mem::FillZ(textNull, sizeof(textNull));

	// cache the text states.
	subcell = 0;
	for (;;)
		{
		if (subcell > lastSubCell)
			{
			break;
			}

		TextUtils::ColumnText(text, subcell, aText);
		if (text == KNullDesC && SubCellIsNotAlwaysDrawn(subcell))
			{
			textNull[subcell] = ETrue;
			}

		subcell++;
		}

	// this loop handles rectangles that are clipped because of other subcells.
	subcell = 0;

	for (;;)
		{
		if (subcell > lastSubCell)
			{
			break;
			}

		if (textNull[subcell])
			{
			++subcell;
			continue;
			}

		TRect bRect(SubCellPosition(subcell), SubCellSize(subcell));
		TMargins m(SubCellMargins(subcell));
        TRect cRect(bRect.iTl+TSize(m.iLeft,m.iTop),bRect.Size()-TSize(m.iRight+m.iLeft,m.iBottom+m.iTop));

		for (subcell2 = subcell + 1; subcell2 <= lastSubCell; subcell2++)
			{
			if (textNull[subcell2])
				{
				continue;
				}

			// This is called O(N^2) times - Do not put anything extra to it, it'll slow down drawing!
            TRect bRect2 = TRect(SubCellPosition(subcell2),SubCellSize(subcell2));
            if (cRect.Intersects(bRect2) && bRect.Intersects(bRect2) && !SubCellIsTransparent(subcell) && !SubCellIsTransparent(subcell2)) 
				{
				if (!layoutMirrored)
					{
					cRect.iBr.iX = bRect2.iTl.iX;
					bRect.iBr.iX = bRect2.iTl.iX;
					}
				else
					{
					cRect.iTl.iX = bRect2.iBr.iX;
					bRect.iTl.iX = bRect2.iBr.iX;
					}
				}
			}
		SetSubCellRealTextSize(subcell, cRect.Size());
		SetSubCellRealSize(subcell, bRect.Size());
		subcell++;
		}

	// This loop does the drawing.    
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	subcell = 0;
	for (;;)
		{
		if (subcell > lastSubCell)
			{
			break;
			}

		if (textNull[subcell])
			{
			++subcell;
			continue;
			}

		// SetPosition, SetSize and margins support
		TRect bRect(SubCellPosition(subcell), SubCellRealSize(subcell));
		TMargins m(SubCellMargins(subcell));
        TRect cRect(bRect.iTl+TSize(m.iLeft,m.iTop),SubCellRealTextSize(subcell));
        

		if (bRect.iBr.iX == 0)
			{
			++subcell;
			continue;
			}

		if (layoutMirrored)
			{
            TRect bRect = TRect(SubCellPosition(subcell),SubCellSize(subcell));
            TRect cRect2 = TRect(bRect.iTl+TSize(m.iLeft,m.iTop),bRect.Size()-TSize(m.iRight+m.iLeft,m.iBottom+m.iTop));
            

			TInt shift = (cRect2.Size() - SubCellRealTextSize(subcell)).iWidth;
			cRect.iTl.iX += shift;
			cRect.iBr.iX += shift;
			}

		textRect.SetRect(aItemRect.iTl + cRect.iTl, cRect.Size());

		if (UseSubCellColors(subcell))
			{
			subcellColors = &SubCellColors(subcell);
			}
		else
			{
			subcellColors = &aColors;
			}

		if (aHighlight)
			{
			aGc.SetPenColor(subcellColors->iHighlightedText);
			aGc.SetBrushColor(subcellColors->iHighlightedBack);
			bmpBackColor = subcellColors->iHighlightedBack;
			bmpForeColor = subcellColors->iHighlightedText;
			if (skinEnabled && iExtension)
				{
				if (iExtension->iHighlightedTextColor != NULL)
					{
					aGc.SetPenColor(iExtension->iHighlightedTextColor);
					bmpForeColor = iExtension->iHighlightedTextColor;
					}
				}
			}
		else
			{
			aGc.SetPenColor(subcellColors->iText);
			aGc.SetBrushColor(subcellColors->iBack);
			bmpBackColor = subcellColors->iBack;
			bmpForeColor = subcellColors->iText;

			if (skinEnabled && iExtension)
				{
				if (iExtension->iTextColor != NULL)
					{
					aGc.SetPenColor(iExtension->iTextColor);
					bmpForeColor = iExtension->iTextColor;
					}
				}
			}

		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

		// The following draws subcells to textRect
		if (textRect.iBr.iX != textRect.iTl.iX)
			{
			TextUtils::ColumnText(text, subcell, aText);

			// graphics or text column
			CGraphicsContext::TTextAlign align = SubCellAlignment(subcell);
			if (!SubCellIsGraphics(subcell))
				{
                const CFont* rowAndCellFont=RowAndSubCellFont(iExtension->iCurrentlyDrawnItemIndex,subcell);
                const CFont* cellFont=Font(aProperties, subcell);
                const CFont* tempFont=(cellFont) ? cellFont : font;
                const CFont* usedFont=(rowAndCellFont) ? rowAndCellFont : tempFont;
				aGc.UseFont(usedFont);
				SetUnderlineStyle(aProperties, aGc, subcell);

				// baseline calc needed for each cell.
				baseLineOffset = SubCellBaselinePos(subcell);
				baseLineOffset -= cRect.iTl.iY;
				if (!baseLineOffset)
					{
                    baseLineOffset = (cRect.Size().iHeight-usedFont->HeightInPixels())/2 + usedFont->AscentInPixels();
					}

				TBuf<KMaxColumnDataLength + KAknBidiExtraSpacePerLine> clipbuf =
						text.Left(KMaxColumnDataLength);

				// Note that this potentially modifies the text so its lenght in pixels
				// might increase. Therefore, this should always be done before
				// wrapping/clipping text. In some cases, WordWrapListItem is called
				// before coming here. Is it certain that it is not done for number subcells?
				if (SubCellIsNumberCell(subcell))
					{
					AknTextUtils::LanguageSpecificNumberConversion(clipbuf);
					}

				TBool clipped(EFalse);
				TInt clipgap = SubCellTextClipGap(subcell);

                if ( iExtension->iUseLogicalToVisualConversion &&
                     subcell != iExtension->iFirstWordWrappedSubcellIndex &&
                     subcell != iExtension->iSecondWordWrappedSubcellIndex )
                    {
                    TInt maxClipWidth = textRect.Size().iWidth + clipgap;
                    
                    clipped = AknBidiTextUtils::ConvertToVisualAndClip(
                        text.Left(KMaxColumnDataLength), 
                        clipbuf,
                        *usedFont,
                        textRect.Size().iWidth, 
                        maxClipWidth );
					}

				if (clipped)
					{
					if (!layoutMirrored)
						{
						textRect.iBr.iX += clipgap;
						}
					else
						{
						textRect.iTl.iX -= clipgap;
						}
					}

				if (aHighlight)
					{
					if (clipped)
						{
						iExtension->iClippedSubcells |= (1 << subcell);
						}
					else
						{
						iExtension->iClippedSubcells &= ~(1 << subcell);
						}

					if (iExtension->iUseClippedByWrap) // override if already clipped
						{
                        iExtension->iClippedSubcells = iExtension->iClippedByWrap;
						}
					}

                CAknMarqueeControl* marquee =
                    subcell == 1 ? iExtension->iMarquee :
                                   iExtension->i2ndLineMarquee;
                
                TBool marqueeDisabled =
                        static_cast<CEikListBox*>(
                            Control() )->View()->ItemDrawer()->Flags() & CListItemDrawer::EDisableMarquee;
                
                if ( aHighlight && iExtension->IsMarqueeOn() && clipped && !marqueeDisabled )
                    {                    
                    // Let marquee know if it needs to do bidi conversion.
                    marquee->UseLogicalToVisualConversion( clipped );
                    
                    if ( marquee->DrawText( aGc, textRect, text, baseLineOffset, align, *usedFont ) )
                        {
                        // All the loops have been executed -> the text needs to be truncated.
                        aGc.DrawText( clipbuf, textRect, baseLineOffset, align, 0 );
						}
					}
				else
					{
					if (marquee && marqueeDisabled)
						{
						marquee->Stop();
						}

					if (IsBackgroundDrawingEnabled())
						{
                        aGc.DrawText( clipbuf, textRect, baseLineOffset, align, 0 );
                        }
                    else  // "Transparent" list, draw text shadow first, then the actual text.
                        {
                        textShadowRect = textRect;
                        textShadowRect.Move( 1, 1 );
                        aGc.SetPenColor( textShadowColour );
                        aGc.DrawText( clipbuf, textShadowRect, baseLineOffset, align, 0 );
                        if ( aHighlight )
                            {
                            aGc.SetPenColor( subcellColors->iHighlightedText );
                            }
                        else
                            {
                            aGc.SetPenColor( subcellColors->iText );
                            }
                        aGc.DrawText( clipbuf, textRect, baseLineOffset, align, 0 );
						}
					}

				if (iExtension->iPictoInterface)
					{
					iExtension->iPictoInterface->Interface()->DrawPictographsInText(
                        aGc, *usedFont, clipbuf, textRect, baseLineOffset, align, 0 );
					}
				// disable underline after first text.
				// at least phonebook uses this. See SetSubcellUnderlined to override
				aGc.SetUnderlineStyle(EUnderlineOff);
				aGc.DiscardFont(); // Release the font cache
				}
			else // Graphics subcell
				{
				TLex lex(text);
				TInt index;
				//__ASSERT_ALWAYS(lex.Val(index)==KErrNone,Panic(EAknPanicFormattedCellListInvalidBitmapIndex));
				//__ASSERT_DEBUG(iIconArray, Panic(EAknPanicOutOfRange));
				if (index == KColorIconFlag)
					index = KColorIconIdx;
				//__ASSERT_DEBUG((index>=0 && index<iIconArray->Count()),Panic(EAknPanicFormattedCellListInvalidBitmapIndex));
				TRect bmpRect;
				bmpRect.iTl = textRect.iTl;

				if (index >= 0 && iIconArray)
					{
					CGulIcon* icon = (*iIconArray)[index];
					CFbsBitmap* bitmap = icon->Bitmap();

					if (iExtension)
						{
						TInt w, h;
						TSize sz = bitmap->SizeInPixels();

						w = iExtension->iSubCellIconSize[subcell].iWidth;
						h = iExtension->iSubCellIconSize[subcell].iHeight;

						if (h != 0 && w != 0 && !(sz.iWidth == w || sz.iHeight
								== h))
							{
							AknIconUtils::SetSize(bitmap,
									iExtension->iSubCellIconSize[subcell],
									EAspectRatioPreservedAndUnusedSpaceRemoved);
							}
						else if (sz.iWidth == 0 || sz.iHeight == 0) // check if size is set at all
							{
							// just in case fallback
							AknIconUtils::SetSize(bitmap, textRect.Size(),
									EAspectRatioPreservedAndUnusedSpaceRemoved);
							}
						}

					TSize size = bitmap->SizeInPixels();

					if (size.iWidth > textRect.Size().iWidth)
						{
						size.iWidth = textRect.Size().iWidth;
						}

					if (size.iHeight > textRect.Size().iHeight)
						{
						size.iHeight = textRect.Size().iHeight;
						}

					TPoint bmpPos = textRect.iTl;
					bmpPos.iY += (textRect.Height() - size.iHeight) >> 1;
					switch (align)
						{
						case ELeft:
							break;
						case ECenter:
							bmpPos.iX += (textRect.Width() - size.iWidth) >> 1;
							break;
						case ERight:
							bmpPos.iX = textRect.iBr.iX - size.iWidth;
							break;
						}
					bmpRect = TRect(bmpPos, size);
                    TPoint posInBitmap = TPoint(0,0) + bitmap->SizeInPixels() - textRect.Size();
                    posInBitmap.iX /= 2;
                    posInBitmap.iY /= 2;
					if (posInBitmap.iX < 0)
						{
						posInBitmap.iX = 0;
						}
					if (posInBitmap.iY < 0)
						{
						posInBitmap.iY = 0;
						}
					TRect rect(posInBitmap, textRect.Size());

					if (icon->Mask())
						{
						aGc.BitBltMasked(bmpPos, bitmap, rect, icon->Mask(),
								ETrue);
						}
					else
						{
						aGc.BitBlt(bmpPos, bitmap, rect);
						}
					}
				aGc.SetPenStyle(CGraphicsContext::ESolidPen);
				}
			}
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		++subcell;
		}

	// Clear information of wordwrapped and bidi-reordered subcells.
	const_cast<CCCAppCommLauncherCustomListBoxData*> (this)-> SetWordWrappedSubcellIndices(
			-1, -1);
	}

void CCCAppCommLauncherCustomListBoxData::SetClippedByWrap(
		TUint32 aClippedCells, TBool aUseClippedByWrap)
	{
	if (iExtension)
		{
		iExtension->iClippedByWrap = aClippedCells;
		iExtension->iUseClippedByWrap = aUseClippedByWrap;
		}
	}

 TUint32 CCCAppCommLauncherCustomListBoxData::CurrentItemTextWasClipped() const
	{
	return iExtension ? iExtension->iClippedSubcells : 0;
	}

TBool CCCAppCommLauncherCustomListBoxData::KineticScrollingEnabled() const
	{
	if (iExtension)
		{
		return iExtension->iKineticScrolling;
		}
	return EFalse;
	}

CEikListBox* CCCAppCommLauncherCustomListBoxData::ListBox() const
	{
	if (iExtension && iExtension->iControl)
		{
		return static_cast<CEikListBox*> (iExtension->iControl);
		}
	return NULL;
	}

// End of File

