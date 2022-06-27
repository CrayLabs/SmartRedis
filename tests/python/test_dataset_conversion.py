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
    """
    
## assumptions -- reconsrtuctuon will depedn on somekind of split method being used
# if metadata includes commas will that affect how I do reconstruction 

    #Create a test xarray dataarray to put 
    np.random.seed(0)
    temperature = 15 + 8 * np.random.randn(2, 2, 3)
    lon = [[-99.83, -99.32], [-99.79, -99.23]]
    lat = [[42.25, 42.21], [42.63, 42.59]]
    time = pd.date_range("2014-09-06", periods=3)
    reference_time = pd.Timestamp("2014-09-05")


    da = xr.DataArray(
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
    
    
    #global attributes ? 
    # 
        
    #call xarray to dataset method 
    xrdata = Dataset("xarray_da")
    #print("before conversion")
    #print(da)
    
    # FROM XARRAY TO SOMETHING THAT CAN BE ADDED TO SMARTREDIS 
    coord_list, attr_list, data, coord_string, dims_string = xrdata.import_from_xarray(da)  
    
  
    #dataset = Dataset("xarray_da")
       
    xrdata.add_tensor("data",data)
    xrdata.add_meta_string("attributes",attr_list)
    xrdata.add_meta_string("coordinates",coord_string)  # <class 'list'>, not <class 'str'>
    xrdata.add_meta_string("coordinate_data",coord_list) # <class 'list'>, not <class 'str'> 
    xrdata.add_meta_string("dimensions", dims_string)
   
    
    
    # Retrieve the dataset
    rdataset = xrdata.get_tensor("data")
    rattr_list = xrdata.get_meta_strings("attributes")
    rcoord_string = xrdata.get_meta_strings("coordinates")
    rcoord_list = xrdata.get_meta_strings("coordinate_data")
    rdims_list = xrdata.get_meta_strings("dimensions")
    
    
    #pass to reconstruction 
    
    #rdata = Dataset("r_da")
    ret_xarray = xrdata.export_to_xarray(rdataset,rattr_list,rcoord_string,rcoord_list,rdims_list)
    
    #print('after conversion')
    #print(ret_xarray)
    
    
    print(ret_xarray.equals(da))
    assert ret_xarray.equals(da)
    
    
    
    ###################
    
    #BEFORE 
    #<xarray.DataArray (x: 2, y: 2, time: 3)>
#array([[[29.11241877, 18.20125767, 22.82990387],
#        [32.92714559, 29.94046392,  7.18177696]],

#       [[22.60070734, 13.78914233, 14.17424919],
#        [18.28478802, 16.15234857, 26.63418806]]])
#Coordinates:
#    lon             (x, y) float64 -99.83 -99.32 -99.79 -99.23
#    lat             (x, y) float64 42.25 42.21 42.63 42.59
#  * time            (time) datetime64[ns] 2014-09-06 2014-09-07 2014-09-08
#    reference_time  datetime64[ns] 2014-09-05
#Dimensions without coordinates: x, y
#Attributes:
#    description:  Ambient temperature.
#    units:        degC
    
    
    #AFTER
#    <xarray.DataArray (x: 2, y: 2, time: 3)>
#array([[[29.11241877, 18.20125767, 22.82990387],
#        [32.92714559, 29.94046392,  7.18177696]],

   #    [[22.60070734, 13.78914233, 14.17424919],
   #     [18.28478802, 16.15234857, 26.63418806]]])
#Coordinates:
#    lon             (x, y) float64 -99.83 -99.32 -99.79 -99.23
#    lat             (x, y) float64 42.25 42.21 42.63 42.59
#  * time            (time) datetime64[ns] 2014-09-06 2014-09-07 2014-09-08
#    reference_time  datetime64[ns] 2014-09-05
#Dimensions without coordinates: x, y
#Attributes:
#    description:  Ambient temperature.
#    units:        degC
    
    
    
    
    ####################
    
    
    
    
    
    #print("rattr_list",rattr_list)
    #print("rcoord_string",rcoord_string)
    #print("rcoord_list",rcoord_list)
    #print("dims_list",rdims_list)
    
    
    
   # dataset.add_tensor("coordinates_",np.array([t1]))
    
    #add coordinate arrays 
    
    #print("coot",coot)
   # for coord in coot:
   #   print("2:",coord)
      #print(coot[f'{coord}'].values)
      #print(coot['lat'].values)
     # dataset.add_tensor(f"coordinates_{coord}",np.array(coot))
    #  dataset.add_tensor(f"coordinates_{coord}",np.array(coot))
     # dataset.add_tensor(f"coordinates_{coord}",coot[f'{coord}'].values)
    
    
    #lat = coot['lat'].values
    #lon = coot['lon'].values
    
    
    #time = coot['time'].values
    
    #time = time.astype(datetime.datetime)
    #print("time type",type(time))
    #ref = coot['reference_time'].values
    
    #ref = ref.astype(datetime.datetime)
    #print("time ref",type(ref))
    
    #dataset.add_tensor("coordinates_1",np.array(lat))
    #dataset.add_tensor("coordinates_2",np.array(lon))
    #dataset.add_tensor("coordinates_3",np.array(time))
   # dataset.add_tensor("coordinates_4",np.array([ref]))
    # add list of coorindates 
    
  
      
    #dataset.add_tensor("coordinates_",t1)
    #
    #dataset.add_tensor(f"coordinates_{c}",c1[3])
    
    #they are in tuples, want just a list of the value probably
   # for attr in a:
   #   dataset.add_meta_string(f"attributes_{attr}",attr)
            
   # dataset.add_meta_string("attributes.list",a)
        #list of attribures 
  
   # x=0
    #for coord in c:
      
      #datetime64 to numpy.ndarray 
      
      #dataset.add_tensor(f"coordinates_{coord}",c1[3])
     # print(coord)
     # print(type(c1[3]))   #<class 'numpy.datetime64'
     # c1[3] = np.array(c1[3])
      #print("here",type(c1[3]))
      #dataset.add_tensor(f"coordinates_{coord}",c1[3])
      #'numpy.datetime64'>, not <class 'numpy.ndarray'>
      #convert datatime to 
     # here = c1[3]
      #print(ny.array(here))
      

    
    #assert to_da.all() == da.data.all()
    #assert att == list(att_.items())
    
    #variable tensor 
    #comma separated string of attribures 
    #comma separated string of coordinate
    #coorindate tensors 
    
     #uo :data array 

   # uod = Dataset("") 

    #uod.add_tensor("data",uox.values)

        #uo -- values 
   # for coord in uox.coords:
   #   uod.add_tensor(f"coordinates_{coord}",uox.coord())
            
   # uod.add_metastring("coordinates.list",uox.coords)
        #list of coordinates

    #for attr in uox.attrs:
   #   uod.add_metastring(f"attributes_{attr}",uox.attr)
            
   # uod.add_metastring("attributes.list",uox.attr)
        #list of attribures 

        #uox.attrs -- is a comma separated list   '',join in python 


        # labelling the source format 
        #what kind of dataformat this corresponds to 

    #  uod.addmetastring("source_format","xarray")
    
    # Send the dataset
   # dataset.add_tensor(tensor_name, tensor)
    

    # Retrieve the dataset
   # rdataset = dataset.get_tesnor(dataset_name)
  