#! /bin/sh
#Copyright (c) 2021 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

set -e

case "$1" in
  start)
        echo -n "Starting rmt_storage: "
        start-stop-daemon -S -b -a /sbin/rmt_storage

        echo "done"
        ;;
  stop)
        echo -n "Stopping rmt_storage: "
        start-stop-daemon -K -n rmt_storage
        echo "done"
        ;;
  restart)
        $0 stop
        $0 start
        ;;
  *)
        echo "Usage rmt_storage { start | stop | restart}" >&2
        exit 1
        ;;
esac

exit 0
