! BSD 2-Clause License
!
! Copyright (c) 2021-2023, Hewlett Packard Enterprise
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

module smartredis_configoptions

use iso_c_binding,   only : c_ptr, c_bool, c_null_ptr, c_char, c_int
use iso_c_binding,   only : c_int8_t, c_int16_t, c_int32_t, c_int64_t, c_float, c_double, c_size_t
use iso_c_binding,   only : c_loc, c_f_pointer

use, intrinsic :: iso_fortran_env, only: stderr => error_unit

use fortran_c_interop, only : enum_kind

implicit none; private

include 'enum_fortran.inc'
include 'configoptions/configoptions_interfaces.inc'

public :: enum_kind !< The kind of integer equivalent to a C enum. According to C an Fortran
                    !! standards this should be c_int, but is renamed here to ensure that
                    !! users do not have to import the iso_c_binding module into their
                    !! programs

!> Contains multiple tensors and metadata used to describe an entire set of data
type, public :: configoptions_type
  type(c_ptr) :: configoptions_ptr !< A pointer to the initialized dataset object

  contains

  !> Access the raw C pointer for the configoptions
  procedure :: get_c_pointer

  ! Factory methods
  !> Instantiate ConfigOptions, getting selections from environment variables
  procedure :: create_configoptions_from_environment
  !> Instantiate ConfigOptions, getting selections from a file with JSON data
  procedure :: create_configoptions_from_file
  !> Instantiate ConfigOptions, getting selections from a string containing a JSON blob
  procedure :: create_configoptions_from_string
  !> Instantiate ConfigOptions, getting selections from the current default source
  procedure :: create_configoptions_from_default

end type configoptions_type

contains


!> Access the raw C pointer for the dataset
function get_c_pointer(self)
  type(c_ptr)                           :: get_c_pointer
  class(configoptions_type), intent(in) :: self
  get_c_pointer = self%configoptions_ptr
end function get_c_pointer

!> Instantiate ConfigOptions, getting selections from environment variables
function create_configoptions_from_environment(self, db_prefix, log_context) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  character(len=*),    intent(in)          :: db_prefix   !< Prefix to apply to environment
                                                          !! variables; empty string for none
  character(len=*),    intent(in)          :: log_context !< Logging context
  integer(kind=enum_kind)                  :: code !< Result of the operation

  ! Local variables
  integer(kind=c_size_t) :: db_prefix_length, log_context_length
  character(kind=c_char, len=len_trim(db_prefix)) :: c_db_prefix
  character(kind=c_char, len=len_trim(log_context)) :: c_log_context

  db_prefix_length = len_trim(db_prefix)
  log_context_length = len_trim(log_context)
  c_db_prefix = trim(db_prefix)
  c_log_context = trim(log_context)

  code = create_configoptions_from_environment_c( &
    c_db_prefix, db_prefix_length, c_log_context, &
    log_context_length, self%configoptions_ptr)
end function create_configoptions_from_environment

!> Instantiate ConfigOptions, getting selections from a file with JSON data
function create_configoptions_from_file(self, filename, log_context) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  character(len=*),    intent(in)          :: filename    !< File containing JSON data
  character(len=*),    intent(in)          :: log_context !< Logging context
  integer(kind=enum_kind)                  :: code !< Result of the operation

  ! Local variables
  integer(kind=c_size_t) :: filename_length, log_context_length
  character(kind=c_char, len=len_trim(filename)) :: c_filename
  character(kind=c_char, len=len_trim(log_context)) :: c_log_context

  filename_length = len_trim(filename)
  log_context_length = len_trim(log_context)
  c_filename = trim(filename)
  c_log_context = trim(log_context)

  code = create_configoptions_from_file_c( &
    c_filename, filename_length, c_log_context, &
    log_context_length, self%configoptions_ptr)
end function create_configoptions_from_file

!> Instantiate ConfigOptions, getting selections from a string containing a JSON blob
function create_configoptions_from_string(self, json_blob, log_context) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  character(len=*),    intent(in)          :: json_blob   !< String containing JSON data
  character(len=*),    intent(in)          :: log_context !< Logging context
  integer(kind=enum_kind)                  :: code !< Result of the operation

  ! Local variables
  integer(kind=c_size_t) :: json_blob_length, log_context_length
  character(kind=c_char, len=len_trim(json_blob)) :: c_json_blob
  character(kind=c_char, len=len_trim(log_context)) :: c_log_context

  json_blob_length = len_trim(json_blob)
  log_context_length = len_trim(log_context)
  c_json_blob = trim(json_blob)
  c_log_context = trim(log_context)

  code = create_configoptions_from_string_c( &
    c_json_blob, json_blob_length, c_log_context, &
    log_context_length, self%configoptions_ptr)
end function create_configoptions_from_string

!> Instantiate ConfigOptions, getting selections from the current default source
function create_configoptions_from_default(self, log_context) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  character(len=*),    intent(in)          :: log_context !< Logging context
  integer(kind=enum_kind)                  :: code !< Result of the operation

  ! Local variables
  integer(kind=c_size_t) :: log_context_length
  character(kind=c_char, len=len_trim(log_context)) :: c_log_context

  log_context_length = len_trim(log_context)
  c_log_context = trim(log_context)

  code = create_configoptions_from_default_c( &
    c_log_context, log_context_length, self%configoptions_ptr)
end function create_configoptions_from_default

end module smartredis_configoptions
