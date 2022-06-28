# BSD 2-Clause License
#
# Copyright (c) 2021-2022, Hewlett Packard Enterprise
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import numpy as np
import pandas as pd
import xarray as xr
from smartredis import Dataset
import datetime

def test_dataset_conversion():
    """
    Testing creating xarray dataset, converting 
    to SmartRedis dataset and then back into a xarray dataset
    
    get an xarray
    call import to make a dataset out of it
    call export to make an xarray out of the dataset
    compare the two xarrays
    """

# Create a mock xarray dataarrays
## EX 1 
    np.random.seed(0)
    temperature = 15 + 8 * np.random.randn(2, 2, 3)
    lon = [[-99.83, -99.32], [-99.79, -99.23]]
    lat = [[42.25, 42.21], [42.63, 42.59]]
    time = pd.date_range("2014-09-06", periods=3)
    reference_time = pd.Timestamp("2014-09-05")


    da_1 = xr.DataArray(
        data=temperature,
        dims=["x", "y", "time"],
        coords=dict(
            lon=(["x", "y"], lon),
            lat=(["x", "y"], lat),
            time=time,
            reference_time=reference_time,
        ),
        attrs=dict(
            description="Ambient temperature.",
            units="degC",
        )
    )
    
## EX 2 
    da_2 = xr.DataArray(np.random.randn(2, 3), coords={'x': ['a', 'b']}, dims=('x', 'y'))
      
## EX 3
    
    da_3 = {"dims": "t", "data": [1, 2, 3]}
    da_3 = xr.DataArray.from_dict(da_3)
        
## EX 4 
    da_4 = {
           "coords": {
                "t": {"dims": "t", "data": [0, 1, 2], "attrs": {"units": "s"}}
            },
          "attrs": {"title": "air temperature"},
          "dims": "t",
          "data": [10, 20, 30],
           "name": "a",
    }
    da_4 = xr.DataArray.from_dict(da_4)
    
## Test with global attributes? 
  
# Create Dataset 
    d = Dataset("a_dataset")
    d.import_from_xarray(da_1)
   
# Pass to reconstruction 
    ret_xarray = d.export_to_xarray()
  
    print(ret_xarray.equals(da_1))
    assert ret_xarray.equals(da_1)