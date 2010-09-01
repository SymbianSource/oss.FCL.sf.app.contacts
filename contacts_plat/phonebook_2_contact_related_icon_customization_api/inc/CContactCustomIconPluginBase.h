/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines an ECOM interface for third parties to customize
*               contact related icons in certain platform applications.
*/

#ifndef CCONTACTICONCUSTOMPLUGINBASE_H
#define CCONTACTICONCUSTOMPLUGINBASE_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <ContactCustomIconPluginDefs.h>

// FORWARD DECLARATIONS
class CGulIcon;
class CCustomIconIdMap;
class CCustomIconDecisionData;
class MCustomIconChangeObserver;

// CONSTANTS

/**
*  Interface UID for ECOM plug-ins, which inherit from this interface.
*  This is used in plug-in registration resource file (interface_uid). Example:
*
*   RESOURCE REGISTRY_INFO cs_ecom_reg_info
*       {
*       dll_uid = <UID_OF_ECOM_DLL>;
*       interfaces =
*           {
*           INTERFACE_INFO
*               {
*               // Make sure to use KContactIconCustomizationInterfaceUidValue
*               interface_uid = 0x2001E2DA;
*
*               implementations =
*                   {
*                   IMPLEMENTATION_INFO
*                       {
*                       implementation_uid = <...>; //  maps to IMPLEMENTATION_PROXY_ENTRY in CPP exporting plugin factory functions
*                       version_no = 1;
*                       display_name = "CMyContactIconCustomizationPlugin";
*                       default_data = "";
*                       opaque_data = "";
*                       }
*                   };
*               }
*           };
*       }
*/
const TInt KContactIconCustomizationInterfaceUidValue = 0x2001E2DA;
const TUid KContactIconCustomizationInterfaceUid =
    { KContactIconCustomizationInterfaceUidValue };


// CLASS DECLARATION

