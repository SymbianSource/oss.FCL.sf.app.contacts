/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 icon information.
*
*/


#ifndef CPBK2ICONINFO_H
#define CPBK2ICONINFO_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <TPbk2IconId.h>
#include <AknsItemID.h>

// CLASS DECLARATION
class CFbsBitmap;

/**
 * Phonebook 2 icon information.
 * Responsible for reading the icon information from a PBK2_ICON_INFO
 * resource structure.
 */
class CPbk2IconInfo : public CBase
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aReader           Resource reader pointed
         *                          to a PBK2_ICON_INFO resource.
         * @param aMbmFileNames     Descriptor array for bitmap file names.
         * @return  A new instance of this class.
         */
        static CPbk2IconInfo* NewLC(
                TResourceReader& aReader,
                CDesCArray& aMbmFileNames );

        /**
         * Creates a new instance of this class.
         *
         * @param aPbk2IconId   Phonebook icon id.
         * 
         * @param aBitmap       Bitmap.
         * @param aMask         Mask.
         * @param aSize         Size to use. 
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconInfo* NewLC(
                TPbk2IconId aPbk2IconId,
                const CFbsBitmap* aBitmap,
                const CFbsBitmap* aMask,                 
                const TSize aSize );

        /**
         * Creates a new instance of this class.
         *
         * @param aPbk2IconId   Phonebook icon id.
         * @param aIconId   Icon bitmap id.
         * @param aMaskId   Mask bitmap id.
         * @param aMbmFileName  Mbm file name.
         * @param aSkinId  Skin id.
         * @param aColorId  skin color table id.
         * @param aColorIndex   Index of the icon color in color table.
         * @param aDefaultColorIndex    Index of the default color.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconInfo* NewLC(
                TPbk2IconId aPbk2IconId,
                TInt aIconId,
                TInt aMaskId,
                TPtrC aMbmFileName,
                TAknsItemID aSkinId,
                TAknsItemID aColorId,
                TInt aColorIndex,
                TInt aDefaultColorIndex );

        /**
         * Destructor.
         */
        ~CPbk2IconInfo();

    public: // Interface

        /**
         * Returns the unique Phonebook 2 icon id of this icon.
         *
         * @return  Unique Phonebook2 icon id of this icon.
         */
        const TPbk2IconId& Pbk2IconId() const;

        /**
         * Returns the bitmap file name of this icon.
         *
         * @return  The bitmap file name of this icon.
         */
        const TDesC& MbmFileName() const;

        /**
         * Returns this icon's id in the bitmap file.
         *
         * @return  This icon's id in the bitmap file.
         */
        TInt IconId() const;

        /**
         * Returns this icon's mask id in the bitmap file.
         *
         * @return  This icon's mask id in the bitmap file.
         */
        TInt MaskId() const;

        /**
         * Returns this icon's skin id.
         *
         * @return  This icon's skin id.
         */
        TAknsItemID SkinId() const;

        /**
         * Returns this icon's color table id.
         *
         * @return  This icon's color table id.
         */
        TAknsItemID ColorId() const;

        /**
         * Returns this icon's index in the color table.
         *
         * @return  This icon's index in the color table;
         *          one of the TAknsQsnIconColorsIndex or KErrNotFound.
         */
        TInt ColorIndex() const;

        /**
         * Returns this icon default color index.
         * TRgb value can be retrieved using AKN_LAF_COLOR_STATIC macro.
         *
         * @return  The default color index or KErrNotFound.
         */
        TInt DefaultColorIndex() const;

        /**
         * Returns external bitmap. This is used when icon is provided by an 
         * external provider (e.g a branding package). 
         *
         * @return  Bitmap for external icons. 
         */
        const CFbsBitmap* ExternalBitmap() const;

        /**
         * Returns external bitmap mask. This is used when icon is provided by an 
         * external provider (e.g a branding package). 
         *
         * @return  Bitmap mask for not own icons.
         */
        const CFbsBitmap* ExternalMask() const;

        /**
         * Check if icon is not own.
         *
         * @return  ETrue if icon is not own, EFalse for own icons.
         */
        TBool IsIconExternal() const;

        /**
         * Returns external icon size. This is used when icon is provided by an 
         * external provider (e.g a branding package). 
         *
         * @return  Size not own icons.
         */
        TSize ExternalIconSize() const;        
        
    private: // Implementation
        CPbk2IconInfo();
        CPbk2IconInfo(TPbk2IconId aPbk2IconId, TInt aIconId,
                TInt aMaskId, TPtrC aMbmFileName, TAknsItemID aSkinId,
                TAknsItemID aColorId, TInt aColorIndex,
                TInt aDefaultColorIndex );
        CPbk2IconInfo(TPbk2IconId aPbk2IconId, const CFbsBitmap* aBitmap,
                const CFbsBitmap* aMask, const TSize aSize );

        void ConstructL(
                TResourceReader& aReader,
                CDesCArray& aMbmFileNames );

    private: // Data
        /// Own: Unique Phonebook 2 icon id
        TPbk2IconId iPbk2IconId;
        /// Own: Icon bitmap
        TInt iIconId;
        /// Own: Mask bitmap
        TInt iMaskId;
        /// Own: Mbm file name
        TPtrC iMbmFileName;
        /// Own: Skin id
        TAknsItemID iSkinId;
        /// Own: Color id
        TAknsItemID iColorId;
        /// Own: Color index
        TInt iColorIndex;
        /// Own: Default color index
        TInt iDefaultColorIndex;
        /// Not own: bitmap (for not own icons only)
        const CFbsBitmap* iExternalBitmap;
        /// Not own: bitmap mask (for not own icons only)
        const CFbsBitmap* iExternalMask;
        /// Own: indicates if bitmaps are external
        TBool iExternalBitmaps;
        /// Own: Size for external bitmaps
        TSize iExternalIconSize;
    };

#endif // CPBK2ICONINFO_H

// End of File
