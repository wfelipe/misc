import paramiko

class SSHConnection:
    "gerencia as conexoes de ssh"
    hostname = None
    port = 22

    def __init__ (self, hostname, username, password, port=22):
        self.hostname = hostname
        self.port = port
        self.username = username
        self.password = password

    def connect (self):
        pass

sw = CatOS ('192.168.215.221', 'quebec', 'quebec.', port=1198)
sw = CatOS ('localhost', 'username', 'passwd', port=22)
