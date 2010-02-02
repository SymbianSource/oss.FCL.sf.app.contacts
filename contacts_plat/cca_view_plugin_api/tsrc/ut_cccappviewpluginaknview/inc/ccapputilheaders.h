/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCApplication's headers
*
*/


#ifndef __CCAPPUTILHEADERS_H__
#define __CCAPPUTILHEADERS_H__

#include <e32base.h>
#include <bautils.h>
#include <akndef.h>
#include <aknsconstants.h>
#include <vwsdef.h>

// Logging related
#include <flogger.h>
_LIT(KLogDir, "cca");
#define KCCAppUtilLogFile CCA_L("ccaapputil.txt")
#define CCA_L(s) _L(s)
#define CCA_L8(s) _L8(s)
#define CCA_DP CCADebugWriteFormat
inline void CCADebugWriteFormat( TRefByValue<const TDesC> aFile, TRefByValue<const TDesC> aFmt,...) //lint !e960
    {
    TBuf< 256 > buffer;
        {
        VA_LIST list;
        VA_START (list, aFmt); //lint !e960
        TBuf< 300 > formatted;
        formatted.FormatList( aFmt, list );
        buffer.Append( formatted.Left( buffer.MaxLength() - buffer.Length() ) ); 
        }
    RFileLogger::Write( KLogDir, aFile, EFileLoggingModeAppend, buffer );
    RDebug::RawPrint(buffer);
    }
inline void CCADebugWriteFormat( TRefByValue<const TDesC> aFile, TRefByValue<const TDesC8> aFmt,...) //lint !e960
    {
    TBuf8< 256 > buffer;
        {
        VA_LIST list;
        VA_START (list, aFmt); //lint !e960
        TBuf8< 300 > formatted;
        formatted.FormatList( aFmt, list );
        buffer.Append( formatted.Left( buffer.MaxLength() - buffer.Length() ) ); 
        }    
    RFileLogger::Write( KLogDir, aFile, EFileLoggingModeAppend, buffer );
    RDebug::RawPrint(buffer);
    }

class CCCAppViewPluginAknView;
class CCCAppViewPluginAknContainer;

// Dummy classes

//
// Dummy class
//
#include <w32std.h>
#include <coedef.h>
class CCoeAppUi : public CBase
    {
public:
    
    //void AddToViewStackL(const MCoeView& aView,CCoeControl* aControl,TInt aPriority=ECoeStackPriorityDefault,TInt aStackingFlags=ECoeStackFlagStandard);   // deprecated
    void AddToViewStackL(const CCCAppViewPluginAknView& /*aView*/, CCCAppViewPluginAknContainer* /*aControl*/ )// modified
        { iAddToViewStackL_called = ETrue; };

    //void RemoveFromViewStack(const MCoeView& aView,CCoeControl* aControl); // deprecated
    void RemoveFromViewStack(const CCCAppViewPluginAknView& /*aView*/, CCCAppViewPluginAknContainer* /*aControl*/ )
        { iRemoveFromViewStack_called = ETrue; };
    
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
        { iHandleKeyEventL_called = ETrue;
          return EKeyWasConsumed; };
    
    TBool iHandleKeyEventL_called;
    TBool iAddToViewStackL_called;
    TBool iRemoveFromViewStack_called;
    };

//
// Dummy class
//
class CAknViewAppUi : public CCoeAppUi
    {
public:
    void HandleCommandL( TInt /*aCommand*/ )
        { iHandleCommandL_called = ETrue; };
    
    TBool iHandleCommandL_called;
    };

//
// Dummy class
//
class CCCAAppAppUi : public CAknViewAppUi
    {
public:
    void RecoverFromBadPluginL()
        { iRecoverFromBadPluginL_called = ETrue; };
    
    void SetTitleL( const TDesC& )
    		{ iSetTitleL_called = ETrue; };
    		
    TBool iRecoverFromBadPluginL_called;
    TBool iSetTitleL_called;
    };

//
// Dummy class
//
class CCoeEnv : public CBase
    {
public:
    };

//
// Dummy class
//
class CAknView : public CBase
    {
public:
    CAknViewAppUi* AppUi() const
        { return iAppUi; };
    
    TRect ClientRect() const
        { return TRect(); };
    
    CCoeEnv* iCoeEnv;
    CAknViewAppUi* iAppUi;
    };

//
// Dummy class
//
class RConeResourceLoader : public CBase
    {
public:
    RConeResourceLoader(CCoeEnv& /*aEnv*/){};
    
    void Close(){};
    };

//
// Dummy class
//
class CCCAppViewPluginAknContainer : public CBase
    {
public:
    
    //void SetMopParent(MObjectProvider* aParent);
    void SetMopParent(CCCAppViewPluginAknView* /*aParent*/ )
        { iSetMopParent_called = ETrue; };
    
    void BaseConstructL( const TRect& /*aRect*/, CCoeAppUi& /*aAppUi*/ )
        { iBaseConstructL_called = ETrue; };
    
    TBool iSetMopParent_called;
    TBool iBaseConstructL_called;
    };

#define __CCAPPUTILUNITTESTMODE
#include <ccappviewpluginaknview.h>

#endif // __CCAPPUTILHEADERS_H__
