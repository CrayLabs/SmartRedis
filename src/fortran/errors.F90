! BSD 2-Clause License
!
! Copyright (c) 2021-2022, Hewlett Packard Enterprise
! All rights reserved.
!
! Redistribution and use in source and binary forms, with or without
! modification, are permitted provided that the following conditions are met:
!
! 1. Redistributions of source code must retain the above copyright notice, this
!    list of conditions and the following disclaimer.
!
! 2. Redistributions in binary form must reproduce the above copyright notice,
!    this list of conditions and the following disclaimer in the documentation
!    and/or other materials provided with the distribution.
!
! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
! DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
! FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
! DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
! SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
! CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
! OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
! OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

module smartredis_errors

use iso_c_binding, only : c_ptr, c_char, c_size_t, c_f_pointer

use, intrinsic :: iso_fortran_env, only: stderr => error_unit
implicit none; private

#include "enum_fortran.inc"
#include "errors/errors_interfaces.inc"

public :: get_last_error, get_last_error_location

contains

!> Convert a pointer view of a string to a Fortran string
function make_str(strptr, str_len)
  character(kind=c_char, len=:), allocatable :: make_str
  type(c_ptr), intent(in), value             :: strptr
  integer(kind=c_size_t)                     :: str_len

  character(len=str_len, kind=c_char), pointer :: ptrview
  call c_f_pointer(strptr, ptrview)
  make_str = ptrview
end function make_str

!> Get the text of the last error that occurred in the SmartRedis library
function get_last_error()
  character(kind=c_char, len=:), allocatable :: get_last_error  !< Text associated with the last error

  type(c_ptr)               :: cstr
  integer(kind=c_size_t)    :: cstr_len

  cstr = c_get_last_error()
  cstr_len = c_strlen(cstr)
  get_last_error = make_str(cstr, cstr_len)
end function get_last_error

!> Get the location of the last error that occurred in the SmartRedis library
function get_last_error_location()
  character(kind=c_char, len=:), allocatable :: get_last_error_location  !< Location information for the last error

  type(c_ptr)               :: clocstr
  integer(kind=c_size_t)    :: clocstr_len

  clocstr = c_get_last_error_location()
  clocstr_len = c_strlen(clocstr)
  get_last_error_location = make_str(clocstr, clocstr_len)
!
!  block
!    character(len=n, kind=c_char), pointer :: ptrview
!    call c_f_pointer(clocstr, ptrview)
!    get_last_error_location = ptrview
!  end block
end function get_last_error_location

end module smartredis_errors

