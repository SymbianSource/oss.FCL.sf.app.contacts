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
* Description:  Data container to icons and their unique identifiers.
*/

#ifndef CCUSTOMICONIDMAP_H
#define CCUSTOMICONIDMAP_H

#include <gulicon.h>
#include <ContactCustomIconPluginDefs.h>

/**
 * Container for the icon pointer and its identifier.
 * This is used by CCustomIconIdMap.
 *
 * Note that this class does not control ownership of the
 * icon in any way.
 */
class TCustomIconIdMapItem
    {
    public: // Construction

        /**
         * Constructor.
         */
        TCustomIconIdMapItem();

        /**
         * Constructor.
         *
         * @param aIcon     Icon.
         * @param aId       Icon id.
         */
        TCustomIconIdMapItem(
                CGulIcon* aIcon,
                TCustomIconId aId );

    public: // Data
        /// Ref: Icon
        CGulIcon* iIconPtr;
        /// Ref: Icon id
        TCustomIconId iId;
    };

/**
 * Array of custom icons and their identifiers.
 * The class holds TCustomIconIdMapItem items.
 *
 * For leave covery purposes the class deletes any icons referenced in the
 * items. If an owner of this class wants to take an ownership of an icon,
 * it must also remove the corresponding icon item.
 */
class CCustomIconIdMap : public CArrayFixFlat<TCustomIconIdMapItem>
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aGranularity  Array granularity.
         */
        CCustomIconIdMap( TInt aGranularity );

        /**
         * Destructor.
         */
        ~CCustomIconIdMap();

    public: // Interface

     /**
      * Convenience method for adding an item.
      *
      * Use this method to add all the icons, which potentially replace
      * original contact related icons during their life time.
      * Note that one icon must not be added twice.
      *
      * aIcon may be NULL. This indicates the application that when the
      * plug-in refers to an icon with aId, the application should
      * use its default icon.
      *
      * @param aIcon    The icon instance. Ownership is taken, if the method
      *                 does not leave. The value may be NULL.
      *
      * @param aId      Unique identifier for identifying an icon. Plug-in needs
      *                 to remember the ids since application uses the id to later
      *                 refer to an icon. The id could be for example
      *                - unique id in database, or
      *                - predefined enum value, if the icons are well known
      *                  during customization build time and won't change
      */
     void AppendL(
            CGulIcon* aIcon,
            TCustomIconId aId );
    };

#include <CCustomIconIdMap.inl>

#endif // CCUSTOMICONIDMAP_H

// End of File
