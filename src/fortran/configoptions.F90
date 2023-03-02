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

use fortran_c_interop, only : convert_char_array_to_c, enum_kind, C_MAX_STRING

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

  ! Defaults configuration
  !> Set environment variables as the default configuration source
  procedure, nopass :: set_default_from_environment
  !> Set a UTF-8 file containing JSON data as the default configuration source
  procedure, nopass :: set_default_from_file
  !> Set a string containing a JSON blob as the default configuration source
  procedure, nopass :: set_default_from_string

  ! Option access
  !> Retrieve the value of a numeric configuration option
  procedure :: get_integer_option
  !> Retrieve the value of a string configuration option
  procedure :: get_string_option
  !> Retrieve the value of a boolean configuration option
  procedure :: get_boolean_option
  !> Check whether a configuration option is set
  procedure :: is_defined

  ! Option value overrides
  !> Override the value of a numeric configuration option
  procedure :: override_integer_option
  !> Override the value of a string configuration option
  procedure :: override_string_option
  !> Override the value of a boolean configuration option
  procedure :: override_boolean_option

end type configoptions_type

contains


!> Access the raw C pointer for the dataset
function get_c_pointer(self)
  type(c_ptr)                           :: get_c_pointer
  class(configoptions_type), intent(in) :: self
  get_c_pointer = self%configoptions_ptr
end function get_c_pointer

!> Instantiate ConfigOptions, getting selections from environment variables
function create_configoptions_from_environment(self, db_prefix) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  character(len=*),    intent(in)          :: db_prefix   !< Prefix to apply to environment
                                                          !! variables; empty string for none
  integer(kind=enum_kind)                  :: code !< Result of the operation

  ! Local variables
  integer(kind=c_size_t) :: db_prefix_length
  character(kind=c_char, len=len_trim(db_prefix)) :: c_db_prefix

  db_prefix_length = len_trim(db_prefix)
  c_db_prefix = trim(db_prefix)

  code = create_configoptions_from_environment_c( &
    c_db_prefix, db_prefix_length, self%configoptions_ptr)
end function create_configoptions_from_environment

!> Instantiate ConfigOptions, getting selections from a file with JSON data
function create_configoptions_from_file(self, filename) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  character(len=*),    intent(in)          :: filename    !< File containing JSON data
  integer(kind=enum_kind)                  :: code !< Result of the operation

  ! Local variables
  integer(kind=c_size_t) :: filename_length
  character(kind=c_char, len=len_trim(filename)) :: c_filename

  filename_length = len_trim(filename)
  c_filename = trim(filename)

  code = create_configoptions_from_file_c( &
    c_filename, filename_length, self%configoptions_ptr)
end function create_configoptions_from_file

!> Instantiate ConfigOptions, getting selections from a string containing a JSON blob
function create_configoptions_from_string(self, json_blob) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  character(len=*),    intent(in)          :: json_blob   !< String containing JSON data
  integer(kind=enum_kind)                  :: code !< Result of the operation

  ! Local variables
  integer(kind=c_size_t) :: json_blob_length
  character(kind=c_char, len=len_trim(json_blob)) :: c_json_blob

  json_blob_length = len_trim(json_blob)
  c_json_blob = trim(json_blob)

  code = create_configoptions_from_string_c( &
    c_json_blob, json_blob_length, self%configoptions_ptr)
end function create_configoptions_from_string

!> Instantiate ConfigOptions, getting selections from the current default source
function create_configoptions_from_default(self) result(code)
  class(configoptions_type), intent(inout) :: self        !< Receives the configoptions
  integer(kind=enum_kind)                  :: code !< Result of the operation

  code = create_configoptions_from_default_c(self%configoptions_ptr)
end function create_configoptions_from_default

!> Set environment variables as the default configuration source
function set_default_from_environment(db_prefix) result(code)
  character(len=*),          intent(in) :: db_prefix !< Prefix to be prepended
                                                     !! to environment vars
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(db_prefix)) :: c_db_prefix
  integer(kind=c_size_t) :: c_db_prefix_length

  c_db_prefix = trim(db_prefix)
  c_db_prefix_length = len_trim(db_prefix)

  code = set_default_from_environment_c(c_db_prefix, c_db_prefix_length)
end function set_default_from_environment

!> Set a UTF-8 file containing JSON data as the default configuration source
function set_default_from_file(filename) result(code)
  character(len=*),          intent(in) :: filename  !< UTF-8 file with JSON data
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(filename)) :: c_filename
  integer(kind=c_size_t) :: c_filename_length

  c_filename = trim(filename)
  c_filename_length = len_trim(filename)

  code = set_default_from_file_c(c_filename, c_filename_length)
end function set_default_from_file

!> Set a string containing a JSON blob as the default configuration source
function set_default_from_string(json_blob) result(code)
  character(len=*),          intent(in) :: json_blob !< A JSON blob containing the
                                                     !! configuration data
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(json_blob)) :: c_json_blob
  integer(kind=c_size_t) :: c_json_blob_length

  c_json_blob = trim(json_blob)
  c_json_blob_length = len_trim(json_blob)

  code = set_default_from_string_c(c_json_blob, c_json_blob_length)
end function set_default_from_string

