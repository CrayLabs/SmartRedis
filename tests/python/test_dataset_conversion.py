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
import numpy as np
from smartredis.util import DatasetConverter 

import xarray as xr


# Create data 

lon = np.linspace(0,360,10)
lat = np.linspace(-90,90,5)
lon_coord = (
'x',
lon,
{ 'x_coord_units':'degrees E', 'x_coord_longname':'Longitude'},
None
) 
lat_coord = (
'y',
lat,
{ 'y_coord_units':'degrees N', 'y_coord_longname':'Latitude'},
None
)
data_attributes = {
'units':'m/s',
'longname':'velocity',
'convention':'CF1.5'
}

data1d = np.random.random([10])

data2d = np.random.random([10,5])

ds_1d = xr.DataArray(
data=data1d,
dims='x',
coords = (lon_coord,), 
attrs=data_attributes
)


ds_2d = xr.DataArray(
data=data2d,
dims=['x','y'],
coords = (lon_coord,lat_coord),
attrs=data_attributes
)


# Create Datasets 

ds1 = Dataset("ds-1d")

ds2 = Dataset("ds-2d")


  
def test_add_metadata_for_xarray():
    
  
# Calling add_metadata_for_xarray method on the two dataset exmaples and the list of field names


# User adding 1D xarray data to SmartRedis dataset

    ###
     # 1d data
    ds1.add_tensor("1ddata",data1d) 
    
    # coord 
    ds1.add_tensor("x",lon)
    ds1.add_meta_string("x_coord_units","degrees E")    # this might not work actually 
    ds1.add_meta_string("x_coord_longname","Longitude")

    #data attr 
    ds1.add_meta_string("units","m/s")
    ds1.add_meta_string("longname","velocity")
    ds1.add_meta_string("convention","CF1.5")

    # dims   
    ds1.add_meta_string("dim_data_x","x")




# Calling the first method for the 1d array 
    
    DatasetConverter.add_metadata_for_xarray(
        ds1, 
        data_names=["1ddata"],
        dim_names=["dim_data_x"],
        coord_names=["x"], # this has to be the same as 
        attr_names=["units","longname","convention"]
    )
    
    # coordinate 1
    DatasetConverter.add_metadata_for_xarray(
    ds1,
    data_names=["x"], 
    dim_names=["dim_data_x"],
    attr_names=["x_coord_units","x_coord_longname"]    # cant have the same name for retrieval 
    )

    
    

# Assert that created data and data put into the dataset is the same 

    assert(ds1.get_tensor("1ddata") == data1d).all()

    # # #attr stuff 
    assert(ds1.get_meta_strings("units")[0] == "m/s")
    assert(ds1.get_meta_strings("longname")[0] == "velocity")
    assert(ds1.get_meta_strings("convention")[0] == "CF1.5")

    # # # # dims stuff 
    assert(ds1.get_meta_strings("dim_data_x")[0] == "x")
    
    # # # coord stuff - just lon
    assert(ds1.get_tensor("x") == lon).all()
  

    assert(ds1.get_meta_strings("x_coord_units")[0] == "degrees E")
    assert(ds1.get_meta_strings("x_coord_longname")[0] == "Longitude")


#-------------------------------------------------------------------------------


# User adding 2D xarray data to SmartRedis dataset


    # 2d data
    ds2.add_tensor("2ddata",data2d) 
    
    # coord 
    ds2.add_tensor("x",lon)
    ds2.add_meta_string("x_coord_units","degrees E")    # this might not work actually 
    ds2.add_meta_string("x_coord_longname","Longitude")

    ds2.add_tensor("y",lat)
    ds2.add_meta_string("y_coord_units","degrees N")    # because these are both named the same 
    ds2.add_meta_string("y_coord_longname","Latitude")

    #data attr 
    ds2.add_meta_string("units","m/s")
    ds2.add_meta_string("longname","velocity")
    ds2.add_meta_string("convention","CF1.5")

    # dims   
    ds2.add_meta_string("dim_data_x","x")
    ds2.add_meta_string("dim_data_y","y")


  

    # Calling add_metadata_for_xarray for 
    DatasetConverter.add_metadata_for_xarray(
        ds2, 
        data_names=["2ddata"],
        dim_names=["dim_data_x","dim_data_y"],
        coord_names=["x","y"], # this has to be the same as 
        attr_names=["units","longname","convention"]
    )
    
    # coordinate 1
    DatasetConverter.add_metadata_for_xarray(
    ds2,
    data_names=["x"], 
    dim_names=["dim_data_x"],
    attr_names=["x_coord_units","x_coord_longname"]    # cant have the same name for retrieval 
    )
    
    # coordinate 1
    DatasetConverter.add_metadata_for_xarray(
        ds2,
        data_names=["y"], 
        dim_names=["dim_data_y"],
        attr_names=["y_coord_units","y_coord_longname"]
    )
    
# Assert that created data and data put into the dataset is the same 
      # 2d data
    assert(ds2.get_tensor("2ddata") == data2d).all()
    
    # coord 
    assert(ds2.get_tensor("x")==lon).all()
    assert(ds2.get_meta_strings("x_coord_units")[0]=="degrees E")    # this might not work actually 
    assert(ds2.get_meta_strings("x_coord_longname")[0]=="Longitude")

    assert(ds2.get_tensor("y") == lat).all()
    assert(ds2.get_meta_strings("y_coord_units")[0] == "degrees N")    # because these are both named the same 
    assert(ds2.get_meta_strings("y_coord_longname")[0] == "Latitude")

    #data attr 
    assert(ds2.get_meta_strings("units")[0] == "m/s")
    assert(ds2.get_meta_strings("longname")[0] =="velocity")
    assert(ds2.get_meta_strings("convention")[0] == "CF1.5")

    # # dims   
    assert(ds2.get_meta_strings("dim_data_x")[0] == "x")
    assert(ds2.get_meta_strings("dim_data_y")[0] == "y")


  
def test_transform_to_xarray():
    
    
# 1D array test
    d1_xarray_ret = DatasetConverter.transform_to_xarray(ds1)

    d1_transformed = d1_xarray_ret["1ddata"]

    assert(ds_1d.equals(d1_transformed))
    
    assert(ds_1d.identical(d1_transformed))
    

# 2D array test
    d2_xarray_ret = DatasetConverter.transform_to_xarray(ds2)
   
    d2_transformed = d2_xarray_ret["2ddata"]
    
    assert(ds_2d.equals(d2_transformed))
    
    assert(ds_2d.identical(d2_transformed))
   
