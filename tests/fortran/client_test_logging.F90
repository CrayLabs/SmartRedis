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

program main
  use smartredis_client, only : client_type
  use smartredis_logger, only : log_data, log_warning, log_error
  use test_utils, only : use_cluster
  use iso_fortran_env, only : STDERR => error_unit
  use iso_c_binding, only : c_ptr, c_bool, c_null_ptr, c_char, c_int
  use iso_c_binding, only : c_int8_t, c_int16_t, c_int32_t, c_int64_t, c_float, c_double, c_size_t

  implicit none

#include "enum_fortran.inc"

  type(client_type) :: client
  integer :: result

  result = client%initialize(use_cluster(), "client_test_logging")
  if (result .ne. SRNoError) error stop

  call log_data(LLQuiet, "This is data logged at the Quiet level")
  call log_data(LLInfo, "This is data logged at the Info level")
  call log_data(LLDebug, "This is data logged at the Debug level")
  call log_data(LLDeveloper, &
    "This is data logged at the Developer level")

  call log_warning(LLQuiet, "This is a warning logged at the Quiet level")
  call log_warning(LLInfo, "This is a warning logged at the Info level")
  call log_warning(LLDebug, "This is a warning logged at the Debug level")
  call log_warning(LLDeveloper, &
    "This is a warning logged at the Developer level")
  write(*,*) "client logging: passed"

  call log_error(LLQuiet, "This is an error logged at the Quiet level")
  call log_error(LLInfo, "This is an error logged at the Info level")
  call log_error(LLDebug, "This is an error logged at the Debug level")
  call log_error(LLDeveloper, &
    "This is an error logged at the Developer level")
end program main
