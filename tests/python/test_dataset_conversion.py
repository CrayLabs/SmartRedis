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

import xarray as xr
import numpy as np
import pytest
from smartredis import Dataset,Client
from smartredis.dataset_utils import DatasetConverter 
from smartredis.error import *


# ----------Create reference 1D data for 1D tests ---------
# Generate coordinate data
longitude_1d = np.linspace(0,360,10)
# Coordinate tuples 
longitude_coord_1d = (
    'x',
    longitude_1d,
    { 'x_coord_units':'degrees E', 'x_coord_longname':'Longitude'},
    None
) 
# Data attributes dict 
data_attributes_1d = {
    'units':'m/s',
    'longname':'velocity',
    'convention':'CF1.5'
}
# Generate 1D reference tensor data
data1d = np.random.random([10])
# Create 1D reference Xarray for testing 
ds_1d = xr.DataArray(
    name="1ddata",
    data=data1d,
    dims='x',
    coords = (longitude_coord_1d,), 
    attrs=data_attributes_1d
)


# ----------Create reference 2D data for 2D tests ---------
longitude_2d = np.linspace(0,360,10)
latitude_2d = np.linspace(-90,90,5)
longitude_coord_2d = (
    'x',
    longitude_2d,
    { 'x_coord_units':'degrees E', 'x_coord_longname':'Longitude'},
    None
) 
latitude_coord_2d = (
    'y',
    latitude_2d,
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
    name="2ddata",
    data=data2d,
    dims=['x','y'],
    coords = (longitude_coord_2d,latitude_coord_2d),
    attrs=data_attributes_2d
)


# ----helper methods -------

def add_1d_data(dataset):
    
    # Add 1D tensor data to dataset 
    dataset.add_tensor("1ddata",data1d)
    # Add coordinate data to dataset
    dataset.add_tensor("x",longitude_1d)
    dataset.add_meta_string("x_coord_units",longitude_coord_1d[2]["x_coord_units"]) 
    dataset.add_meta_string("x_coord_longname",longitude_coord_1d[2]["x_coord_longname"])
    # Add attribute data to dataset 
    dataset.add_meta_string("units",data_attributes_1d['units'])
    dataset.add_meta_string("longname",data_attributes_1d["longname"])
    dataset.add_meta_string("convention",data_attributes_1d["convention"])
    # Add dimension data to dataset
    dataset.add_meta_string("dim_data_x",longitude_coord_1d[0])
    
    
def add_1d_data_bad_string(dataset):
    
    # Add 1D tensor data to dataset 
    dataset.add_tensor("1ddata",data1d)
    # Add coordinate data to dataset
    dataset.add_tensor("x",longitude_1d)
    dataset.add_meta_string("x_coord_units",longitude_coord_1d[2]["x_coord_units"]) 
    dataset.add_meta_string("x_coord_longname",longitude_coord_1d[2]["x_coord_longname"])
    # Add attribute data to dataset 
    dataset.add_meta_string("units",data_attributes_1d['units'])
    dataset.add_meta_string("longname",data_attributes_1d["longname"])
    dataset.add_meta_string("convention",data_attributes_1d["convention"])
    # Add dimension data to dataset
    dataset.add_meta_string("dim_data_x",longitude_coord_1d[0])
    
def add_1d_data_bad_tensor(dataset):
    baddata = np.array([1,2,3,4,5])

    # Add 1D tensor data to dataset 
    dataset.add_tensor("1ddata",baddata)
    # Add coordinate data to dataset
    dataset.add_tensor("x",longitude_1d)
    dataset.add_meta_string("x_coord_units",longitude_coord_1d[2]["x_coord_units"]) 
    dataset.add_meta_string("x_coord_longname",longitude_coord_1d[2]["x_coord_longname"])
    # Add attribute data to dataset 
    dataset.add_meta_string("units",data_attributes_1d['units'])
    dataset.add_meta_string("longname",data_attributes_1d["longname"])
    dataset.add_meta_string("convention",data_attributes_1d["convention"])
    # Add dimension data to dataset
    dataset.add_meta_string("dim_data_x",longitude_coord_1d[0])
    
