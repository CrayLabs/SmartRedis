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

from .error import *
from functools import wraps
from .smartredisPy import RedisReplyError as PybindRedisReplyError

import xarray as xr
import numpy as np
class Dtypes:
    @staticmethod
    def tensor_from_numpy(array):
        mapping = {
            "float64": "DOUBLE",
            "float32": "FLOAT",
            "uint8": "UINT8",
            "uint16": "UINT16",
            "int8": "INT8",
            "int16": "INT16",
            "int32": "INT32",
            "int64": "INT64",
        }
        dtype = str(array.dtype)
        if dtype in mapping:
            return mapping[dtype]
        raise TypeError(f"Incompatible tensor type provided {dtype}")

    @staticmethod
    def metadata_from_numpy(array):
        mapping = {
            "float64": "DOUBLE",
            "float32": "FLOAT",
            "uint32": "UINT32",
            "uint64": "UINT64",
            "int32": "INT32",
            "int64": "INT64",
        }
        dtype = str(array.dtype)
        if dtype in mapping:
            return mapping[dtype]
        raise TypeError(f"Incompatible metadata type provided {dtype}")


def init_default(default, init_value, expected_type=None):
    """Used for setting a mutable type to a default value.

    PEP standards forbid setting a default value to a mutable type
    Use this function to get around that.
    """
    if init_value is None:
        return default
    if expected_type is not None and not isinstance(init_value, expected_type):
        raise TypeError(f"Argument was of type {type(init_value)}, not {expected_type}")
    return init_value

def exception_handler(func):
    """Route exceptions raised in processing SmartRedis API calls to our
    Python wrappers

    :param func: the API function to decorate with this wrapper
    :type func: function
    :raises RedisReplyError: if the wrapped function raised an exception
    """
    @wraps(func)
    def smartredis_api_wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        # Catch RedisReplyErrors for additional processing (convert from
        # pyerror to our error module).
        # TypeErrors and ValueErrors we pass straight through
        except PybindRedisReplyError as cpp_error:
            # query args[0] (i.e. 'self') for the class name
            method_name = args[0].__class__.__name__ + "." + func.__name__
            # Get our exception from the global symbol table.
            # The smartredis.error hierarchy exactly
            # parallels the one built via pybind to enable this
            exception_name = cpp_error.__class__.__name__
            raise globals()[exception_name](str(cpp_error), method_name) from None
    return smartredis_api_wrapper

def typecheck(arg, name, _type):
    """Validate that an argument is of a given type

    :param arg: the variable to be type tested
    :type arg: variable, expected to match _type
    :param name: the name of the variable
    :type name: str
    :param _type: the expected type for the variable
    :type _type: a Python type
    :raises TypeError exception if arg is not of type _type
    """
    if not isinstance(arg, _type):
        raise TypeError(f"Argument {name} is of type {type(arg)}, not {_type}")



class DatasetConverter:   
    
    def add_metadata_for_xarray(ds,data_names,dim_names,coord_names=None,attr_names=None):
        """Extracts metadata from a SmartRedis dataset and adds it to
        dataformat specific fieldnames 

        :param ds: a SmartRedis Dataset with metadata added to Xarray 
        specific field names 
        :type ds: SmartRedis Dataset
        :param data_names: variable data field name
        :type data_names: list or string 
        :param dim_names: dimension field names. Defaults to None.
        :type dim_names: list
        :param coord_names: coordinate field names. Defaults to None.
        :type coord_names: list, optional
        :param attr_names: attribute field names. Defaults to None.
        :type attr_names: list, optional
        """        
        
        args = ['dim_names','coord_names','attr_names'] 

        for d in data_names:  

            ds.add_meta_string("_xarray_data_names",d)
   
            
            for arg in args:

                if isinstance(eval(arg),list):
                    items = []
                    for item in eval(arg): 
                        items.append(item)

                    arg_field=",".join(items)
                    ds.add_meta_string(f"_xarray_{d}_{arg}",arg_field)
            
                else:
                    if eval(arg) != None: 
                        ds.add_meta_string(f"_xarray_{d}_{arg}",eval(arg))
                        print(f"_xarray_{d}_{arg}")
    
                    else: 
                        ds.add_meta_string(f"_xarray_{d}_{arg}","null")
    
        
     
    def transform_to_xarray(ds): 
        """ Uses dataformat specifc field names added to the 
        SmartRedis dataset in the add_metadata method to construct 
        xarray in transform method 

        :param ds: a SmartRedis dataset that has had  metadata added 
        to dataformat specificfield names 
        :type ds: SmartRedis Dataset
        
        :return: a dictionary of keys as the data field anme and the
        value as the built Xarray DataArray constructed using 
        fieldnames and appropriately formatted metadata. 
        """
        
        coord_dict= {}
        coord_final = {}
        attr_dict = {}
        
        
        iter_ = ds.get_meta_strings("_xarray_data_names")
  
        for item in ds.get_meta_strings("_xarray_data_names"):
       
            # make the coordinate dict 
            for d in ds.get_meta_strings("_xarray_data_names"):
                for k in ds.get_meta_strings(f"_xarray_{item}_coord_names")[0].split(","):
                    if d == k:
                        
                        if d in iter_:
                            iter_.remove(d)
              
                        # coordinate dimensions
                        test_dims_final = []
                        for qitem in ds.get_meta_strings(f"_xarray_{d}_dim_names")[0].split(","):
                            test_xarray_dims = ds.get_meta_strings(qitem)
   
                            test_dims_final.append(test_xarray_dims[0])
                        
                        # coordinate attributes 
                        for atitem in ds.get_meta_strings(f"_xarray_{d}_attr_names")[0].split(","):
                            
                            attrt = ds.get_meta_strings(atitem)
                            attr_dict[atitem] = attrt[0] 
                                            
                        coord_dict[d]= (test_dims_final,ds.get_tensor(d),attr_dict)
        
                        # clear the coordinate attributes dict 
                        attr_dict = {}
                
                    # dict: keys= data names, value = their coordinate dictionaries 
                        coord_final[item] = coord_dict
                        
        # rest of the data items (not coordinates)                
        ret_xarray = {}

        for item in iter_:

            _xarray_data = ds.get_tensor(item)
            data_final = _xarray_data

            
            dims_final=[]
            for ditem in ds.get_meta_strings(f"_xarray_{item}_dim_names")[0].split(","):
                _xarray_dims = ds.get_meta_strings(ditem)
                dims_final.append(_xarray_dims[0])
            

            attrs_final = {}
            for aitem in ds.get_meta_strings(f"_xarray_{item}_attr_names")[0].split(","):
                _xarray_attrs = ds.get_meta_strings(aitem)
        
                attrs_final[aitem] = _xarray_attrs[0]     
            

            # add coords to the correct data array 
            for name in coord_final.keys():
                if name == item: 
                    coords_final = coord_final.get(name)

            _xarray = xr.DataArray(data=data_final,coords=coords_final,dims=dims_final,attrs=attrs_final)
            
            ret_xarray[item] = _xarray 
            
        return ret_xarray 