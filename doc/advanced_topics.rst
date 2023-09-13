***************
Advanced Topics
***************

This page of documentation is reserved for advanced topics
that may not be needed for all users.

.. _advanced_topics_dataset_aggregation:

Dataset Aggregation
===================

In addition to the ability to work with individual datasets, SmartRedis lets
users manage lists of datasets and work with them collectively. This is
particularly useful for producer/consumer relationships.

The DataSet Aggregation API manages references to multiple ``DataSet`` objects
on one or more database nodes through an interface referred to as
``aggregation lists``.
An ``aggregation list`` in SmartRedis stores references to
``DataSet`` objects that are stored in the database.  The SmartRedis client
in one application can append ``DataSet`` objects to the ``aggregation list``;
subsequently, ``SmartRedis`` clients in the same application or a different
application can retrieve some or all of the ``DataSet`` objects referenced
in that ``aggregation list``.

Appending to a DataSet aggregation list
---------------------------------------

The C++ client function to append a reference to a ``DataSet`` to an
aggregation list is shown below:

.. code-block:: cpp

    # C++ aggregation list append interface
    void append_to_list(const std::string& list_name,
                        const DataSet& dataset);

NOTE: The ``DataSet`` must have already been added to the database via
the ``put_dataset()`` method in the SmartRedis ``Client``.

The above function will append the a reference to the provided ``DataSet`` to the
``aggregation list``, which can be referenced in all user-facing functions
by the provided list name.  Note that a list can be appended by
any client in the same or different application.  Implicitly, all
``DataSets``, when appended, are added to the end of the list. If the list does not
already exist, it is automatically created.

Retrieving the DataSets in an aggregation list
----------------------------------------------

To retrieve the ``DataSet`` referenced in an ``aggregation list``,
the SmartRedis ``Client`` provides an API function that
returns an iterable object containing all of ``DataSets``
appended to the ``aggregation list``.  For example, the C++ client
function to retrieve the entire ``aggregation list`` contents is shown below:

.. code-block:: cpp

    # C++ aggregation list retrieval interface
    std::vector<DataSet> get_datasets_from_list(const std::string& list_name);

It is also possible to retrieve a subset of the DataSets within an aggregation
list:

.. code-block:: cpp

    # C++ aggregation list subset retrieval interface
    std::vector<DataSet> get_dataset_list_range(const std::string& list_name,
                                                    const int start_index,
                                                    const int end_index);

The start_index and end_index may be specified as negative numbers.
In this case, the offset is from the most recently appended DataSets.
For example, an offset of -1 is the last element in the list, and -3 is the
antepenultimate DataSet.

An application can determine how many DataSets are in an aggregation list
via the following API:

.. code-block:: cpp

    # C++ aggregation list length query
    int get_list_length(const std::string& list_name);


Synchronization
---------------

A SmartRedis Client can use the following APIs to block until a
predicate is matched on the length of the list:

.. code-block:: cpp

    # Block until the list reaches a specific length
    bool poll_list_length(const std::string& name, int list_length,
                            int poll_frequency_ms, int num_tries);

    # Block until the list reaches or exceeds a specific length
    bool poll_list_length_gte(const std::string& name, int list_length,
                                int poll_frequency_ms, int num_tries);

    # Block until the list is no longer than a specific length
    bool poll_list_length_lte(const std::string& name, int list_length,
                                int poll_frequency_ms, int num_tries);

Other operations
----------------

Finally, aggregation lists may be copied, renamed, or deleted. Note
that there is no synchronization support for these operations; performing
these operations when multiple applications are accessing the list may
lead to race conditions:

.. code-block:: cpp

    # Copy an aggregation list
    void copy_list(const std::string& src_name,
                    const std::string& dest_name);

    # Rename an aggregation list
    void rename_list(const std::string& src_name,
                        const std::string& dest_name);

    # Delete an aggregation list
    void delete_list(const std::string& list_name);