def assert_equality_1d(dataset):
    
    # Assert that the original data and data put into the dataset are the same 
    # Comparing tensor data extracted from dataset after 
    # add_metadata_for_xarray call to generated 1D data 
    assert(dataset.get_tensor("1ddata") == data1d).all()
    assert(dataset.get_meta_strings("units")[0] == data_attributes_1d['units'])
    assert(dataset.get_meta_strings("longname")[0] == data_attributes_1d["longname"])
    assert(dataset.get_meta_strings("convention")[0] == data_attributes_1d["convention"])
    assert(dataset.get_meta_strings("dim_data_x")[0] == longitude_coord_1d[0])
    # Comparing tensor data and metadata for coordinate data extracted from dataset after 
    # add_metadata_for_xarray call to generated 1D coordinate data 
    assert(dataset.get_tensor("x") == longitude_1d).all()
    assert(dataset.get_meta_strings("x_coord_units")[0] == longitude_coord_1d[2]["x_coord_units"]) 
    assert(dataset.get_meta_strings("x_coord_longname")[0] == longitude_coord_1d[2]["x_coord_longname"])
    
    
def add_2d_data(dataset):
    # Add 2D tensor data to dataset
    dataset.add_tensor("2ddata",data2d) 
    # Add coordinate data to dataset
    dataset.add_tensor("x",longitude_2d)
    dataset.add_meta_string("x_coord_units",longitude_coord_2d[2]["x_coord_units"])    
    dataset.add_meta_string("x_coord_longname",longitude_coord_2d[2]["x_coord_longname"])
    dataset.add_tensor("y",latitude_2d)
    dataset.add_meta_string("y_coord_units",latitude_coord_2d[2]["y_coord_units"])
    dataset.add_meta_string("y_coord_longname",latitude_coord_2d[2]["y_coord_longname"])
    # Add attribute data to dataset 
    dataset.add_meta_string("units",data_attributes_1d["units"])
    dataset.add_meta_string("longname",data_attributes_1d["longname"])
    dataset.add_meta_string("convention",data_attributes_1d["convention"])
    # Add dimension data to dataset 
    dataset.add_meta_string("dim_data_x",longitude_coord_2d[0])
    dataset.add_meta_string("dim_data_y",latitude_coord_2d[0])
    
    
def assert_equality_2d(dataset):
        
    # Assert that created data and data put into the dataset are the same 
    # Comparing tensor data and metadata extracted from dataset after 
    # add_metadata_for_xarray call to generated 1D data 
    assert(dataset.get_tensor("2ddata") == data2d).all()
    assert(dataset.get_meta_strings("units")[0] == "m/s")
    assert(dataset.get_meta_strings("longname")[0] =="velocity")
    assert(dataset.get_meta_strings("convention")[0] == "CF1.5")  
    assert(dataset.get_meta_strings("dim_data_x")[0] == "x")
    assert(dataset.get_meta_strings("dim_data_y")[0] == "y")
    # Comparing tensor data and metadata for coordinate data extracted from dataset after 
    # add_metadata_for_xarray call to generated 2D coordinate data 
    assert(dataset.get_tensor("x")==longitude_2d).all()
    assert(dataset.get_meta_strings("x_coord_units")[0]=="degrees E")    
    assert(dataset.get_meta_strings("x_coord_longname")[0]=="Longitude")
    assert(dataset.get_tensor("y") == latitude_2d).all()
    assert(dataset.get_meta_strings("y_coord_units")[0] == "degrees N")    
    assert(dataset.get_meta_strings("y_coord_longname")[0] == "Latitude")
    
