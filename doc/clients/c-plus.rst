***
C++
***

Client API
==========

The Client API is purpose-built for interaction with RedisAI, 
which extends the capabilities of the Redis in-memory data store. 
It's important to note that the SmartRedis Client API is the exclusive 
means for altering, transmitting, and receiving data within the in-memory 
database. All other data structures are confined to local operations.

.. doxygenclass:: SmartRedis::Client
   :project: cpp_client
   :members:
   :undoc-members:


Dataset API
===========

The SmartRedis DataSet API enables a user to manage a group of tensors 
and associated metadata within a datastructure called a ``DataSet`` object. 
The DataSet API operates independently of the database and solely 
maintains the dataset object **in-memory**. The actual interaction with the redis database, 
where a snapshot of the DataSet object is sent, is handled by the Client API.

.. doxygenclass:: SmartRedis::DataSet
   :project: cpp_client
   :members:
   :undoc-members:

