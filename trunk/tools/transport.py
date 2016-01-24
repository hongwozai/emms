#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import sys
import paramiko

def open_connection(ip, port, username, password):
    t = paramiko.Transport((ip, port))
    t.connect(username=username, password=password)
    sftp = paramiko.SFTPClient.from_transport(t)
    return sftp

def transport_dir(sftp, local_dir, remote_dir):
    for root, dirs, files in os.walk(local_dir):
        remote_root = root.replace(local_dir, remote_dir)
        try:
            sftp.mkdir(remote_root)
        except IOError:
            print root, 'exists!'

        for f in files:
            path = os.path.join(root, f)
            rpath = os.path.join(remote_root, f)
            print rpath
            sftp.put(path, rpath)


if __name__ == '__main__':
    if len(sys.argv) < 5:
        print "args must > 5 (ip username password localdir transportdir)!"
        exit(1)

    ip, username, password, localdir, transportdir = sys.argv[1:]
    print "%s, %s, %s, %s, %s" % (ip, username, password, localdir, transportdir)
    sftp = open_connection(ip, 22, username, password)
    transport_dir(sftp, localdir, transportdir)
