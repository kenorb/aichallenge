########################################################################
##  Copyright (C) 2009 by Juhani Simola
##  <ojs@iki.fi>
##
##  Distributed under the terms of the GNU General Public License (GPL)
##  http://www.gnu.org/copyleft/gpl.html
########################################################################

"""
Functions to create java code.
Needed for exporting trained network to java source.
"""

import os
import os.path

def file2class(filename):
    """Returns Java package and class names from file path name"""
    path, file = os.path.split(filename)
    name, ext = os.path.splitext(file)
    if len(path) == 0:
        return None, name
    else:
        pathparts = path.split(os.sep)
        return '.'.join(pathparts), name
    
def array2j(array):
    """Formats a Python list of numbers or a Numpy array to Java array"""
    try:
        javastr = '{'
        for element in array:
            javastr += array2j(element) + ', '
        javastr += '}'
        return javastr
    except TypeError:
        return str(array)

def jcomment(text):
    """Formats a string into Java block comment"""
    lines = text.splitlines()
    return "/*\n * " + "\n * ".join(lines) + "\n */"

def java(filename, net, version):
    """Generates Java code for FFNet network"""
    package, classname = file2class(filename)
    code = []

    if package != None:
        code.append('package %s;' % (package))

    code.append(jcomment("""##################################################################
THIS FILE IS AUTOMATICALLY GENERATED WITH:

ffnet-%s, feed-forward neural network for python
http://ffnet.sourceforge.net

Copyright (C) 2006 by Marek Wojciechowski
<mwojc@p.lodz.pl>
 
Distributed under the terms of the GNU General Public License:
http://www.gnu.org/copyleft/gpl.html
##################################################################
 
NETWORK SPECIFICATION
%s

INPUT LIMITS
%s
OUTPUT LIMITS
%s
NOTE: You need the class org.ffnet.FFNet distributed with ffnet-%s
      sources to use the code below.
""" % (version, net.__repr__(), net.inlimits, net.outlimits, version)))

    code.append("""
import org.ffnet.FFNet;

public class %s extends FFNet {
    private static final double[] weights = %s;
    private static final int[][] conec = %s;
    private static final int[] inno = %s;
    private static final int[] outno = %s;
    private static final double[][] eni = %s;
    private static final double[][] deo = %s;

    public %s() {
        super(weights, conec, inno, outno, eni, deo);
    }
}
""" % (classname, array2j(net.weights), array2j(net.conec - 1), 
       array2j(net.inno - 1), array2j(net.outno - 1),
       array2j(net.eni), array2j(net.deo),
       classname))
    # Indices in conec, inno and outno are 1-based Fortran indices. 
    # -1 is to convert them to Java.
    return '\n'.join(code)
