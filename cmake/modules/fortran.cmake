# cmake/modules/fortran.cmake
#
# F77/F95 binding configuration 
#
# Copyright (C) 2006  Andrew Ross
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# Module for determining F77/F95 bindings configuration options

# Options to enable Fortran bindings
OPTION(ENABLE_F77 "Enable f77 bindings" ON)
OPTION(ENABLE_F95 "Enable f95 bindings" OFF)

IF (ENABLE_F77 OR ENABLE_F95)

# Check for fortran compiler
ENABLE_LANGUAGE(Fortran)

# Don't compile Fortran 95 binding if compiler doesn't support it
IF (ENABLE_F95 AND NOT CMAKE_Fortran_COMPILER_SUPPORTS_F90)
  MESSAGE("Fortran compiler does not support f90/95. Disabling f95 bindings")
  SET(ENABLE_F95 OFF CACHE BOOL "Enable f95 bindings" FORCE)
ENDIF (ENABLE_F95 AND NOT CMAKE_Fortran_COMPILER_SUPPORTS_F90)

ENDIF (ENABLE_F77 OR ENABLE_F95)