# -------- start of tests --------------
# check  
def test_add_metadata_for_xarray_1d():
    """Test add_metadata_for_xarray method with 1d tensor
    data and metadata
    """
    ds1 = Dataset("ds-1d")
    add_1d_data(ds1)
  
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
    
    assert_equality_1d(ds1)

    
#check   
def test_string_single_variable_param_names_add_metadata_for_xarray_1d():
    # Testing single variable string parameter names 

    ds1 = Dataset("ds-1d")
    add_1d_data(ds1)

    # Calling method add_metadata_for_xarray on 1D dataset 
    DatasetConverter.add_metadata_for_xarray(
        ds1, 
        data_names="1ddata",
        dim_names="dim_data_x",
        coord_names="x",
        attr_names=["units","longname","convention"]
    )
    
    # Calling method add_metadata_for_xarray for longitude coordinate
    DatasetConverter.add_metadata_for_xarray(
    ds1,
    data_names="x", 
    dim_names="dim_data_x",
    attr_names=["x_coord_units","x_coord_longname"] 
    )
    assert_equality_1d(ds1)

    
#check   
def test_bad_type_data_names_add_metadata_for_xarray_1d():
    # Test types for bad data_name type 
    # Calling method add_metadata_for_xarray on 1D dataset 
    # instantiating and building dataset locally 
    ds1 = Dataset("ds-1d")
    add_1d_data(ds1)
    with pytest.raises(RedisKeyError):
    # which type of error you expect to be raised 
    # where and why it is not throwing the error 
        DatasetConverter.add_metadata_for_xarray(
            ds1, 
            data_names="incorrect_data_name",
            dim_names="dim_data_x",
            coord_names="x",
            attr_names=["units","longname","convention"]
        )
        #assert_equality_1d(ds1)
        assert(ds1.get_tensor("incorrect_data_name") == data1d).all()
        # not sure how to make this better 
   
# check test - if RedisRuntimeErroris sufficient  
def test_bad_type_dim_names_add_metadata_for_xarray_1d():
    
    ds1 = Dataset("ds-1d")
    add_1d_data(ds1)
    
    with pytest.raises(RedisRuntimeError):
        DatasetConverter.add_metadata_for_xarray(
            ds1, 
            data_names=["1ddata"],
            dim_names=["incorrect_dim_name"],
            coord_names=["x"],
            attr_names=["units","longname","convention"]
        )
        assert(ds1.get_meta_strings("incorrect_dim_name")[0] == longitude_coord_1d[0])

#check    
def test_bad_type_coord_names_add_metadata_for_xarray_1d():
    ds1 = Dataset("ds-1d")
    add_1d_data(ds1)
    with pytest.raises(RedisKeyError):
        DatasetConverter.add_metadata_for_xarray(
        ds1, 
        data_names=["1ddata"],
        dim_names=["dim_data_x"],
        coord_names=["incorrect_coord_name"],
        attr_names=["units","longname","convention"]
        )
        
        assert(ds1.get_tensor("incorrect_coord_name") == longitude_1d).all()

#check
def test_bad_type_attr_names_add_metadata_for_xarray_1d():
    ds1 = Dataset("ds-1d")
    add_1d_data(ds1)
    with pytest.raises(RedisRuntimeError):
        DatasetConverter.add_metadata_for_xarray(
        ds1, 
        data_names=["1ddata"],
        dim_names=["dim_data_x"],
        coord_names=["x"],
        attr_names=["incorrect_attr_name","incorrect_attr_name","incorrect_attr_name"]
        )
        assert(ds1.get_meta_strings("incorrect_attr_name")[0] == data_attributes_1d['units'])
        assert(ds1.get_meta_strings("incorrect_attr_name")[0] == data_attributes_1d["longname"])
        assert(ds1.get_meta_strings("incorrect_attr_name")[0] == data_attributes_1d["convention"])

   
   

