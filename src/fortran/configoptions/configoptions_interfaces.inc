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
    db_prefix, db_prefix_len, configoptions) &
    bind(c, name="create_configoptions_from_environment")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_environment_c
    character(kind=c_char)        :: db_prefix     !< Prefix to add to environment vars
    integer(kind=c_size_t), value :: db_prefix_len !< How many characters in db_prefix
    type(c_ptr)                   :: configoptions !< Receives the constructed configoptions
  end function create_configoptions_from_environment_c

  function create_configoptions_from_file_c( &
    filename, filename_len, configoptions) &
    bind(c, name="create_configoptions_from_file")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_file_c
    character(kind=c_char)        :: filename      !< File containing JSON data
    integer(kind=c_size_t), value :: filename_len  !< How many characters in filename
    type(c_ptr)                   :: configoptions !< Receives the constructed configoptions
  end function create_configoptions_from_file_c

  function create_configoptions_from_string_c( &
    json_blob, json_blob_len, configoptions) &
    bind(c, name="create_configoptions_from_string")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_string_c
    character(kind=c_char)        :: json_blob     !< String containing JSON data
    integer(kind=c_size_t), value :: json_blob_len !< How many characters in json_blob
    type(c_ptr)                   :: configoptions !< Receives the constructed configoptions
  end function create_configoptions_from_string_c

  function create_configoptions_from_default_c(configoptions) &
    bind(c, name="create_configoptions_from_default")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: create_configoptions_from_default_c
    type(c_ptr)                   :: configoptions      !< Receives the constructed configoptions
  end function create_configoptions_from_default_c

  function set_default_from_environment_c(db_prefix, db_prefix_len) &
    bind(c, name="set_default_from_environment")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: set_default_from_environment_c
    character(kind=c_char)        :: db_prefix(*)  !< Prefix to be prepended to environment
                                                   !! variables in the form {db_prefix}_
                                                   !! {environment variable}. If the prefix
                                                   !! is an empty string, no prepending is done
    integer(kind=c_size_t), value :: db_prefix_len !< The length of the db_prefix string
  end function set_default_from_environment_c

  function set_default_from_file_c(filename, filename_len) &
    bind(c, name="set_default_from_file")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: set_default_from_file_c
    character(kind=c_char)        :: filename(*)   !< A UTF-8 file with JSON data containing the
                                                   !! configuration data
    integer(kind=c_size_t), value :: filename_len  !< The length of the filename string
  end function set_default_from_file_c

  function set_default_from_string_c(json_blob, json_blob_len) &
    bind(c, name="set_default_from_string")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    integer(kind=enum_kind)       :: set_default_from_string_c
    character(kind=c_char)        :: json_blob(*)      !< A JSON blob containing the
                                                       !! configuration data
    integer(kind=c_size_t), value :: json_blob_len  !< The length of the json_blob string
  end function set_default_from_string_c

  function get_integer_option_c(cfgopts, key, key_len, default_value, result) &
    bind(c, name="get_integer_option")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    type(c_ptr)                   :: cfgopts
    integer(kind=enum_kind)       :: get_integer_option_c
    character(kind=c_char)        :: key(*)            !< The name of the configuration
                                                       !! option to retrieve
    integer(kind=c_size_t), value :: key_len           !< The length of the key string
    integer(kind=8), value        :: default_value     !< The baseline value of the
                                                       !! configuration option to be returned
                                                       !! if a value is not set
    integer(kind=8)               :: result            !< Receives the integer result
  end function get_integer_option_c

  function get_string_option_c(cfgopts, key, key_len, default_value, &
    default_value_len, result, result_len) &
    bind(c, name="get_string_option")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    type(c_ptr)                   :: cfgopts
    integer(kind=enum_kind)       :: get_string_option_c
    character(kind=c_char)        :: key(*)            !< The name of the configuration
                                                       !! option to retrieve
    integer(kind=c_size_t), value :: key_len           !< The length of the key string
    character(kind=c_char)        :: default_value(*)  !< The baseline value of the
                                                       !! configuration option to be returned
                                                       !! if a value is not set
    integer(kind=c_size_t), value :: default_value_len !< The length of the key string
    character(kind=c_char)        :: result(*)         !< Receives the string result
    integer(kind=c_size_t)        :: result_len        !< The length of the result string
  end function get_string_option_c

  function get_boolean_option_c(cfgopts, key, key_len, default_value, result) &
    bind(c, name="get_boolean_option")
    use iso_c_binding, only : c_ptr, c_char, c_size_t, c_bool
    import :: enum_kind
    type(c_ptr)                   :: cfgopts
    integer(kind=enum_kind)       :: get_boolean_option_c
    character(kind=c_char)        :: key(*)            !< The name of the configuration
                                                       !! option to retrieve
    integer(kind=c_size_t), value :: key_len           !< The length of the key string
    logical(kind=c_bool), value   :: default_value     !< The baseline value of the
                                                       !! configuration option to be returned
                                                       !! if a value is not set
    logical(kind=c_bool)          :: result            !< Receives the boolean result
  end function get_boolean_option_c

  function is_defined_c(cfgopts, key, key_len, result) &
    bind(c, name="is_defined")
    use iso_c_binding, only : c_ptr, c_char, c_size_t, c_bool
    import :: enum_kind
    type(c_ptr)                   :: cfgopts
    integer(kind=enum_kind)       :: is_defined_c
    character(kind=c_char)        :: key(*)            !< The name of the configuration
                                                       !! option to check
    integer(kind=c_size_t), value :: key_len           !< The length of the key string
    logical(kind=c_bool)          :: result            !< Receives the boolean result
  end function is_defined_c

  function override_integer_option_c(cfgopts, key, key_len, value) &
    bind(c, name="override_integer_option")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    type(c_ptr)                   :: cfgopts
    integer(kind=enum_kind)       :: override_integer_option_c
    character(kind=c_char)        :: key(*)            !< The name of the configuration
                                                       !! option to override
    integer(kind=c_size_t), value :: key_len           !< The length of the key string
    integer(kind=8), value        :: value             !< The value to store for the option
  end function override_integer_option_c

  function override_string_option_c(cfgopts, key, key_len, value, value_len) &
    bind(c, name="override_string_option")
    use iso_c_binding, only : c_ptr, c_char, c_size_t
    import :: enum_kind
    type(c_ptr)                   :: cfgopts
    integer(kind=enum_kind)       :: override_string_option_c
    character(kind=c_char)        :: key(*)            !< The name of the configuration
                                                       !! option to override
    integer(kind=c_size_t), value :: key_len           !< The length of the key string
    character(kind=c_char)        :: value(*)          !< The value to store for the option
    integer(kind=c_size_t), value :: value_len         !< The length of the value string
  end function override_string_option_c

  function override_boolean_option_c(cfgopts, key, key_len, value) &
    bind(c, name="override_boolean_option")
    use iso_c_binding, only : c_ptr, c_char, c_size_t, c_bool
    import :: enum_kind
    type(c_ptr)                   :: cfgopts
    integer(kind=enum_kind)       :: override_boolean_option_c
    character(kind=c_char)        :: key(*)            !< The name of the configuration
                                                       !! option to override
    integer(kind=c_size_t), value :: key_len           !< The length of the key string
    logical(kind=c_bool), value   :: value             !< The value to store for the option
  end function override_boolean_option_c

end interface
