#!/bin/bash
# Check to see if the Redis database has started by running 'ping'
# Note: This is the recommended way as described by Redis

port="${1:-6780}"
max_tries="${2:-10}"
sleep_interval=${3:-5}

# Set the correct option for either connecting via port or socket
# Note the "bash"-ism below checks to see if port is an integer
if [ "$port" -eq "$port" ] 2>/dev/null; then
  redis_option="-p"
else
  redis_option="-s"
fi

for i in {1..$max_tries}; do
  redis-cli $redis_option $port ping &> /dev/null
  if [[ $? -eq 0 ]]; then
    echo "Redis DB is ready"
    exit 0
  else
    echo "Redis DB not ready...sleeping $sleep_interval"
    sleep $sleep_interval
  fi
done

echo "Database could not be found on port $port"
exit 1