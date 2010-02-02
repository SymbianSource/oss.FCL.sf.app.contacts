/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     This file contains the definition of CSpdiaDialogs class.
*
*/





#ifndef SPEEDDIALPRIVATE_H
#define SPEEDDIALPRIVATE_H

#include <e32base.h>
#include <coecntrl.h>
#include <MPbk2ImageOperationObservers.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactFindObserver.h>
#include <MVPbkContactAttributeManager.h>

#include <MVPbkStoreContact.h>
#include <AknUtils.h>

#include <CPbk2ImageManager.h>
#include <MVPbkContactViewObserver.h>

#include <AiwCommon.h>
#include <AiwContactSelectionDataTypes.h>
#include <AiwContactAssignDataTypes.h>

#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactObserver.h>
#include "CVPbkContactManager.h"
#include "centralrepository.h"

// Forward Declarations
class TSpdiaIndexDataVPbk;
class CSpdiaGridVPbk;
class CAknGrid;
class CAknQueryDialog;
class CSpdiaGridDlgVPbk;
class CVPbkSpeedDialAttribute;
class CVPbkContactLinkArray;
class TPbk2IconId;
class MVPbkContactAttribute;
class CAiwServiceHandler;
class MRefreshObserver;

const TInt KArraySize =9;
//for the timebeing..
#define KErrCancelled 0x99

NONSHARABLE_CLASS( CSpeedDialPrivate ) : public CCoeControl, public MPbk2ImageGetObserver,
										 public MVPbkSingleContactOperationObserver,
										 public MVPbkSetAttributeObserver,
										 public MVPbkContactFindObserver,
										 public MVPbkContactStoreObserver,
										 public MAiwNotifyCallback,
										 public MVPbkContactObserver
										 
