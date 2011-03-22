
import os
import re
from subprocess import Popen

class Environ(object):
    """ Context manager that sets and restores an environment variable """
    def __init__(self, var, value):
        self.env_var = var
        self.new_value = value

    def __enter__(self):
        self.start_value = os.environ.get(self.env_var, None)
        os.environ[self.env_var] = self.new_value
        return self.new_value

    def __exit__(self, type, value, traceback):
        if self.start_value is not None:
            os.environ[self.env_var] = self.start_value
        else:
            del os.environ[self.env_var]

class CD(object):
    """ Context manager to change and restore the current working directory """
    def __init__(self, new_dir):
        self.new_dir = new_dir

    def __enter__(self):
        self.org_dir = os.getcwd()
        os.chdir(self.new_dir)
        return self.new_dir

    def __exit__(self, type, value, traceback):
        os.chdir(self.org_dir)

def file_matches(filename, line_pattern):
    """ Checks if a file has a line matching the given pattern """
    regex = re.compile(line_pattern)
    with open(filename, 'r') as src:
        for line in src:
            if regex.search(line):
                return True
    return False

def append_line(filename, line):
    """ Appends a line to a file """
    with open(filename, "a+") as afile:
        afile.write(line + '\n')

class CmdError(StandardError):
    """ Exception raised on an error return code results from run_cmd """
    def __init__(self, cmd, returncode):
        self.cmd = cmd
        self.returncode = returncode
        StandardError.__init__(self, "Error %s returned from %s"
            % (returncode, cmd))

def run_cmd(cmd):
    """ Run a command in a shell """
    print "Executing:", cmd
    proc = Popen(cmd, shell=True)
    status = proc.wait()
    if status != 0:
        raise CmdError(cmd, status)

def install_apt_packages(packages):
    """ Install system packages using aptitude """
    apt_cmd = "aptitude install -y "
    try:
        cmd = apt_cmd + packages
    except TypeError:
        cmd = apt_cmd + " ".join(packages)
    run_cmd(cmd)

