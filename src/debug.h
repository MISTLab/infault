/***************************************************************************
 *   Copyright (C) 2007 by Simon Schulz   *
 *   Simon.Schulz@esa.int   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#define DEBUG 1
#define VPARSER_DEBUG 2
#define ANALYZER_DEBUG 2

#if DEBUG == 0
    #define debug_printf(...) {};
    #define debug_verbose_printf(...) {};
#elif DEBUG == 1
    #define debug_printf(...) printf(__VA_ARGS__);
    #define debug_verbose_printf(...) {};
#else
    #define debug_printf(...) printf(__VA_ARGS__);
    #define debug_verbose_printf(...) printf(__VA_ARGS__);;
#endif

#if VPARSER_DEBUG == 0
    #define debug_vparse_printf(...) {};
    #define debug_vparse_verbose_printf(...) {};
#elif VPARSER_DEBUG == 1
    #define debug_vparse_printf(...) printf(__VA_ARGS__);
    #define debug_vparse_verbose_printf(...) {};
#else
    #define debug_vparse_printf(...) printf(__VA_ARGS__);
    #define debug_vparse_verbose_printf(...) printf(__VA_ARGS__);;
#endif

#if ANALYZER_DEBUG == 0
    #define debug_analyze_printf(...) {};
    #define debug_analyze_verbose_printf(...) {};
#elif ANALYZER_DEBUG == 1
    #define debug_analyze_printf(...) printf(__VA_ARGS__);
    #define debug_analyze_verbose_printf(...) {};
#else
    #define debug_analyze_printf(...) printf(__VA_ARGS__);
    #define debug_analyze_verbose_printf(...) printf(__VA_ARGS__);;
#endif
