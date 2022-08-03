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
from smartredis.dataset_utils import DatasetConverter 

import xarray as xr

# Create reference 1D data for 1D tests 
lon_1d = np.linspace(0,360,10)

lon_coord_1d = (
    'x',
    lon_1d,
    { 'x_coord_units':'degrees E', 'x_coord_longname':'Longitude'},
    None
) 

data_attributes_1d = {
    'units':'m/s',
    'longname':'velocity',
    'convention':'CF1.5'
}

# Generate 1D reference tensor data
data1d = np.random.random([10])
# Create 1D reference Xarray for testing 
ds_1d = xr.DataArray(
    data=data1d,
    dims='x',
    coords = (lon_coord_1d,), 
    attrs=data_attributes_1d
)
# Create 1d Dataset
ds1 = Dataset("ds-1d")


# Create reference 2D data for 2D tests 
lon_2d = np.linspace(0,360,10)
lat_2d = np.linspace(-90,90,5)
lon_coord_2d = (
    'x',
    lon_2d,
    { 'x_coord_units':'degrees E', 'x_coord_longname':'Longitude'},
    None
) 
lat_coord_2d = (
    'y',
    lat_2d,
    { 'y_coord_units':'degrees N', 'y_coord_longname':'Latitude'},
    None
)
data_attributes_2d = {
    'units':'m/s',
    'longname':'velocity',
    'convention':'CF1.5'
}


# Generate 2D reference tensor data
data2d = np.random.random([10,5])
# Create 2D reference Xarray for testing 
ds_2d = xr.DataArray(
    data=data2d,
    dims=['x','y'],
    coords = (lon_coord_2d,lat_coord_2d),
    attrs=data_attributes_2d
)
# Create 2d Dataset 
ds2 = Dataset("ds-2d")



def test_add_metadata_for_xarray_1d():
    
    # Add 1D tensor data to dataset 
    ds1.add_tensor("1ddata",data1d)
    # Add coordinate data to dataset
    ds1.add_tensor("x",lon_1d)
    ds1.add_meta_string("x_coord_units",lon_coord_1d[2]["x_coord_units"]) 
    ds1.add_meta_string("x_coord_longname",lon_coord_1d[2]["x_coord_longname"])
    # Add attribute data to dataset 
    ds1.add_meta_string("units",data_attributes_1d['units'])
    ds1.add_meta_string("longname",data_attributes_1d["longname"])
    ds1.add_meta_string("convention",data_attributes_1d["convention"])
    # Add dimension data to dataset
    ds1.add_meta_string("dim_data_x",lon_coord_1d[0])
    
    # Calling method add_metadata_for_xarray on 1D dataset 
    DatasetConverter.add_metadata_for_xarray(
        ds1, 
        data_names=["1ddata"],
        dim_names=["dim_data_x"],
        coord_names=["x"],
        attr_names=["units","longname","convention"]
    )
    
    # Calling method add_metadata_for_xarray for longitude coordinate
    DatasetConverter.add_metadata_for_xarray(
    ds1,
    data_names=["x"], 
    dim_names=["dim_data_x"],
    attr_names=["x_coord_units","x_coord_longname"] 
    )

    # Assert that the original data and data put into the dataset are the same 
    # Comparing tensor data extracted from dataset after 
    # add_metadata_for_xarray call to generated 1D data 
    assert(ds1.get_tensor("1ddata") == data1d).all()
    assert(ds1.get_meta_strings("units")[0] == data_attributes_1d['units'])
    assert(ds1.get_meta_strings("longname")[0] == data_attributes_1d["longname"])
    assert(ds1.get_meta_strings("convention")[0] == data_attributes_1d["convention"])
    assert(ds1.get_meta_strings("dim_data_x")[0] == lon_coord_1d[0])
    # Comparing tensor data and metadata for coordinate data extracted from dataset after 
    # add_metadata_for_xarray call to generated 1D coordinate data 
    assert(ds1.get_tensor("x") == lon_1d).all()
    assert(ds1.get_meta_strings("x_coord_units")[0] == lon_coord_1d[2]["x_coord_units"]) 
    assert(ds1.get_meta_strings("x_coord_longname")[0] == lon_coord_1d[2]["x_coord_longname"])
    
    

