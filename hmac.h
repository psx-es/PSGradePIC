/*  HMAC-SHA-1 - an implementation of the HMAC message authentication
    Version as of March 4th 2007
    
    Copyright (C) 2007 CHZ-Soft, Christian Zietz, <czietz@gmx.net>
    See README file for more information.
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the 
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
    Boston, MA  02110-1301, USA
*/

#ifndef __HMAC_H__
#define __HMAC_H__

#include "sha1.h"

#define hmacdigest shadigest

void HMACInit(unsigned char* key, unsigned char len);
void HMACBlock(unsigned char* data, unsigned char len);
void HMACDone(void);
void HMACOnce(unsigned char* key, unsigned char klen, 
              unsigned char* data, int len);

#endif

