class Switch:
    connection = None

    def __init__ (self, hostname, username, password, port=22):
        self.connection = SSHConnection (hostname, username, password)

    def connect (self):
        self.connection.connect ()

class CatOS (Switch):
    sw_type = 'CatOS'

    def __init__ (self, hostname, username, password, port=22):
        Switch.__init__ (self, hostname, username, password, port)
