import os.path
import sys
here = sys.path[0]
there = os.path.join(here, 'source')
sys.path = [there] + sys.path
from glad.__main__ import main
main()
