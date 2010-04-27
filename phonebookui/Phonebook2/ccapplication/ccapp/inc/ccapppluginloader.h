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
* Description:  Class for loading and handling the view plugins
*
*/


#ifndef __CCAPPLUGINLOADER_H__
#define __CCAPPLUGINLOADER_H__

#include <e32base.h>
#include <RPbk2LocalizedResourceFile.h>

class CCCAppView;
class CCCAppPluginData;
class CCCAppViewPluginBase;
class CImplementationInformation;
class CEikonEnv;
class MCCAppEngine;
class TCCAPluginsOrderInfo;
class CcaPluginFactoryOwner;
class TVPbkContactStoreUriPtr;
class CPbk2ApplicationServices;
/**
 *  Class for loading and handling the plugins
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */
class CCCAppPluginLoader : public CBase
    {
#ifdef __CCAPPUNITTESTMODE
    friend class T_CCCAppPluginLoader;
#endif// __CCAPPUNITTESTMODE       

public:

    /**
     * Two phase contstructor
     */
    static CCCAppPluginLoader* NewL(
        MCCAppEngine* aAppEngine, 
        const TDesC8& aPluginProperties, 
        const TDesC8& aTypeFilter = KNullDesC8 );

    /**
     * Destructor.
     */
    ~CCCAppPluginLoader();

private:
    
    CCCAppPluginLoader(MCCAppEngine* aAppEngine);
    void ConstructL(const TDesC8& aPluginProperties, const TDesC8& aTypeFilter);

public:
    
    /**
     * Returns the next plug-in data relative to the one in focus
     * and changes the focus
     *
     * @since S60 v5.0
     * @param aOnlyVisiblePlugins EFalse: includes also hidden plugins 
     * @return A plug-in data if it exists, NULL otherwise.
     */
    CCCAppPluginData* NextPlugin( TBool aOnlyVisiblePlugins );
    
    /**
     * Returns the previous plug-in data relative to the one in focus
     * and changes the focus
     *
     * @since S60 v5.0
     * @param aOnlyVisiblePlugins EFalse: includes also hidden plugins
     * @return A plug-in data if it exists, NULL otherwise.
     */
    CCCAppPluginData* PreviousPlugin( TBool aOnlyVisiblePlugins );

    /**
     * Tries to set certain plugin in focus.
     *
     * @since S60 v5.0
     * @param aUid is the plugin wanted to be in focus
     * @return index of succesfully set plugin, otherwise KErrNotFound.
     */
    TInt SetPluginInFocus( const TUid aUid );   
    
    /**
     * Returns the current plug-in data in focus
     *
     * @since S60 v5.0
     * @return A plug-in data, NULL if first
     */
    CCCAppPluginData* PluginInFocus();
    
    /**
     * Returns the number of all available plug-ins
     *
     * @since S60 v5.0
     * @return Number loaded plug-ins.
     */
    TInt PluginsCount() const;

    /**
     * Returns the number of visible plug-ins
     *
     * @since S60 v5.0
     * @return Number visible plug-ins.
     */    
    TInt VisiblePluginCount() const; 
    
    /**
     * Returns the plug-in data at certain index.
     *
     * @since S60 v5.0
     * @param aIndex is the plugin wanted
     * @param aChangefocus if the focus is needed to change also
     * @return A plug-in data if it exists, NULL otherwise.
     */
    CCCAppPluginData* PluginAt( TInt aIndex, TBool aChangefocus = EFalse );

    /**
     * Returns the plug-in data at certain index within visible plugins.
     *
     * @since S60 v5.0
     * @param aIndex Index of the plugin within visible plugins.
     * @param aChangefocus if the focus is needed to change also
     * @return A plug-in data if it exists, NULL otherwise.
     */
    CCCAppPluginData* VisiblePlugin( TInt aIndex, TBool aChangefocus = EFalse );    
    
    /**
     * Removes plugin from plugin loader. 
     * Needed for error cases. 
     *
     * @since S60 v5.0
     * @param index of plugin in loader.
     */
    void RemovePlugin( TUid aPlugin );

    /**
     * Returns plugin.
     *
     * @since S60 v5.0
     * @param aPlugin Uid of plugin
     * @return A plug-in data if it exists, NULL otherwise.
     */
    CCCAppViewPluginBase* Plugin( TUid aPlugin );    

    /**
     * Getter for plugin visiblity.
     *
     * @since S60 v5.0
     * @return plugin visibility, see enum CCCAppPluginData::TPluginVisibility.
     */     
    TInt PluginVisibility(TUid aPlugin, TInt& aTabNbr);    
    
    /**
     * Setter for plugin visiblity.
     *
     * @since S60 v5.0
     * @param aPluginVisibility, see enum CCCAppPluginData::TPluginVisibility.
     */    
    TBool SetPluginVisibility(
        TUid aPlugin,
        TInt aVisibility);
    
private:

    /**
     * Loads multiple views using factory functionality from a factory. Through this interface one 
     * plugin dll can instantiate multiple views.  
     *
     * @since S60 v5.0
     */	
    void LoadOneMultiViewPluginL(CImplementationInformation* aImplementationInformation);    
    
    /**
     * Goes through the iPluginInfoArray, loads all the plugins and 
     * appends them to array.
     *
     * @since S60 v5.0
     */
    void LoadAllPlugins( const RArray<TCCAPluginsOrderInfo>& aPluginOrderInfoArray, const TDesC8& aPluginProperties );
    
    /**
     * Loads plugin and appends it to array.
     *
     * @since S60 v5.0
     * @param aImplementationUid Implementation UID of the plug-in to be loaded.
     */
    void LoadPluginL( TUid aImplementationUid );
    
    /** 
     * Extract sXP plug-in names from name string and put them to name array.
     * 
     * @param aPbksXPExtesionNamesArray, names are stored.
     * @param aNameString, name string contains all sXP plug-in names, each plugin
     *        name is seperated be pipeline "|", e.g Google|MSN|Ovi|
     */
    void GetPbksXPExtesionNamesL( RArray<TPtrC>& aPbksXPExtesionNamesArray,
            const TDesC& aNameString);
    
    /**
    * copy pluginInfo address from pluginInforArrays to RArray<TCCAPluginsOrderInfo> array.
    * plus opaques values. make RArray<TCCAPluginsOrderInfo> array 3 group: ECCAInHousePlugins,
    * ECCAPlugindInBothCCAAndNameList and ECCAPluginsOnlyInCCA, store group info.
    * 
    * @param aOrderInfoArray, store address copy pluginInfo object from iPluginInforArray,
    *        plus order and groupInfo.
    * @param aNameListPluginNameArray, phonebook mainview sXPExtension plugins name list
    * @param aOldPluginInfoArray	
    * @param aNewPluginInfoArray 
    */
    void GetPluginsInfoL( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
            const RArray<TPtrC>& aNameListPluginNameArray,
            RPointerArray<CImplementationInformation>& aOldPluginInfoArray,
            RPointerArray<CImplementationInformation>& aNewPluginInfoArray);

    /**
    * copy pluginInfo address from pluginInforArray to RArray<TCCAPluginsOrderInfo> array.
    * plus opaques values. make RArray<TCCAPluginsOrderInfo> array 3 group: ECCAInHousePlugins,
    * ECCAPlugindInBothCCAAndNameList and ECCAPluginsOnlyInCCA, store group info.
    * 
    * @param aOrderInfoArray, store address copy pluginInfo object from iPluginInforArray,
    *        plus order and groupInfo.
    * @param aNameListPluginNameArray, phonebook mainview sXPExtension plugins name list
    * @param aPluginInfoArray    
    * @param aIsOldInterFaceType ETrue if old interface type plugins are processed  
    */
    void AppendOrderInfoL( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
            const RArray<TPtrC>& aNameListPluginNameArray,
            RPointerArray<CImplementationInformation>& aPluginInfoArray,
            TBool aIsOldInterFaceType);    
    
    /**
     * Find smallest opaque value from plugins only in cca.
     * 
     * @param aOrderInfoArray array contains TCCAPluginsOrderInfo
     * @return smallest opaque value
     */
    TInt SmallestOpaqueFromPluginsOnlyInCCA( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray );
    
    /**
     * Find largest opaque value from in-house plugins.
     * 
     * @param aOrderInfoArray array contains TCCAPluginsOrderInfo
     * @return largest opaque value
     */
    TInt LargestOpaqueFromInHousePlugins( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray );

    /**
     * Update orders for plugins only in CCA view
     * 
     * @param aStartPosition used for order updating
     * @param aOrderInfoArray array of TCCAPluginsOrderInfo
     */
    void UpdateOrdersForPluginsOnlyInCCA(const TInt aStartPosition,
            RArray<TCCAPluginsOrderInfo>& aOrderInfoArray);
    
    /**
      * Update orders for plugins in both CCA and name list
      * 
      * @param aLastPosition used for order updating, itself is also updated
      * @param aOrderInfoArray array of TCCAPluginsOrderInfo
      * @param aNameListPluginNameArray array contains plugins names in Name list view
      */
    void UpdateOrdersForPluginsInBothCCAAndNameList( TInt& aLastPosition,
            RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
            const RArray<TPtrC>& aNameListPluginNameArray);
    /**
     * changed iOrder informatin of each element in orderinfoarray.
     * then Sort sXP plug-ins orderinforarray by iOrder. 
     *       
     * @param aOrderInfoArray order information will be updated, the array will be sorted
     *        then by order.
     *        
     * @param aNameListPluginNameArray contains sXP plug-in names for Name List.
     *        Used for find element in aOrderInfoArray.
     */
    void SortPluginsOrderInfoL( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
            const RArray<TPtrC>& aNameListPluginNameArray);
   
    /**
     * Find phonebook main view sXPExtension Plugin name from name array 
     *
     * @param aCCAPluginInfo, a CImplementationInformation type object.
     * @param aNameListPluginNameArray, phonebook main view sXPExtension Plugins name array
     * 
     * @return found name's position in aNameListPluginNameArray, KErrNotFound if not found.
     */
    TInt FindPluginNameFromNameListByCCAPluginInfo(const CImplementationInformation& aCCAPluginInfo,
            const RArray<TPtrC>& aNameListPluginNameArray);
    
    /**
     * Get Opaque integer value from a refrerence of CImplementationInformation
     * 
     * @param aInfo, object of CImplementationInformation
     * @return opaque integer value.
     */
    TInt OpaqueValueFrom( const CImplementationInformation& aInfo );
    
    /**
     * Loads a plugin.
     * 
     * @param aPluginsOrderInfo ordering info
     * @param aPluginProperties properties to fulfill  
     * @return opaque integer value.
     */
    void CheckAndLoadPluginL(
            const TCCAPluginsOrderInfo& aPluginsOrderInfo,            
            const TDesC8& aPluginProperties );
    
    /**
     * Returns plugin order.
     * 
     * @param aImplementationInformation ecom implementation information
     * @return order
     */    
    TInt GetOrderValueL( const CImplementationInformation& aInfo );
    
    /**
     * Compares properties of two propertystrings.
     * 
     * @param aData properties to compare
     * @param aRefData properties to compare
     * @param aMatchValuesToo ETrue if also property values need to becompared 
     * @return matching properties.
     */    
    HBufC8* GetmachingPropertiesLC(
            const TDesC8& aData, const TDesC8& aRefData, TBool aMatchValuesToo);
    
    /**
     * Extracts name value pair data from property string. Extracted data
     * is removed from property string
     * 
     * @param aName name extracted
     * @param aValue value extracted
     * @param aDataPtr property string  
     */    
    void GetNameValue( TPtrC8& aName, TPtrC8& aValue, TPtrC8& aDataPtr );

    /**
     * Determine whether a Contact Store URI is SIM Card storage.
     *
     * @param aUri, the Contact Store URI to compare.
     */
    TBool IsSimStoreUri( const TVPbkContactStoreUriPtr& aUri ) const;

    /**
     * Determine whether an implementation plug-in is supported by Current Contact Store.
     *
     * @param aImplmentationUid, the plug-in's ImplmentationUid.
     */
    TBool IsCurrentContactStoreSupportL( const TInt& aImplmentationUid ) const;
    
    /**
     * Prepare application services
     *
     */
    void PreparePbk2ApplicationServicesL();


private://data


    /**
     * Pointer array of loaded plugin data structures.
     * Own.
     */
    RPointerArray<CCCAppPluginData> iPluginDataArray;
    /**
     * Current plugin in use
     */
    TInt iCurrentPlugin;
    
    /**
     * Reference to the cca application engine
     * not owned
     */
    MCCAppEngine* iAppEngine;
    
    /**
     * Temporary holder for CcaPluginFactoryOwner, 
     * owned unless ownership handed over.
     */    
    CcaPluginFactoryOwner* iFactoryTempPtr;
    
    /// Own: Phonebook2 application services
    CPbk2ApplicationServices* iAppServices;
    
    /// Own: Phonebook2 commands dll resource file
    RPbk2LocalizedResourceFile iCommandsResourceFile;

    /// Own: Phonebook2 UI controls dll resource file
    RPbk2LocalizedResourceFile iUiControlsResourceFile;

    /// Own: Phonebook2 common UI dll resource file
    RPbk2LocalizedResourceFile iCommonUiResourceFile;
	};

#endif // __CCAPPUNITTESTMODE
//End of file
