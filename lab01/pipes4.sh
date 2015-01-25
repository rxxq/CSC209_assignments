#!/bin/bash
ps aux | tr -s ' ' | cut -d ' ' -f 1,11 | grep sshd | grep -v grep| cut -d ' ' -f 1 | sort | uniq | wc -l
