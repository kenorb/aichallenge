import MySQLdb
import os
from server_info import server_info
import sys

groupscript = """
  groupadd jailusers;
  echo "@jailusers hard nproc 10 # contest" >> /etc/security/limits.conf
  echo "@jailusers hard cpu 4 # contest" >> /etc/security/limits.conf
  echo "@jailusers hard rss 1040000 # contest" >> /etc/security/limits.conf
"""

userscript = """
  useradd -G jailusers -d /home/[username] -m [username];
  mkdir /home/[username]/.ssh;
  iptables -A OUTPUT -m owner --uid-owner [username] -j DROP;
"""

def main(argv):
  if len(argv) != 2:
    print "USAGE: python create_jail_users.py num_users_to_create"
    return 0
  n = int(argv[1])
  connection = MySQLdb.connect(host = server_info["db_host"],
                             user = server_info["db_username"],
                             passwd = server_info["db_password"],
                             db = server_info["db_name"])
  cursor = connection.cursor(MySQLdb.cursors.DictCursor)
  os.system(groupscript)
  for i in range(n):
    username = "jailuser" + str(i + 1)
    print "CREATING " + username
    command = userscript.replace("[username]", username)
    os.system(command)
    cursor.execute("INSERT INTO jail_users (username,in_use) VALUES ('" + \
       username + "',0)")
  cursor.close()
  connection.close()

if __name__ == "__main__":
  main(sys.argv)
