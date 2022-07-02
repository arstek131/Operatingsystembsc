#!/bin/bash

for fname in *; do
    if grep -q "#!/bin/bash" "$fname"; then
      echo "$fname"
    fi
done

exit 0
