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

from .dataset import Dataset
from .util import Dtypes, exception_handler, typecheck
import xarray as xr

from itertools import permutations
from .error import *


class DatasetConverter:   

    @staticmethod
    def add_metadata_for_xarray(dataset, data_names, dim_names, coord_names=None, attr_names=None):
        """Extract metadata from a SmartRedis dataset and add it to 
            dataformat specific fieldnames 

            :param dataset: a Dataset instance
            :type dataset: Dataset
            :param data_names: variable data field name
            :type data_names: list[str] or str 
            :param dim_names: dimension field names.
            :type dim_names: list[str]
            :param coord_names: coordinate field names. Defaults to None.
            :type coord_names: list[str], optional
            :param attr_names: attribute field names. Defaults to None.
            :type attr_names: list[str], optional
        """        
        
        typecheck(dataset, "dataset", Dataset)
        typecheck(data_names, "data_names", list)
        for data_name in data_names:
            typecheck(data_name, "data_name", str)
        typecheck(dim_names, "dim_names", list)
        for dim_name in dim_names:
            typecheck(dim_name, "dim_name", str)
        if coord_names:
            typecheck(coord_names, "coord_names", list)
        if attr_names: 
            typecheck(attr_names, "attr_names", list)        
        
        args = ['dim_names','coord_names','attr_names'] 

        for d in data_names:  
            dataset.add_meta_string("_xarray_data_names",d)
            for arg in args:
                if isinstance(eval(arg),list):
                    values = []
                    for val in eval(arg): 
                        values.append(val)
                    arg_field=",".join(values)
                    dataset.add_meta_string(f"_xarray_{d}_{arg}",arg_field)
                else:
                    if eval(arg) != None: 
                        dataset.add_meta_string(f"_xarray_{d}_{arg}",eval(arg))
                        print(f"_xarray_{d}_{arg}")
                    else: 
                        dataset.add_meta_string(f"_xarray_{d}_{arg}","null")

    @staticmethod
    def transform_to_xarray(dataset): 
        """Transform a SmartRedis Dataset, with the appropriate metadata,
        to an Xarray Dataarray 

        :param dataset: a Dataset instance
        :type dataset: Dataset
        
        :return: a dictionary of keys as the data field name and the
        value as the built Xarray DataArray constructed using 
        fieldnames and appropriately formatted metadata. 
        rtype: dict
        """
        typecheck(dataset, "dataset", Dataset)
        
        coord_dict= {}
        coord_final = {}
        variable_names = dataset.get_meta_strings("_xarray_data_names")
             
        for j,d in list(permutations(variable_names,2)):
            for k in dataset.get_meta_strings(f"_xarray_{j}_coord_names")[0].split(","):
                if d == k:
                    # Remove coordinate data names from data names - may refactor this to make more clear
                    variable_names.remove(d)
                    # Get coordinate dimensions in the appropriate format for Xarray 
                    coord_dims = []
                    for coord_dim_field_name in dataset.get_meta_strings(f"_xarray_{d}_dim_names")[0].split(","):
                        coord_dims.append(dataset.get_meta_strings(coord_dim_field_name)[0])
                    # Get coordinate attributes in the appropriate format for Xarray 
                    coord_attrs = {}
                    for coord_attr_field_name in dataset.get_meta_strings(f"_xarray_{d}_attr_names")[0].split(","):
                        coord_attrs[coord_attr_field_name] = dataset.get_meta_strings(coord_attr_field_name)[0] 
                    # Add dimensions, data, and attributes to the coordinate variable       
                    coord_dict[d]= (coord_dims,dataset.get_tensor(d),coord_attrs)
                    # Add coordinate names and relative values in the appropriate form to add to Xarray coords variable
                    coord_final[j] = coord_dict        
        
        ret_xarray = {}
        # Loop thorugh the rest of the data variable and construct xr.DataArrays    
        for variable_name in variable_names: # for variable_name in variable_names
            data_final = dataset.get_tensor(variable_name) 
            dims_final=[]
            # Extract dimensions in correct form
            for dim_field_name in dataset.get_meta_strings(f"_xarray_{variable_name}_dim_names")[0].split(","):
                dims_final.append(dataset.get_meta_strings(dim_field_name)[0])
            attrs_final = {}
            #Extract attributes in correct form 
            for attr_field_name in dataset.get_meta_strings(f"_xarray_{variable_name}_attr_names")[0].split(","):
                attrs_final[attr_field_name] = dataset.get_meta_strings(attr_field_name)[0]     
            # Add coordinates to the correct data name
            for name in coord_final.keys():
                if name == variable_name: 
                    coords_final = coord_final.get(name)
            
            # Construct a xr.DataArray using extracted dataset data, append the dataarray to corresponding variable names
            ret_xarray[variable_name] = xr.DataArray(data=data_final,coords=coords_final,dims=dims_final,attrs=attrs_final)
            
        return ret_xarray 