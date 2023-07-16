/*
 * Copyright (C) 2017-2023 Adrien Gesta-Fline
 *
 * This file is part of libAAF.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __debug_h__
#define __debug_h__

#include <stdio.h>


#define DEBUG_FUNCTION_POINTER (*debug_callback)(void *ctxdata, int lib, int type, const char *srcfile, const char *srcfunc, int lineno, const char *msg, void *user )

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


enum debug_source_id {
	DEBUG_SRC_ID_LIB_CFB,
	DEBUG_SRC_ID_AAF_CORE,
	DEBUG_SRC_ID_AAF_IFACE,
	DEBUG_SRC_ID_TRACE,
	DEBUG_SRC_ID_DUMP
};


typedef enum verbosityLevel_e {
	VERB_QUIET = 0,
	VERB_ERROR,
	VERB_WARNING,
	VERB_DEBUG,
	MAX_VERB
} verbosityLevel_e;


struct dbg {

	void DEBUG_FUNCTION_POINTER;

	verbosityLevel_e verb;

	char *_dbg_msg;
	int   _dbg_msg_size;

	void *user;
};




#define _dbg( dbg, ctxdata, lib, type, ... ) \
	{ const char *dbgfile = __FILENAME__; const char *dbgfunc = __func__; int dbgline = __LINE__; \
	if ( dbg && dbg->verb >= type && dbg->debug_callback ) { \
		int msgsize = snprintf( NULL, 0, __VA_ARGS__ ) + 1; \
		if ( msgsize >= dbg->_dbg_msg_size ) { \
			char *msgtmp = realloc( dbg->_dbg_msg, msgsize ); \
			if ( msgtmp ) { \
				dbg->_dbg_msg = msgtmp; \
				dbg->_dbg_msg_size = msgsize; \
				snprintf( dbg->_dbg_msg, dbg->_dbg_msg_size, __VA_ARGS__ ); \
				dbg->debug_callback( (void*)ctxdata, lib, type, dbgfile, dbgfunc, dbgline, dbg->_dbg_msg, dbg->user ); \
			} else { \
				/* Should we print error to stderr ? */ \
			} \
		} else { \
			snprintf( dbg->_dbg_msg, dbg->_dbg_msg_size, __VA_ARGS__ ); \
			dbg->debug_callback( (void*)ctxdata, lib, type, dbgfile, dbgfunc, dbgline, dbg->_dbg_msg, dbg->user ); \
		} \
	}} \


struct dbg * new_debug( void );

void free_debug( struct dbg *dbg );

void debug_callback( void *ctxdata, int lib, int type, const char *srcfile, const char *srcfunc, int lineno, const char *msg, void *user );


#endif // ! __debug_h__
