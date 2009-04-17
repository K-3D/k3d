#python

# This script uses Python to start an interactive "K-3D server" that listens for
# Python commands on port 8888, executing each command and printing its output.
# Although this example is a toy (a real RPC server wouldn't incorporate interactive
# Python prompts into its output, for example), it demonstrates how K-3D can be
# easily integrated into larger systems.
#
# To start a K-3D "server", do the following at the shell:
#
# $ k3d --ui=none --script=simple_rpc.py
#
# To communicate with the running K-3D "server" use netcat to send interactive
# Python "commands":
#
# $ nc localhost 8888
#
# You can use "exit()" or "quit()" to end a client session cleanly.  You will
# have to use Ctrl-C or similar to end the server process.

import code
import k3d
import sys
import SocketServer

class request_handler(SocketServer.BaseRequestHandler):

	def __init__(self, request, client_address, server):
		sys.stdout = self
		sys.stderr = self
		SocketServer.BaseRequestHandler.__init__(self, request, client_address, server)

	def write(self, output):
		self.request.send(output)

	def handle(self):
		console = code.InteractiveConsole()
		console.push("__close = False\n")
		console.push("def exit():\n  global __close\n  __close = True\n")
		console.push("def quit():\n  global __close\n  __close = True\n")

		sys.stdout.write(">>> ")
		while True:
			more_input = console.push(self.request.recv(1024))

			if console.locals["__close"]:
				break

			if more_input:
				sys.stdout.write("... ")
			else:
				sys.stdout.write(">>> ")
    
address = ("localhost", 8888)
server = SocketServer.TCPServer(address, request_handler)

k3d.log.info("K-3D listening for Python commands on port 8888 ...")
server.serve_forever()

