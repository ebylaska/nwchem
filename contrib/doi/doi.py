#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vi:ts=4:et

# Collect DOI-s and then query the CrossRef site for bibliographic data on 
# each of them.

from argparse import ArgumentParser
import pycurl
try:
    from io import BytesIO
except ImportError:
    from StringIO import StringIO as BytesIO

def parse_args():
    """Parse the command line arguments"""
    parser = ArgumentParser(description="Read DOI-s and lookup bibliographic data.")
    parser.add_argument("filename",help="A file containing a list of DOI-s, one DOI per line", nargs="+")
    args = parser.parse_args()
    return vars(args)

def parse_files(filenames):
    """Go through every file and add DOI-s to the doi-table."""
    doi_table = []
    #nfiles = len(filenames)
    #if nfiles < 1:
    #    raise Exception("Need at least 1 file name to read DOI-s from.")
    for filename in filenames:
        try:
            file = open(filename)
            lines = file.readlines()
            #lines.sort()
            doi_table += lines
            #doi_table.sort()
            file.close()
        except:
            raise Exception('Failed to read DOI-s from file: '+'"'+filename+'"')
    return doi_table

def remove_duplicates(doi_table):
    """Remove all duplicate DOI entries. We exploit that each element in a set
    must be unique"""
    doi_set = set(doi_table)
    doi_table = list(doi_set)
    return doi_table

def lookup_dois(doi_table):
    result = []
    buffer = BytesIO()
    c = pycurl.Curl()
    for doi in doi_table:
        c.setopt(c.URL, 'http://dx.doi.org/'+doi)
        c.setopt(c.HTTPHEADER, ['Accept: text/bibliography; style=bibtex'])
        c.setopt(c.WRITEDATA, buffer)
        # For older PycURL versions:
        #c.setopt(c.WRITEFUNCTION, buffer.write)
        c.setopt(c.FOLLOWLOCATION, True)
        c.perform()

    c.close()

    body = buffer.getvalue()
    # Body is a string on Python 2 and a byte string on Python 3.
    # If we know the encoding, we can always decode the body and
    # end up with a Unicode string.
    #print(body.decode('iso-8859-1'))
    result.append(body)

    return result

def print_bibliography(bibliography):
    """Print the list of bibliography entries."""
    for reference in bibliography:
        print reference+"\n"

def main ():
    """Run the whole thing (limited capability at the moment)"""
    opts = []
    args = []
    doi_table = []
    args = parse_args()
    doi_table = parse_files(args["filename"])
    doi_table = remove_duplicates(doi_table)
    bibliography = lookup_dois(doi_table)
    print_bibliography(bibliography)

if __name__ == "__main__":
    main()

