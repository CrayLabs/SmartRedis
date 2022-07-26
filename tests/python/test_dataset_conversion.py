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



from smartredis import Dataset

import xarray as xr
import numpy as np
import pandas as pd

from smartredis.util import DatasetConverter 



def test_dataset_conversion():
    
    # Testing:
    # adding data and metadata to a SmartRedis dataset, 
    # calling add_metadata_for_xarray method on the dataset and the list of field names,
    # calling transform_to_xarray on the dataset,
    # returning an Xarray DataArray 
    
    # Out of Scope for now: 
    # time coordinate
    # coordinates with other fields: date, dimensions, attributes
    # named DataArrays 


  
# EX1 DATA 

    np.random.seed(0)
    temperature = 15 + 8 * np.random.randn(2, 2, 3)
    lon = np.array([[-99.83, -99.32], [-99.79, -99.23]])
    lat = np.array([[42.25, 42.21], [42.63, 42.59]])
  
    data = np.array(temperature)
    dims=["x", "y", "time"]
    #coords=dict(
    #     lon=(["x", "y"], lon),
    #     lat=(["x", "y"], lat))
         #time=time,
         #reference_time=reference_time)
   # attrs=dict(
   #     description="Ambient temperature.",
   #      units="degC")
         

    # User construction of the SmartRedis dataset
    ds = Dataset("example-1") 
  
    ds.add_tensor("data",temperature)
    ds.add_meta_string("lon_dim_name","x")
    ds.add_meta_string("lat_dim_name","y")
    ds.add_meta_string("description","Ambient temperature")
    ds.add_meta_string("units","degC")
    ds.add_meta_string("time_dim_name","time")
    
   # ds.add_tensor("lon",lon)
   # ds.add_tensor("lat",lat)
    # ds.add_meta_string("time_units","days since 2014-09-06")
    # ds.add_tensor("time",time)
    # ds.add_tensor("reference_time",reference_time)
            # coords=["lon","lat"], 
                                          # record_dimension='time_dim_name') 
                                          #"time","reference_time"]
    

    DatasetConverter.add_metadata_for_xarray(ds, data="data",
                                             dims=["lon_dim_name","lat_dim_name","time_dim_name"],attrs=["description","units"])
                                             
    an_xarray = DatasetConverter.transform_to_xarray(ds)
    
    #print(an_xarray)
    
    
    
    
# Ex 2  DATA

    #coords = {"t": {"dims": "t","data": [0, 1, 2], "attrs": {"units": "s"}} }
    attrs = {"title": "air temperature"}
    dims = "t"
    gdata = np.array([10, 20, 30])
    # name = "a"

# User construction of dataset 
    da = Dataset("example-2") 
    da.add_tensor("global-data",gdata)
    da.add_meta_string("title","air temperature")
    da.add_meta_string("dim_name","t")
    

    DatasetConverter.add_metadata_for_xarray(da, data="global-data",dims=["dim_name"],attrs=["title"])
                           
    xarray_example2 = DatasetConverter.transform_to_xarray(da)
    
   # print(xarray_example2)
    