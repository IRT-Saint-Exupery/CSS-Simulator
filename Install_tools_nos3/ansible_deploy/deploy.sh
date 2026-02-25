#!/usr/bin/env bash
set -euo pipefail

ANSIBLE_DIR="/home/nos3/Install_tools_nos3/ansible_deploy"
SECOND_SCRIPT_DIR="/home/nos3/Install_tools_nos3"
FIRST_SCRIPT_NAME="Do_MakeClean_All_VM.sh"
SECOND_SCRIPT_NAME="Do_Make_All_VM.sh"

echo "==> Running ansible playbook in $ANSIBLE_DIR"
cd "$ANSIBLE_DIR"
ansible-playbook -i inventory.ini playbook.yaml -v

echo "==> Running first script: $FIRST_SCRIPT_NAME in $SECOND_SCRIPT_DIR"
cd "$SECOND_SCRIPT_DIR"
chmod +x "$FIRST_SCRIPT_NAME" "$SECOND_SCRIPT_NAME"
"./$FIRST_SCRIPT_NAME"

echo "==> Running second script: $SECOND_SCRIPT_NAME in $SECOND_SCRIPT_DIR"
"./$SECOND_SCRIPT_NAME"

echo "==> All steps completed successfully."