def test_add_metadata_for_xarray_2d():
    
    # Add 2D tensor data to dataset
    ds2.add_tensor("2ddata",data2d) 
    # Add coordinate data to dataset
    ds2.add_tensor("x",lon_2d)
    ds2.add_meta_string("x_coord_units",lon_coord_2d[2]["x_coord_units"])    
    ds2.add_meta_string("x_coord_longname",lon_coord_2d[2]["x_coord_longname"])
    ds2.add_tensor("y",lat_2d)
    ds2.add_meta_string("y_coord_units",lat_coord_2d[2]["y_coord_units"])
    ds2.add_meta_string("y_coord_longname",lat_coord_2d[2]["y_coord_longname"])
    # Add attribute data to dataset 
    ds2.add_meta_string("units",data_attributes_1d["units"])
    ds2.add_meta_string("longname",data_attributes_1d["longname"])
    ds2.add_meta_string("convention",data_attributes_1d["convention"])
    # Add dimension data to dataset 
    ds2.add_meta_string("dim_data_x",lon_coord_2d[0])
    ds2.add_meta_string("dim_data_y",lat_coord_2d[0])


    # Calling add_metadata_for_xarray for 2D data
    DatasetConverter.add_metadata_for_xarray(
        ds2, 
        data_names=["2ddata"],
        dim_names=["dim_data_x","dim_data_y"],
        coord_names=["x","y"], 
        attr_names=["units","longname","convention"]
    )
    
    # Calling add_metadata_for_xarray for longitude coordinate
    DatasetConverter.add_metadata_for_xarray(
    ds2,
    data_names=["x"], 
    dim_names=["dim_data_x"],
    attr_names=["x_coord_units","x_coord_longname"] 
    )
    
    # Calling add_metadata_for_xarray for latitude coordinate
    DatasetConverter.add_metadata_for_xarray(
        ds2,
        data_names=["y"], 
        dim_names=["dim_data_y"],
        attr_names=["y_coord_units","y_coord_longname"]
    )
    
    # Assert that created data and data put into the dataset are the same 
    # Comparing tensor data and metadata extracted from dataset after 
    # add_metadata_for_xarray call to generated 1D data 
    assert(ds2.get_tensor("2ddata") == data2d).all()
    assert(ds2.get_meta_strings("units")[0] == "m/s")
    assert(ds2.get_meta_strings("longname")[0] =="velocity")
    assert(ds2.get_meta_strings("convention")[0] == "CF1.5")  
    assert(ds2.get_meta_strings("dim_data_x")[0] == "x")
    assert(ds2.get_meta_strings("dim_data_y")[0] == "y")
    # Comparing tensor data and metadata for coordinate data extracted from dataset after 
    # add_metadata_for_xarray call to generated 2D coordinate data 
    assert(ds2.get_tensor("x")==lon_2d).all()
    assert(ds2.get_meta_strings("x_coord_units")[0]=="degrees E")    
    assert(ds2.get_meta_strings("x_coord_longname")[0]=="Longitude")
    assert(ds2.get_tensor("y") == lat_2d).all()
    assert(ds2.get_meta_strings("y_coord_units")[0] == "degrees N")    
    assert(ds2.get_meta_strings("y_coord_longname")[0] == "Latitude")
    

def test_transform_to_xarray_1d():
 
# Compare generated Xarray from 1D data to initial Xarray
    d1_xarray_ret = DatasetConverter.transform_to_xarray(ds1)
    d1_transformed = d1_xarray_ret["1ddata"]
    assert(ds_1d.identical(d1_transformed))
    
def test_transform_to_xarray_2d():

# Compare generated Xarray to initial Xarray 
    d2_xarray_ret = DatasetConverter.transform_to_xarray(ds2)
    d2_transformed = d2_xarray_ret["2ddata"]
    assert(ds_2d.identical(d2_transformed))
