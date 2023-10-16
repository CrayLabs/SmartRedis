******
Python
******

Client API
==========

Client Class Method Overview
----------------------------

.. currentmodule::  smartredis

.. autosummary::

    Client.__init__
    Client.__str__
    Client.put_tensor
    Client.get_tensor
    Client.delete_tensor
    Client.copy_tensor
    Client.rename_tensor
    Client.tensor_exists
    Client.poll_tensor
    Client.put_dataset
    Client.get_dataset
    Client.delete_dataset
    Client.copy_dataset
    Client.rename_dataset
    Client.dataset_exists
    Client.poll_dataset
    Client.set_function
    Client.set_function_multigpu
    Client.set_script
    Client.set_script_multigpu
    Client.set_script_from_file
    Client.set_script_from_file_multigpu
    Client.get_script
    Client.run_script
    Client.run_script_multigpu
    Client.delete_script
    Client.delete_script_multigpu
    Client.set_model
    Client.set_model_multigpu
    Client.set_model_from_file
    Client.set_model_from_file_multigpu
    Client.get_model
    Client.run_model
    Client.run_model_multigpu
    Client.delete_model
    Client.delete_model_multigpu
    Client.model_exists
    Client.poll_model
    Client.key_exists
    Client.poll_key
    Client.set_data_source
    Client.use_tensor_ensemble_prefix
    Client.use_dataset_ensemble_prefix
    Client.use_model_ensemble_prefix
    Client.use_list_ensemble_prefix
    Client.get_db_node_info
    Client.get_db_cluster_info
    Client.get_ai_info
    Client.flush_db
    Client.config_get
    Client.config_set
    Client.save
    Client.append_to_list
    Client.delete_list
    Client.copy_list
    Client.rename_list
    Client.get_list_length
    Client.poll_list_length
    Client.poll_list_length_gte
    Client.poll_list_length_lte
    Client.get_datasets_from_list
    Client.get_dataset_list_range

Client Class Method Detailed View
---------------------------------

.. autoclass:: Client
   :members:
   :show-inheritance:


DataSet API
===========

Dataset Class Method Overview
-----------------------------

.. currentmodule::  smartredis

.. autosummary::

    Dataset.__init__
    Dataset.__str__
    Dataset.add_tensor
    Dataset.get_tensor
    Dataset.add_meta_scalar
    Dataset.get_meta_scalars
    Dataset.add_meta_string
    Dataset.get_meta_strings
    Dataset.get_metadata_field_names
    Dataset.get_metadata_field_type
    Dataset.get_name
    Dataset.get_tensor_type
    Dataset.get_tensor_names
    Dataset.get_tensor_dims

Dataset Class Method Detailed View
----------------------------------

.. autoclass:: Dataset
   :members:
   :show-inheritance:

Logging API
===========

The SmartRedis logging functionality is split across multiple classes as well
as three standalone methods. All logging requires a context, which is the text
that is attached to a log message so that when reading the log, one can tell
which part of their program generated the log message. A context can be a
Client object, a DataSet object, or a LogContext object (which only contains the
string for identifying context); or it can be a simple text string. The three
classes all derive from the SRObject class, which contains logging methods.
Three standalone methods support logging against a string context.

Logging Functionality Overview
------------------------------

.. currentmodule::  smartredis

.. autosummary::

    SRObject.log_data
    SRObject.log_warning
    SRObject.log_error
    LogContext.__init__
    log_data
    log_warning
    log_error

LogContext Class Method Detailed View
-------------------------------------

.. autoclass:: LogContext
   :members:
   :show-inheritance:

SRObject Class Method Detailed View
-----------------------------------

.. autoclass:: SRObject
   :members:
   :show-inheritance:

Standalone Logging Method Detailed View
---------------------------------------

.. autofunction:: log_data

.. autofunction:: log_warning

.. autofunction:: log_error
