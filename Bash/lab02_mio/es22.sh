#!/bin/bash

touch $1
chmod $2 $1

echo '#!/bin/bash' > $1

echo echo GNU is not Unix >> $1

echo exit 0 >> $1

exit 0
