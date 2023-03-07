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

program main
  use smartredis_configoptions, only : configoptions_type
  use test_utils,  only : setenv, unsetenv
  use iso_fortran_env, only : STDERR => error_unit
  use iso_c_binding, only : c_ptr, c_bool, c_null_ptr, c_char, c_int
  use iso_c_binding, only : c_int8_t, c_int16_t, c_int32_t, c_int64_t, c_float, c_double, c_size_t

  implicit none

#include "enum_fortran.inc"

  type(configoptions_type)      :: co
  integer                       :: result
  integer(kind=8)               :: ivalue, idefault, iresult ! int
  logical(kind=c_bool)          :: bvalue, bdefault, bresult ! bool
  character(kind=c_char, len=:), allocatable :: svalue, sdefault, sresult ! string

  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !! Establish test keys
  ! non-prefixed testing keys
  call setenv("test_integer_key", "42")
  call setenv("test_string_key", "charizard")
  call setenv("test_boolean_key", "False")
  call setenv("test_boolean_key_f0", "false")
  call setenv("test_boolean_key_f1", "FALSE")
  call setenv("test_boolean_key_f2", "0")
  call setenv("test_boolean_key_f3", "fAlse")
  call setenv("test_boolean_key_t0", "pikachu")
  call setenv("test_boolean_key_t1", "1")
  call setenv("test_boolean_key_t2", "fail")
  ! prefixed testing keys
  call setenv("prefixtest_integer_key", "42")
  call setenv("prefixtest_string_key", "charizard")
  call setenv("prefixtest_boolean_key", "False")
  call setenv("prefixtest_boolean_key_f0", "false")
  call setenv("prefixtest_boolean_key_f1", "FALSE")
  call setenv("prefixtest_boolean_key_f2", "0")
  call setenv("prefixtest_boolean_key_f3", "fAlse")
  call setenv("prefixtest_boolean_key_t0", "pikachu")
  call setenv("prefixtest_boolean_key_t1", "1")
  call setenv("prefixtest_boolean_key_t2", "fail")

  ! Check unimplemented bits
  write(*,*) "ConfigOption testing: unimplemented bits"
  result = co%create_configoptions_from_file("some_file.json")
  if (result .eq. SRNoError) error stop
  result = co%create_configoptions_from_string('{ "key" = "value" }')
  if (result .eq. SRNoError) error stop


  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !! non-prefixed option testing
  result = co%create_configoptions_from_environment("");
  if (result .ne. SRNoError) error stop

  ! integer option tests
  write(*,*) "ConfigOption testing: integer option tests"
  idefault = 0
  result = co%get_integer_option("test_integer_key", idefault, iresult)
  if (result .ne. SRNoError) error stop
  if (iresult .ne. 42) error stop

  result = co%is_defined( &
    "test_integer_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .eqv. .true.) error stop

  result = co%get_integer_option( &
    "test_integer_key_that_is_not_really_present", idefault, iresult)
  if (result .ne. SRNoError) error stop
  if (iresult .ne. 0) error stop

  result = co%is_defined("test_integer_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  ivalue = 42
  result = co%override_integer_option( &
    "test_integer_key_that_is_not_really_present", ivalue)
  if (result .ne. SRNoError) error stop

  result = co%get_integer_option( &
    "test_integer_key_that_is_not_really_present", idefault, iresult)
  if (result .ne. SRNoError) error stop
  if (iresult .ne. 42) error stop


  ! string option tests
  write(*,*) "ConfigOption testing: string option tests"
  sdefault = "missing"
  result = co%get_string_option("test_string_key", sdefault, sresult)
  if (result .ne. SRNoError) error stop
  if (sresult .ne. "charizard") error stop

  result = co%is_defined("test_string_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .eqv. .true.) error stop

  result = co%get_string_option( &
    "test_string_key_that_is_not_really_present", sdefault, sresult)
  if (result .ne. SRNoError) error stop
  if (sresult .ne. "missing") error stop

  svalue = "meowth"
  result = co%override_string_option( &
    "test_string_key_that_is_not_really_present", svalue)
  if (result .ne. SRNoError) error stop

  result = co%get_string_option( &
    "test_string_key_that_is_not_really_present", sdefault, sresult)
  if (result .ne. SRNoError) error stop
  if (sresult .ne. "meowth") error stop

  result = co%is_defined("test_string_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop


  ! boolean option tests
  write(*,*) "ConfigOption testing: boolean option tests"
  bdefault = .true.
  result = co%get_boolean_option("test_boolean_key", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop

  result = co%is_defined("test_boolean_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop

  result = co%get_boolean_option( &
      "test_boolean_key_that_is_not_really_present", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  bvalue = .true.
  result = co%override_boolean_option( &
      "test_boolean_key_that_is_not_really_present", bvalue)
  if (result .ne. SRNoError) error stop

  result = co%get_boolean_option( &
      "test_boolean_key_that_is_not_really_present", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  result = co%is_defined( &
    "test_boolean_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  result = co%get_boolean_option("test_boolean_key_f0", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop
  result = co%get_boolean_option("test_boolean_key_f1", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop
  result = co%get_boolean_option("test_boolean_key_f2", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop
  result = co%get_boolean_option("test_boolean_key_f3", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop

  result = co%get_boolean_option("test_boolean_key_t0", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop
  result = co%get_boolean_option("test_boolean_key_t1", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop
  result = co%get_boolean_option("test_boolean_key_t2", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop


  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  ! Prefixed testing
  result = co%create_configoptions_from_environment("prefixtest");
  if (result .ne. SRNoError) error stop

  ! integer option tests
  write(*,*) "ConfigOption testing: prefixed integer option tests"

  idefault = 0
  result = co%get_integer_option("integer_key", idefault, iresult)
  if (result .ne. SRNoError) error stop
  if (iresult .ne. 42) error stop

  result = co%is_defined( &
    "integer_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .eqv. .true.) error stop

  result = co%get_integer_option( &
    "integer_key_that_is_not_really_present", idefault, iresult)
  if (result .ne. SRNoError) error stop
  if (iresult .ne. 0) error stop

  result = co%is_defined("integer_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  ivalue = 42
  result = co%override_integer_option( &
    "integer_key_that_is_not_really_present", ivalue)
  if (result .ne. SRNoError) error stop

  result = co%get_integer_option( &
    "integer_key_that_is_not_really_present", idefault, iresult)
  if (result .ne. SRNoError) error stop
  if (iresult .ne. 42) error stop


  ! string option tests
  write(*,*) "ConfigOption testing: prefixed string option tests"
  sdefault = "missing"
  result = co%get_string_option("string_key", sdefault, sresult)
  if (result .ne. SRNoError) error stop
  if (sresult .ne. "charizard") error stop

  result = co%is_defined("string_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .eqv. .true.) error stop

  result = co%get_string_option( &
    "string_key_that_is_not_really_present", sdefault, sresult)
  if (result .ne. SRNoError) error stop
  if (sresult .ne. "missing") error stop

  svalue = "meowth"
  result = co%override_string_option( &
    "string_key_that_is_not_really_present", svalue)
  if (result .ne. SRNoError) error stop

  result = co%get_string_option( &
    "string_key_that_is_not_really_present", sdefault, sresult)
  if (result .ne. SRNoError) error stop
  if (sresult .ne. "meowth") error stop

  result = co%is_defined("string_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop


  ! boolean option tests
  write(*,*) "ConfigOption testing: prefixed boolean option tests"
  bdefault = .true.
  result = co%get_boolean_option("boolean_key", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop

  result = co%is_defined("boolean_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop

  result = co%get_boolean_option( &
      "boolean_key_that_is_not_really_present", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  bvalue = .true.
  result = co%override_boolean_option( &
      "boolean_key_that_is_not_really_present", bvalue)
  if (result .ne. SRNoError) error stop

  result = co%get_boolean_option( &
      "boolean_key_that_is_not_really_present", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  result = co%is_defined( &
    "boolean_key_that_is_not_really_present", bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop

  result = co%get_boolean_option("boolean_key_f0", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop
  result = co%get_boolean_option("boolean_key_f1", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop
  result = co%get_boolean_option("boolean_key_f2", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop
  result = co%get_boolean_option("boolean_key_f3", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .false.) error stop

  result = co%get_boolean_option("boolean_key_t0", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop
  result = co%get_boolean_option("boolean_key_t1", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop
  result = co%get_boolean_option("boolean_key_t2", bdefault, bresult)
  if (result .ne. SRNoError) error stop
  if (bresult .neqv. .true.) error stop


  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !! Clean up test keys
  ! non-prefixed testing keys
  call unsetenv("test_integer_key")
  call unsetenv("test_string_key")
  call unsetenv("test_boolean_key")
  call unsetenv("test_boolean_key_f0")
  call unsetenv("test_boolean_key_f1")
  call unsetenv("test_boolean_key_f2")
  call unsetenv("test_boolean_key_f3")
  call unsetenv("test_boolean_key_t0")
  call unsetenv("test_boolean_key_t1")
  call unsetenv("test_boolean_key_t2")
  ! prefixed testing keys
  call unsetenv("prefixtest_integer_key")
  call unsetenv("prefixtest_string_key")
  call unsetenv("prefixtest_boolean_key")
  call unsetenv("prefixtest_boolean_key_f0")
  call unsetenv("prefixtest_boolean_key_f1")
  call unsetenv("prefixtest_boolean_key_f2")
  call unsetenv("prefixtest_boolean_key_f3")
  call unsetenv("prefixtest_boolean_key_t0")
  call unsetenv("prefixtest_boolean_key_t1")
  call unsetenv("prefixtest_boolean_key_t2")

  ! Done
  write(*,*) "ConfigOption testing: passed"
end program main