{
public:

		enum eSpeedDialState
		{
			STATE_IDLE = 0,
			STATE_INITIAL,
			STATE_CONTACTCHANGE,
			STATE_ASSIGN,
			STATE_REMOVE	
		};
		
		 // The type of the caller application.
	    enum TGridUse
	    {
	    	EGridUse,               // SpeedDial
	    	EGridUseAndUpdate,      // PhoneBook
	    	EGridNoUse              // PhoneClient
	    };
			/**
	    * Creates a new instace of this class.
	    *
	    */
		IMPORT_C static CSpeedDialPrivate* NewL(CVPbkContactManager* aContactManager);
			
        /**
        * The number of voice mail returned, if voice mail is assigned.
        * Note: If the location is empty, required dialog
        * will be displayed and voice mail will be defined.
        *
        * @param aNumber    reference of phone number.
        * @retunr   EFalse if user cancel or failed.
        */
        IMPORT_C TBool VoiceMailL(TDes& aNumber);
        
        /**
        * The number of voice mail returned, if voice mail is assigned.
        * Note: If the location is empty, required dialog
        * will be displayed and voice mail will be defined.
        *
        * @param aNumber    reference of phone number.
        * @retunr   EFalse if user cancel or failed.
        */
        IMPORT_C TBool ExVoiceMailL(TDes& aNumber);
        
        IMPORT_C TBool ExVideoMailL(TDes& aNumber);
        /**
        * The value converted into the index of an array from
        * the location of speeddial is returned.
        *
        * @param aDial      a location of speed dial.
        * @return   the index of a speeddial index data, if not found -1
        */
        IMPORT_C TInt Index(TInt aDial) const;
        
        /**
        * The value converted into the the location of speeddial
        * from index of an array is returned.
        *
        * @param aIndex     a index of array.
        * @return   location of speed dial(1-9), if not found -1
        */
        IMPORT_C TInt Number(TInt aIndex) const;
        
        /**
        * Returns the phone number of a speed dial contact.
        * Note: left and right spaces are removed.
        *
        * @param aIndex     a index of array.
        */
        IMPORT_C const TDesC& PhoneNumber(TInt aIndex) const;
        
          
        /**
        * Returns the thumbnail index of a grid array.
        *
        * @param aIndex     a index of array.
        */
        IMPORT_C TInt ThumbIndex(TInt aIndex) const;
        
        /**
        * Icons, thumbnails and text array for the grid are created.
        * Note: If a aIndex parameter is set, only the data of the value
        *       will be set up.
        * Note: CreateDataL() is used if App to be used is except SD.
        *
        * @param aGrid      the grid to set up
        * @param aIndex     (TInt) if <-1, All values are set up.
        */
        IMPORT_C TBool CreateGridDataL(CAknGrid* aGrid, TBool aIndex);
        
        /**
        * The position which a shadow draws is set up.
        *
        * @param aRect
        */
        IMPORT_C void SetLayout(const TRect& aRect);
        
         /**
        * Returns the position of voice mail box
        */
        IMPORT_C TInt VMBoxPosition() const;
        IMPORT_C TInt VdoMBoxPosition() const;
              
        /**
        * Returns the icon index of grid array.
        */
        IMPORT_C TInt SpdIconIndex(TInt aIndex) const;
				
		/**
        */
		IMPORT_C void DeleteIconArray();
		
		/** 
        */
		IMPORT_C void ReloadIconArray();
		
		
		IMPORT_C CArrayPtr<CGulIcon>* IconArray() const;
		
		
		IMPORT_C TInt VoiceMailType();
		
		/**
        * Returns last error.
        */
		IMPORT_C TInt GetSpdCtrlLastError();
		       
        /**
        * Draw this control
        */
        void Draw(const TRect& aRect) const;
        
        /**
        * Set size for the skin background context.
        */
        void SizeChanged();
        
        /**
        * Passes skin information when needed.
        **/
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
                        
        
        /**
        * Creates a grid itemArray.
        * Note: It calls, when using a grid except SDM.
        *
        * @param aGrid      the grid to set up
        * @param aUpdate    updates in an event.
        */
        TBool CreateDataL(const CAknGrid& aGrid);
        
      
      	IMPORT_C void SetUpdateFlag(TBool aFlag);
    	IMPORT_C TBool UpdateFlag();
    	IMPORT_C  void SetState(eSpeedDialState);
    	IMPORT_C  eSpeedDialState State();
    	IMPORT_C  TGridUse GridStatus();
    	IMPORT_C  void SetGridStatus(TGridUse aStatus);
    	
        /**
        * Returns type of the phone number.
        */
        IMPORT_C TInt NumberType(TInt aIndex) const;
        
        
        IMPORT_C MVPbkStoreContact* Contact(TInt aIndex);
        
        IMPORT_C HBufC* ContactTitleL( MVPbkBaseContact* aContact, TBool aUnnamed = ETrue);
        
		
		/**
        * Thumbnail's shadow is drawn.
        *
        * @param aGc    window gc
        */
        void DrawShadow(CWindowGc& aGc) const;
        
       	/**
        * Internal rutine to check if pop up has to be shown
        * to enable the One-Key-Dialling setting.
        * @return   bool value of one key calling status on or off.
        */
       IMPORT_C  TBool CheckingIfPopUpNeededL( );
        
       /*********************************************************
 		********** MVPbkContactFindObserver APIs ****************
 		*********************************************************/
        
        void FindCompleteL(MVPbkContactLinkArray* aResults);
        void FindFailed(TInt aError);
        
        
         /********************************************************
 		 ********** MPbk2ImageGetObserver APIs *******************
 		 *********************************************************/
       	
       	/**
    	 * Called when image loading is completed.
         *
         * @param aOperation    The completed operation.
         * @param aBitmap       The loaded bitmap. Callee is responsible of
         *                      the bitmap.
         */
         void Pbk2ImageGetComplete(
                 MPbk2ImageOperation& aOperation,
                 CFbsBitmap* aBitmap ) ;
 
         /**
          * Called if the image loading fails.
          *
          * @param aOperation    The failed operation.
          * @param aError        Error code of the failure.
          */
         void Pbk2ImageGetFailed(
                 MPbk2ImageOperation& aOperation,
                 TInt aError );
        
        /*********************************************************
 		 ****** MVPbkSetAttributeObserver APIs *******************
 		 *********************************************************/
        
        /**
        * Attribute operation completed.
        * @param aOperation    Contact operation that was completed.
        */
        void AttributeOperationComplete(
             MVPbkContactOperationBase& aOperation) ;
             
        /**
        * Attribute operation failed.
        * @param aOperation    Contact operation that failed.
        * @param aError        Error code.
        */
         void AttributeOperationFailed(
             MVPbkContactOperationBase& aOperation, TInt aError) ;

		 /*********************************************************
 		 ****** MVPbkSingleContactOperationObserver APIs **********
 		 **********************************************************/
                        
    	/* Called when operation is completed.
      	*
      	* @param aOperation the completed operation.
      	* @param aContact  the contact returned by the operation.
      	*                  Client must take the ownership immediately.
      	*
      	*                  !!! NOTICE !!!
      	*                  If you use Cleanupstack for MVPbkStoreContact
      	*                  Use MVPbkStoreContact::PushL or
      	*                  CleanupDeletePushL from e32base.h.
      	*                  (Do Not Use CleanupStack::PushL(TAny*) because
      	*                  then the virtual destructor of the M-class
      	*                  won't be called when the object is deleted).
      	*/
      void VPbkSingleContactOperationComplete(
             MVPbkContactOperationBase& aOperation,
             MVPbkStoreContact* aContact);

         /**
          * Called if the operation fails.
          *
          * @param aOperation    the failed operation.
          * @param aError        error code of the failure.
          */
         void VPbkSingleContactOperationFailed(
                 MVPbkContactOperationBase& aOperation, 
                 TInt aError);
 		
 	   /**********************************************************
 		**************** MAiwNotifyCallback APIs******************
 		**********************************************************/
        TInt HandleNotifyL(
		        TInt aCmdId,
		        TInt aEventId,
		        CAiwGenericParamList& aEventParamList,
		        const CAiwGenericParamList& aInParamList);
		
	   /**********************************************************
 		**************** MAiwNotifyCallback APIs******************
 		**********************************************************/       
        void StoreReady(MVPbkContactStore& aContactStore);
        void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent);
                
   	   /**********************************************************
 		**************** MVPbkContactObserver APIs****************
 		**********************************************************/               
        void ContactOperationCompleted(TContactOpResult aResult);
        void ContactOperationFailed(TContactOp aOpCode, TInt aErrorCode, 
        							TBool aErrorNotified);
 		/**********************************************************
 		 ********************* Public APIs*************************
 		 **********************************************************/
	
		/**
	    * Shows a selection grid dialog for assigning a speed dial.
	    * After selecting the index from the grid, speed dial will be set for
	    * the given contact.
	    *
	    * @param aContactLink Contact link pointing to contact and field.
	    * @param aIndex On return contains the index where speed dial was assigned to.
	    * @return ETrue if all dialgs were accepted.
	    */
       	IMPORT_C TInt ShowAssign( MVPbkContactLink& aContactLink, TInt& aIndex );
	 	/**
	    * Shows necessary dialog(s) to assign a contact to the given speed dial
	    * index. After accepting the dialogs and selecting the contact, speed dial
	    * will be set to the given index.
	    *
	    * @param aIndex Speed dial index.
	    * @param aContactLink On return contains the contact, for which the speed dial was assigned to.
	    * @return ETrue if all dialgs were accepted.
	    */
	    IMPORT_C TInt ShowAssign( TInt aIndex, MVPbkContactLink*& aContactLink );
	    
	    /**
	    * Shows necessary dialog(s) to remove a speed dial from the given index. 
	    * After accepting the dialog(s), speed dial will be removed from the index. 
	    *
	    * @param aIndex Index of the speed dial to be removed.
	    * @return ETrue if all dialgos were accepted.
	    */
	    IMPORT_C TInt ShowRemove( TInt aIndex );
	    
	    /**
        * Method is used to notify the cancel event
        * 
        * @return  KErrNone if Cancel is successful.
        */
	    IMPORT_C TBool Cancel();
	    /**
        * D'tor
        * 
        */
	        
	    IMPORT_C ~CSpeedDialPrivate();
	    /**
        * Retirves the Email Field attribute value
        * 
        */
	          
	    IMPORT_C HBufC* EMail(MVPbkStoreContact* aContact);
        /**
        * GetWait,monitors the Asynchronous behaviour upon client side
        * 
        */
        
        IMPORT_C CActiveSchedulerWait* GetWait();

        /**
         * Set the refresh observer.
         * @param aObserver   the observer for refreshment.
         */
        IMPORT_C void SetRefreshObserver( MRefreshObserver* aObserver );
        
        /**
         * Set Current Grid Index
         * @param aCurrentGridIndex the current grid index.
         */
        IMPORT_C void SetCurrentGridIndex( TInt aCurrentGridIndex );
        
        /**
         * Set Current iGrid and iIconArray as Null
         */        
        IMPORT_C void SetGridDataAsNull();
