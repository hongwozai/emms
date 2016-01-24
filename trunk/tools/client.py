#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import cmd
import socket

class InteractiveCmd(cmd.Cmd):
    def __init__(self):
        cmd.Cmd.__init__(self)
        self.prompt = '$ '

    def do_conn(self, arg):
        global ip, port, sockfd

        if len(arg) is not 0:
            if len(arg.split()) is 2:
                ip, port = arg.split()
                port = int(port)
            else:
                print 'Syntax error!'
                self.help_conn()
                return

        print 'Connecting %s, %d' % (ip, port)
        sockfd.close()
        try:
            sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sockfd.connect((ip, port))
        except socket.error as error:
            print 'Please retry! %s' % error

    def help_conn(self):
        print 'conn ip, port'

    def do_sendline(self, arg):
        global sockfd

        try:
            sockfd.send('%s\n' % arg)
            print 'sending %s' % arg
            print sockfd.recv(1024)
        except socket.error as error:
            print 'Please retry! %s' % error

    def do_send(self, arg):
        try:
            sockfd.send('%s' % arg)
            print 'sending %s' % arg
            print sockfd.recv(1024)
        except socket.error as error:
            print 'Please retry! %s' % error

    def do_close(self, arg):
        global sockfd
        sockfd.close()

    def do_quit(self, arg):
        sys.exit(0)

    do_q = do_quit
    do_exit = do_quit
    do_sl = do_sendline
    do_s  = do_send



if __name__ == '__main__':
    global ip, port, sockfd
    if len(sys.argv) <= 1:
        ip = "192.168.100.39"
        port = 8888
    else:
        ip = sys.argv[1]
        port = sys.argv[2]
        port = int(port)
    print 'Connecting %s, %d' % (ip, port)

    sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockfd.settimeout(10)
    try:
        sockfd.connect((ip, port))
    except socket.error as error:
        print 'Please retry! %s' % error
        sys.exit(0)

    cli = InteractiveCmd()
    cli.cmdloop()
