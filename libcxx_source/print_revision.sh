#!/bin/sh
echo "libcxx: "`svn info libcxx | grep '^Revision: ' | awk '{print $2}'`
echo "libcxxabi: "`svn info libcxxabi | grep '^Revision: ' | awk '{print $2}'`
