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
* Description:  Class for handling the view plugin data
*
*/


#ifndef __CCAPPLUGINDATA_H__
#define __CCAPPLUGINDATA_H__

#include <e32base.h>

class CCCAppViewPluginBase;
class CCCAppPluginLoader;

/**
 *  Class for handling data related to one plugin
 *
 *  Todo; When CCApp was supporting UI Accelerator, this
 *  class had more functionality. Now you can think is it
 *  really still needed. 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */
class CCCAppPluginData : public CBase
{
public:

    /**
     * Two phased constructor.
     */
    static CCCAppPluginData* NewLC( 
        CCCAppViewPluginBase& aPlugin );

    /**
     * Destructor.
     */
    ~CCCAppPluginData();

public:

    /**
     * Returns pointer to actual plugin.
     *
     * @since S60 v5.0
     * @return pointer to plugin
     */
    CCCAppViewPluginBase& Plugin();

    /**
     * This sets the flag indicating the preparation of plugin is done.
     *
     * @since S60 v5.0
     */   
    void PluginIsPrepared();

    /**
     * Boolean indicating is the Plugin already prepared.
     *
     * @since S60 v5.0
     * @return Boolean indicating is the Plugin already prepared.
     */     
    TBool IsPluginPrepared();

    /**
     * Enum for controlling plugin visiblity.
     *
     * @since S60 v5.0
     */    
    enum TPluginVisibility
        {
        EPluginVisibilityUnDefined = KErrNotFound,
        EPluginHidden,
        EPluginVisible
        };    
    
    /**
     * Getter for plugin visiblity.
     *
     * @since S60 v5.0
     * @return plugin visibility, see TPluginVisibility.
     */     
    TInt PluginVisibility();

    /**
     * Setter for plugin visiblity.
     *
     * @since S60 v5.0
     * @param aPluginVisibility, see enum TPluginVisibility.
     */    
    void SetPluginVisibility( TInt aPluginVisibility );    
    
private:
    
    CCCAppPluginData( 
        CCCAppViewPluginBase& aPlugin );
    void ConstructL();    

public:     
    /**
     * Cache for plugin bitmaps.
     */    
    CFbsBitmap* iPluginBitmap;
    CFbsBitmap* iPluginBitmapMask;
    
private:

    /**
     * Pointer to actual plugin
     * Own.
     */
    CCCAppViewPluginBase& iPlugin;
    /**
     * To indicate if plugin is already prepared.
     * Own.
     */
    TBool iPluginIsPrepared;

    /**
     * Plugin visiblity, see enum TPluginVisibility.
     */    
    TInt iPluginVisibility;
};

#endif // __CCAPPLUGINDATA_H__
//End of file
