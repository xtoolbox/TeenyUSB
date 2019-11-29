/*------------------------------------------------------------------------*/
/* Unicode handling functions for FatFs R0.13+                            */
/*------------------------------------------------------------------------*/
/* This module will occupy a huge memory in the .const section when the    /
/  FatFs is configured for LFN with DBCS. If the system has any Unicode    /
/  utilitiy for the code conversion, this module should be modified to use /
/  that function to avoid silly memory consumption.                        /
/-------------------------------------------------------------------------*/
/*
/ Copyright (C) 2014, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
*/


#include "ff.h"

#if FF_USE_LFN	/* This module will be blanked if non-LFN configuration */

#error TeenyUSB demo not support LFN


#endif /* #if FF_USE_LFN */
