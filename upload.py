Import('env')
from base64 import b64decode

#
# Dump build environment (for debug)

#

env.Replace(UPLOADHEXCMD='avrdude1 -c usbasp -p t88 -B 0.5  -U flash:w:'+'"$SOURCES"'+':i')

# uncomment line below to see environment variables
