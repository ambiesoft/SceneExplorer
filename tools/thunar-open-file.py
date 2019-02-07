#!/usr/bin/env python

# https://unix.stackexchange.com/a/341829

# Open thunar and select file

import dbus
import os
import sys
import urlparse
import urllib




bus = dbus.SessionBus()
obj = bus.get_object('org.xfce.Thunar', '/org/xfce/FileManager')
iface = dbus.Interface(obj, 'org.xfce.FileManager')

_thunar_display_folder = iface.get_dbus_method('DisplayFolder')
_thunar_display_folder_and_select = iface.get_dbus_method('DisplayFolderAndSelect')


def display_folder(uri, display='', startup_id=''):
    _thunar_display_folder(uri, display, startup_id)


def display_folder_and_select(uri, filename, display='', startup_id=''):
    _thunar_display_folder_and_select(uri, filename, display, startup_id)


def path_to_url(path):
    return urlparse.urljoin('file:', urllib.pathname2url(path))


def url_to_path(url):
    return urlparse.urlparse(url).path


def main(args):
    path = args[1]  # May be a path (from cmdline) or a file:// URL (from OS)
    # path = url_to_path(path)
    path = os.path.realpath(path)
    # url = path_to_url(path)

    # print('url=' + url)

    if os.path.isfile(path):
        dirname = os.path.dirname(path)
        filename = os.path.basename(path)
        # print('filename=' + filename)
        # filename = urllib.unquote(filename)
        # print(filename)
        display_folder_and_select(dirname, filename)
    else:
        print('path='+path)
        display_folder(path)


if __name__ == '__main__':
    main(sys.argv)
