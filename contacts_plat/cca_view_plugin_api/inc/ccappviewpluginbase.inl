/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Instation of ecom-plugin
*
*/


#include <mccappengine.h>

// -----------------------------------------------------------------------------
// Constructor for interface
// -----------------------------------------------------------------------------
//
inline CCCAppViewPluginBase* CCCAppViewPluginBase::NewL( TUid aImplementationUid )
    {
    TAny* ptr = REComSession::CreateImplementationL(
        aImplementationUid,
        _FOFF( CCCAppViewPluginBase, iDtor_ID_Key ) 
        // _FOFF specifies offset of iDtor_ID_Key so that
        // ECOM framework can update its value.
        );    
    
    // The type of TAny* ptr should be CCCAppViewPluginBase.
    return REINTERPRET_CAST( CCCAppViewPluginBase*, ptr );
    }

// -----------------------------------------------------------------------------
// Interface's (abstract base class's) destructor
// -----------------------------------------------------------------------------
// 
inline CCCAppViewPluginBase::~CCCAppViewPluginBase()
	{
	if ( !iSharedParentCleaner )
	    {
	    // Inform the ECOM framework that this specific instance of the
	    // interface has been destroyed.
	    REComSession::DestroyedImplementation ( iDtor_ID_Key );
	    }
	else
	    {
	    // Inform the parent cleaner that this specific instance of the
	    // interface has been destroyed. Parent cleaner keeps reference count
	    // and returns it's value. If all deleted, delete iSharedParentCleaner. 
	    if ( iSharedParentCleaner->DeletedChild() == 0 )
	        {
	        delete iSharedParentCleaner;  
	        }
	     }
	}

// -----------------------------------------------------------------------------
// Empty implementation of PreparePluginViewL
// -----------------------------------------------------------------------------
// 
inline void CCCAppViewPluginBase::PreparePluginViewL( 
    MCCAppPluginParameter& /*aPluginParameter*/)
    {};
    
// -----------------------------------------------------------------------------
// Empty implementation of ProvideBitmapL
// -----------------------------------------------------------------------------
// 
inline void CCCAppViewPluginBase::ProvideBitmapL( 
    TCCAppIconType /*aIconType*/, 
    CAknIcon& /*aIcon*/ )
    {};

// -----------------------------------------------------------------------------
// Default implementation for plugin visibility
// -----------------------------------------------------------------------------
// 
inline TBool CCCAppViewPluginBase::CheckVisibilityL(
    MCCAppPluginParameter& /*aPluginParameter*/ )
    {
    return ETrue;
    }    
    
// -----------------------------------------------------------------------------
// Hides this plugin from UI
// -----------------------------------------------------------------------------
// 
inline void CCCAppViewPluginBase::HidePluginL()
    {
    AppEngine()->HidePluginL( Id() );
    }    
    
// -----------------------------------------------------------------------------
// Shows this plugin in UI
// -----------------------------------------------------------------------------
// 
inline void CCCAppViewPluginBase::ShowPluginL()
    {
    AppEngine()->ShowPluginL( Id() );
    }

// -----------------------------------------------------------------------------
// Sets the clear up functionality for this view. Takes shared ownership 
// -----------------------------------------------------------------------------
//    
inline void CCCAppViewPluginBase::TakeSharedOwnerShip( 
    MCcaParentCleaner* aParentCleaner )
    {
    iSharedParentCleaner = aParentCleaner;
    }
