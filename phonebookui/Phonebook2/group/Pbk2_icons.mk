#
# Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#             Phonebook 2 icons makefile.
# ===========================================================================
#


# ---------------------------------------------------------------------------
# Inspect platform variables to find out root of the Z drive.
# ---------------------------------------------------------------------------
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
  ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
  ZDIR=\epoc32\data\z
endif

# ---------------------------------------------------------------------------
# Set target directories for the icons.
#----------------------------------------------------------------------------

AIF_TARGETDIR=$(ZDIR)\resource\apps
MIF_TARGETDIR=$(ZDIR)\resource\apps

# ---------------------------------------------------------------------------
# Define the icon and header files using directories defined above.
# ---------------------------------------------------------------------------
HEADERDIR=\epoc32\include
AIF_ICONFILE=$(AIF_TARGETDIR)\phonebook2_aif.mif
MIF_ICONFILE=$(MIF_TARGETDIR)\phonebook2.mif
MIF_HEADERFILE=$(HEADERDIR)\phonebook2.mbg

# ---------------------------------------------------------------------------
# Inspect the SCALABLE_FLAG to decide which bitmaps will be used.
# ---------------------------------------------------------------------------
ifeq ($(SCALABLE_FLAG), off)
  AIF_APPLICATION_ICON=/c8,8 qgn_menu_phob_lst /c8,8 qgn_menu_phob_cxt
  AIF_APPLICATION_CONTEXT_ICON=/c8,8 qgn_menu_phob_cxt
else
  AIF_APPLICATION_ICON=/c8,8 qgn_menu_phob
  AIF_APPLICATION_CONTEXT_ICON=/c8,8 qgn_menu_phob
endif

# ---------------------------------------------------------------------------
# Empty rules.
# ---------------------------------------------------------------------------
do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ---------------------------------------------------------------------------
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2,
# \s60\bitmaps.
# The directory \s60\icons is included in the search only if the feature
# flag __SCALABLE_ICONS is defined.
# ---------------------------------------------------------------------------

RESOURCE : AIFICONS MIFICONS

AIFICONS :
	mifconv $(AIF_ICONFILE) \
		$(AIF_APPLICATION_ICON)

MIFICONS :
	mifconv $(MIF_ICONFILE) /h$(MIF_HEADERFILE) \
		/c8,1 qgn_prop_nrtyp_date \
		/c8,1 qgn_prop_nrtyp_tone \
		/c8,1 qgn_indi_qdial_add \
		/c8,1 qgn_indi_voice_add \
		/c8,1 qgn_prop_note_list_alpha_tab2 \
		/c8,1 qgn_prop_list_tab3 \
		/c8,1 qgn_prop_group_tab2 \
		/c8,1 qgn_prop_group_tab3 \
		/c8,1 qgn_prop_group_open_tab1 \
		/c8,8 qgn_prop_pb_contacts_tab1 \
		/c8,1 qgn_prop_pb_contacts_tab3 \
		/c8,1 qgn_prop_pb_personal_tab4 \
		/c8,1 qgn_prop_pb_photo_tab3 \
		/c8,1 qgn_graf_pb_status_backg \
		/c8,1 qgn_menu_empty_cxt \
		$(AIF_APPLICATION_CONTEXT_ICON) \
		/c8,1 qgn_prop_empty \
		/c8,8 qgn_graf_bg_phob_thumbnail \
		/c8,8 qgn_prop_nrtyp_poc \
		/c8,8 qgn_prop_nrtyp_swis \
		/c8,8 qgn_prop_nrtyp_sip \
		/c8,8 qgn_indi_tb_call \
		/c8,8 qgn_indi_tb_mce \
		/c8,8 qgn_indi_dialer_add_contacts	\
		/c8,8 qgn_prop_pb_topc \
		/c8,8 qgn_prop_locev_map \
		/c8,8 qgn_prop_pb_no_valid_lm \
		/c8,8 qgn_prop_blid_waypoint
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(AIF_ICONFILE) && \
	@echo $(MIF_HEADERFILE) && \
	@echo $(MIF_ICONFILE)

FINAL : do_nothing

# End of File
