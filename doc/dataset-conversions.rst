********
DataSet Conversions
********

Xarray DataSet Conversions 
======
Dataset conversion to Xarray within the DatasetConversion class is two-step process: 
1) where additional metadata is added to an existing Dataset to add the possibility of conversion and 
2) a transformation method which ingests a Dataset and returns an object in the native dataformat. 
The ``add_metadata_for_xarray()`` method performs this first task, allowing the ``transform_to_xarray()`` 
method to identify which fields should be used in the construction of the specific data format. 

.. code-block:: python
    // add meta_data_for_xarray interface
    add_metadata_for_xarray(dataset, data_names, dim_names, coord_names=None, attr_names=None)

.. code-block:: python
    // transform_to_xarray interface
    transform_to_xarray(dataset)

The ``add_metadata_for_xarray()`` method should prepare the SmartRedis dataset for transformation, and then the 
``transform_to_xarray()`` converts from a SmartRedis dataset into a dictionary of keys as the name of the Xarray DataArrays, and the values
as the actual converted Xarray DataArrays.  

The add_metadata_for_xarray() transform_to_xarray() methods should support adding multiple tensors into SmartRedis datasets and storing their common metadata.
If multiple data items are present with common metadata then multiple xarrays will be built. Support for multiple data items with 
differing metadata is not yet supported. 

Separating the adding of the metadata and the transformation into the appropriate data format minimizes 
the SmartRedis interference with the existing dataset. 

add_metadata_for_xarray
-------
The ability to extract data (metadata,tensors, etc.) by their original field names remains intact after any call to 
``add_metadata_for_xarray()``.

Metadata names that are going to be reserved by and on behalf of the add_metadata_for_xarray() method:
.. code-block:: python
    _xarray_data_name
    _xarray_dim_name
    _xarray_coord_name 
    _xarray_attr_name 

The ``add_metadata_for_xarray()`` method supports attaching data and metadata to a tensor within a SmartRedis dataset. 

The ``add_metadata_for_xarray()`` method has the ability to define the coordinates of each dimension of a tensor added to the dataset 
(e.g. the actual x, y, z values of every element of a 3D tensor or vector of timestamps for a 1D timeseries) 
If the user would like to add data variables as coordinates to their Xarray DataArray, the data name of the data variable
must match the name of the coordinate_name being specified in the ``add_metadata_for_xarray()`` parameters, and the method will recognize the appropriately named data variable
as a coordianate variable to be added to the Xarray DataArray. 

The add_metadata_for_xarray() method should not interfere with the existing Dataset API to extract and manipulate data 


transform_to_xarray
-------

The transform to xarray method will retrieve the field names store in the Dataset under these metadata names 
for populating the native xarray conversion to DataArray method. 

We expect users to construct the datasets themselves using the Dataset API before calling the add_metadata_for_xarray() method.
Only the field names will be being passed into ``add_metadata_for_xarray()``, so the actual structure of the dataset and any of the metadata will 
not be affected after calling the method. 