def test_bad_tensor_add_metadata_for_xarray_1d():
    ds1 = Dataset("ds-1d")
    add_1d_data_bad_tensor(ds1) # everything good but give it bad types
    with pytest.raises(AssertionError): 
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
        assert(ds1.get_tensor("1ddata") == data1d)
        
        # Relevant error message? bad tensor data 

def test_bad_type_add_metadata_for_xarray_1d():
    #     with pytest.raises(TypeError):  # make sure you are getting type error 
    # for various versions of passing wrong param types     
    ds1 = Dataset("ds-1d")
    add_1d_data_bad_tensor(ds1) # everything good but give it bad types
    with pytest.raises(TypeError): 
    # Calling method add_metadata_for_xarray on 1D dataset 
        DatasetConverter.add_metadata_for_xarray(
            ds1, 
            data_names={"1ddata"},
            dim_names=["dim_data_x"],
            coord_names=["x"],
            attr_names=["units","longname","convention"]
        )
    

def test_empty_param_add_metadata_for_xarray_1d():
    #     with pytest.raises(TypeError):  # make sure you are getting type error 
    # for various versions of passing wrong param types     
    ds1 = Dataset("ds-1d")
    add_1d_data_bad_tensor(ds1) # everything good but give it bad types
    with pytest.raises(AssertionError): 
    # Calling method add_metadata_for_xarray on 1D dataset 
        DatasetConverter.add_metadata_for_xarray(
            ds1, 
            data_names=[],
            dim_names=["dim_data_x"],
            coord_names=["x"],
            attr_names=["units","longname","convention"]
        )
    # assert_equality_1d(ds1)
        assert(ds1.get_tensor("1ddata") == data1d)
    # Relevant error message: empty string ??
  


#------ beginning of 2d add_metadata_to_xarray_tests ----------


def test_add_metadata_for_xarray_2d():
    """Test add_metadata_for_xarray method with 2d tensor
    data and metadata
    """
    # Create 2d Dataset 
    ds2 = Dataset("ds-2d")
    # add data to Datset
    add_2d_data(ds2)

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

    assert_equality_2d(ds2)

    
#------- beginning of 1d transform_to_xarray tests-------

def test_transform_to_xarray_1d():
    """Test transform_to_xarray method with 1d
    tensor data
    """
    ds1 = Dataset("ds-1d")
    add_1d_data(ds1)
    
    # Calling method add_metadata_for_xarray on 1D dataset - good data and prerequisite for
    # transform_to_xarray 
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
    # Compare generated Xarray from 1D data to initial Xarray
    d1_xarray_ret = DatasetConverter.transform_to_xarray(ds1)
    d1_transformed = d1_xarray_ret["1ddata"]
    assert(ds_1d.identical(d1_transformed))
    
    
#------- beginning of 2d transform_to_xarray tests-------


def test_transform_to_xarray_2d():
    """Test transform_to_xarray method with21d
    tensor data
    """
     # Create 2d Dataset 
    ds2 = Dataset("ds-2d")
    # add data to Datset
    add_2d_data(ds2)

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
    # Compare generated Xarray to initial Xarray 
    d2_xarray_ret = DatasetConverter.transform_to_xarray(ds2)
    d2_transformed = d2_xarray_ret["2ddata"]
    assert(ds_2d.identical(d2_transformed))



# test data names being list of string or a string
# what if string you are pointingn to is tensor 
# what if string is not present 
# what if it;s metascalar instead of metastring 

#Just thinking of edge cases to handle gracefully, have we tested 
# (or what happens) if they pass in an empty list? I think add_meta_string is OK 
# with empty strings, but not sure about all of the xarray logic.


## This file is only checking the successful conversion paths.
# We need to test with bad parameters or when other things go wrong 
# to make sure that our code will behave predictably and reasonably in these cases


# tests that make sure that if we pass in bad types that they get caught --

# ---transform to xarray --
#set up metdata for conversion and then remove some of it, 
# then making it blow up ?
# if they didnt call add metadata first -- some intelligable error message 
# pointing to something that's not there, wrong type etc.  

