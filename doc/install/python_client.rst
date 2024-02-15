

First, activate your Python virtual environment:

.. code-block:: bash

    conda activate <env name>

Install SmartRedis Python client from PyPI:

.. code-block:: bash

    pip install smartredis

Developers installing the Python client from PyPI
can install the SmartRedis client with additional
dependencies for testing and documentation with:

.. code-block:: bash

    pip install smartredis[dev]
    # or if using ZSH
    pip install smartredis\[dev\]

Now, when inside your virtual environment, you should be able to import
the ``Client`` from ``smartredis`` as follows:

.. code-block:: bash

    python -c 'from smartredis import Client'
