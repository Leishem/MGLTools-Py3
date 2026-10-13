"""Stand-in for Python 2's ``user`` module, which MGLTools imported for
``user.home`` (the user's home directory)."""
import os

home = os.path.expanduser("~")
