#
# Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:    Icons makefile for project Speeddial.
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
# Configure these
# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\spdctrl.mif
HEADERFILENAME=$(HEADERDIR)\spdctrl.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	if exist $(HEADERFILENAME) del $(HEADERFILENAME)
	if exist $(ICONTARGETFILENAME) del $(ICONTARGETFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# Configure these.
#
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

RESOURCE :
	mifconv $(ICONTARGETFILENAME)  /h$(HEADERFILENAME) \
/c8,8 qgn_menu_smsvo.svg\
/c8,8 qgn_graf_quick_two.svg\
/c8,8 qgn_graf_quick_three.svg\
/c8,8 qgn_graf_quick_four.svg\
/c8,8 qgn_graf_quick_five.svg\
/c8,8 qgn_graf_quick_six.svg\
/c8,8 qgn_graf_quick_seven.svg\
/c8,8 qgn_graf_quick_eight.svg\
/c8,8 qgn_graf_quick_nine.svg\
/c8,8 qgn_graf_deva_quick_one.svg\
/c8,8 qgn_graf_deva_quick_two.svg\
/c8,8 qgn_graf_deva_quick_three.svg\
/c8,8 qgn_graf_deva_quick_four.svg\
/c8,8 qgn_graf_deva_quick_five.svg\
/c8,8 qgn_graf_deva_quick_six.svg\
/c8,8 qgn_graf_deva_quick_seven.svg\
/c8,8 qgn_graf_deva_quick_eight.svg\
/c8,8 qgn_graf_deva_quick_nine.svg\
/c8,8 qgn_prop_video_mb.svg
   
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing