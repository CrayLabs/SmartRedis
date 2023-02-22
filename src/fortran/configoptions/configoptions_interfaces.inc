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

interface

  function create_configoptions_from_environment_c( &
    db_prefix, db_prefix_length, configoptions) &
    bind(c, name="create_configoptions_from_environment")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_environment_c
    character(kind=c_char)        :: db_prefix          !< Prefix to add to environment vars
    integer(kind=c_size_t), value :: db_prefix_length   !< How many characters in db_prefix
    type(c_ptr)                   :: configoptions      !< Receives the constructed configoptions
  end function create_configoptions_from_environment_c

  function create_configoptions_from_file_c( &
    filename, filename_length, configoptions) &
    bind(c, name="create_configoptions_from_file")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_file_c
    character(kind=c_char)        :: filename           !< File containing JSON data
    integer(kind=c_size_t), value :: filename_length    !< How many characters in filename
    type(c_ptr)                   :: configoptions      !< Receives the constructed configoptions
  end function create_configoptions_from_file_c

  function create_configoptions_from_string_c( &
    json_blob, json_blob_length, configoptions) &
    bind(c, name="create_configoptions_from_string")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_string_c
    character(kind=c_char)        :: json_blob          !< String containing JSON data
    integer(kind=c_size_t), value :: json_blob_length   !< How many characters in json_blob
    type(c_ptr)                   :: configoptions      !< Receives the constructed configoptions
  end function create_configoptions_from_string_c

  function create_configoptions_from_default_c(configoptions) &
    bind(c, name="create_configoptions_from_default")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_default_c
    type(c_ptr)                   :: configoptions      !< Receives the constructed configoptions
  end function create_configoptions_from_default_c

end interface
