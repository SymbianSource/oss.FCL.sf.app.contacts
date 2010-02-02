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

// Dummy classes

//
// Dummy class
//
class CCoeControl : public CBase
    {
public:    
    CCoeControl()
        {};

    virtual void ActivateL()
        { iActivateL_called = ETrue; };

    virtual void HandleResourceChange( TInt aType )
        { iHandleResourceChange_called_with_param_aType = aType; };

    void SetRect( const TRect& aRect )
        { iSetRect_called = ETrue;
          iSetRect_called_with_param_aRect = aRect; };

    void CreateWindowL()
        { iCreateWindowL_called = ETrue; };
    
    TBool iActivateL_called;
    TInt iHandleResourceChange_called_with_param_aType;
    TBool iSetRect_called;
    TRect iSetRect_called_with_param_aRect;
    TBool iCreateWindowL_called;
    };

//
// Dummy class
//
class AknLayoutUtils : public CBase
    {
public:
    
    enum TAknLayoutMetrics
    {
    EDummy,
    EMainPane
    };
    
    static TBool LayoutMetricsRect(TAknLayoutMetrics /*aParam*/, TRect& /*aRect*/ )
        { return ETrue; };
    };

//
// Dummy class
//
#include <w32std.h>
#include <coedef.h>
class CCoeAppUi : public CBase
    {
public:
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
        { iHandleKeyEventL_called = ETrue;
          return EKeyWasConsumed; };
    
    TBool iHandleKeyEventL_called;
    };

//
// Dummy class
//
class CCCAAppAppUi : public CCoeAppUi
    {
public:
    };

#define __CCAPPUTILUNITTESTMODE
#include <CCAppViewPluginAknContainer.h>

#endif // __CCAPPUTILHEADERS_H__
