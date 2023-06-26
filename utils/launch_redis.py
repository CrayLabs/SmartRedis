# BSD 2-Clause License
#
# Copyright (c) 2021-2023, Hewlett Packard Enterprise
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

from subprocess import Popen, SubprocessError, run
from time import sleep
import argparse
import os

def stop_db(n_nodes, port, udsport):
    """Stop a redis cluster and clear the files
    associated with it
    """
    is_uds = udsport is not None
    if is_uds:
        n_nodes = 1
    is_cicd = os.getenv('SR_CICD_EXECUTION').lower() == "true"

    # It's clobberin' time!
    if is_cicd:
        rediscli = 'redis-cli'
    else:
        rediscli = os.path.abspath(
            os.path.dirname(__file__) + "/../third-party/redis/src/redis-cli"
        )

    # Clobber the server(s)
    procs = []
    for i in range(n_nodes):
        connection = f"-s udsport" if is_uds else f"-p {str(port + i)}"
        cmd = f"{rediscli} {connection} shutdown"
        proc = Popen(cmd, shell=True)
        procs.append(proc)

    # Make sure that all servers are down
    # Let exceptions propagate to the caller
    for proc in procs:
        _,_ = proc.communicate(timeout=15)
        if proc.returncode != 0:
            raise RuntimeError("Failed to kill Redis server!")

    # clean up after ourselves
    for i in range(n_nodes):
        fname = str(port+i) + ".log"
        if os.path.exists(fname):
            os.remove(fname)

        fname = str(port+i) + ".conf"
        if os.path.exists(fname):
            os.remove(fname)

    other_files = [
        'dump.rdb',
        'single.log',
        'UDS.log',
    ]
    for fname in other_files:
        if os.path.exists(fname):
            os.remove(fname)

    # Pause to give Redis time to die
    sleep(5)

def prepare_uds_socket(udsport):
    """Sets up the UDS socket
    """
    if udsport is not None:
        uds_abs = os.path.abspath(udsport)
        basedir = os.path.dirname(uds_abs)
        if not os.path.exists(basedir):
            os.makedir(basedir)
        if not os.path.exists(uds_abs):
            with open(uds_abs, 'a'):
                pass
        os.chmod(uds_abs, 0o777)

def create_db(n_nodes, port, device, rai_ver, udsport):
    """Creates a redis database starting with port at 127.0.0.1

    For a standalone server, the command issued should be equivalent to:
        redis-server --port $PORT --daemonize yes \
                     --logfile "single.log" \
                     --loadmodule $REDISAI_MODULES

    For a clustered server, the command issued should be equivalent to:
        redis-server --port $port --cluster-enabled yes --daemonize yes \
                        --cluster-config-file "$port.conf" --protected-mode no --save "" \
                        --logfile "$port.log" \
                        --loadmodule $REDISAI_MODULES

    For a UDS server, the command issued should be equivalent to:
        redis-server --unixsocket $SOCKET --unixsocketperm 777 --port 0 --bind 127.0.0.1 \
                    --daemonize yes --protected-mode no --logfile "uds.log" \
                    --loadmodule $REDISAI_MODULES

    where:
        PORT ranges from port to port + n_nodes - 1
        REDISAI_MODULES is read from the environment or calculated relative to this file
    """

    # Set up configuration
    is_uds = udsport is not None
    if is_uds:
        n_nodes = 1
    is_cluster = n_nodes > 1
    is_cicd = os.getenv('SR_CICD_EXECUTION').lower() == "true"

    if is_cicd:
        redisserver = "redis-server"
    else:
        redisserver = os.path.abspath(
            os.path.dirname(__file__) + "/../third-party/redis/src/redis-server"
        )
    rediscli = "redis-cli" if is_cicd else os.path.dirname(redisserver) + "/redis-cli"
    test_device = device if device is not None else os.environ.get(
        "SMARTREDIS_TEST_DEVICE","cpu").lower()
    if is_cicd:
        redisai = os.getenv(f'REDISAI_{test_device.upper()}_INSTALL_PATH') + '/redisai.so'
        redisai_modules = os.getenv("REDISAI_MODULES")
        if redisai_modules is None:
            raise RuntimeError("REDISAI_MODULES environment variable is not set!")
        rai_clause = f"--loadmodule {redisai_modules}"
    else:
        if rai_ver is None or rai_ver == "":
            raise RuntimeError("RedisAI version not specified")
        redisai_dir = os.path.abspath(
            os.path.dirname(__file__) + "/../third-party/RedisAI/" + rai_ver +
            "/install-" + test_device
        )
        redisai = redisai_dir + "/redisai.so"
        tf_loc = redisai_dir + "/backends/redisai_tensorflow/redisai_tensorflow.so"
        torch_loc = redisai_dir + "/backends/redisai_torch/redisai_torch.so"
        rai_clause = f"--loadmodule {redisai} TF {tf_loc} TORCH {torch_loc}"
    uds_clause = ""
    if is_uds:
        prepare_uds_socket(udsport)
        uds_clause = f"--bind 127.0.0.1 --unixsocket {udsport} --unixsocketperm 777"
    daemonize_clause = "--daemonize yes"
    cluster_clause = "--cluster-enabled yes" if is_cluster else ""
    prot_clause = "--protected-mode no" if is_cluster or is_uds else ""
    save_clause = "--save \"\"" if is_cluster else ""

    # Start servers
    procs = []
    for i in range(n_nodes):
        l_port = port + i
        port_clause = f"--port {str(l_port)}" if not is_uds else "--port 0"
        if is_cluster:
            log_clause = f"--logfile {str(l_port)}.log"
            cluster_cfg_clause = f"--cluster-config-file {str(l_port)}.conf"
        else:
            log_clause = "--logfile " + ("UDS.log" if is_uds else "single.log")
            cluster_cfg_clause = ""
        log_clause += " --loglevel notice"
        cmd = f"{redisserver} {port_clause} {daemonize_clause} {cluster_clause} " + \
              f"{cluster_cfg_clause} {log_clause} {uds_clause} {rai_clause} " + \
              f"{prot_clause} {save_clause}"

        print(cmd)
        proc = Popen(cmd, shell=True)
        procs.append(proc)

    # Make sure that all servers are up
    # Let exceptions propagate to the caller
    sleep(2)
    for proc in procs:
        _,_ = proc.communicate(timeout=15)
        if proc.returncode != 0:
            raise RuntimeError("Failed to launch Redis server!")

    # Create cluster for clustered Redis request
    if n_nodes > 1:
        cluster_str=' '
        for i in range(n_nodes):
            cluster_str += '127.0.0.1:' + str(port+i) + ' '
        cmd = f"{rediscli} --cluster create {cluster_str} --cluster-replicas 0"
        print(cmd)
        proc = run([cmd], input="yes", encoding="utf-8", shell=True)
        if proc.returncode != 0:
            raise SubprocessError("Cluster could not be created!")
        sleep(2)
        print("Cluster has been setup!")
    else:
        print("Server has been setup!")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=6379)
    parser.add_argument('--nodes', type=int, default=3)
    parser.add_argument('--rai', type=str, default=None)
    parser.add_argument('--device', type=str, default="cpu")
    parser.add_argument('--udsport', type=str, default=None)
    parser.add_argument('--stop', action='store_true')
    args = parser.parse_args()

    if args.stop:
        stop_db(args.nodes, args.port, args.udsport)
    else:
        create_db(args.nodes, args.port, args.device, args.rai, args.udsport)