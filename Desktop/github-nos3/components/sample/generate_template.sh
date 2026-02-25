#!/usr/bin/env bash
#
# Generate a new component by:
#   Copy all files in current directory to component name directory
#   Change the ../component filenames to the provided component name
#

# Check argument
if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters!"
    echo "  Expected utilization:"
    echo "    ./generate_template.sh <New_Component_Name>"
    echo ""
    exit 0
fi
COMPONENT_NAME=$1
COMPONENT_NAME_LOWER=$(echo "$COMPONENT_NAME" | sed -e 's/\(.*\)/\L\1/')
COMPONENT_NAME_FIRST=${COMPONENT_NAME_LOWER^}
COMPONENT_NAME_UPPER=${COMPONENT_NAME_LOWER^^}
echo "New component name = " $COMPONENT_NAME
echo "New component name lower = " $COMPONENT_NAME_LOWER
echo "New component name first = " $COMPONENT_NAME_FIRST
echo "New component name upper = " $COMPONENT_NAME_UPPER
echo ""

SCRIPT_DIR=$(cd `dirname $0` && pwd)
CODE_DIR=.
NEW_DIR=../${COMPONENT_NAME_LOWER}

# Debugging
echo ""
echo "Script directory = " $SCRIPT_DIR
echo "Code directory = " $CODE_DIR
echo "New directory = " $NEW_DIR
echo ""


echo "Copy all files in $CODE_DIR to $NEW_DIR directory"
cp -R $CODE_DIR $NEW_DIR
rm $NEW_DIR/generate_template.sh
echo ""

echo "Change $NEW_DIR file contents from sample to the provided component name"
find $NEW_DIR -type f -exec sed -i "s/SAMPLE/$COMPONENT_NAME_UPPER/g" {} \;
find $NEW_DIR -type f -exec sed -i "s/Sample/$COMPONENT_NAME_FIRST/g" {} \;
find $NEW_DIR -type f -exec sed -i "s/sample/$COMPONENT_NAME_LOWER/g" {} \;
echo ""

echo "Change the $NEW_DIR filenames to the provided component name"
mv $NEW_DIR/gsw/SAMPLE $NEW_DIR/gsw/$COMPONENT_NAME_UPPER
find $NEW_DIR -type f | sed "p;s/SAMPLE/$COMPONENT_NAME_UPPER/g" | xargs -d '\n' -n 2 mv 2> /dev/null
find $NEW_DIR -type f | sed "p;s/Sample/$COMPONENT_NAME_FIRST/g" | xargs -d '\n' -n 2 mv 2> /dev/null
find $NEW_DIR -type f | sed "p;s/sample/$COMPONENT_NAME_LOWER/g" | xargs -d '\n' -n 2 mv 2> /dev/null
echo ""

echo "generate_template.sh complete!"
echo ""