/**
*  Base class for contact related icon customization ECOM plugins.
*
*  This API allows customizing contact related icons of certain
*  applications at runtime.
*
*  The API defines two roles:
*  - Customizable entity is typically an application supporting icon
*    customization. It is a caller of this API.
*  - Customizer is an entity implementing ecom plug-in, a callee.
*
*  As a concrete example, Logs application has 'Dialled calls' view
*  where it by default shows in each listbox row a call specific icon.
*  Plugin can change each of the icons by implementing this API.
*
*  The customizer needs only one plug-in implementation, which affects
*  multiple applications at a same time. This means the API cannot define
*  application specific info.
*
*  Note that customizable entity - typically an application - has to provide
*  a connection to Font and Bitmap server. The plug-in may fail to provide
*  the icon (KErrCouldNotConnect), if the connection does not exist.
*/
class CContactCustomIconPluginBase : public CBase
    {
    public: // API to be implemented by ECOM plug-in

        /**
         * An icon size hint from application to plug-in. Plug-in
         * may use this to load icons of appropriate size.
         * @see IconsL
         */
        enum TIconSizeHint
            {
            EReallySmall,
            ESmall,
            EMedium,
            EMediumLarge,
            ELarge
            };

        /**
         * Calling application uses this method to register itself as an observer
         * for the plug-in state change notifications.
         *
         * Plug-in uses observer interface for notifying calling application
         * about icon related changes.
         *
         * @param aObserver     Observer interface, implemented by calling client
         *                      code. Ownership of the object is not transferred.
         */
        virtual void AddObserverL(
                MCustomIconChangeObserver* aObserver ) = 0;

        /**
         * Calling application uses this method to deregister itself as an observer
         * from the plug-in state change notifications.
         *
         * @param aObserver     Observer interface, implemented by calling client
         *                      code. Ownership of the object is not transferred.
         */
        virtual void RemoveObserver(
                MCustomIconChangeObserver* aObserver ) = 0;

        /**
         * Gets an array of custom icons from the plug-in.
         *
         * Application calls this method typically in its initialization phase
         * to prepare for replacing its default icons. When it is time to show
         * the icons, calling application may call IconChoiceL.
         *
         * ECOM plug-in implementation should fill in the potential icons
         * and give each icon an identifier, which it can use later to refer
         * to a certain icon it wants to use as replacement for application's
         * default icon.
         *
         * @see CCustomIconIdMap.h
         * @param aContext  Context of the function call. For more information
         *                  about context semantics see CCustomIconDecisionData
         *                  documentation.
         * @param aSizeHint Hint for plug-in to load icons of appropriate size.
         * @return  Icon-to-id map.
         * @leave On typical leave level problems. Application should handle
         *        this by ignoring ECOM plug-in functionality and continue
         *        using its default icons.
         */
        virtual CCustomIconIdMap* IconsL(
                TUid aContext,
                TIconSizeHint aSizeHint ) = 0;

        /**
         * Application uses this method to make a choice of the icon to be
         * displayed in certain UI condition.
         *
         * Note that application should have always had called the IconsL
         * function before this gets called.
         *
         * ECOM plug-in implementation should use aHint parameter to make a decision
         * which icon it wants to use.
         *
         * @param aHint     Hint from the application to the plug-in, which tells
         *                  what kind of icon is being replaced. For example, the
         *                  plug-in may want to choose the icon based on the
         *                  phonenumber. For more details see
         *                  CCustomIconDecisionData.h.
         *                  Plug-in must not take ownership of the object's
         *                  members. The instance and its members are guaranteed to
         *                  be valid only during the method call.
         *
         *                  Caller must fill at least one of the members, otherwise
         *                  plug-in can and should leave with KErrArgument.
         *
         * @param aIdResult Icon reference value filled by the ECOM plug-in. The id
         *                  should be one of the ids, which were used to build the
         *                  CCustomIconIdMap object in IconsL function. If the
         *                  calling application cannot find an icon by aIdResult
         *                  from the map, it should use application's default icon.
         *
         * @return  Return value indicating if the decision could be made.
         *          ETrue, if the icon choice decision could be made.
         *          EFalse, if the icon choice could not be made.
         *          This is an indication from plug-in to application that
         *          application should use its default icon.
         *
         * @leave On typical leave level problems like database access failure
         *        or condition where application may not be able to provide the
         *        requested functionality. It is up to application how it
         *        handles leave from the plug-in. Recommendation is that it
         *        ignores plug-in's functionality and uses default icons.
         */
        virtual TBool IconChoiceL(
                const CCustomIconDecisionData& aHint,
                TCustomIconId& aIdResult ) = 0;

        /**
         * Application uses this method when it dynamically needs to replace
         * its default icon. This can typically happen at any time of the
         * application runtime.
         *
         * Application should favor using the IconsL/IconChoiceL pair, if it
         * is likely to use multiple icons and several times. This function is
         * useful if the application needs only couple of icons only couple
         * of times. This is discouraged for use in frequent draw requests.
         *
         * The plug-in implementation should use the aHint to make a decision
         * which icon it wants to use.
         *
         * @param aHint     Hint from application to plug-in, which tells what
         *                  kind of icon is being replaced. For example, the
         *                  plug-in may want to choose the icon based on the
         *                  phonenumber. For more details see
         *                   CCustomIconDecisionData.h
         *                  Plug-in must not take ownership of the object or its
         *                  members. The instance and its members are guaranteed to
         *                  be valid only during the method call.
         *                  Caller must fill at least one of the members, otherwise
         *                  plug-in can and should leave with KErrArgument.
         *
         * @param aSizeHint Hint for plug-in to load icon of appropriate size.
         *
         * @return  Icon instance. NULL, if plug-in wants to indicate it is
         *          not willing to replace caller's default icon.
         */
        virtual CGulIcon* IconL(
                const CCustomIconDecisionData& aHint,
                TIconSizeHint aSizeHint ) = 0;

        /**
         * Application uses this method when it dynamically needs to update
         * previously loaded custom icon. Typically this happens after
         * application has received update notification from plug-in.
         *
         * @param aContext   Context of the function call. While this override
         *                   provides known icon id rather than
         *                   CCustomIconDecisionData, the context is given here
         *                   separately. For more information about context
         *                   semantics see CCustomIconDecisionData
         *                   documentation.
         *
         * @param aIconId   Specifies the icon for the client to provide. This is
         *                   valid when a known custom icon has changed and is
         *                   being replaced.
         *
         * @param aSizeHint Hint for plug-in to load icon of appropriate size.
         *
         * @return  Icon instance. NULL, if plug-in wants to indicate it is
         *          not willing to replace application's default icon.
         */
        virtual CGulIcon* IconL(
                TUid aContext,
                TCustomIconId aIconId,
                TIconSizeHint aSizeHint ) = 0;

        /**
         * Factory function for getting access to extended API functionality.
         * Uid defines the extension interface being used.
         *
         * @param aExtensionUid     Identifier for extension API class.
         * @return  Extension API class, casted to one of M-interfaces defined
         *         by this API (later). Ownership of the object is not transferred.
         */
        virtual TAny* ContactCustomIconExtension( TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    public:

        /**
         * ~CContactIconCustomPluginBase
         *
         * Cleans up resources, specifically notify ECOM server that this
         * instance is being deleted.
         */
        inline virtual ~CContactCustomIconPluginBase()
            {
            REComSession::DestroyedImplementation( iDtorKey );
            }

    public:
        /// Own: Key UID
        TUid iDtorKey;
    };

#endif // CCONTACTICONCUSTOMPLUGINBASE_H

// End of File
