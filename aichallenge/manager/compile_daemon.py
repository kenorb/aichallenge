#!/usr/bin/python

import cPickle
import logging
import logging.handlers
import os
import signal
import sys
import time
from subprocess import Popen, PIPE

import gmail
import MySQLdb
from server_info import server_info

logger = logging.getLogger('compile_logger')
logger.setLevel(logging.INFO)
handler = logging.handlers.RotatingFileHandler("compile.log",
                                                                                             maxBytes=1000000,
                                                                                             backupCount=5)
logger.addHandler(handler)
my_pid = os.getpid()

def log_message(message):
    logger.info(str(my_pid) + ": " + message)
    print message

# Looks in the given directory for a file called entry.zip, entry.tar.gz, or
# entry.gz. Returns some output and some error output. If the error output has
# length greater than zero, then something went wrong.
def unpack():
    out = ""
    err = ""
    import platform
    if platform.system() == 'Windows':
        zip_files = [
            ("entry.tar.gz", "7z x entry.tar.gz"),
            ("entry.tgz", "7z x entry.tgz"),
            ("entry.zip", "7z x entry.zip")
        ]
    else:
        zip_files = [
            ("entry.tar.gz", "tar xfz entry.tar.gz > /dev/null 2> /dev/null"),
            ("entry.tgz", "tar xfz entry.tgz > /dev/null 2> /dev/null"),
            ("entry.zip", "unzip -u entry.zip > /dev/null 2> /dev/null")
        ]
    found_archive_file = False
    for file_name, command in zip_files:
        if os.path.exists(file_name):
            out += "Found " + file_name + ". Attempting to unpack.\n"
            out += command + "\n"
            os.system(command)
            found_archive_file = True
            # this is for dealing with starter packs being submitted
            #    start pack structer should be changed
            if True:
                files = os.listdir(os.getcwd())
                num_files = 0
                num_dirs = 0
                dir_names = []
                for file in files:
                    if os.path.isfile(file):
                        num_files += 1
                    if os.path.isdir(file):
                        dir_names.append(file)
                if len(dir_names) > 0 and num_files == 1:
                    for dir_name in dir_names:
                        os.system("mv " + str(dir_name) + "/* .")
                        os.system("rm -rf " + str(dir_name))
                os.system("rm -rf tools maps example_bots")
            if platform.system() != 'Windows':
                os.system("chmod -R go+r . 2> /dev/null")
                os.system("find . -type d -exec chmod 755 {} \;")
            break
    if not found_archive_file:
        err +=    "Failed to unpack the submission. This is probably " + \
            "because the submission zip file was missing. This shouldn't " + \
            "happen. Please let the contest administrators know!\n"
    return out, err

# Sends an email message to a user letting them know that their code compiled
# successfully.
def send_success_mail(email_address, output_messages):
    subject = "Compile Success!"
    body = "This is just to let you know that your latest submissions to the "
    body += "Google AI Challenge has successfully compiled. Here is the "
    body += "output of the compile script, in case you're curious:\n\n"
    body += output_messages
    body += "\n\nSincerely,\nThe Compile Script"
    gmail.send(email_address, subject, body)

# Sends an email to a user letting them know that their code failed to compile.
def send_fail_mail(email_address, output_messages, error_messages):
    subject = "Compile Failure!"
    body = "Unfortunately, your latest submission to the Google AI Challenge "
    body += "did not compile successfully. Check out the error messages below "
    body += "for more information as to why this might be. Fix as many of the "
    body += "errors as you can, and then submit your code again.\n\n"
    body += "COMPILER OUTPUT\n\n" + output_messages + "\n\n"
    body += "COMPILER ERRORS\n\n" + error_messages + "\n\n"
    body += "Sincerely,\nThe Compile Script"
    gmail.send(email_address, subject, body)

