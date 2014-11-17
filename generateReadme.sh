#!/bin/sh
# generateReadme.sh: Regenerates the help section of the README.md using output from ./maim --help.

# Remove help section
sed -i '/^help$/,/^```$/d' README.md

# Add the help section again.
echo 'help' >> README.md
echo '----' >> README.md
echo 'Join us on irc at freenode in *#maim*.' >> README.md
echo '```text' >> README.md
echo "$(./maim --help)" >> README.md
echo '```' >> README.md
