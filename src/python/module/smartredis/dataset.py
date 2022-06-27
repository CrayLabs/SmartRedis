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
        # do the conversion we discussed here
 
        # data variables
        ## convert xr to a dict -- easier to separate vars -- already in a form that is right for reconstruction 
        ##  could do differently --> direclty accessing from xarray 
        
        # XARRAY TO DICTIONARY 
        dict = xr.to_dict()   
    
        #to_da = xr.data 
        #att= xr.attrs  
        # when adding metastring and tensor should keep track of how things are named for reconstruction  
       # print(dict) 
      #  print(dict.keys())   #(['dims', 'attrs', 'data', 'coords', 'name'])
      #  print(dict["dims"])    #('x', 'y', 'time') 
      #  print(dict["attrs"])  # {'description': 'Ambient temperature.', 'units': 'degC'}
      
      
        # ------------- ATTRIBUTES TO STRING -------------
        attr_list = []    
     #   print("",dict["attrs"].items())
        for k, v in dict["attrs"].items():                                  
            attr_list.extend([k,v])             #attr_list.append(k)      # attr_list.append(v)
        attr_string = ",".join(attr_list)
        
        #print(attr_string)  #description,Ambient temperature.,units,degC
        
        #attributes to a string 
    
    #    print(dims_string)
    #    print("extended",attr_list)
  

        data = dict["data"]   # why did I do this ?
        #print(dict["dims"])  #('x', 'y', 'time')
    
   

       #-------------DIMS TO STRING -------------
        dims_string = ",".join(dict["dims"])
   
   #     print(dims_string)  #x,y,time
               

        # -------------COORDS TO STRING -------------
        coord_list = []  
        for q in dict["coords"]:
            coord_list.append(q)
        
        coord_string = ",".join(coord_list)
    
#       print(coord_string)             # lon, lat, time, reference_time
                                         # just the comma separated list of coordinates in this dataset 

                                                       
    # ------ the rest of the coordinate object to STRING ------- # could just keep it all in a metastring for reconstruction 
        coord_data = []
        for item in coord_list:
            coord_to_str = dict['coords'][f'{item}']
            coord_to_str = str(coord_to_str)
          #  print(type(coord_to_str))
          #  print(coord_to_str)
            coord_data.append(coord_to_str)
            
       # print(coord_data)   ## just the coordinate data (minus the name of coords but with all other structure )
        #print(dict['coords'])   ## or should I just turn this whole dict into a string? 
       # print(type(coord_data))  # this is a list 
        coord_data = ",".join(coord_data)  # do this to turn it into a string 
       # print(type(coord_data))  # now it's of type string and can be put into a metastring 
       


       # why did I do this? was in the wrong form? 
       
        data = np.array(data)        # all in string format or in a numpy array 
 
        return coord_data, attr_string, data, coord_string, dims_string
    

 
    def export_to_xarray(self,dataset,attr_list,coord_string,coord_list,dims_list): # -> xr: 
        
   #     print(dataset)
   #     print("rattr_list",attr_list)
   #     print("rcoord_string",coord_string)
   #     print("rcoord_list",coord_list)
   #     print("dims_list",dims_list)
        
        
        data = dataset
        
       #from  rattr_list ['description,Ambient temperature.,units,degC']
       #to   ['description', 'Ambient temperature.', 'units', 'degC']
       
        #--- attr string to something that can be used to reconstruct xarray 
        attr_list = attr_list[0].split(",")
   
        
        #-----from attribute list to dictionary -- for constructing xarray 
        # can it just stay a list? 
        attrs = {attr_list[i]: attr_list[i + 1] for i in range(0, len(attr_list), 2)}
        
        # --- splitting dims string to list 
        dims_list = dims_list[0].split(",")
        
        # ----- splitting coords list 
        coord_string = coord_string[0].split(",")
        #{'description': 'Ambient temperature.', 'units': 'degC'}

  
        coords = eval(coord_list[0])

    # is this needed
        listn = []
        for n in coords: 
    #        print(n)
            listn.append(n)
   #     for m in coord_string:
   #         print(m)



#need these in the form coord:{lon: {'dims': ('x', 'y'), 'attrs': {}, 'data': [[-99.83, -99.32], [-99.79, -99.23]]

       #         }
       #this form --> 
        #{
        # 'lon': {'dims': ('x', 'y'), 'attrs': {}, 'data': [[-99.83, -99.32], [-99.79, -99.23]]},
        # 'lat': {'dims': ('x', 'y'), 'attrs': {}, 'data': [[42.25, 42.21], [42.63, 42.59]]}, 
        # 'time': {'dims': ('time',), 'attrs': {}, 'data': [datetime.datetime(2014, 9, 6, 0, 0), datetime.datetime(2014, 9, 7, 0, 0), datetime.datetime(2014, 9, 8, 0, 0)]},
        # 'reference_time': {'dims': (), 'attrs': {}, 'data': datetime.datetime(2014, 9, 5, 0, 0)}
        # }
        
        test = {k: {} for k in coord_string}

    
        k=0
        for i in test:
            test[i] = listn[k]
            k += 1
            

       # print(test)
 
      #  test2 = {'lon': '', 'lat': '', 'time':[1,2,3], 'reference_time': ''}
   
   #from dict to something accessible by xarray 
        coords1 = {
            k: (v["dims"], v["data"], v.get("attrs"))
                for k, v in test.items()
            }

     # the reconstruction -- might be missing 
        xr_ = xr.DataArray(data = dataset, 
                           dims = dims_list,
                           attrs = attrs,
                           coords = coords1)
        return xr_