private:

        /**
        * Index data are initialized.
        */
        void InitIndexDataL();
        
        /**
        * Creates Item's descriptor for Grid.
        *
        * @param aSdmData   reference to the data currently used.
        * @returen  a buffer containing the descriptor text.
        */
        HBufC* CreateItemDescriptorLC(const TSpdiaIndexDataVPbk& aSdmData);
                 
                          
        /**
        * A descriptor is added to a text.
        *
        * @param aSdmData   reference to the data currently used.
        * @param aText      the text for storing
        */
        void AppendTextL(const TSpdiaIndexDataVPbk& aSdmData, TPtr& aText);
        
  
                        
        /**
        * Operations to a CPbk2ImageManager are set.
        * Note: call GetImageAsyncL().
        */
        void SetOperationsL();
        
        /**
        * Updates data set to match index of the owned index data array.
        */
        TBool UpdateIndexDataL(TInt aIndex);
        
        /**
        * Creates icon data for SDM control data by the data of parameters.
        *
        * @param aIndex             a index of array.
        * @param aSdmData           reference to the data currently used.
        * @param aSetUpdateThumb    thumbnail load start parameter set in index data
        */
        TBool CreateIndexIconL(TInt aIndex, TSpdiaIndexDataVPbk& aSdmData,
                            TBool aSetUpdateThumb);
                            
        /**
        * The position on the array that a grid uses from
        * an item and a phone number is returned.
        *
        * @param aItem      contacd item
        * @param aDial      phone number
        */
        //TInt IconPositionL( TInt aDial, TInt& aFieldId);
        
        /**
        * Speeddial index data is changed by the data of parameters.
        *
        * @param aIndex     a index of array.
        * @param aSdmData   reference to the data currently used.
        */
        void ChangeIndexDataL(TInt aIndex, const TSpdiaIndexDataVPbk& aSdmData);
        
        /**
        * Searches icon of grid array for a match.
        *
        * @param aId     phone book icon id to match to.
        * @return   the matching index, -1 if not found.
        */
      
      	TInt FindIconIndex( TInt aId) const;


		/**
        * Gets Item's descriptor for Grid.
        *
        * @param aIndex     a index of array.
        * @param aSdmData   reference to the data currently used.
        * @return   a buffer containing the descriptor text.
        */
        HBufC* ItemDescriptorLC(TInt aIndex, const TSpdiaIndexDataVPbk& aSdmData);       
        
        /**
        * The data of items are added to an array.
        *
        * @param aArray
        */
        void SetItemDataL(CDesCArray& aArray);
        
        /**
        * Show note when the contact is in using.
        *
        * @param aResourceId    reaource id of the string to prompt.
        */
        void ShowNoteL( TInt aResourceId );
        
		/**
        * From MContactDbObserver, Handles an event of type aEventType.
        */
  //      void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
        
         /*********************************************************
 		 * 				Thumb operations 						  *
 		 **********************************************************/
               
        /**
        * Gets actual size of thumbnial to be drawn.
        * @return  size of thumbnail.
        */
        
        TSize GetThumbnailSize();
        /**
        * Returns the thumbnail type.
        *
        * @param aIndex     a index of array.
        */
        TInt ThumbType(TInt aIndex) const;
                       
        /**
        * Returns the thumbnail size.
        *
        * @param aIndex     a index of array.
        */
        TSize ThumbSize(TInt aIndex) const;
        
        /**
        * Returns the type from a size of thumbnail.
        *
        * @param aSize  a thumbnail size
        */
        TInt ConvThumbType(const TSize& aSize) const; 
        
        /**
        * The descriptor for thumbnail is added.
        *
        * @param aSdmData           reference to the data currently used.
        * @param aText
        * @param aFixedLocation
        */
        void AppendThumbnail(const TSpdiaIndexDataVPbk& aSdmData,
                            TPtr& aText,
                            TBool aFixedLocation);
                                          
                            
	    /*********************************************************
		*					Construction 						 *
		**********************************************************/   
        CSpeedDialPrivate(CVPbkContactManager* aContactManager);
    	void ConstructL();
    	
    	/**
        * A text array that a grid has is erased.
        */
        void ResetArray();
    	
    	/**
        * Speeddial index data is set by the data of parameters.
        *
        * @param aIndex     a index of array.
        * @param aSdmData   reference to the data currently used.
        */
        void SetIndexDataL(TInt aIndex, TSpdiaIndexDataVPbk& aSdmData);
        
        TInt HasImageOperation(MPbk2ImageOperation& aOperation);
        
        TInt  HasOperation(MVPbkContactOperationBase* aContactOpertion);
        
        TBool CheckSpaceBelowCriticalLevelL();
        
        void ExecuteSingleEntryFetchL(TInt aIndex,
        TAiwSingleEntrySelectionDataV2 aData,
        const TDesC& aUri1 = KNullDesC );
        
        void ExecuteSingleEntryAssignL(TInt aIndex,
        AiwContactAssign::TAiwContactAttributeAssignDataV1 aData);
    
    	TInt SetIconArrayL(CArrayPtrFlat<CGulIcon>* aArray);
    	void InitializeArray();
    	void CreateFieldIconsL(CArrayPtrFlat<CGulIcon>* aArray);
    	
    	TBool AssignSpeedDialL(TInt aIndex);
    	TBool RemoveSpeedDialL(TInt aIndex);
    	
    	void SetSpeedDialIndexL(MVPbkStoreContact* aContact);
    	void Wait();
    	void Release();
    	
    	void SetDetails(TInt aIndex);
    	
    	// Change to TInt from TBool for more value
    	TInt FindInSpeedDialContacts(MVPbkContactLink* aContactLink);
    	// TBool FindInSpeedDialContacts(MVPbkContactLink* aContactLink);
    	
    	TBool CheckContactAssignedFromPhoneBookL(MVPbkContactLink* aContactLink);
    	
    	TBool ShowAssignL( MVPbkContactLink& aContactLink, TInt& aIndex );
	    TBool ShowAssignL( TInt aIndex, MVPbkContactLink*& aContactLink );
	    TBool ShowRemoveL( TInt aIndex );
	    
	    void InitBmpArray();
    	    
	private:
		void Pbk2ImageGetCompleteL(
                 MPbk2ImageOperation& aOperation,
                 CFbsBitmap* aBitmap ) ;
                 
        void ReplaceNonGraphicCharacters(TDes& aText, TText aChar);
 	
 	private:
 	
    TGridUse iGridUsed;
    CArrayPtrFlat<CGulIcon>* iIconArray;
    TAknLayoutRect icellLayout[KArraySize];
    CArrayFixFlat<TAknLayoutRect>* iShadowLayout;     // owned by this
    CAknsBasicBackgroundControlContext* iBgContext;
    
    TInt iCurrentIndex;
    TInt iType;
    TInt iError;
    TInt iSdmCount;                             // Grid cell count(Row * col)
    TBool iSaveDlg;
    TBool iPopupDialog;
	CAknQueryDialog* iSaveChangesDialog;
	CSpdiaGridDlgVPbk* iGridDlg; 
	TBuf<254> iBmpPath;
	TBool iBooleanVarForAddingDialogVariation;
	TInt    iFileOffset;
	
	RArray<TAknsItemID>*iDialSkinBmp;
	RArray<TAknsItemID>*iSkinIcons;

	/* Member variables added for migration */
	CVPbkContactManager* iContactManager;
    MVPbkContactAttributeManager* iAttributeManager;
    CPbk2ImageManager* iImageManager;
    
    const MVPbkContactLink* iContactLink;
    MVPbkStoreContact *iContact;
    MVPbkContactLinkArray* iContactLinkArray;
    const MVPbkFieldType* iThumbNailFieldType;
    
    CVPbkSpeedDialAttribute* iSpeedDial;
    
    eSpeedDialState iState;
    
    TBool iRetrieveComplete;
    TBool iOperationComplete;
    TBool iFetchAll;
    TBool iUpdateFlag;
    TBool iFetchmail;
        
	CAknGrid* iGrid;	 
	CArrayFixFlat<TSpdiaIndexDataVPbk>* iSdmArray;  // owned by this
	CPbk2ImageManager* aImageManager;
	CActiveSchedulerWait *iWait;
	CAknQueryDialog* iQueryDialog;       
	CRepository*  ivmbxvariation;
	CRepository* ivmbxkey2;
	TInt iVmbxkeypos;
	TInt iVmbxsupported;
	TBool iShowAssignCalled;
	
	TUint iDialBmp[9];
	TUint iDialBmpMask[9];
    TUint iDialHindiBmp[9];
    TUint iDialHindiBmpMask[9];
    HBufC* iMail;
    TInt iVideoMail;
    CAiwServiceHandler* iServiceHandler;

    MRefreshObserver* iRefreshObserver;
    TBool iCancelFlag;

    CAknQueryDialog* iRemoveConfirmQueryDialog;
};


#endif //SPEEDDIALPRIVATE_H
