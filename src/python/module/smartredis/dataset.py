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

from numbers import Number

import numpy as np

from .smartredisPy import PyDataset
from .util import Dtypes, exception_handler, typecheck

from .error import *

#from .dataset_conversion import Xarray
import datetime
import xarray as xr
import ast

class Dataset:
    def __init__(self, name):
        """Initialize a Dataset object

        :param name: name of dataset
        :type name: str
        """
        typecheck(name, "name", str)
        self._data = PyDataset(name)
        
        #self.xarray = Xarray() 

    @staticmethod
    def from_pybind(dataset):
        """Initialize a Dataset object from
        a PyDataset object

        :param dataset: The pybind PyDataset object
                        to use for construction
        :type dataset: PyDataset
        :return: The newly constructor Dataset from
                 the PyDataset
        :rtype: Dataset
        """
        typecheck(dataset, "dataset", PyDataset)
        new_dataset = Dataset(dataset.get_name())
        new_dataset.set_data(dataset)
        return new_dataset

    @exception_handler
    def get_data(self):
        """Return the PyDataset attribute

        :return: The PyDataset attribute containing
                 the dataset information
        :rtype: PyDataset
        """
        return self._data

    @exception_handler
    def set_data(self, dataset):
        """Set the PyDataset attribute

        :param dataset: The PyDataset object
        :type dataset: PyDataset
        """
        typecheck(dataset, "dataset", PyDataset)
        self._data = dataset

    @exception_handler
    def add_tensor(self, name, data):
        """Add a named tensor to this dataset

        :param name: tensor name
        :type name: str
        :param data: tensor data
        :type data: np.array
        """
        typecheck(name, "name", str)
        typecheck(data, "data", np.ndarray)
        dtype = Dtypes.tensor_from_numpy(data)
        self._data.add_tensor(name, data, dtype)

    @exception_handler
    def get_tensor(self, name):
        """Get a tensor from the Dataset

        :param name: name of the tensor to get
        :type name: str
        :return: a numpy array of tensor data
        :rtype: np.array
        """
        typecheck(name, "name", str)
        return self._data.get_tensor(name)

    @exception_handler
    def add_meta_scalar(self, name, data):
        """Add metadata scalar field (non-string) with value to the DataSet

            If the field does not exist, it will be created.
            If the field exists, the value
            will be appended to existing field.

        :param name: The name used to reference the metadata
                     field
        :type name: str
        :param data: a scalar
        :type data: int | float
        """
        typecheck(name, "name", str)

        # We want to support numpy datatypes and avoid pybind ones
        data_as_array = np.asarray(data)
        if data_as_array.size > 1:
            raise TypeError("Argument provided is not a scalar")
        # We keep dtype, in case data has a non-standard python format
        dtype = Dtypes.metadata_from_numpy(data_as_array)
        self._data.add_meta_scalar(name, data_as_array, dtype)

    @exception_handler
    def add_meta_string(self, name, data):
        """Add metadata string field with value to the DataSet

        If the field does not exist, it will be created
        If the field exists the value will
        be appended to existing field.

        :param name: The name used to reference the metadata
                     field
        :type name: str
        :param data: The string to add to the field
        :type data: str
        """
        typecheck(name, "name", str)
        typecheck(data, "data", str)
        self._data.add_meta_string(name, data)

    @exception_handler
    def get_meta_scalars(self, name):
        """Get the metadata scalar field values from the DataSet

        :param name: The name used to reference the metadata
                     field in the DataSet
        :type name: str
        """
        typecheck(name, "name", str)
        return self._data.get_meta_scalars(name)

    @exception_handler
    def get_meta_strings(self, name):
        """Get the metadata scalar field values from the DataSet

        :param name: The name used to reference the metadata
                        field in the DataSet
        :type name: str
        """
        typecheck(name, "name", str)
        return self._data.get_meta_strings(name)


    def import_from_xarray(self,xr):
        """Get xarray DataArray and convert into a SmartRedis Dataset

        :param xr: The xarray DataArray to Convert 
        
        :type xr: xarray DataArray
        """
        
# Convert xr to a dict  
        dict = xr.to_dict()   
              
# Convert attribute list to string 
        attr_list = []   
        for k, v in dict["attrs"].items():                                  
            attr_list.extend([k,v])            
        attr_string = ",".join(attr_list)
        
        ## case to include :: no attributes ! 
        
# Extract data variable array
# Convert data from type list to type numpy.ndarray
        data = np.array(dict["data"])   
      
# Convert dimensions list to string
        dims_string = ",".join(dict["dims"])
               
# Convert coordinate names into string  
        coord_list = []  
        for q in dict["coords"]:
            coord_list.append(q)
        coord_string = ",".join(coord_list)
                                                       
# Convert coordinate data into string 
        coord_data = []
        for item in coord_list:
            coord_to_str = str(dict['coords'][f'{item}'])
            coord_data.append(coord_to_str)

        coord_data = ",".join(coord_data)  # do this to turn it into a string 
 
 # Add to dataset   
        self.add_tensor("data",data)
        self.add_meta_string("attributes",attr_string) 
        self.add_meta_string("coordinates",coord_string)  
        self.add_meta_string("coordinate_data",coord_data)    
        self.add_meta_string("dimensions", dims_string) 

    

    def export_to_xarray(self):  #-> xr: 
        """Get SmartRedis DataSet and convert into an xarray DataArray
        
        Assumptions: meta_string attributes, coordinates, coordinate_data
        and dimensions exist in the correct form. 
        
        Returns: xarray DataArray
        """

# Retrieve the dataset
        data_ = self.get_tensor("data")
        attr_string = self.get_meta_strings("attributes")
        coord_string = self.get_meta_strings("coordinates")
        coord_data_string = self.get_meta_strings("coordinate_data")
        dims_string = self.get_meta_strings("dimensions")    
       
# Convert attr string into list 
        attr_string = attr_string[0].split(",")    
      
# Convert from attribute list to dictionary
        attrs = {attr_string[i]: attr_string[i + 1] for i in range(0, len(attr_string), 2)}
                    
# Convert dims string to list 
        dims_list = dims_string[0].split(",")
        
# Convert coords name string into list 
        coord_list = coord_string[0].split(",")    
        
 #convert coord string into dict then traverse the dict into a form that we can input into xarray  
        coords_ = eval(coord_data_string[0]) 
      
        listn = []
        for n in coords_: 
            listn.append(n)
        test = {k: {} for k in coord_list}
        k=0
        for i in test:
            test[i] = listn[k] 
            k += 1
             
   #from dict to something accessible by xarray    {coords = {} }  coord = {} 
        coords = {
            k: (v["dims"], v["data"], v.get("attrs"))
                for k, v in test.items()
            }

     # The reconstruction
        xr_ = xr.DataArray(data = data_, 
                           dims = dims_list,
                           attrs = attrs,
                           coords = coords)
        return xr_