!> Retrieve the value of a numeric configuration option
function get_integer_option(self, key, default_value, result) result(code)
  class(configoptions_type), intent(in) :: self          !< The configoptions
  character(len=*),          intent(in) :: key           !< The name of the configuration
                                                         !! option to retrieve
  integer(kind=8),           intent(in) :: default_value !< The baseline value of the
                                                         !! configuration option to be returned
  integer(kind=8),           intent(inout) :: result     !< Receives value of option
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(key)) :: c_key
  integer(kind=c_size_t) :: c_key_length

  c_key = trim(key)
  c_key_length = len_trim(key)

  code = get_integer_option_c( &
    self%configoptions_ptr, c_key, c_key_length, default_value, result)
end function get_integer_option

!> Retrieve the value of a string configuration option
function get_string_option(self, key, default_value, result) result(code)
  class(configoptions_type), intent(in)  :: self          !< The configoptions
  character(len=*),          intent(in)  :: key           !< The name of the configuration
                                                          !! option to retrieve
  character(len=*),          intent(in)  :: default_value !< The baseline value of the
                                                          !! configuration option to be returned
  character(len=:), allocatable, intent(out) :: result    !< Receives value of option
  integer(kind=enum_kind)                :: code

  ! Local variables
  character(kind=c_char, len=len_trim(key)) :: c_key
  character(kind=c_char, len=len_trim(default_value)) :: c_default_value
  integer(kind=c_size_t) :: c_key_length, c_default_value_length
  integer(kind=c_size_t) :: c_result_length, i
  character(kind=c_char), dimension(:), pointer :: f_result_ptr
  type(c_ptr) :: c_result_ptr

  c_key = trim(key)
  c_key_length = len_trim(key)
  c_default_value = trim(default_value)
  c_default_value_length = len_trim(default_value)

  code = get_string_option_c( &
    self%configoptions_ptr, c_key, c_key_length, c_default_value, &
      c_default_value_length, c_result_ptr, c_result_length)
  call c_f_pointer(c_result_ptr, f_result_ptr, [ c_result_length ])

  ALLOCATE(character(len=c_result_length) :: result)
  do i = 1, c_result_length
    result(i:i) = f_result_ptr(i)
  enddo
end function get_string_option

!> Retrieve the value of a boolean configuration option
function get_boolean_option(self, key, default_value, result) result(code)
  class(configoptions_type), intent(in) :: self          !< The configoptions
  character(len=*),          intent(in) :: key           !< The name of the configuration
                                                         !! option to retrieve
  logical(kind=c_bool),      intent(in) :: default_value !< The baseline value of the
                                                         !! configuration option to be returned
  logical(kind=c_bool),      intent(inout) :: result     !< Receives value of option
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(key)) :: c_key
  integer(kind=c_size_t) :: c_key_length

  c_key = trim(key)
  c_key_length = len_trim(key)

  code = get_boolean_option_c( &
    self%configoptions_ptr, c_key, c_key_length, default_value, result)
end function get_boolean_option

!> Check whether a configuration option is set
function is_defined(self, key, result) result(code)
  class(configoptions_type), intent(in) :: self          !< The configoptions
  character(len=*),          intent(in) :: key           !< The name of the configuration
                                                         !! option to check
  logical(kind=c_bool),      intent(inout) :: result     !< Receives value of option
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(key)) :: c_key
  integer(kind=c_size_t) :: c_key_length

  c_key = trim(key)
  c_key_length = len_trim(key)

  code = is_defined_c(self%configoptions_ptr, c_key, c_key_length, result)
end function is_defined

!> Override the value of a numeric configuration option
function override_integer_option(self, key, value) result(code)
  class(configoptions_type), intent(in) :: self      !< The configoptions
  character(len=*),          intent(in) :: key       !< The name of the configuration
                                                     !! option to override
  integer(kind=8),           intent(in) :: value     !< The value to store for the option
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(key)) :: c_key
  integer(kind=c_size_t) :: c_key_length

  c_key = trim(key)
  c_key_length = len_trim(key)

  code = override_integer_option_c( &
    self%configoptions_ptr, c_key, c_key_length, value)
end function override_integer_option

!> Override the value of a string configuration option
function override_string_option(self, key, value) result(code)
  class(configoptions_type), intent(in)  :: self   !< The configoptions
  character(len=*),          intent(in)  :: key    !< The name of the configuration
                                                   !! option to override
  character(len=*),          intent(in)  :: value  !< The value to store for the option
  integer(kind=enum_kind)                :: code

  ! Local variables
  character(kind=c_char, len=len_trim(key)) :: c_key
  character(kind=c_char, len=len_trim(value)) :: c_value
  integer(kind=c_size_t) :: c_key_length, c_value_length

  c_key = trim(key)
  c_key_length = len_trim(key)
  c_value = trim(value)
  c_value_length = len_trim(value)

  code = override_string_option_c( &
    self%configoptions_ptr, c_key, c_key_length, c_value, c_value_length)
end function override_string_option

!> Override the value of a boolean configuration option
function override_boolean_option(self, key, value) result(code)
  class(configoptions_type), intent(in) :: self      !< The configoptions
  character(len=*),          intent(in) :: key       !< The name of the configuration
                                                     !! option to override
  logical(kind=c_bool),      intent(in) :: value     !< The value to store for the option
  integer(kind=enum_kind)               :: code

  ! Local variables
  character(kind=c_char, len=len_trim(key)) :: c_key
  integer(kind=c_size_t) :: c_key_length

  c_key = trim(key)
  c_key_length = len_trim(key)

  code = override_boolean_option_c( &
    self%configoptions_ptr, c_key, c_key_length, value)
end function override_boolean_option

end module smartredis_configoptions