# Changes the numerical status of a submission in the database.
def set_submission_status(submission_id, status):
    connection = MySQLdb.connect(host = server_info["db_host"],
                                                             user = server_info["db_username"],
                                                             passwd = server_info["db_password"],
                                                             db = server_info["db_name"])
    cursor = connection.cursor()
    query = "UPDATE submission SET status = " + str(status) + " " + \
        "WHERE submission_id = " + str(submission_id)
    cursor.execute(query)
    if status == 40:
        query = "UPDATE submission SET latest = 1 " + \
            "WHERE submission_id = " + str(submission_id)
        cursor.execute(query)
    cursor.close()
    connection.close()

def set_submission_language(submission_id, language_id):
    connection = MySQLdb.connect(host = server_info["db_host"],
                                                             user = server_info["db_username"],
                                                             passwd = server_info["db_password"],
                                                             db = server_info["db_name"])
    cursor = connection.cursor()
    query = "UPDATE submission SET language_id = " + str(language_id) + " " + \
        "WHERE submission_id = " + str(submission_id)
    cursor.execute(query)
    cursor.close()
    connection.close()

# Given a submission_id, compiles the submission. If the compile was
# successful, return True. Otherwise, returns False. Also sends mail to the
# user letting them know how the compile went.
def compile_submission(submission_id, email_address):
    output_messages = ""
    error_messages = ""
    path = os.path.join(server_info["submissions_path"], str(submission_id), '')
    if os.path.exists(path):
        os.chdir(path)
    else:
        error_messages += "Unable to locate submission directory.\n"
    if len(error_messages) == 0:
        out, err = unpack()
        output_messages += out
        error_messages += err
    if len(error_messages) == 0:
        comp_proc = Popen(['python',
                os.path.join(server_info["root_path"], 'manager/compile_anything.py')],
            stdout=PIPE)
        stop_time = time.time() + 300
        while comp_proc.poll() is None and time.time() < stop_time:
            time.sleep(1)
        if comp_proc.poll() is None:
            os.kill(comp_proc.pid, signal.SIGKILL)
            error_messages = "Compilation did not finish in 5 minutes"
        else:
            comp_out = comp_proc.stdout.read()
            out, err, language_id = cPickle.loads(comp_out)
            set_submission_language(submission_id, language_id)
            output_messages += out
            error_messages += err
    if len(error_messages) == 0:
        set_submission_status(submission_id, 40)
        send_success_mail(email_address, output_messages)
    else:
        set_submission_status(submission_id, 70)
        log_message('submission_id: %s' % submission_id)
        log_message('output: %s' % output_messages)
        log_message('error_messages: %s' % error_messages)
        send_fail_mail(email_address, output_messages, error_messages)

# Returns a list of submissions that have to be compiled. Each element of the
# list is a dictionary with keys submission_id and email.
def submissions_to_be_compiled():
    connection = MySQLdb.connect(host = server_info["db_host"],
                                                             user = server_info["db_username"],
                                                             passwd = server_info["db_password"],
                                                             db = server_info["db_name"])
    cursor = connection.cursor(MySQLdb.cursors.DictCursor)
    cursor.execute("""
        SELECT
            s.submission_id,
            u.email
        FROM submission s
        INNER JOIN user u ON u.user_id = s.user_id
        WHERE s.status = 20
        ORDER BY s.timestamp ASC
        """)
    submissions = []
    while True:
        row = cursor.fetchone()
        if row is None:
            break
        s = {
            "submission_id" : row["submission_id"],
            "email" : row["email"]
        }
        submissions.append(s)
    cursor.close()
    connection.close()
    return submissions

# This is the main loop of the compile daemon. It cycles waiting for new
# submissions to compile, and compiles them one after the other. If the
# maximum amount of time has passed, then no new compiles are started.
if len(sys.argv) < 2:
    print "USAGE: python compile_daemon.py max_time_in_seconds"
    sys.exit(1)
max_time = int(sys.argv[1])
start_time = time.time()
while time.time() - start_time < max_time:
    submissions = submissions_to_be_compiled()
    if time.time() - start_time > max_time:
        break
    if len(submissions) > 0:
        s = submissions[0]
        submission_id = s["submission_id"]
        email = s["email"]
        log_message( "Compiling submission " + str(submission_id) + ". Backlog size: " + \
            str(len(submissions) - 1) )
        compile_submission(submission_id, email)
    else:
        time.sleep(2)
